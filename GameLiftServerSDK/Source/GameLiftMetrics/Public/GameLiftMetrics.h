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

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "GameLiftMetricsTypes.h"

#include <aws/gamelift/metrics/GameLiftMetrics.h>

// Forward declarations
struct FMetricsParameters;

class FSocket;
class FUnrealStatCollector;
class FTickableCollector;
class UGameLiftMetricsConfig;

namespace Aws {
namespace GameLift {
namespace Server {
namespace Model {
    class GameSession;
} // Model
} // Server
} // GameLift
} // Aws

class GAMELIFTMETRICS_API FGameLiftMetricsModule : public IModuleInterface
{
public:
    // IModuleInterface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual bool SupportsDynamicReloading() override { return false; }
    virtual bool SupportsAutomaticShutdown() override { return true; }
    // ~IModuleInterface

    static FGameLiftMetricsModule& Load();
    static FGameLiftMetricsModule& Get();
    static FGameLiftMetricsModule* GetPtr();

    /**
     * Initialize GameLift Metrics using default configuration. These
     * may be overriden by environment variables.
     */
    void Initialize();

    /**
     * Initialize GameLift Metrics with metrics parameters.
     *
     * @param metricsParameters Metrics parameters to use.
     */
    void Initialize(const FMetricsParameters& metricsParameters);

    /**
     * Terminate the GameLift Metrics SDK.
     */
    void Terminate();

    /**
     * Enable or disable metrics at runtime.
     * If metrics were previously disabled, this will initialize the metrics system.
     * If metrics were previously enabled, this will terminate the metrics system.
     *
     * @param bEnable Whether to enable or disable metrics
     * @return True if the operation was successful
     */
    bool SetMetricsEnabled(bool bEnable);

    /**
     * Check if metrics are currently enabled and running
     *
     * @return True if metrics are currently enabled and running
     */
    bool IsMetricsEnabled() const;

    /**
     * Records a new game session.
     * Call when starting a new session from the FProcessSettings::OnStartGameSession callback.
     */
    void OnStartGameSession(const Aws::GameLift::Server::Model::GameSession&);

    /**
     * Records a new player session.
     *
     * Call when accepting a player session.
     */
    void OnAcceptPlayerSession();

    /**
     * Records a removed player session.
     *
     * Call when removing a player session.
     */
    void OnRemovePlayerSession();

private:
    void StartMetricsCollector();
    
    TSharedPtr<FSocket> Socket;
    TUniquePtr<FTickableCollector> Collector;
    TSharedPtr<FUnrealStatCollector> UnrealStatCollector;
    static FString CurrentGameSessionId;
    static FThreadSafeCounter CurrentPlayerCount;
    bool bMetricsRunning = false;  // Tracks if metrics are currently running

    static void CrashHandler(const FGenericCrashContext& Context);

    void ReEmitMetrics();

    friend class FTickableCollector;
};
