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

#include "GameLiftMetrics.h"
#include "GameLiftMetricsTypes.h"
#include "GameLiftMetricsConfig.h"
#include "UnrealStatCollector.h"
#include "TickableCollector.h"

#include "GameLiftServerSDK.h"
#include "aws/gamelift/server/GameLiftServerAPI.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Common/UdpSocketBuilder.h"

#include <aws/gamelift/metrics/GlobalMetricsProcessor.h>

#if PLATFORM_LINUX
#include <unistd.h>
#endif

FString FGameLiftMetricsModule::CurrentGameSessionId;
FThreadSafeCounter FGameLiftMetricsModule::CurrentPlayerCount(0);

FGameLiftMetricsModule& FGameLiftMetricsModule::Load()
{
    return FModuleManager::LoadModuleChecked<FGameLiftMetricsModule>(FName("GameLiftMetrics"));
}

FGameLiftMetricsModule& FGameLiftMetricsModule::Get()
{
    return FModuleManager::GetModuleChecked<FGameLiftMetricsModule>(FName("GameLiftMetrics"));
}

FGameLiftMetricsModule* FGameLiftMetricsModule::GetPtr()
{
    return FModuleManager::GetModulePtr<FGameLiftMetricsModule>(FName("GameLiftMetrics"));
}

#if WITH_GAMELIFT_METRICS
namespace
{
    const TCHAR* GAMELIFT_METRICS_FLEET_ID = TEXT("GAMELIFT_SDK_FLEET_ID");
    const TCHAR* GAMELIFT_METRICS_PROCESS_ID = TEXT("GAMELIFT_SDK_PROCESS_ID");

    GAMELIFT_METRICS_DECLARE_GAUGE(MetricMaxPlayers, "server_max_players", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricMaxPlayers);

    GAMELIFT_METRICS_DECLARE_GAUGE(MetricPlayers, "server_players", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricPlayers);

    GAMELIFT_METRICS_DECLARE_COUNTER(MetricServerCrashes, "game_server_crashes", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DEFINE_COUNTER(MetricServerCrashes);
}

void FGameLiftMetricsModule::StartupModule() {}

void FGameLiftMetricsModule::ShutdownModule()
{
    // Terminate here just in case the user forgot.
    Terminate();
}

void FGameLiftMetricsModule::StartMetricsCollector()
{
    FPlatformMisc::SetCrashHandler(&FGameLiftMetricsModule::CrashHandler);

    bMetricsRunning = true;

    auto *Module = FGameLiftMetricsModule::GetPtr();
    if (!Module)
    {
        UE_LOG(LogGameLiftMetrics, Error, TEXT("GameLiftMetrics module no longer loaded."));
        return;
    }

    /*
     * Begin ticking the collector.
     */
    const bool bComputeFallbackGameThreadTime = !STATS;
    Module->Collector.Reset(new FTickableCollector(bComputeFallbackGameThreadTime));

    /*
     * Create stats collector.
     */
    if (STATS)
    {
        Module->UnrealStatCollector = TSharedPtr<FUnrealStatCollector>(new FUnrealStatCollector());
        Module->UnrealStatCollector->Subscribe();
    }

    UE_LOG(LogGameLiftMetrics, Log, TEXT("GameLift metrics initialized successfully."));
}

void FGameLiftMetricsModule::Initialize()
{
    if (GIsEditor) return;

    // Initialize GameLift Server SDK metrics
    auto& ServerSDKModule = FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>("GameLiftServerSDK");
    auto InitResult = ServerSDKModule.InitMetrics();
    if (!InitResult.IsSuccess())
    {
        UE_LOG(LogGameLiftMetrics, Error, TEXT("Failed to initialize GameLift metrics: %s"), *InitResult.GetError().m_errorMessage);
        return;
    }

    StartMetricsCollector();
}

void FGameLiftMetricsModule::Initialize(const FMetricsParameters& metricsParameters)
{
    if (GIsEditor) return;

    // Initialize GameLift Server SDK metrics with parameters
    auto& ServerSDKModule = FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>("GameLiftServerSDK");
    auto InitResult = ServerSDKModule.InitMetrics(metricsParameters);
    if (!InitResult.IsSuccess())
    {
        UE_LOG(LogGameLiftMetrics, Error, TEXT("Failed to initialize GameLift metrics with parameters: %s"), *InitResult.GetError().m_errorMessage);
        return;
    }

    StartMetricsCollector();
}

void FGameLiftMetricsModule::Terminate()
{
    if (GIsEditor) { return; }

    if (UnrealStatCollector)
    {
        UnrealStatCollector->Unsubscribe();
        UnrealStatCollector.Reset();
    }

    Collector.Reset();

    UE_LOG(LogGameLiftMetrics, Log, TEXT("GameLift metrics terminated successfully."));
}

void FGameLiftMetricsModule::OnStartGameSession(const Aws::GameLift::Server::Model::GameSession& Session)
{
    if (GIsEditor) { return; }

    // Store the session ID regardless of metrics being enabled
    CurrentGameSessionId = FString(Session.GetGameSessionId());
    CurrentPlayerCount.Set(0);

    const UGameLiftMetricsConfig& Config = UGameLiftMetricsConfig::Get();
    if (!Config.bEnableMetrics)
    {
        UE_LOG(LogGameLiftMetrics, Verbose, TEXT("Metrics disabled: Skipping OnStartGameSession metrics"));
        return;
    }

    GAMELIFT_METRICS_SET(MetricMaxPlayers, Session.GetMaximumPlayerSessionCount());
    GAMELIFT_METRICS_RESET(MetricPlayers);
}

void FGameLiftMetricsModule::OnAcceptPlayerSession()
{
    if (GIsEditor) { return; }

    // Always track player count even if metrics are disabled
    int32 NewPlayerCount = CurrentPlayerCount.Increment();

    const UGameLiftMetricsConfig& Config = UGameLiftMetricsConfig::Get();
    if (!Config.bEnableMetrics)
    {
        return;
    }

    GAMELIFT_METRICS_SET(MetricPlayers, NewPlayerCount);
}

void FGameLiftMetricsModule::OnRemovePlayerSession()
{
    if (GIsEditor) { return; }

    // Always track player count even if metrics are disabled
    int32 NewPlayerCount = CurrentPlayerCount.Decrement();
    if (NewPlayerCount < 0)
    {
        CurrentPlayerCount.Set(0);
        NewPlayerCount = 0;
    }

    const UGameLiftMetricsConfig& Config = UGameLiftMetricsConfig::Get();
    if (!Config.bEnableMetrics)
    {
        return;
    }

    GAMELIFT_METRICS_SET(MetricPlayers, NewPlayerCount);
}

void FGameLiftMetricsModule::ReEmitMetrics()
{
    if (GIsEditor) { return; }

    const UGameLiftMetricsConfig& Config = UGameLiftMetricsConfig::Get();
    if (!Config.bEnableMetrics)
    {
        return;
    }

    GAMELIFT_METRICS_SET(MetricPlayers, CurrentPlayerCount.GetValue());
}

void FGameLiftMetricsModule::CrashHandler(const FGenericCrashContext& GenericContext)
{
    UE_LOG(LogGameLiftMetrics, Log, TEXT("Server crashed, CrashHandler being called..."));

    const UGameLiftMetricsConfig& Config = UGameLiftMetricsConfig::Get();
    if (!Config.bEnableMetrics)
    {
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Metrics disabled: Skipping crash metrics"));
        return;
    }

    if (!CurrentGameSessionId.IsEmpty())
    {
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Incrementing crash metrics for session: %s"), *CurrentGameSessionId);
        GAMELIFT_METRICS_INCREMENT(MetricServerCrashes);
        GAMELIFT_METRICS_TAG_SET(MetricServerCrashes, "game_session_id", TCHAR_TO_UTF8(*CurrentGameSessionId));
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Crash metrics incremented"));

        // Force process the metrics
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Sending crash metrics..."));
        Aws::GameLift::Metrics::MetricsProcess();

        // Small sleep to allow the metrics to be sent
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Waiting for metrics to send..."));
        FPlatformProcess::Sleep(0.2f);

        UE_LOG(LogGameLiftMetrics, Log, TEXT("Crash metrics incremented and processed"));
    }
    else
    {
        UE_LOG(LogGameLiftMetrics, Warning, TEXT("Server crashed with no active game session"));
    }
}

bool FGameLiftMetricsModule::SetMetricsEnabled(bool bEnable)
{
    if (GIsEditor)
    {
        UE_LOG(LogGameLiftMetrics, Warning, TEXT("Cannot enable/disable metrics in editor - only applicable in game server"));
        return false;
    }

    // If the state is already what's requested, no change needed
    if (bEnable == bMetricsRunning)
    {
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Metrics already %s, no change needed"),
            bEnable ? TEXT("enabled") : TEXT("disabled"));
        return true;
    }
    // Get current configuration and check current state
    UGameLiftMetricsConfig* Config = const_cast<UGameLiftMetricsConfig*>(&UGameLiftMetricsConfig::Get());

    if (bEnable)
    {
        // Enable metrics
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Enabling GameLift metrics at runtime"));

        // Initialize with default parameters
        Initialize();

        // Flag is set inside Initialize method based on success
        return bMetricsRunning;
    }
    else
    {
        // Disable metrics
        UE_LOG(LogGameLiftMetrics, Log, TEXT("Disabling GameLift metrics at runtime"));

        // Terminate metrics system
        Terminate();
        Config->bEnableMetrics = false;
        bMetricsRunning = false;
        return true;
    }
}

bool FGameLiftMetricsModule::IsMetricsEnabled() const
{
    return bMetricsRunning;
}

#else

void FGameLiftMetricsModule::StartupModule() {}
void FGameLiftMetricsModule::ShutdownModule() {}
void FGameLiftMetricsModule::Initialize() {}
void FGameLiftMetricsModule::Initialize(const FMetricsParameters& metricsParameters) {}
void FGameLiftMetricsModule::Terminate() {}
void FGameLiftMetricsModule::OnStartGameSession(const Aws::GameLift::Server::Model::GameSession& Session) {}
void FGameLiftMetricsModule::OnAcceptPlayerSession() {}
void FGameLiftMetricsModule::OnRemovePlayerSession() {}
void FGameLiftMetricsModule::ReEmitMetrics() {}
bool FGameLiftMetricsModule::SetMetricsEnabled(bool bEnable) { return false; }
bool FGameLiftMetricsModule::IsMetricsEnabled() const { return false; }

#endif // WITH_GAMELIFT_METRICS

IMPLEMENT_MODULE(FGameLiftMetricsModule, GameLiftMetrics)
