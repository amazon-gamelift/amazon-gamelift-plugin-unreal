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

#include "TickableCollector.h"
#include "TimeStats.h"
#include "GameLiftMetrics.h"

#include "Misc/App.h"

namespace Aws {
namespace GameLift {
namespace Metrics {
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricServerUp);
}
}
}

#if WITH_GAMELIFT_METRICS
void FTickableCollector::Tick(float)
{
    GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricServerUp, 1);

    NetworkStats.Collect();
    MemoryStats.Collect();

    const double DeltaTime = FApp::GetDeltaTime();
    GAMELIFT_METRICS_SET_SEC(Aws::GameLift::Metrics::MetricGameDeltaTime, DeltaTime);
    if (bComputeFallbackGameThreadTime)
    {
        const double EstimatedGameThreadTime = DeltaTime - FApp::GetIdleTime();
        GAMELIFT_METRICS_SET_SEC(Aws::GameLift::Metrics::MetricGameTickTime, EstimatedGameThreadTime);
    }

    // Re-emit specific metrics so that they don't go stale.
    // Grafana reports no data for the metric after 5 minutes of it being stale.
    FGameLiftMetricsModule::Get().ReEmitMetrics();

    Aws::GameLift::Metrics::MetricsProcess();
}
#else
void FTickableCollector::Tick(float) {}
#endif // WITH_GAMELIFT_METRICS


TStatId FTickableCollector::GetStatId() const
{
    return TStatId{};
}
