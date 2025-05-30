/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#include "GameLiftServerSDK.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include <cstdlib>

#define LOCTEXT_NAMESPACE "FGameLiftServerSDKModule"

void* FGameLiftServerSDKModule::GameLiftServerSDKLibraryHandle = nullptr;

static FProcessParameters GameLiftProcessParameters;

void FGameLiftServerSDKModule::StartupModule()
{
}

bool FGameLiftServerSDKModule::LoadDependency(const FString& Dir, const FString& Name, void*& Handle)
{
    FString Lib = Name + TEXT(".") + FPlatformProcess::GetModuleExtension();
    FString Path = Dir.IsEmpty() ? *Lib : FPaths::Combine(*Dir, *Lib);

    Handle = FPlatformProcess::GetDllHandle(*Path);

    if (Handle == nullptr)
    {
        return false;
    }

    return true;
}

void FGameLiftServerSDKModule::FreeDependency(void*& Handle)
{
#if !PLATFORM_LINUX
    if (Handle != nullptr)
    {
        FPlatformProcess::FreeDllHandle(Handle);
        Handle = nullptr;
    }
#endif
}

void FGameLiftServerSDKModule::ShutdownModule()
{
    FreeDependency(GameLiftServerSDKLibraryHandle);
}

FGameLiftStringOutcome FGameLiftServerSDKModule::GetSdkVersion() {
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::GetSdkVersion();
    if (outcome.IsSuccess()){
        return FGameLiftStringOutcome(outcome.GetResult());
    }
    else {
        return FGameLiftStringOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftStringOutcome("");
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::InitSDK() {
#if WITH_GAMELIFT
    auto initSDKOutcome = Aws::GameLift::Server::InitSDK();
    if (initSDKOutcome.IsSuccess()) {
        return FGameLiftGenericOutcome(nullptr);
    }
    else{
        return FGameLiftGenericOutcome(FGameLiftError(initSDKOutcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::InitSDK(const FServerParameters &serverParameters) {
#if WITH_GAMELIFT
    Aws::GameLift::Server::Model::ServerParameters sdkServerParameters;
    sdkServerParameters.SetWebSocketUrl(TCHAR_TO_UTF8(*serverParameters.m_webSocketUrl));
    sdkServerParameters.SetFleetId(TCHAR_TO_UTF8(*serverParameters.m_fleetId));
    sdkServerParameters.SetProcessId(TCHAR_TO_UTF8(*serverParameters.m_processId));
    sdkServerParameters.SetHostId(TCHAR_TO_UTF8(*serverParameters.m_hostId));
    sdkServerParameters.SetAuthToken(TCHAR_TO_UTF8(*serverParameters.m_authToken));
    sdkServerParameters.SetAwsRegion(TCHAR_TO_UTF8(*serverParameters.m_awsRegion));
    sdkServerParameters.SetAccessKey(TCHAR_TO_UTF8(*serverParameters.m_accessKey));
    sdkServerParameters.SetSecretKey(TCHAR_TO_UTF8(*serverParameters.m_secretKey));
    sdkServerParameters.SetSessionToken(TCHAR_TO_UTF8(*serverParameters.m_sessionToken));

    auto initSDKOutcome = Aws::GameLift::Server::InitSDK(sdkServerParameters);
    if (initSDKOutcome.IsSuccess()) {
        return FGameLiftGenericOutcome(nullptr);
    }
    else{
        return FGameLiftGenericOutcome(FGameLiftError(initSDKOutcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::ProcessEnding() {
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::ProcessEnding();
    if (outcome.IsSuccess()){
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::ActivateGameSession() {
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::ActivateGameSession();
    if (outcome.IsSuccess()){
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::AcceptPlayerSession(const FString& playerSessionId) {
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_UTF8(*playerSessionId));
    if (outcome.IsSuccess()){
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::RemovePlayerSession(const FString& playerSessionId) {
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_UTF8(*playerSessionId));
    if (outcome.IsSuccess()){
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::Destroy()
{
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::Destroy();
    if (outcome.IsSuccess()) {
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftDescribePlayerSessionsOutcome FGameLiftServerSDKModule::DescribePlayerSessions(const FGameLiftDescribePlayerSessionsRequest &describePlayerSessionsRequest)
{
#if WITH_GAMELIFT
    Aws::GameLift::Server::Model::DescribePlayerSessionsRequest request;
    request.SetGameSessionId(TCHAR_TO_UTF8(*describePlayerSessionsRequest.m_gameSessionId));
    request.SetPlayerId(TCHAR_TO_UTF8(*describePlayerSessionsRequest.m_playerId));
    request.SetPlayerSessionId(TCHAR_TO_UTF8(*describePlayerSessionsRequest.m_playerSessionId));
    request.SetPlayerSessionStatusFilter(TCHAR_TO_UTF8(*describePlayerSessionsRequest.m_playerSessionStatusFilter));
    request.SetLimit(describePlayerSessionsRequest.m_limit);
    request.SetNextToken(TCHAR_TO_UTF8(*describePlayerSessionsRequest.m_nextToken));

    auto outcome = Aws::GameLift::Server::DescribePlayerSessions(request);

    if (outcome.IsSuccess()) {
        auto& outres = outcome.GetResult();
        FGameLiftDescribePlayerSessionsResult result;
  
        int sessionCount = 0;
        auto sessions = outres.GetPlayerSessions(sessionCount);
        if (sessionCount > 0) {
            TArray<FGameLiftPlayerSession> outSessions;
            outSessions.Reserve(sessionCount);

            for (int i = 0; i < sessionCount; ++i) {
                auto session = sessions + i;
                FGameLiftPlayerSession& outSession = outSessions.AddDefaulted_GetRef();

                outSession.m_playerSessionId = UTF8_TO_TCHAR(session->GetPlayerSessionId());
                outSession.m_playerId = UTF8_TO_TCHAR(session->GetPlayerId());
                outSession.m_gameSessionId = UTF8_TO_TCHAR(session->GetGameSessionId());
                outSession.m_fleetId = UTF8_TO_TCHAR(session->GetFleetId());
                outSession.m_creationTime = session->GetCreationTime();
                outSession.m_terminationTime = session->GetTerminationTime();

                switch (session->GetStatus()) {
                    case Aws::GameLift::Server::Model::PlayerSessionStatus::NOT_SET: outSession.m_status = EPlayerSessionStatus::NOT_SET; break;
                    case Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED: outSession.m_status = EPlayerSessionStatus::RESERVED; break;
                    case Aws::GameLift::Server::Model::PlayerSessionStatus::ACTIVE: outSession.m_status = EPlayerSessionStatus::ACTIVE; break;
                    case Aws::GameLift::Server::Model::PlayerSessionStatus::COMPLETED: outSession.m_status = EPlayerSessionStatus::COMPLETED; break;
                    case Aws::GameLift::Server::Model::PlayerSessionStatus::TIMEDOUT: outSession.m_status = EPlayerSessionStatus::TIMEDOUT; break;
                }

                outSession.m_ipAddress = UTF8_TO_TCHAR(session->GetIpAddress());
                outSession.m_port = session->GetPort();

                outSession.m_playerData = UTF8_TO_TCHAR(session->GetPlayerData());
                outSession.m_dnsName = UTF8_TO_TCHAR(session->GetDnsName());
            }

            result.m_playerSessions = outSessions;
        }

        result.m_nextToken = (UTF8_TO_TCHAR(outres.GetNextToken()));

        return FGameLiftDescribePlayerSessionsOutcome(result);
    }
    else {
        return FGameLiftDescribePlayerSessionsOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftDescribePlayerSessionsOutcome(FGameLiftDescribePlayerSessionsResult());
#endif
}

static void OnActivateFunctionInternal(Aws::GameLift::Server::Model::GameSession gameSession, void* state) {
    GameLiftProcessParameters.OnActivateFunction(gameSession);
}

static void OnUpdateFunctionInternal(Aws::GameLift::Server::Model::UpdateGameSession updateGameSession, void* state) {
    GameLiftProcessParameters.OnUpdateFunction(updateGameSession);
}

static void OnTerminateFunctionInternal(void* state) {
    GameLiftProcessParameters.OnTerminateFunction();
}

static bool OnHealthCheckInternal(void* state) {
    return GameLiftProcessParameters.OnHealthCheckFunction();
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::ProcessReady(FProcessParameters &processParameters) {
#if WITH_GAMELIFT
    GameLiftProcessParameters = processParameters;

	char logPathsBuffer[MAX_LOG_PATHS][MAX_PATH_LENGTH];
	const char* logPaths[MAX_LOG_PATHS];

	memset(logPaths, 0, sizeof(logPaths));
	memset(logPathsBuffer, 0, sizeof(logPathsBuffer));

    //only use the first MAX_LOG_PATHS values (duplicate logic in cpp SDK)
	int32 numLogs = FMath::Min(processParameters.logParameters.Num(), MAX_LOG_PATHS);

	for (int i = 0; i < numLogs; i++)
	{
		FTCHARToUTF8 utf8text(*processParameters.logParameters[i]);
		if (utf8text.Length() < MAX_PATH_LENGTH)

		{
			memcpy(logPathsBuffer[i], utf8text.Get(), utf8text.Length());
		}

		logPaths[i] = logPathsBuffer[i];
	}

    const TSharedPtr<IPlugin> StandalonePlugin = IPluginManager::Get().FindPlugin(TEXT("GameLiftPlugin"));
    const TSharedPtr<IPlugin> LightweightPlugin = IPluginManager::Get().FindPlugin(TEXT("GameLiftServerSDK"));

    FString pluginName;
    FString pluginVersion;

    if (LightweightPlugin.IsValid())
    {
        pluginName = LightweightPlugin->GetName();
        pluginVersion = LightweightPlugin->GetDescriptor().VersionName;

    }
    else if (StandalonePlugin.IsValid())
    {
        pluginName = StandalonePlugin->GetName();
        pluginVersion = StandalonePlugin->GetDescriptor().VersionName;
    }
    else
    {
        return FGameLiftGenericOutcome(FGameLiftError(Aws::GameLift::GAMELIFT_ERROR_TYPE::SDK_VERSION_DETECTION_FAILED, "Unknown SDK Tool Name", "Couldn't find the GameLift plugin name or version. "
            "Please update this code to search for a valid name defined inside GameLift's .uplugin file.")
        );
    }

    // Don't use Unreal's FPlatformMisc::SetEnvironmentVar because it uses Windows specific SetEnvironmentVariable API 
    // which doesn't mix with GameLift SDK's use of C++ std::getenv()
    FString pluginNameEnv = "GAMELIFT_SDK_TOOL_NAME=Unreal" + pluginName;
    FString pluginVersionEnv = "GAMELIFT_SDK_TOOL_VERSION=" + pluginVersion;

    static std::string pluginNameEnvStr = TCHAR_TO_UTF8(*pluginNameEnv);
    static std::string pluginVersionEnvStr = TCHAR_TO_UTF8(*pluginVersionEnv);

#if PLATFORM_WINDOWS
    _putenv(pluginNameEnvStr.c_str());
    _putenv(pluginVersionEnvStr.c_str());
#else
    putenv(const_cast<char*>(pluginNameEnvStr.c_str()));
    putenv(const_cast<char*>(pluginVersionEnvStr.c_str()));
#endif

    Aws::GameLift::Server::ProcessParameters processParams = Aws::GameLift::Server::ProcessParameters(
        OnActivateFunctionInternal,
        nullptr,
        OnUpdateFunctionInternal,
        nullptr,
        OnTerminateFunctionInternal,
        nullptr,
        OnHealthCheckInternal,
        nullptr,
        processParameters.port,
        Aws::GameLift::Server::LogParameters(logPaths, numLogs)
        );

    auto outcome = Aws::GameLift::Server::ProcessReady(processParams);
    if (outcome.IsSuccess()){
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::UpdatePlayerSessionCreationPolicy(EPlayerSessionCreationPolicy policy)
{
#if WITH_GAMELIFT
    Aws::GameLift::Server::Model::PlayerSessionCreationPolicy internalPolicy = Aws::GameLift::Server::Model::PlayerSessionCreationPolicyMapper::GetPlayerSessionCreationPolicyForName(TCHAR_TO_UTF8(*GetNameForPlayerSessionCreationPolicy(policy)));
    auto outcome = Aws::GameLift::Server::UpdatePlayerSessionCreationPolicy(internalPolicy);
    if (outcome.IsSuccess()){
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftStringOutcome FGameLiftServerSDKModule::GetGameSessionId() {
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::GetGameSessionId();
    if (outcome.IsSuccess()){
        return FGameLiftStringOutcome(outcome.GetResult());
    }
    else {
        return FGameLiftStringOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftStringOutcome("");
#endif
}

FGameLiftLongOutcome FGameLiftServerSDKModule::GetTerminationTime() {
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::GetTerminationTime();
    if (outcome.IsSuccess()) {
        return FGameLiftLongOutcome(outcome.GetResult());
    }
    else {
        return FGameLiftLongOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftLongOutcome(-1);
#endif
}

FGameLiftStringOutcome FGameLiftServerSDKModule::StartMatchBackfill(const FStartMatchBackfillRequest& request) {
#if WITH_GAMELIFT
    Aws::GameLift::Server::Model::StartMatchBackfillRequest sdkRequest;
    sdkRequest.SetTicketId(TCHAR_TO_UTF8(*request.m_ticketId));
    sdkRequest.SetGameSessionArn(TCHAR_TO_UTF8(*request.m_gameSessionArn));
    sdkRequest.SetMatchmakingConfigurationArn(TCHAR_TO_UTF8(*request.m_matchmakingConfigurationArn));
    for (auto player : request.m_players) {
        Aws::GameLift::Server::Model::Player sdkPlayer;
        sdkPlayer.SetPlayerId(TCHAR_TO_UTF8(*player.m_playerId));
        sdkPlayer.SetTeam(TCHAR_TO_UTF8(*player.m_team));
        for (auto entry : player.m_latencyInMs) {
            sdkPlayer.WithLatencyMs(TCHAR_TO_UTF8(*entry.Key), entry.Value);
        }

        std::map<std::string, Aws::GameLift::Server::Model::AttributeValue> sdkAttributeMap;
        for (auto attributeEntry : player.m_playerAttributes) {
            FAttributeValue value = attributeEntry.Value;
            Aws::GameLift::Server::Model::AttributeValue attribute;
            switch (value.m_type)
            {
                case FAttributeType::STRING:
                    attribute = Aws::GameLift::Server::Model::AttributeValue(TCHAR_TO_UTF8(*value.m_S));
                break;
                case FAttributeType::DOUBLE:
                    attribute = Aws::GameLift::Server::Model::AttributeValue(value.m_N);
                break;
                case FAttributeType::STRING_LIST:
                    attribute = Aws::GameLift::Server::Model::AttributeValue::ConstructStringList();
                    for (auto sl : value.m_SL) {
                        attribute.AddString(TCHAR_TO_UTF8(*sl));
                    };
                break;
                case FAttributeType::STRING_DOUBLE_MAP:
                    attribute = Aws::GameLift::Server::Model::AttributeValue::ConstructStringDoubleMap();
                    for (auto sdm : value.m_SDM) {
                        attribute.AddStringAndDouble(TCHAR_TO_UTF8(*sdm.Key), sdm.Value);
                    };
                break;
            }
            sdkPlayer.WithPlayerAttribute((TCHAR_TO_UTF8(*attributeEntry.Key)), attribute);
        }
        sdkRequest.AddPlayer(sdkPlayer);
    }
    auto outcome = Aws::GameLift::Server::StartMatchBackfill(sdkRequest);
    if (outcome.IsSuccess()) {
        return FGameLiftStringOutcome(outcome.GetResult().GetTicketId());
    }
    else {
        return FGameLiftStringOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftStringOutcome("");
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::StopMatchBackfill(const FStopMatchBackfillRequest& request)
{
#if WITH_GAMELIFT
    Aws::GameLift::Server::Model::StopMatchBackfillRequest sdkRequest;
    sdkRequest.SetTicketId(TCHAR_TO_UTF8(*request.m_ticketId));
    sdkRequest.SetGameSessionArn(TCHAR_TO_UTF8(*request.m_gameSessionArn));
    sdkRequest.SetMatchmakingConfigurationArn(TCHAR_TO_UTF8(*request.m_matchmakingConfigurationArn));
    auto outcome = Aws::GameLift::Server::StopMatchBackfill(sdkRequest);
    if (outcome.IsSuccess()) {
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}


FGameLiftGetComputeCertificateOutcome FGameLiftServerSDKModule::GetComputeCertificate()
{
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::GetComputeCertificate();
    if (outcome.IsSuccess()) {
        auto& outres = outcome.GetResult();
        FGameLiftGetComputeCertificateResult result;
        result.m_certificate_path = UTF8_TO_TCHAR(outres.GetCertificatePath());
        result.m_computeName = UTF8_TO_TCHAR(outres.GetComputeName());
        return FGameLiftGetComputeCertificateOutcome(result);
    }
    else {
        return FGameLiftGetComputeCertificateOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGetComputeCertificateOutcome(FGameLiftGetComputeCertificateResult());
#endif
}

FGameLiftGetFleetRoleCredentialsOutcome FGameLiftServerSDKModule::GetFleetRoleCredentials(const FGameLiftGetFleetRoleCredentialsRequest &request)
{
#if WITH_GAMELIFT
    Aws::GameLift::Server::Model::GetFleetRoleCredentialsRequest sdkRequest;
    sdkRequest.SetRoleArn(TCHAR_TO_UTF8(*request.m_roleArn));
    sdkRequest.SetRoleSessionName(TCHAR_TO_UTF8(*request.m_roleSessionName));

    auto outcome = Aws::GameLift::Server::GetFleetRoleCredentials(sdkRequest);

    if (outcome.IsSuccess()) {
        auto& outres = outcome.GetResult();
        FGameLiftGetFleetRoleCredentialsResult result;
        result.m_assumedUserRoleArn = UTF8_TO_TCHAR(outres.GetAssumedUserRoleArn());
        result.m_assumedRoleId = UTF8_TO_TCHAR(outres.GetAssumedRoleId());
        result.m_accessKeyId = UTF8_TO_TCHAR(outres.GetAccessKeyId());
        result.m_secretAccessKey = UTF8_TO_TCHAR(outres.GetSecretAccessKey());
        result.m_sessionToken = UTF8_TO_TCHAR(outres.GetSessionToken());
        result.m_expiration = FDateTime::FromUnixTimestamp(outres.GetExpiration());
        return FGameLiftGetFleetRoleCredentialsOutcome(result);
    }
    else {
        return FGameLiftGetFleetRoleCredentialsOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftGetFleetRoleCredentialsOutcome(FGameLiftGetFleetRoleCredentialsResult());
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameLiftServerSDKModule, GameLiftServerSDK)
