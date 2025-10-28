// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "IAWSScenariosDeployer.h"
#include "aws/gamelift/core/errors.h"
#include "AwsScenarios/IAWSScenario.h"

class AWSScenariosDeployer : public IAWSScenariosDeployer
{
public:
	bool DeployManagedEC2Scenario(
		const FText& Scenario,
		IAWSAccountInstance* AccountInstance,
		const FString& GameName,
		const FString& BuildOperatingSystem,
		const FString& BuildFolderPath,
		const FString& BuildFilePath,
		const FString& OutConfigFilePath,
		const FString& ExtraServerResourcesPath,
		bool EnableMetrics
	) override;
	bool DeployCustomScenario(
		const FString& CustomScenarioPath,
		IAWSAccountInstance* AccountInstance,
		const FString& GameName,
		const FString& BuildOperatingSystem,
		const FString& BuildFolderPath,
		const FString& BuildFilePath,
		const FString& OutConfigFilePath,
		const FString& ExtraServerResourcesPath
	) override;
	bool DeployContainerScenario(
		const FText& Scenario,
		IAWSAccountInstance* AwsAccountInstance,
		const FString& ContainerGroupDefinitionName,
		const FString& ContainerImageName,
		const FString& ContainerImageUri,
		const FString& IntraContainerLaunchPath,
		const FString& GameName,
		const FString& OutConfigFilePath,
		const FText& ConnectionPortRange,
		const FString& TotalVcpuLimit,
		const FString& TotalMemoryLimit,
		bool EnableMetrics
	) override;

	bool StopDeployment(IAWSAccountInstance* AwsAccountInstance) override;

	FString GetLastCognitoClientId() const override;
	FString GetLastApiGatewayEndpoint() const override;
	FString GetLastError() const override;
	FString GetLastErrorMessage() const override;

	TArray<FText> GetScenarios(const IAWSScenariosCategory Category) const override;
	FText GetToolTip(const FText& ScenarioName, const IAWSScenariosCategory Category) const override;

	void SetMainFunctionsReplacementId(const char* ReplacementId);
	void SetLastCognitoClientId(const FString& ClientId);
	void SetLastApiGatewayEndpoint(const FString& ApiGateway);
	void SetClientConfigPath(const char* ConfigPath);

private:

	bool DeployScenarioImpl(
		IAWSAccountInstance* AccountInstance,
		AwsScenarios::IAWSScenario* AwsScenario, AwsScenarios::BaseInstanceTemplateParams& Params, const FString&
		OutConfigFilePath
	);
	int UpdateDeploymentResults(
		IAWSAccountInstance* AccountInstance,
		const FString& ScenarioInstancePath,
		const FString& GameName,
		const FString& BucketName,
		const FString& OutConfigFilePath
	);

private:
	std::string MainFunctionsReplacementId;
	int LastAwsError = GameLift::GAMELIFT_ERROR_GENERAL;
	FString LastCognitoClientId;
	FString LastApiGatewayEndpoint;
	FString ClientConfigPath;
	FThreadSafeBool ShouldBeStopped{ false };
};