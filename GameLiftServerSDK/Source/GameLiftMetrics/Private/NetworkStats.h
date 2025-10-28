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

class UNetDriver;

namespace Aws {
namespace GameLift {
namespace Metrics {
    GAMELIFT_METRICS_DECLARE_GAUGE(MetricConnectionCount, "server_connections", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());

    GAMELIFT_METRICS_DECLARE_COUNTER(MetricBytesIn, "server_bytes_in", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DECLARE_COUNTER(MetricBytesOut, "server_bytes_out", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());

    GAMELIFT_METRICS_DECLARE_COUNTER(MetricPacketsIn, "server_packets_in", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DECLARE_COUNTER(MetricPacketsInLost, "server_packets_in_lost", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());

    GAMELIFT_METRICS_DECLARE_COUNTER(MetricPacketsOut, "server_packets_out", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
    GAMELIFT_METRICS_DECLARE_COUNTER(MetricPacketsOutLost, "server_packets_out_lost", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
}
}
}

class FNetworkStats
{
public:
    FNetworkStats();

    void Collect();

private:
    const UNetDriver* GetNetDriver() const;

private:
    int32 LastInBytes = 0;
    int32 LastOutBytes = 0;
    int32 LastInPackets = 0;
    int32 LastOutPackets = 0;
    int32 LastInPacketsLost = 0;
    int32 LastOutPacketsLost = 0;
};
