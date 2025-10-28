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

#include "NetworkStats.h"

#include "Engine/NetDriver.h"
#include "Engine.h"

namespace Aws {
namespace GameLift {
namespace Metrics {
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricConnectionCount);

    GAMELIFT_METRICS_DEFINE_COUNTER(MetricBytesIn);
    GAMELIFT_METRICS_DEFINE_COUNTER(MetricBytesOut);
    GAMELIFT_METRICS_DEFINE_COUNTER(MetricPacketsIn);
    GAMELIFT_METRICS_DEFINE_COUNTER(MetricPacketsInLost);
    GAMELIFT_METRICS_DEFINE_COUNTER(MetricPacketsOut);
    GAMELIFT_METRICS_DEFINE_COUNTER(MetricPacketsOutLost);
}
}
}

FNetworkStats::FNetworkStats()
{
    if (auto NetDriver = GetNetDriver())
    {
        LastInBytes = NetDriver->InTotalBytes;
        LastOutBytes = NetDriver->OutTotalBytes;
        LastInPackets = NetDriver->InTotalPackets;
        LastOutPackets = NetDriver->OutTotalPackets;
        LastInPacketsLost = NetDriver->InTotalPacketsLost;
        LastOutPacketsLost = NetDriver->OutTotalPacketsLost;
    }
}

void FNetworkStats::Collect()
{
    if (auto NetDriver = GetNetDriver())
    {
        GAMELIFT_METRICS_ADD(Aws::GameLift::Metrics::MetricBytesIn, NetDriver->InTotalBytes - LastInBytes);
        GAMELIFT_METRICS_ADD(Aws::GameLift::Metrics::MetricBytesOut, NetDriver->OutTotalBytes - LastOutBytes);
        GAMELIFT_METRICS_ADD(Aws::GameLift::Metrics::MetricPacketsIn, NetDriver->InTotalPackets - LastInPackets);
        GAMELIFT_METRICS_ADD(Aws::GameLift::Metrics::MetricPacketsOut, NetDriver->OutTotalPackets - LastOutPackets);
        GAMELIFT_METRICS_ADD(Aws::GameLift::Metrics::MetricPacketsInLost, NetDriver->InTotalPacketsLost - LastInPacketsLost);
        GAMELIFT_METRICS_ADD(Aws::GameLift::Metrics::MetricPacketsOutLost, NetDriver->OutTotalPacketsLost - LastOutPacketsLost);

        LastInBytes = NetDriver->InTotalBytes;
        LastOutBytes = NetDriver->OutTotalBytes;
        LastInPackets = NetDriver->InTotalPackets;
        LastOutPackets = NetDriver->OutTotalPackets;
        LastInPacketsLost = NetDriver->InTotalPacketsLost;
        LastOutPacketsLost = NetDriver->OutTotalPacketsLost;

        GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricConnectionCount, NetDriver->ClientConnections.Num());
    }
    else
    {
        UE_LOG(LogGameLiftMetrics, Error, TEXT("Net driver is null. Cannot log network metrics."));
    }
}

const UNetDriver* FNetworkStats::GetNetDriver() const
{
    UEngine* Engine = GEngine;
    if (!IsValid(Engine))
    {
        UE_LOG(LogGameLiftMetrics, Error, TEXT("Engine pointer is not valid when retrieving net driver."));
        return nullptr;
    }

    const auto& WorldContexts = Engine->GetWorldContexts();
    if (WorldContexts.Num() == 0)
    {
        // no worlds are loaded
        // generally can only be true before everything is fully initialized
        UE_LOG(LogGameLiftMetrics, Error, TEXT("No world is loaded when retrieving net driver."));
        return nullptr;
    }

    // Only one world context should be present outside Editor.
    // (editor has multiple)
    // Refer to FWorldContext comment in Engine.h
    const FWorldContext& WorldContext = WorldContexts[0];
    check(WorldContext.WorldType == EWorldType::Game);

    // In theory multiple net drivers might be present.
    // One will be for gameplay but additional ones might be opened for non-gameplay traffic.
    // Might be null during initialisation.
    if (WorldContext.ActiveNetDrivers.Num() == 0)
    {
        UE_LOG(LogGameLiftMetrics, Error, TEXT("No active net drivers. Cannot acquire network metrics."));
        return nullptr;
    }

    return WorldContext.ActiveNetDrivers[0].NetDriver;
}
