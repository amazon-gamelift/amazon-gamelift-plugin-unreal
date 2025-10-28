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

#include <aws/gamelift/metrics/GameLiftMetrics.h>

DECLARE_LOG_CATEGORY_EXTERN(LogGameLiftMetrics, Log, All);

#define WITH_GAMELIFT_METRICS (WITH_GAMELIFT && UE_SERVER)

namespace Aws {
namespace GameLift {
namespace Metrics {
    /**
     * Logs in all server builds.
     */
    GAMELIFT_METRICS_DEFINE_PLATFORM_API(GAMELIFTMETRICS_API, Server, WITH_GAMELIFT_METRICS && UE_SERVER);

    /**
     * Logs in all development or debug server builds.
     */
    GAMELIFT_METRICS_DEFINE_PLATFORM_API(GAMELIFTMETRICS_API, ServerDevelopment, WITH_GAMELIFT_METRICS && UE_SERVER && (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG));

    /**
     * Logs only in debug builds.
     */
    GAMELIFT_METRICS_DEFINE_PLATFORM_API(GAMELIFTMETRICS_API, ServerDebug, WITH_GAMELIFT_METRICS && UE_SERVER && UE_BUILD_DEBUG);
} // Metrics
} // GameLift
} // Aws
