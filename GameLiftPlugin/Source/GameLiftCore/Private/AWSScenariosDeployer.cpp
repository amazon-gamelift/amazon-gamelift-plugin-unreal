// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "AWSScenariosDeployer.h"

#include "Misc/FileHelper.h"
#include "Misc/EngineVersionComparison.h"

#include "aws/gamelift/core/exports.h"
#include "aws/gamelift/core/errors.h"

#include "GameLiftCoreLog.h"
#include "GameLiftCoreConstants.h"

#include "AwsAccount/AwsAccountCredentials.h"
#include "AwsAccount/AwsAccountInstanceManager.h"
#include "AwsAccount/AwsAccountInfo.h"
#include "Utils/StringPaths.h"
#include "Utils/StringConvertors.h"
#include "Utils/LogMessageStorage.h"
#include "Utils/NativeLogPrinter.h"
#include "Utils/UnrealVersion.h"
#include "AwsErrors/Converter.h"
#include "AwsScenarios/ContainerFlexMatch.h"
#include "AwsScenarios/ContainerSingleRegionFleet.h"

#include "AwsScenarios/FlexMatch.h"
#include "AwsScenarios/SingleRegionFleet.h"
#include "AwsScenarios/CustomScenario.h"

#define LOCTEXT_NAMESPACE "AWSScenariosDeployer"

namespace AwsDeployerInternal
{
	static Logs::MessageStorage sLatestDeploymentLogErrorMessage = {};

	void LogCallback(unsigned int InLevel, const char* InMessage, int InSize)
	{
		auto Level = Logs::PrintAwsLog(InLevel, InMessage, InSize, Deploy::Logs::kLogReceived);

		if (Level == ELogVerbosity::Error)
		{
			sLatestDeploymentLogErrorMessage.Set(InMessage);
		}
	}

	void ReceiveReplacementId(DISPATCH_RECEIVER_HANDLE DispatchReceiver, const char* ReplacementId)
	{
		UE_LOG(GameLiftCoreLog, Verbose, TEXT("%s %s"), Deploy::Logs::kReceiveReplacementIdCallback, *Convertors::ASToFS(ReplacementId));
		((AWSScenariosDeployer*)DispatchReceiver)->SetMainFunctionsReplacementId(ReplacementId);
	}

	void ReceiveClientConfigPath(DISPATCH_RECEIVER_HANDLE DispatchReceiver, const char* ConfigPath)
	{
		UE_LOG(GameLiftCoreLog, Verbose, TEXT("%s %s"), Deploy::Logs::kReceiveClientConfigPathCallback, *Convertors::ASToFS(ConfigPath));
		((AWSScenariosDeployer*)DispatchReceiver)->SetClientConfigPath(ConfigPath);
	}

	auto BuildAwsScenarios(const IAWSScenariosCategory Category)
	{
		TMap<FString, TUniquePtr<AwsScenarios::IAWSScenario>> ScenarioMap;
		switch (Category)
		{
		case IAWSScenariosCategory::ManagedEC2:
		{
			ScenarioMap.Add(AwsScenarios::SingleRegionFleet::Name().ToString(),
				AwsScenarios::SingleRegionFleet::Create());
			ScenarioMap.Add(AwsScenarios::FlexMatch::Name().ToString(),
				AwsScenarios::FlexMatch::Create());
			break;
		}
		case IAWSScenariosCategory::Containers:
		{
			ScenarioMap.Add(AwsScenarios::ContainersSingleRegionFleet::Name().ToString(),
				AwsScenarios::ContainersSingleRegionFleet::Create());
			ScenarioMap.Add(AwsScenarios::ContainersFlexMatch::Name().ToString(),
				AwsScenarios::ContainersFlexMatch::Create());
			break;
		}
		}

		return ScenarioMap;
	}

	const auto& GetAwsScenarios(const IAWSScenariosCategory Category)
	{
		static TMap<FString, TUniquePtr<AwsScenarios::IAWSScenario>>
			ManagedEC2ScenarioMap(BuildAwsScenarios(IAWSScenariosCategory::ManagedEC2));
		static TMap<FString, TUniquePtr<AwsScenarios::IAWSScenario>>
			ContainersScenarioMap(BuildAwsScenarios(IAWSScenariosCategory::Containers));

		switch (Category)
		{
		case IAWSScenariosCategory::ManagedEC2:
		{
			return ManagedEC2ScenarioMap;
		}
		case IAWSScenariosCategory::Containers:
		{
			return ContainersScenarioMap;
		}
		default:
		{
			static TMap<FString, TUniquePtr<AwsScenarios::IAWSScenario>> DefaultMap = {};
			return DefaultMap;
		}
		}
	}

	AwsScenarios::IAWSScenario* GetAwsScenarioByName(const FText& ScenarioName, const IAWSScenariosCategory Category)
	{
		auto Invariant = FText::AsCultureInvariant(ScenarioName);
		return GetAwsScenarios(Category).FindChecked(Invariant.ToString()).Get();
	}

	FString ExtractYamlValue(const TArray<FString>& YamlStrings, const char* Name)
	{
		for (const FString& Line : YamlStrings)
		{
			auto FoundPosition = Line.Find(Name);

			if (FoundPosition >= 0)
			{
				FoundPosition += strlen(Name);
				return Line.RightChop(FoundPosition + 1);
			}
		}
		return FString();
	}

	class ResourcesInstanceGuard
	{
	public:
		ResourcesInstanceGuard(IAWSAccountInstance* AwsAccountInstance) :
			Instance(GameLiftAccountCreateGameLiftResourcesInstance(AwsAccountInstance->GetInstance()))
		{
		}

		~ResourcesInstanceGuard()
		{
			GameLiftResourcesInstanceRelease(Instance);
		}

		GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE Get()
		{
			return Instance;
		}

	private:
		GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE Instance;
	};

	bool IsDeployedSuccessfully(const std::string& StackStatus)
	{
		static std::unordered_set<std::string> SuccessfulStatuses
		{
			Aws::Status::kStackUpdateComplete, Aws::Status::kStackCreateComplete
		};

		return SuccessfulStatuses.find(StackStatus) != SuccessfulStatuses.end();
	}
} // namespace AwsDeployerInternal

void AWSScenariosDeployer::SetMainFunctionsReplacementId(const char* ReplacementId)
{
	MainFunctionsReplacementId = ReplacementId;
}

void AWSScenariosDeployer::SetClientConfigPath(const char* ConfigPath)
{
	ClientConfigPath = Convertors::ASToFS(ConfigPath);
}

void AWSScenariosDeployer::SetLastCognitoClientId(const FString& ClientId)
{
	LastCognitoClientId = ClientId;
}

void AWSScenariosDeployer::SetLastApiGatewayEndpoint(const FString& ApiGateway)
{
	LastApiGatewayEndpoint = ApiGateway;
}

/**
 * Returns the Unreal Engine version used to package the game server build
 *
 * @param AwsAccountInstance	Pointer to AwsAccountInstance to call APIs
 * @param BuildFilePath			Path to the packaged game server executable
 * @return PackagedVersion		Unreal Engine version used to package the game server build
 */
UnrealVersion::Version GetUnrealPackagedBuildVersion(IAWSAccountInstance* AwsAccountInstance, const FString& BuildFilePath)
{
	AwsDeployerInternal::ResourcesInstanceGuard ResourcesInstance(AwsAccountInstance);

	std::string ExecutablePath = Convertors::FSToStdS(BuildFilePath);
	FString PackagedVersionString = Convertors::ASToFS(GameLiftResourcesGetUnrealPackagedBuildVersion(ResourcesInstance.Get(), ExecutablePath));

	UnrealVersion::Version PackagedVersion = UnrealVersionUtils::ParseVersionString(PackagedVersionString);

	if (PackagedVersion == UnrealVersion::INVALID_VERSION) {
		// We failed to determine version, proceed with deployment without copying dependencies
		UE_LOG(GameLiftCoreLog, Log, TEXT("Failed to determine version from %s"), *BuildFilePath);
	}
	else {
		UE_LOG(GameLiftCoreLog, Log, TEXT("Detected game server build packaged with Unreal Version %s from game server executable path %s"),
			*UnrealVersionUtils::GetVersionString(PackagedVersion), *BuildFilePath);
	}

	return PackagedVersion;
}

/**
 * Sets extra server resources path for older Unreal Engine versions if needed
 * For UE versions older than 5.6.0, we need to copy additional dependencies
 *
 * @param ExtraServerResources	Path Path to the extra resources that may need to be copied
 * @param Params				Deployment parameters to be updated with extra resources path if needed
 * @return True if extra server resources path was set
 */
bool SetExtraServerResourcesPath(UnrealVersion::Version PackagedVersion, const FString& ExtraServerResourcesPath, AwsScenarios::ManagedEC2InstanceTemplateParams& Params)
{
	// UE5.6.0 does not require extra dependencies
	if (PackagedVersion < UnrealVersion::UE5_6_0) {
		UE_LOG(GameLiftCoreLog, Log, TEXT("Required dependencies will automatically be copied from %s since game server packaged version is < 5.6.0"),
			*ExtraServerResourcesPath);
		Params.ExtraServerResourcesPath = Convertors::FSToStdS(ExtraServerResourcesPath);
		return true;
	}

	return false;
}

bool AWSScenariosDeployer::DeployManagedEC2Scenario(
	const FText& Scenario,
	IAWSAccountInstance* AwsAccountInstance,
	const FString& GameName,
	const FString& BuildOperatingSystem,
	const FString& BuildFolderPath,
	const FString& BuildFilePath,
	const FString& OutConfigFilePath,
	const FString& ExtraServerResourcesPath,
	bool EnableMetrics
)
{
	UE_LOG(GameLiftCoreLog, Log, TEXT("%s %s"), Deploy::Logs::kRunAwsScenario, *Scenario.ToString());

	AwsScenarios::IAWSScenario* BaseAwsScenario = AwsDeployerInternal::GetAwsScenarioByName(Scenario, IAWSScenariosCategory::ManagedEC2);
	AwsScenarios::ScenarioWithGameServer* AwsScenario = static_cast<AwsScenarios::ScenarioWithGameServer*>(BaseAwsScenario);

	std::string stdLaunchPathParameter;
	AwsScenario->CreateLaunchPathParameter(BuildOperatingSystem, BuildFolderPath, BuildFilePath, stdLaunchPathParameter);

	AwsScenarios::ManagedEC2InstanceTemplateParams Params;

	Params.GameNameParameter = Convertors::FSToStdS(GameName);
	Params.BuildFolderPath = Convertors::FSToStdS(BuildFolderPath);

	UnrealVersion::Version PackagedVersion = GetUnrealPackagedBuildVersion(AwsAccountInstance, BuildFilePath);
	SetExtraServerResourcesPath(PackagedVersion, ExtraServerResourcesPath, Params);

	Params.BuildOperatingSystemParameter = Convertors::FSToStdS(BuildOperatingSystem);
	Params.LaunchPathParameter = stdLaunchPathParameter;
	Params.EnableMetricsParameter = EnableMetrics  ? "true" : "false";

	return DeployScenarioImpl(AwsAccountInstance, AwsScenario, Params, OutConfigFilePath);
}

bool AWSScenariosDeployer::DeployCustomScenario(
	const FString& CustomScenarioPath,
	IAWSAccountInstance* AwsAccountInstance,
	const FString& GameName,
	const FString& BuildOperatingSystem,
	const FString& BuildFolderPath,
	const FString& BuildFilePath,
	const FString& OutConfigFilePath,
	const FString& ExtraServerResourcesPath
)
{
	UE_LOG(GameLiftCoreLog, Log, TEXT("%s %s"), Deploy::Logs::kRunCustomScenario, *CustomScenarioPath);

	TUniquePtr<AwsScenarios::CustomScenario> AwsScenario = AwsScenarios::CustomScenario::Create(CustomScenarioPath);

	std::string stdLaunchPathParameter;
	AwsScenario->CreateLaunchPathParameter(BuildOperatingSystem, BuildFolderPath, BuildFilePath, stdLaunchPathParameter);

	AwsScenarios::ManagedEC2InstanceTemplateParams Params;

	Params.GameNameParameter = Convertors::FSToStdS(GameName);
	Params.BuildFolderPath = Convertors::FSToStdS(BuildFolderPath);
	Params.ExtraServerResourcesPath = Convertors::FSToStdS(ExtraServerResourcesPath);

	Params.BuildOperatingSystemParameter = Convertors::FSToStdS(BuildOperatingSystem);
	Params.LaunchPathParameter = stdLaunchPathParameter;

	return DeployScenarioImpl(AwsAccountInstance, AwsScenario.Get(), Params, OutConfigFilePath);
}

bool AWSScenariosDeployer::DeployContainerScenario(
	const FText& Scenario, IAWSAccountInstance* AwsAccountInstance, const FString& ContainerGroupDefinitionName,
	const FString& ContainerImageName, const FString& ContainerImageUri, const FString& IntraContainerLaunchPath,
	const FString& GameName, const FString& OutConfigFilePath, const FText& ConnectionPortRange, const FString& TotalVcpuLimit,
	const FString& TotalMemoryLimit, bool EnableMetrics)
{
	AwsScenarios::IAWSScenario* AwsScenario = AwsDeployerInternal::GetAwsScenarioByName(
		Scenario,
		IAWSScenariosCategory::Containers);

	auto StdBootstrapBucketName = Convertors::FSToStdS(AwsAccountInstance->GetBucketName());

	AwsScenarios::ContainerInstanceTemplateParams Params;

	Params.GameNameParameter = Convertors::FSToStdS(GameName);
	Params.ContainerGroupDefinitionNameParameter = Convertors::FSToStdS(ContainerGroupDefinitionName);
	Params.ContainerImageNameParameter = Convertors::FSToStdS(ContainerImageName);
	Params.ContainerImageUriParameter = Convertors::FSToStdS(ContainerImageUri);
	Params.LaunchPathParameter = Convertors::FSToStdS(IntraContainerLaunchPath);
	FString FleetUdpFromPort;
	FString FleetUdpToPort;
	ConnectionPortRange.ToString().Split("-", &FleetUdpFromPort, &FleetUdpToPort);
	Params.FleetUdpFromPortParameter = Convertors::FSToStdS(FleetUdpFromPort);
	Params.FleetUdpToPortParameter = Convertors::FSToStdS(FleetUdpToPort);
	Params.TotalVcpuLimitParameter = Convertors::FSToStdS(TotalVcpuLimit);
	Params.TotalMemoryLimitParameter = Convertors::FSToStdS(TotalMemoryLimit);
	Params.EnableMetricsParameter = EnableMetrics  ? "true" : "false";

	return DeployScenarioImpl(AwsAccountInstance, AwsScenario, Params, OutConfigFilePath);
}

bool AWSScenariosDeployer::StopDeployment(IAWSAccountInstance* AwsAccountInstance)
{
	UE_LOG(GameLiftCoreLog, Log, TEXT("%s"), Deploy::Logs::kDeploymentStop);

	AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Clear();

	if (!AwsAccountInstance->IsValid())
	{
		AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Set(Deploy::Errors::kAccountIsInvalidText);
		return false;
	}

	AwsDeployerInternal::ResourcesInstanceGuard ResourcesInstance(AwsAccountInstance);
	std::string StackStatus = GameLiftResourcesGetCurrentStackStatus(ResourcesInstance.Get());

	if (StackStatus.compare(Aws::Status::kStackUndeployed) == 0)
	{
		AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Set(Deploy::Errors::kNoStacksToStopDeployment);
		return false;
	}

	ShouldBeStopped = true;
	bool IsStopped = GameLiftResourcesInstanceCancelUpdateStack(ResourcesInstance.Get()) == GameLift::GAMELIFT_SUCCESS;

	if (!IsStopped)
	{
		AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Set(Deploy::Errors::kUnableToStopDeployment);
	}

	return IsStopped;
}

bool AWSScenariosDeployer::DeployScenarioImpl(
	IAWSAccountInstance* AwsAccountInstance,
	AwsScenarios::IAWSScenario* AwsScenario,
	AwsScenarios::BaseInstanceTemplateParams& Params,
	const FString& OutConfigFilePath
)
{
	constexpr auto DeployAbortResult = false;
	constexpr auto DeployCompletedResult = true;

	AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Clear();

	ShouldBeStopped = false;

	auto AccountHandle = AwsAccountInstance->GetInstance();

	if (AccountHandle == nullptr)
	{
		AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Set(Deploy::Errors::kAccountIsInvalidText);
		LastAwsError = GameLift::GAMELIFT_ERROR_GENERAL;
		UE_LOG(GameLiftCoreLog, Error, TEXT("%s"), Deploy::Logs::kAccountInstanceIsNull);
		return DeployAbortResult;
	}

	if (Params.GameNameParameter.size() > Deploy::kMaxGameNameWithPrefixLength)
	{
		AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Set(Deploy::Errors::kGameNameIsTooLongText);
		LastAwsError = GameLift::GAMELIFT_ERROR_GENERAL;
		return DeployAbortResult;
	}

	GameLiftAccountSetGameName(AccountHandle, Params.GameNameParameter.c_str());

	auto ScenarioPath = AwsScenario->GetScenarioPath();
	auto StdScenarioPath = Convertors::FSToStdS(ScenarioPath);
	GameLiftAccountSetPluginRootPath(AccountHandle, StdScenarioPath.c_str());

	auto ScenarioInstancePath = AwsScenario->GetScenarioInstancePath();
	auto StdScenarioInstancePath = Convertors::FSToStdS(ScenarioInstancePath);
	GameLiftAccountSetRootPath(AccountHandle, StdScenarioInstancePath.c_str());

	std::string StdAwsAccountId = GameLiftGetAwsAccountIdByAccountInstance(AccountHandle);

	if (ShouldBeStopped)
	{
		LastAwsError = GameLift::GAMELIFT_ERROR_GENERAL;
		UE_LOG(GameLiftCoreLog, Error, TEXT("%s"), Deploy::Logs::kDeploymentHasBeenStopped);
		return DeployAbortResult;
	}

	auto ShouldDeployBeAborted = [this](int ErrorCode, auto&& ErrorDebugString)
		{
			LastAwsError = ErrorCode;

			if (LastAwsError != GameLift::GAMELIFT_SUCCESS)
			{
				UE_LOG(GameLiftCoreLog, Error, TEXT("%s %s"), ErrorDebugString, *GameLiftErrorAsString::Convert(LastAwsError));
				return true;
			}

			if (ShouldBeStopped)
			{
				LastAwsError = GameLift::GAMELIFT_ERROR_GENERAL;
				UE_LOG(GameLiftCoreLog, Error, TEXT("%s"), Deploy::Logs::kDeploymentHasBeenStopped);
				return true;
			}

			return false;
		};

	if (ShouldDeployBeAborted(GameLiftAccountCreateAndSetFunctionsReplacementId(AccountHandle), Deploy::Logs::kCreateAndSetFunctionsReplacementIdFailed))
	{
		return DeployAbortResult;
	}

	if (ShouldDeployBeAborted(GameLiftAccountGetMainFunctionsReplacementId(AccountHandle, this, AwsDeployerInternal::ReceiveReplacementId),
		Deploy::Logs::kGetMainFunctionsReplacementIdFailed))
	{
		return DeployAbortResult;
	}

	auto StdBootstrapBucketName = Convertors::FSToStdS(AwsAccountInstance->GetBucketName());

	if (ShouldDeployBeAborted(AwsScenario->UploadGameServer(AwsAccountInstance, Params.BuildFolderPath.c_str(), Params.ExtraServerResourcesPath.c_str()),
		Deploy::Logs::kUploadGameServerFailed))
	{
		return DeployAbortResult;
	}

	Params.AccountId = StdAwsAccountId;
	Params.ApiGatewayStageNameParameter = AwsAccountInstance->GetBuildConfiguration();
	Params.BuildS3BucketParameter = StdBootstrapBucketName;
	Params.LambdaZipS3BucketParameter = StdBootstrapBucketName;
	Params.LambdaZipS3KeyParameter = AwsScenarios::GetLambdaS3Key(Params.GameNameParameter, MainFunctionsReplacementId);
	Params.UnrealEngineVersionParameter = Convertors::FSToStdS(UnrealVersionUtils::GetCurrentEngineVersion());

	if (ShouldDeployBeAborted(AwsScenario->SaveFeatureInstanceTemplate(AwsAccountInstance, Params.ToMap()),
		Deploy::Logs::kSaveFeatureInstanceTemplatesFailed))
	{
		return DeployAbortResult;
	}

	if (ShouldDeployBeAborted(GameLiftAccountUploadFunctions(AccountHandle), Deploy::Logs::kAccountUploadFunctionsFailed))
	{
		return DeployAbortResult;
	}

	if (ShouldDeployBeAborted(GameLiftAccountCreateOrUpdateMainStack(AccountHandle), Deploy::Logs::kCreateOrUpdateMainStackFailed))
	{
		return DeployAbortResult;
	}

	if (ShouldDeployBeAborted(GameLiftAccountDeployApiGatewayStage(AccountHandle), Deploy::Logs::kDeployApiGatewayStageFailed))
	{
		return DeployAbortResult;
	}

	if (ShouldDeployBeAborted(UpdateDeploymentResults(AwsAccountInstance, ScenarioInstancePath, FString(Params.GameNameParameter.c_str()), AwsAccountInstance->GetBucketName(), OutConfigFilePath),
		Deploy::Logs::kDeploymentStackStatusFailed))
	{
		return DeployAbortResult;
	}

	return DeployCompletedResult;
}

int AWSScenariosDeployer::UpdateDeploymentResults(IAWSAccountInstance* AwsAccountInstance, const FString& ScenarioInstancePath, const FString& GameName, const FString& BucketName, const FString& OutConfigFilePath)
{
	AwsDeployerInternal::ResourcesInstanceGuard ResourcesInstance(AwsAccountInstance);

	// Result will be stored to ClientConfigPath with sync call.
	GameLiftResourcesGetInstanceClientConfigPath(ResourcesInstance.Get(), this, AwsDeployerInternal::ReceiveClientConfigPath);
	std::string StackStatus = GameLiftResourcesGetCurrentStackStatus(ResourcesInstance.Get());
	UE_LOG(GameLiftCoreLog, Log, TEXT("%s %s"), Deploy::Logs::kDeploymentStackStatus, *Convertors::ASToFS(StackStatus.c_str()));

	auto CurrentScenarioInstancePath = Paths::ScenarioInstancePath(Menu::DeploymentServer::kCurrentScenarioFolder);

	FString ConfigPathPart, ConfigFilenamePart, ConfigExtensionPart;
	FPaths::Split(ClientConfigPath, ConfigPathPart, ConfigFilenamePart, ConfigExtensionPart);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.DeleteDirectoryRecursively(*CurrentScenarioInstancePath);
	PlatformFile.CopyDirectoryTree(*CurrentScenarioInstancePath, *ConfigPathPart, true);

	FString DestinationFileName = ConfigFilenamePart + "." + ConfigExtensionPart;
	PlatformFile.CreateDirectoryTree(*OutConfigFilePath);
	PlatformFile.CopyFile(*FPaths::Combine(OutConfigFilePath, DestinationFileName), *ClientConfigPath);

	FString UUID;
	FString Temp;
	BucketName.Split("-", &Temp, &UUID, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString Region;
	Temp.Split("-", &Temp, &Region, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString DestinationFileNameExtra = GameName + "_" + UUID + "_" + Region + "_" + ConfigFilenamePart + "." + ConfigExtensionPart;
	PlatformFile.CopyFile(*FPaths::Combine(OutConfigFilePath, DestinationFileNameExtra), *ClientConfigPath);

	TArray<FString> YamlStrings;
	FFileHelper::LoadANSITextFileToStrings(*ClientConfigPath, NULL, YamlStrings);
	LastCognitoClientId = AwsDeployerInternal::ExtractYamlValue(YamlStrings, Aws::Config::kUserPoolClientIdYamlName).TrimStartAndEnd();
	LastApiGatewayEndpoint = AwsDeployerInternal::ExtractYamlValue(YamlStrings, Aws::Config::kApiGatewayEndpointYamlName).TrimStartAndEnd();

	return AwsDeployerInternal::IsDeployedSuccessfully(StackStatus) ?
		GameLift::GAMELIFT_SUCCESS : GameLift::GAMELIFT_ERROR_GENERAL;
}

FString AWSScenariosDeployer::GetLastCognitoClientId() const
{
	return LastCognitoClientId;
}

FString AWSScenariosDeployer::GetLastApiGatewayEndpoint() const
{
	return LastApiGatewayEndpoint;
}

FString AWSScenariosDeployer::GetLastError() const
{
	return GameLiftErrorAsString::Convert(LastAwsError);
}

FString AWSScenariosDeployer::GetLastErrorMessage() const
{
	return AwsDeployerInternal::sLatestDeploymentLogErrorMessage.Get();
}

TArray<FText> AWSScenariosDeployer::GetScenarios(const IAWSScenariosCategory Category) const
{
	TArray<FText> Result;
	const auto& Scenarios = AwsDeployerInternal::GetAwsScenarios(Category);
	Result.Reserve(Scenarios.Num());

	for (const auto& Item : Scenarios)
	{
		Result.Add(Item.Value->GetUserName());
	}

	return Result;
}

FText AWSScenariosDeployer::GetToolTip(const FText& ScenarioName, const IAWSScenariosCategory Category) const
{
	return AwsDeployerInternal::GetAwsScenarioByName(ScenarioName, Category)->GetTooltip();
}

#undef LOCTEXT_NAMESPACE