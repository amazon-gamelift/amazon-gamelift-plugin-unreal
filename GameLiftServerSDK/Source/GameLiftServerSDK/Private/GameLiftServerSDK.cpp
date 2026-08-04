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

DEFINE_LOG_CATEGORY(LogGameLiftServerSDK);

#if WITH_GAMELIFT
/**
 * Maps an SDK log level to the corresponding Unreal ELogVerbosity value.
 * Used both for the early-out suppression check and for dispatching the
 * final UE_LOG call, keeping the two in sync.
 *
 * Returns ELogVerbosity::NumVerbosity for levels that should be discarded
 * outright (Off) — callers must check for this sentinel and return early.
 */
static ELogVerbosity::Type MapSdkLogLevelToUEVerbosity(Aws::GameLift::Server::LogLevel Level)
{
    switch (Level)
    {
    case Aws::GameLift::Server::LogLevel::Trace: return ELogVerbosity::VeryVerbose;
    case Aws::GameLift::Server::LogLevel::Debug: return ELogVerbosity::Verbose;
    case Aws::GameLift::Server::LogLevel::Info:  return ELogVerbosity::Log;
    case Aws::GameLift::Server::LogLevel::Warn:  return ELogVerbosity::Warning;
    case Aws::GameLift::Server::LogLevel::Error: return ELogVerbosity::Error;
    // Fatal maps to Error verbosity for suppression purposes — we never use
    // UE Fatal (which terminates the process).
    case Aws::GameLift::Server::LogLevel::Fatal: return ELogVerbosity::Error;
    case Aws::GameLift::Server::LogLevel::Off:   return ELogVerbosity::NumVerbosity;
    default:                                     return ELogVerbosity::Warning;
    }
}

/**
 * Callback function that routes C++ Server SDK log messages to UE_LOG.
 * This is passed to the SDK via CustomLoggerConfiguration during InitCustomLogger.
 *
 * Thread safety: The SDK serializes callback invocations via an internal mutex
 * (per SDK documentation). UE_LOG itself is thread-safe (FOutputDeviceRedirector).
 * This callback may be invoked from any SDK worker thread (WebSocket, ASIO, etc.).
 */
static void GameLiftUELogCallback(Aws::GameLift::Server::LogLevel Level, const char* Message, void* /*UserData*/)
{
    if (!Message) return;

    // Early-out: check UE's live runtime verbosity BEFORE doing any string work.
    // This keeps runtime -LogCmds / console toggling effective while avoiding the
    // cost of truncation, strlen, and UTF8_TO_TCHAR for messages that would be
    // suppressed anyway.
    const ELogVerbosity::Type MappedVerbosity = MapSdkLogLevelToUEVerbosity(Level);
    if (MappedVerbosity == ELogVerbosity::NumVerbosity || LogGameLiftServerSDK.IsSuppressed(MappedVerbosity))
    {
        return;
    }

    // Guard against excessively long messages that could overflow alloca-based
    // UTF8_TO_TCHAR conversion on worker threads with limited stack space.
    static constexpr int32 MaxLogMessageLength = 4096;
    char TruncatedBuffer[MaxLogMessageLength + 1];
    const char* SafeMessage = Message;

    const int32 MessageLen = FCStringAnsi::Strlen(Message);
    if (MessageLen > MaxLogMessageLength)
    {
        FMemory::Memcpy(TruncatedBuffer, Message, MaxLogMessageLength - 3);
        TruncatedBuffer[MaxLogMessageLength - 3] = '.';
        TruncatedBuffer[MaxLogMessageLength - 2] = '.';
        TruncatedBuffer[MaxLogMessageLength - 1] = '.';
        TruncatedBuffer[MaxLogMessageLength] = '\0';
        SafeMessage = TruncatedBuffer;
    }

    // Dispatch using the mapped verbosity. Fatal retains its [FATAL] prefix for
    // visibility even though it maps to Error verbosity.
    switch (Level)
    {
    case Aws::GameLift::Server::LogLevel::Fatal:
        UE_LOG(LogGameLiftServerSDK, Error, TEXT("[FATAL] %s"), UTF8_TO_TCHAR(SafeMessage));
        break;
    default:
        // Dispatch at the mapped verbosity. The outer switch exists only to give
        // Fatal its [FATAL] prefix; all other levels share this path.
        switch (MappedVerbosity)
        {
        case ELogVerbosity::VeryVerbose:
            UE_LOG(LogGameLiftServerSDK, VeryVerbose, TEXT("%s"), UTF8_TO_TCHAR(SafeMessage));
            break;
        case ELogVerbosity::Verbose:
            UE_LOG(LogGameLiftServerSDK, Verbose, TEXT("%s"), UTF8_TO_TCHAR(SafeMessage));
            break;
        case ELogVerbosity::Log:
            UE_LOG(LogGameLiftServerSDK, Log, TEXT("%s"), UTF8_TO_TCHAR(SafeMessage));
            break;
        case ELogVerbosity::Warning:
            UE_LOG(LogGameLiftServerSDK, Warning, TEXT("%s"), UTF8_TO_TCHAR(SafeMessage));
            break;
        case ELogVerbosity::Error:
            UE_LOG(LogGameLiftServerSDK, Error, TEXT("%s"), UTF8_TO_TCHAR(SafeMessage));
            break;
        default:
            UE_LOG(LogGameLiftServerSDK, Warning, TEXT("[UnknownLevel:%d] %s"), static_cast<int32>(Level), UTF8_TO_TCHAR(SafeMessage));
            break;
        }
        break;
    }
}
#endif

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
#if WITH_GAMELIFT
    if (bSdkInitialized)
    {
        auto destroyOutcome = Aws::GameLift::Server::Destroy();
        if (!destroyOutcome.IsSuccess())
        {
            UE_LOG(LogGameLiftServerSDK, Warning, TEXT("SDK Destroy() in ShutdownModule returned error (non-fatal): %s"), UTF8_TO_TCHAR(destroyOutcome.GetError().GetErrorMessage()));
        }
        bSdkInitialized = false;
    }
#endif
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

/**
 * Parses a log level string into the corresponding SDK LogLevel enum value.
 * Supported values: "Trace", "Debug", "Info", "Warn", "Error", "Fatal", "Off" (case-insensitive).
 * Returns the provided DefaultLevel for unrecognized strings.
 */
#if WITH_GAMELIFT
static Aws::GameLift::Server::LogLevel ParseLogLevel(const FString& LevelStr, Aws::GameLift::Server::LogLevel DefaultLevel)
{
    if (LevelStr.Equals(TEXT("Trace"), ESearchCase::IgnoreCase)) return Aws::GameLift::Server::LogLevel::Trace;
    if (LevelStr.Equals(TEXT("Debug"), ESearchCase::IgnoreCase)) return Aws::GameLift::Server::LogLevel::Debug;
    if (LevelStr.Equals(TEXT("Info"), ESearchCase::IgnoreCase))  return Aws::GameLift::Server::LogLevel::Info;
    if (LevelStr.Equals(TEXT("Warn"), ESearchCase::IgnoreCase))  return Aws::GameLift::Server::LogLevel::Warn;
    if (LevelStr.Equals(TEXT("Error"), ESearchCase::IgnoreCase)) return Aws::GameLift::Server::LogLevel::Error;
    if (LevelStr.Equals(TEXT("Fatal"), ESearchCase::IgnoreCase)) return Aws::GameLift::Server::LogLevel::Fatal;
    if (LevelStr.Equals(TEXT("Off"), ESearchCase::IgnoreCase))   return Aws::GameLift::Server::LogLevel::Off;
    if (!LevelStr.IsEmpty())
    {
        UE_LOG(LogGameLiftServerSDK, Warning, TEXT("Unrecognized MinLogLevel '%s', defaulting to Trace"), *LevelStr);
    }
    return DefaultLevel;
}
#endif

/**
 * Reads the LogDestination setting from DefaultGame.ini [/Script/GameLiftServerSDK.GameLiftLoggingConfig] section.
 * Returns CustomLoggerConfiguration that routes SDK logs through UE_LOG when LogDestination=UELog (default),
 * or empty parameters (SDK default file+stdout logging) when LogDestination=SDKFile.
 *
 * Configuration (DefaultGame.ini):
 *   [/Script/GameLiftServerSDK.GameLiftLoggingConfig]
 *   LogDestination=UELog
 *   MinLogLevel=Trace
 */
#if WITH_GAMELIFT
static Aws::GameLift::Server::CustomLoggerConfiguration GetConfiguredLogParameters()
{
    FString LogDestination = TEXT("UELog");
    FString MinLogLevelStr = TEXT("Trace");

    // Guard against GConfig being null (e.g., during very early startup or commandlet contexts).
    // Fall back to defaults (UELog destination, Trace minimum level) when unavailable.
    if (GConfig != nullptr)
    {
        GConfig->GetString(TEXT("/Script/GameLiftServerSDK.GameLiftLoggingConfig"), TEXT("LogDestination"), LogDestination, GGameIni);
        GConfig->GetString(TEXT("/Script/GameLiftServerSDK.GameLiftLoggingConfig"), TEXT("MinLogLevel"), MinLogLevelStr, GGameIni);
    }

    if (LogDestination.Equals(TEXT("UELog"), ESearchCase::IgnoreCase))
    {
        // Default to Trace: all SDK messages reach the callback so that UE's runtime
        // verbosity toggling (-LogCmds, console "Log LogGameLiftServerSDK <level>")
        // works as expected. Actual filtering happens in the callback via
        // IsSuppressed(). Setting MinLogLevel above Trace is an opt-in production
        // hard cap that prevents the SDK from even formatting suppressed messages,
        // at the cost of disabling runtime toggling above the cap.
        const Aws::GameLift::Server::LogLevel MinLevel = ParseLogLevel(MinLogLevelStr, Aws::GameLift::Server::LogLevel::Trace);
        return Aws::GameLift::Server::CustomLoggerConfiguration(GameLiftUELogCallback, nullptr, MinLevel);
    }

    // "SDKFile": empty parameters — SDK uses its own file+stdout logging.
    // Return default-constructed CustomLoggerConfiguration with null callback.
    return Aws::GameLift::Server::CustomLoggerConfiguration{};
}

/**
 * Attempt to initialize SDK logging before InitSDK() so that SDK initialization
 * diagnostics are routed to UE_LOG from the very start. Only calls InitCustomLogger when
 * a callback is configured (LogDestination=UELog); a null callback is rejected by
 * the SDK. Failures are warned but never abort — this is best-effort.
 */
static void TryInitCustomLogger()
{
    auto logParams = GetConfiguredLogParameters();
    if (logParams.callback != nullptr)
    {
        auto logOutcome = Aws::GameLift::Server::InitCustomLogger(logParams);
        if (!logOutcome.IsSuccess())
        {
            // ALREADY_INITIALIZED is non-fatal (idempotent); warn but continue.
            UE_LOG(LogGameLiftServerSDK, Warning, TEXT("InitCustomLogger returned error: %s"), UTF8_TO_TCHAR(logOutcome.GetError().GetErrorMessage()));
        }
    }
}
#endif

FGameLiftGenericOutcome FGameLiftServerSDKModule::InitSDK() {
#if WITH_GAMELIFT
    TryInitCustomLogger();

    auto initSDKOutcome = Aws::GameLift::Server::InitSDK();
    if (initSDKOutcome.IsSuccess()) {
        bSdkInitialized = true;
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

    // Call InitCustomLogger BEFORE InitSDK so that SDK initialization diagnostics are
    // routed to UE_LOG from the very start.
    TryInitCustomLogger();

    auto initSDKOutcome = Aws::GameLift::Server::InitSDK(sdkServerParameters);
    if (initSDKOutcome.IsSuccess()) {
        bSdkInitialized = true;
        return FGameLiftGenericOutcome(nullptr);
    }
    else{
        return FGameLiftGenericOutcome(FGameLiftError(initSDKOutcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::InitMetrics() {
#if WITH_GAMELIFT
    auto initMetricsOutcome = Aws::GameLift::Server::InitMetrics();
    if (initMetricsOutcome.IsSuccess()) {
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(initMetricsOutcome.GetError()));
    }
#else
    return FGameLiftGenericOutcome(nullptr);
#endif
}

FGameLiftGenericOutcome FGameLiftServerSDKModule::InitMetrics(const FMetricsParameters &metricsParameters) {
#if WITH_GAMELIFT
    Aws::GameLift::Server::MetricsParameters sdkMetricsParameters(
        TCHAR_TO_UTF8(*metricsParameters.m_statsDHost),
        metricsParameters.m_statsDPort,
        TCHAR_TO_UTF8(*metricsParameters.m_crashReporterHost),
        metricsParameters.m_crashReporterPort,
        metricsParameters.m_flushIntervalMs,
        metricsParameters.m_maxPacketSize
    );

    auto initMetricsOutcome = Aws::GameLift::Server::InitMetrics(sdkMetricsParameters);
    if (initMetricsOutcome.IsSuccess()) {
        return FGameLiftGenericOutcome(nullptr);
    }
    else {
        return FGameLiftGenericOutcome(FGameLiftError(initMetricsOutcome.GetError()));
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
        bSdkInitialized = false;
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

FGameLiftListContainersNetworkInfoOutcome FGameLiftServerSDKModule::ListContainersNetworkInfo()
{
#if WITH_GAMELIFT
    auto outcome = Aws::GameLift::Server::ListContainersNetworkInfo();
    if (outcome.IsSuccess()) {
        auto& outres = outcome.GetResult();
        FGameLiftListContainersNetworkInfoResult result;

        const int count = outres.GetContainersNetworkInfoCount();
        if (count > 0) {
            const auto* containersNetworkInfo = outres.GetContainersNetworkInfo();
            result.m_containersNetworkInfo.Reserve(count);

            for (int i = 0; i < count; ++i) {
                const auto& info = containersNetworkInfo[i];
                FContainerNetworkInfo& dst = result.m_containersNetworkInfo.AddDefaulted_GetRef();
                dst.m_containerName = UTF8_TO_TCHAR(info.GetContainerName());
                dst.m_containerId   = UTF8_TO_TCHAR(info.GetContainerId());
                dst.m_ipAddress     = UTF8_TO_TCHAR(info.GetIpAddress());
                switch (info.GetContainerGroupType()) {
                    case Aws::GameLift::Server::Model::ContainerGroupType::GAME_SERVER:
                        dst.m_containerGroupType = EContainerGroupType::GAME_SERVER;
                        break;
                    case Aws::GameLift::Server::Model::ContainerGroupType::PER_INSTANCE:
                        dst.m_containerGroupType = EContainerGroupType::PER_INSTANCE;
                        break;
                    default:
                        dst.m_containerGroupType = EContainerGroupType::GAME_SERVER;
                        break;
                }
            }
        }

        return FGameLiftListContainersNetworkInfoOutcome(result);
    }
    else {
        return FGameLiftListContainersNetworkInfoOutcome(FGameLiftError(outcome.GetError()));
    }
#else
    return FGameLiftListContainersNetworkInfoOutcome(FGameLiftListContainersNetworkInfoResult());
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameLiftServerSDKModule, GameLiftServerSDK)
