// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "GameLiftAnywhereHandler.h"

#include "aws/gamelift/core/exports.h"
#include "aws/gamelift/GameLiftClient.h"

#include "GameLiftCoreConstants.h"
#include "GameLiftCoreLog.h"
#include "IGameLiftCoreModule.h"
#include "Utils/StringConvertors.h"
#include "Utils/UnrealVersion.h"

#define LOCTEXT_NAMESPACE "GameLiftAnywhereHandler"

namespace GameLiftAnywhereHandlerInternal
{
	FuncErrorCallback ErrorCallback = [](DISPATCH_RECEIVER_HANDLE ErrorReceiver, GAMELIFT_ERROR_INFO* ErrorInfo) -> void
	{
		FString* Error = reinterpret_cast<FString*>(ErrorReceiver);
		*Error = ErrorInfo->errorMessage;
	};
} // namespace GameLiftAnywhereHandlerInternal

GameLiftAnywhereCreateLocationResult GameLiftAnywhereHandler::CreateCustomLocation(const FString& InRegion)
{
	IAWSAccountInstance* AccountInstance = IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance();
	check(AccountInstance);
	check(AccountInstance->GetInstance());

	FString CustomLocationName = FString::Format(TEXT("{0}-{1}"), { Anywhere::kCustomLocationPrefix, InRegion });
	std::string StdCustomLocationName = Convertors::FSToStdS(CustomLocationName);

	GAMELIFT_ANYWHERE_CREATE_LOCATION_REQUEST Request;
	Request.customLocation = StdCustomLocationName.c_str();
	Request.succeedIfExisting = true;

	FString ErrorMessageReceiver;
	Request.errorCb = GameLiftAnywhereHandlerInternal::ErrorCallback;
	Request.errorReceiver = &ErrorMessageReceiver;

	GameLiftAnywhereCreateLocationResult Result;
	Result.bIsSuccessful = GameLiftAnywhereCreateCustomLocation(AccountInstance->GetInstance(), &Request);
	Result.ErrorMessage = ErrorMessageReceiver;
	
	if (Result.bIsSuccessful)
	{
		UE_LOG(GameLiftCoreLog, Log, TEXT("Successfully created the custom location: %s"), *CustomLocationName);

		Result.LocationName = CustomLocationName;
	}
	else
	{
		UE_LOG(GameLiftCoreLog, Error, TEXT("Failed to create a custom location '%s' - %s"), *CustomLocationName, *(Result.ErrorMessage));
	}

	return Result;
}

GameLiftAnywhereAddFleetLocationResult GameLiftAnywhereHandler::AddFleetLocation(const FString& InFleetId, const FString& InCustomLocation)
{
	IAWSAccountInstance* AccountInstance = IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance();
	check(AccountInstance);
	check(AccountInstance->GetInstance());

	std::string StdCustomLocationName = Convertors::FSToStdS(InCustomLocation);
	std::string StdFleetId = Convertors::FSToStdS(InFleetId);

	GAMELIFT_ANYWHERE_ADD_FLEET_LOCATION_REQUEST Request;
	Request.customLocation = StdCustomLocationName.c_str();
	Request.fleetId = StdFleetId.c_str();
	Request.succeedIfExisting = true;

	FString ErrorMessageReceiver;
	Request.errorCb = GameLiftAnywhereHandlerInternal::ErrorCallback;
	Request.errorReceiver = &ErrorMessageReceiver;

	GameLiftAnywhereAddFleetLocationResult Result;
	Result.bIsSuccessful = GameLiftAnywhereAddFleetLocation(AccountInstance->GetInstance(), &Request);
	Result.ErrorMessage = ErrorMessageReceiver;

	if (Result.bIsSuccessful)
	{
		UE_LOG(GameLiftCoreLog, Log, TEXT("Successfully added the custom location: %s to fleet: %s"), *InCustomLocation, *InFleetId);

		Result.LocationName = InCustomLocation;
		Result.FleetId = InFleetId;
	}
	else
	{
		UE_LOG(GameLiftCoreLog, Error, TEXT("Failed to add a custom location '%s' to fleet '%s' - %s"), *InCustomLocation, *InFleetId, *(Result.ErrorMessage));
	}

	return Result;
}

GameLiftAnywhereCreateFleetResult GameLiftAnywhereHandler::CreateFleet(const FString& InFleetName, const FString& InCustomLocation)
{
	IAWSAccountInstance* AccountInstance = IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance();
	check(AccountInstance);
	check(AccountInstance->GetInstance());

	FString CurrentVersion = UnrealVersionUtils::GetCurrentEngineVersion();
	FString FleetDescriptionWithVersion = FString::Format(TEXT("{0} Using Unreal Engine Version {1}"), { Anywhere::kAnywhereFleetDescription, CurrentVersion });

	std::string StdFleetName = Convertors::FSToStdS(InFleetName);
	std::string StdCustomLocation = Convertors::FSToStdS(InCustomLocation);
	std::string StdFleetDescription = Convertors::FSToStdS(FleetDescriptionWithVersion);
	std::string StdTagKey = Convertors::FSToStdS(FString(Anywhere::kAnywhereFleetTagKey));
	std::string StdTagValue = Convertors::FSToStdS(FString(Anywhere::kAnywhereFleetTagValue));

	GAMELIFT_ANYWHERE_CREATE_FLEET_REQUEST Request;
	Request.fleetName = StdFleetName.c_str();
	Request.customLocation = StdCustomLocation.c_str();
	Request.fleetDescription = StdFleetDescription.c_str();
	Request.tagKey = StdTagKey.c_str();
	Request.tagValue = StdTagValue.c_str();

	FString ErrorMessageReceiver;
	Request.errorCb = GameLiftAnywhereHandlerInternal::ErrorCallback;
	Request.errorReceiver = &ErrorMessageReceiver;

	GAMELIFT_ANYWHERE_FLEET_INFO FleetInfo;
	GameLiftAnywhereCreateFleetResult Result;
	Result.bIsSuccessful = GameLiftAnywhereCreateFleet(AccountInstance->GetInstance(), &Request, &FleetInfo);
	Result.ErrorMessage = ErrorMessageReceiver;

	if (Result.bIsSuccessful)
	{
		FString FleetIdString = FString(FleetInfo.fleetId);

		UE_LOG(GameLiftCoreLog, Log, TEXT("Successfully created the Anywhere fleet: %s (id: %s)"), *InFleetName, *FleetIdString);
		Result.FleetId = FleetIdString;
	}
	else
	{
		UE_LOG(GameLiftCoreLog, Error, TEXT("Failed to create an Anywhere fleet '%s' - %s"), *InFleetName, *(Result.ErrorMessage));
	}

	return Result;
}

GameLiftAnywhereDescribeFleetsResult GameLiftAnywhereHandler::DescribeAnywhereFleets()
{
	IAWSAccountInstance* AccountInstance = IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance();
	check(AccountInstance);
	check(AccountInstance->GetInstance());

	GAMELIFT_ANYWHERE_DESCRIBE_FLEETS_REQUEST Request;

	FString ErrorMessageReceiver;
	Request.errorCb = GameLiftAnywhereHandlerInternal::ErrorCallback;
	Request.errorReceiver = &ErrorMessageReceiver;

	GAMELIFT_ANYWHERE_DESCRIBE_FLEETS_INFO DescribeFleetsInfo;
	GameLiftAnywhereDescribeFleetsResult Result;
	if (AccountInstance->GetInstance())
	{
		Result.bIsSuccessful = GameLiftAnywhereDescribeFleets(AccountInstance->GetInstance(), &Request, &DescribeFleetsInfo);
		Result.ErrorMessage = ErrorMessageReceiver;
	}
	else
	{
		Result.bIsSuccessful = false;
	}

	if (Result.bIsSuccessful)
	{
		// Fill rest of Result info
		for (int i = 0; i < DescribeFleetsInfo.numFleets; i++)
		{
			auto& fleetInfo = DescribeFleetsInfo.fleets[i];
			Result.Fleets.Add({ FString(fleetInfo.fleetName), FString(fleetInfo.fleetId) });
		}

		UE_LOG(GameLiftCoreLog, Log, TEXT("Successfully retrieved Anywhere fleet info"));
	}
	else
	{
		UE_LOG(GameLiftCoreLog, Error, TEXT("Failed to retrieve Anywhere fleet ifno - %s"), *(Result.ErrorMessage));
	}

	return Result;
}

GameLiftAnywhereRegisterComputeResult GameLiftAnywhereHandler::RegisterCompute(const FString& InComputeName, const FString& InIPAddress, const FString& InFleetId, const FString& InCustomLocation)
{
	IAWSAccountInstance* AccountInstance = IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance();
	check(AccountInstance);
	check(AccountInstance->GetInstance());

	std::string StdComputeName = Convertors::FSToStdS(InComputeName).c_str();
	std::string StdFleetId = Convertors::FSToStdS(InFleetId).c_str();
	std::string StdIPAddress = Convertors::FSToStdS(InIPAddress).c_str();
	std::string StdCustomLocation = Convertors::FSToStdS(InCustomLocation).c_str();

	GAMELIFT_ANYWHERE_REGISTER_COMPUTE_REQUEST Request;
	Request.computeName = StdComputeName.c_str();
	Request.fleetId = StdFleetId.c_str();
	Request.ipAddress = StdIPAddress.c_str();
	Request.customLocation = StdCustomLocation.c_str();

	FString ErrorMessageReceiver;
	Request.errorCb = GameLiftAnywhereHandlerInternal::ErrorCallback;
	Request.errorReceiver = &ErrorMessageReceiver;

	GameLiftAnywhereRegisterComputeResult Result;
	Result.bIsSuccessful = GameLiftAnywhereRegisterCompute(AccountInstance->GetInstance(), &Request);
	Result.ErrorMessage = ErrorMessageReceiver;

	if (Result.bIsSuccessful)
	{
		UE_LOG(GameLiftCoreLog, Log, TEXT("Successfully registered the Anywhere compute: %s (%s) for fleet '%s'"), *InComputeName, *InIPAddress, *InFleetId);

		Result.ComputeName = InComputeName;
		Result.LinkedFleetId = InFleetId;
		Result.RegisteredIPAddress = InIPAddress;
	}
	else
	{
		UE_LOG(GameLiftCoreLog, Error, TEXT("Failed to register an Anywhere compute '%s' for fleet '%s' - %s"),
			*InComputeName, *InFleetId, *(Result.ErrorMessage));
	}

	return Result;
}

GameLiftAnywhereAuthTokenResult GameLiftAnywhereHandler::GenerateAuthToken(const FString& InFleetId, const FString& InComputeName)
{
	IAWSAccountInstance* AccountInstance = IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance();
	check(AccountInstance);
	check(AccountInstance->GetInstance());

	std::string StdFleetId = Convertors::FSToStdS(InFleetId).c_str();
	std::string StdComputeName = Convertors::FSToStdS(InComputeName).c_str();

	GAMELIFT_ANYWHERE_GENERATE_AUTH_TOKEN_REQUEST Request;
	Request.computeName = StdComputeName.c_str();
	Request.fleetId = StdFleetId.c_str();

	FString ErrorMessageReceiver;
	Request.errorCb = GameLiftAnywhereHandlerInternal::ErrorCallback;
	Request.errorReceiver = &ErrorMessageReceiver;

	GAMELIFT_ANYWHERE_AUTH_TOKEN_INFO AuthTokenInfo;
	GameLiftAnywhereAuthTokenResult Result;
	Result.bIsSuccessful = GameLiftAnywhereGenerateAuthToken(AccountInstance->GetInstance(), &Request, &AuthTokenInfo);
	Result.ErrorMessage = ErrorMessageReceiver;

	if (Result.bIsSuccessful)
	{
		FDateTime ExpirationTime = FDateTime::FromUnixTimestamp(AuthTokenInfo.milisecondsSinceEpoch / 1000);
		FString LocalExpirationTimeString = FText::AsDateTime(ExpirationTime).ToString();
		FString AuthTokenString = FString(AuthTokenInfo.authToken);

		UE_LOG(GameLiftCoreLog, Log, TEXT("Successfully generate an auth token: %s (valid until %s)"), *AuthTokenString, *LocalExpirationTimeString);

		Result.AuthToken = AuthTokenString;
		Result.ExpirationTime = ExpirationTime;
	}
	else
	{
		UE_LOG(GameLiftCoreLog, Error, TEXT("Failed to generate an auth token for fleet '%s' and compute '%s' - %s"),
			*InFleetId, *InComputeName, *(Result.ErrorMessage));
	}

	return Result;
}

#undef LOCTEXT_NAMESPACE
