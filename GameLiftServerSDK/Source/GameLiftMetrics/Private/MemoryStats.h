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

#include "GameLiftMetricsTypes.h"

namespace Aws {
namespace GameLift {
namespace Metrics {
    GAMELIFT_METRICS_DECLARE_GAUGE(MetricMemoryPhysicalTotal, "server_mem_physical_total", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DECLARE_GAUGE(MetricMemoryPhysicalAvailable, "server_mem_physical_available", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DECLARE_GAUGE(MetricMemoryPhysicalUsed, "server_mem_physical_used", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());

    GAMELIFT_METRICS_DECLARE_GAUGE(MetricMemoryVirtualTotal, "server_mem_virtual_total", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DECLARE_GAUGE(MetricMemoryVirtualAvailable, "server_mem_virtual_available", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DECLARE_GAUGE(MetricMemoryVirtualUsed, "server_mem_virtual_used", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
}
}
}

class FMemoryStats
{
public:
    void Collect();
};
