// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "UI/TestCloudDeploymentMenuWidget.h"

#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

#if WITH_GAMELIFT_CLIENT
#include "aws/gamelift/authentication/exports.h"
#include "aws/gamelift/core/errors.h"
#endif

DEFINE_LOG_CATEGORY(TestCloudDeployment);

static FString sGlobalLoggerError;

UTestCloudDeploymentMenuWidget::UTestCloudDeploymentMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Http = &FHttpModule::Get();

	ApiGatewayEndpoint = FString::Printf(TEXT("https://%s.execute-api.%s.amazonaws.com/%s"), TEXT("apigatewayid"), TEXT("region"), TEXT("stage"));
	GetGameConnectionURI = TEXT("/get_game_connection");
	GetGameConnectionRetryDelayMs = 3000; // wait for 3 sec before the next attempt to get game connection info when matchmaking is in porgress
	MatchmakingTimeoutInSecondsParameter = 60; // must match to CFN parameter with the same name
	MatchmakingIsTimedOut = false;
	StartSessionURI = TEXT("/start_game");
	SignupUrl = TEXT("");
	CallbackUrl = TEXT("https://aws.amazon.com/");
	ConfigFilePath = TEXT("CloudFormation/awsGameLiftClientConfig.yml");

	MatchmakingInProgress = false;
}

void UTestCloudDeploymentMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UTestCloudDeploymentMenuWidget::OnLoginClicked()
{
	if (!MatchmakingInProgress)
	{

		int result = AuthAndGetToken();

		if (result != 0)
		{
			LatestError = TEXT("Authorization failed");
			UE_LOG(TestCloudDeployment, Error, TEXT("%s for user '%s'"), *LatestError, *Username);
			return false;
		}

		EstablishGameConnection();
	}
	return true;
}

int UTestCloudDeploymentMenuWidget::OnSignupClicked()
{
#if WITH_GAMELIFT_CLIENT
	auto SessionLogger = [](unsigned int InLevel, const char* InMessage, int InSize)
	{
		if (InLevel == GameLift::Logger::Level::Error)
		{
			sGlobalLoggerError = InMessage;
		}
		UE_LOG(TestCloudDeployment, Log, TEXT("Session manager logger: %s"), UTF8_TO_TCHAR(InMessage));
	};

	auto configFileFullPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectContentDir(), ConfigFilePath));
	auto sessionManagerHandle = GameLiftSessionManagerInstanceCreate(StringCast<ANSICHAR>(*configFileFullPath).Get(), SessionLogger);

	auto cb = [](DISPATCH_RECEIVER_HANDLE dispatchReceiver, const char* charPtr)
	{
		*(static_cast<FString*>(dispatchReceiver)) = charPtr;
	};

	const std::string& _Username = StringCast<ANSICHAR>(*Username).Get();
	const std::string& _Password = StringCast<ANSICHAR>(*Password).Get();

	auto result = GameLiftSignUpByUsernamePassword(sessionManagerHandle, _Username.c_str(), _Password.c_str());

	if (result != 0)
	{
		LatestError = TEXT("SignUp failed");
		UE_LOG(TestCloudDeployment, Error, TEXT("%s for user '%s'"), *LatestError, *Username);
	}

	GameLiftSessionManagerInstanceRelease(sessionManagerHandle);

	return result;

#else
	return 0;
#endif
}

int UTestCloudDeploymentMenuWidget::OnConfirmSignupClicked()
{
#if WITH_GAMELIFT_CLIENT
	auto SessionLogger = [](unsigned int InLevel, const char* InMessage, int InSize)
	{
		if (InLevel == GameLift::Logger::Level::Error)
		{
			sGlobalLoggerError = InMessage;
		}
		UE_LOG(TestCloudDeployment, Log, TEXT("Session manager logger: %s"), UTF8_TO_TCHAR(InMessage));
	};

	auto ConfigFileFullPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectContentDir(), ConfigFilePath));
	if (!FPaths::FileExists(ConfigFileFullPath))
	{
		LatestError = FString::Printf(TEXT("A config file is not found, make sure to setup it at: %s"), *ConfigFileFullPath);
		return GameLift::GAMELIFT_ERROR_GENERAL;
	}

	auto sessionManagerHandle = GameLiftSessionManagerInstanceCreate(StringCast<ANSICHAR>(*ConfigFileFullPath).Get(), SessionLogger);

	auto cb = [](DISPATCH_RECEIVER_HANDLE dispatchReceiver, const char* charPtr)
	{
		*(static_cast<FString*>(dispatchReceiver)) = charPtr;
	};

	const std::string& _Username = StringCast<ANSICHAR>(*Username).Get();
	const std::string& _VerificationCode = StringCast<ANSICHAR>(*VerificationCode).Get();

	auto result = GameLiftConfirmSignUpByUsernameCode(sessionManagerHandle, _Username.c_str(), _VerificationCode.c_str());

	if (result != 0)
	{
		LatestError = TEXT("Confirm SignUp failed");
		UE_LOG(TestCloudDeployment, Error, TEXT("%s for user '%s'"), *LatestError, *Username);
	}

	GameLiftSessionManagerInstanceRelease(sessionManagerHandle);

	return result;

#else	
	return 0;
#endif
}

FString UTestCloudDeploymentMenuWidget::GetLatestError()
{
	if (!LatestError.IsEmpty())
	{
		return FString::Printf(TEXT("Error: %s\n%s"), *LatestError, *sGlobalLoggerError);
	}

	return FString();
}

int UTestCloudDeploymentMenuWidget::AuthAndGetToken()
{
#if WITH_GAMELIFT_CLIENT
	auto sessionLogger = [](unsigned int InLevel, const char* InMessage, int InSize)
	{
		if (InLevel == GameLift::Logger::Level::Error)
		{
			sGlobalLoggerError = InMessage;
		}
		UE_LOG(TestCloudDeployment, Log, TEXT("Session manager logger: %s"), UTF8_TO_TCHAR(InMessage));
	};

	auto ConfigFileFullPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectContentDir(), ConfigFilePath));

	if (!FPaths::FileExists(ConfigFileFullPath))
	{
		LatestError = FString::Printf(TEXT("A config file is not found, make sure to setup it at: %s"), *ConfigFileFullPath);
		return GameLift::GAMELIFT_ERROR_GENERAL;
	}

	auto SessionManagerHandle = GameLiftSessionManagerInstanceCreate(StringCast<ANSICHAR>(*ConfigFileFullPath).Get(), sessionLogger);

	auto CallbackFunc = [](DISPATCH_RECEIVER_HANDLE InDispatchReceiver, const char* InCharPtr)
	{
		*(static_cast<FString *>(InDispatchReceiver)) = InCharPtr;
	};

	const std::string& _Username = StringCast<ANSICHAR>(*Username).Get();
	const std::string& _Password = StringCast<ANSICHAR>(*Password).Get();

	auto result = GameLiftAuthByUsernamePassword(SessionManagerHandle, _Username.c_str(), _Password.c_str());

	if (result == GameLift::GAMELIFT_SUCCESS)
	{
		GameLiftGetTokenId(SessionManagerHandle, &IdToken, CallbackFunc);

		if (GameLiftSessionManagerAreSettingsLoaded(SessionManagerHandle, GameLift::FeatureType::Identity))
		{
			result = GameLiftGetIdentityApiGatewayURL(SessionManagerHandle, &ApiGatewayEndpoint, CallbackFunc);
		}
	}

	GameLiftSessionManagerInstanceRelease(SessionManagerHandle);

	if (result != GameLift::GAMELIFT_SUCCESS)
	{
		return result;
	}

	if (ApiGatewayEndpoint.IsEmpty() || IdToken.IsEmpty())
	{
		return GameLift::GAMELIFT_ERROR_NO_ID_TOKEN;
	}

	return result;

#else
	return 0;
#endif
}

bool UTestCloudDeploymentMenuWidget::OpenLevel(const FString& IpAddress, const FString& Port, const FString& Options)
{
#if WITH_GAMELIFT_CLIENT
	UE_LOG(TestCloudDeployment, Log, TEXT("OpenLevel: IpAddress '%s', Port '%s', Options '%s'"), *(IpAddress), *(Port), *(Options));

	if (!IpAddress.IsEmpty() && !Port.IsEmpty())
	{
		FString LevelName = IpAddress + ":" + Port;
		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false, Options);
		return true;
	}

	return false;
#else
	return true;
#endif
}

void UTestCloudDeploymentMenuWidget::EstablishGameConnection()
{
#if WITH_GAMELIFT_CLIENT

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GameConnectionRequest = Http->CreateRequest();

	GameConnectionRequest->SetVerb("POST");
	GameConnectionRequest->SetURL(ApiGatewayEndpoint + GetGameConnectionURI);
	GameConnectionRequest->SetHeader("Content-Type", "application/json");
	GameConnectionRequest->SetHeader("Auth", IdToken);
	GameConnectionRequest->OnProcessRequestComplete().BindUObject(this, &UTestCloudDeploymentMenuWidget::OnGetGameConnectionResponse);
	GameConnectionRequest->ProcessRequest();

#endif
}

void UTestCloudDeploymentMenuWidget::OnGetGameConnectionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
#if WITH_GAMELIFT_CLIENT
	if (bWasSuccessful)
	{
		auto& WorldTimerManager = GetWorld()->GetTimerManager();

		auto PollReset = [this, &WorldTimerManager]()
		{
			WorldTimerManager.ClearTimer(PollGameConnectionHandle);
			WorldTimerManager.ClearTimer(PollGameConnectionEndHandle);
			MatchmakingInProgress = false;
			MatchmakingIsTimedOut = false;
		};

		auto ResponseCode = static_cast<ServerHttpStatusCode>(Response->GetResponseCode());

		if (ResponseCode == ServerHttpStatusCode::GetGameConnection_Ready)
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				FString IpAddress = JsonObject->GetStringField(TEXT("IpAddress"));
				FString Port = JsonObject->GetStringField(TEXT("Port"));
				const FString& PlayerSessionId = JsonObject->GetStringField(TEXT("PlayerSessionId"));
				const FString& PlayerId = JsonObject->GetStringField(TEXT("PlayerId"));
				const FString& Options = "?PlayerSessionId=" + PlayerSessionId + "?PlayerId=" + PlayerId;
				const FString GameSessionArn = JsonObject->GetStringField(TEXT("GameSessionArn"));

				UE_LOG(TestCloudDeployment, Log, TEXT("Game connection: GameSessionArn '%s', PlayerSessionId '%s', PlayerId '%s'"), *GameSessionArn, *PlayerSessionId, *PlayerId);

				if (MatchmakingInProgress)
				{
					PollReset();
				}

				OpenLevel(IpAddress, Port, Options);
			}
		}
		else if (ResponseCode == ServerHttpStatusCode::GetGameConnection_NotFound)
		{
			UE_LOG(TestCloudDeployment, Log, TEXT("No game is found, starting new game..."));
			StartGame(IdToken);
		}
		else if (ResponseCode == ServerHttpStatusCode::GetGameConnection_MatchmakingInProgress)
		{
			UE_LOG(TestCloudDeployment, Log, TEXT("Wating for the other players to join the game..."));

			if (!MatchmakingInProgress)
			{
				MatchmakingInProgress = true;
				MatchmakingIsTimedOut = false;
				WorldTimerManager.SetTimer(PollGameConnectionHandle, this, &UTestCloudDeploymentMenuWidget::PollGameConnection, GetGameConnectionRetryDelayMs / 1000.0f, true);
				WorldTimerManager.SetTimer(PollGameConnectionEndHandle, this, &UTestCloudDeploymentMenuWidget::PollGameConnectionEnd, (float)MatchmakingTimeoutInSecondsParameter, false);
			}
			else
			{
				if (MatchmakingIsTimedOut)
				{
					PollReset();
					LatestError = TEXT("Game connection is timed out. Try to connect again later");
					UE_LOG(TestCloudDeployment, Error, TEXT("%s"), *LatestError);
				}
			}
		}
		else if (ResponseCode == ServerHttpStatusCode::GetGameConnection_NoServerError)
		{
			LatestError = TEXT("Server is not deployed, please use another deployment scenario");
			UE_LOG(TestCloudDeployment, Error, TEXT("%s. Error code: %d"), *LatestError, ResponseCode);
		} 
		else
		{
			LatestError = TEXT("Failed to get game connection");
			UE_LOG(TestCloudDeployment, Error, TEXT("%s. Error code: %d"), *LatestError, ResponseCode);

			if (MatchmakingInProgress)
			{
				PollReset();
			}
		}
	}
#endif
}

void UTestCloudDeploymentMenuWidget::PollGameConnection()
{
#if WITH_GAMELIFT_CLIENT
	EstablishGameConnection();
#endif
}

void UTestCloudDeploymentMenuWidget::PollGameConnectionEnd()
{
#if WITH_GAMELIFT_CLIENT
	MatchmakingIsTimedOut = true;
	UE_LOG(TestCloudDeployment, Log, TEXT("Game connection timed out is reached"));
#endif
}

void UTestCloudDeploymentMenuWidget::StartGame(FString InIdToken)
{
#if WITH_GAMELIFT_CLIENT
	FString UsedToken = ( IdToken.IsEmpty() ? InIdToken : IdToken );

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> StartSessionHttpRequest = Http->CreateRequest();

	StartSessionHttpRequest->SetVerb("POST");
	StartSessionHttpRequest->SetURL(ApiGatewayEndpoint + StartSessionURI);
	StartSessionHttpRequest->SetHeader("Content-Type", "application/json");
	StartSessionHttpRequest->SetHeader("Auth", UsedToken);
	StartSessionHttpRequest->OnProcessRequestComplete().BindUObject(this, &UTestCloudDeploymentMenuWidget::OnStartGameResponse);
	StartSessionHttpRequest->ProcessRequest();
#endif
}

void UTestCloudDeploymentMenuWidget::OnStartGameResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
#if WITH_GAMELIFT_CLIENT
	if (bWasSuccessful)
	{
		auto responseCode = static_cast<ServerHttpStatusCode>(Response->GetResponseCode());

		if (responseCode == ServerHttpStatusCode::StartGame_Accepted)
		{
			UE_LOG(TestCloudDeployment, Log, TEXT("Game was started successfully, establishing game connection..."));
			EstablishGameConnection();
		}
		else if (responseCode == ServerHttpStatusCode::StartGame_Conflict)
		{
			LatestError = TEXT("Another game request is being processed now");
			UE_LOG(TestCloudDeployment, Error, TEXT("%s"), *LatestError);
		}
		else
		{
			LatestError = TEXT("Starting game request failed");
			UE_LOG(TestCloudDeployment, Error, TEXT("%s with code '%d'"), *LatestError, responseCode);
		}
	}
#endif
}
