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
    /**
     * Server time-step in milliseconds.
     *
     * This is the time between consecutive ticks.
     *
     * When the server is under performing, this will exceed the target tick rate,
     * otherwise it should hover in its vicinity.
     *
     * To compute the tick rate, compute the reciprocal: (1000 / server_delta_time)
     */
    GAMELIFT_METRICS_DECLARE_TIMER(
        MetricGameDeltaTime, "server_delta_time",
        Aws::GameLift::Metrics::Server,
        Aws::GameLift::Metrics::SampleAll(),
        Aws::GameLift::Metrics::Percentiles(0.5, 0.9, 0.95)
    );

    /**
     * The time it takes to process a single tick.
     *
     * A subset of delta time.
     * server_tick_time <= server_delta_time at all times. Typically (much) smaller.
     *
     * This is the time it takes to actually process the tick. The remaining
     * time-step duration is spent waiting for the next tick.
     */
    GAMELIFT_METRICS_DECLARE_TIMER(
        MetricGameTickTime, "server_tick_time",
        Aws::GameLift::Metrics::Server,
        Aws::GameLift::Metrics::SampleAll(),
        Aws::GameLift::Metrics::Percentiles(0.5, 0.9, 0.95)
    );

    /**
     * The time it takes to process the world update.
     *
     * A subset of tick time.
     *
     * This is the time it takes to process the Actors and Blueprints in the
     * currently loaded Map. The remainder of server_tick_time is spent in
     * other engine subsystems like network replication or physics.
     *
     * Only available in Debug or Development builds, or Shipping builds with
     * USE_FORCE_STATS=1.
     */
    GAMELIFT_METRICS_DECLARE_TIMER(
        MetricWorldTickTime,
       "server_world_tick_time",
       Aws::GameLift::Metrics::Server,
       Aws::GameLift::Metrics::SampleAll(),
       Aws::GameLift::Metrics::Percentiles(0.5, 0.9, 0.95)
    );
}
}
}
