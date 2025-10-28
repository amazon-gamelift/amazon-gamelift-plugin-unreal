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

#include "MemoryStats.h"

#include "Engine.h"
#include "EngineStats.h"

namespace Aws {
namespace GameLift {
namespace Metrics {
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricMemoryPhysicalTotal);
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricMemoryPhysicalAvailable);
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricMemoryPhysicalUsed);

    GAMELIFT_METRICS_DEFINE_GAUGE(MetricMemoryVirtualTotal);
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricMemoryVirtualAvailable);
    GAMELIFT_METRICS_DEFINE_GAUGE(MetricMemoryVirtualUsed);
}
}
}

void FMemoryStats::Collect()
{
    auto MemoryStats = FPlatformMemory::GetStats();

    GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricMemoryPhysicalTotal, MemoryStats.TotalPhysical);
    GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricMemoryPhysicalAvailable, MemoryStats.AvailablePhysical);
    GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricMemoryPhysicalUsed, MemoryStats.UsedPhysical);
    GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricMemoryVirtualTotal, MemoryStats.TotalVirtual);
    GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricMemoryVirtualAvailable, MemoryStats.AvailableVirtual);
    GAMELIFT_METRICS_SET(Aws::GameLift::Metrics::MetricMemoryVirtualUsed, MemoryStats.UsedVirtual);
}
