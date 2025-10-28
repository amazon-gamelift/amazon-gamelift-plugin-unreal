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
#include "Tickable.h"

#include "GameLiftMetricsTypes.h"

#include "NetworkStats.h"
#include "MemoryStats.h"


namespace Aws {
namespace GameLift {
namespace Metrics {
    GAMELIFT_METRICS_DECLARE_GAUGE(MetricServerUp, "server_up", Aws::GameLift::Metrics::Server, Aws::GameLift::Metrics::SampleAll());
}
}
}

class FTickableCollector : public FTickableGameObject
{
public:
    FTickableCollector(bool bComputeFallbackGameThreadTime)
        : bComputeFallbackGameThreadTime(bComputeFallbackGameThreadTime) {}

    // FTickableGameObject
    virtual void Tick(float) override;

    virtual bool IsTickable() const override { return true; }

    virtual bool IsTickableWhenPaused() const override { return true; }
    virtual bool IsTickableInEditor() const override { return false; }
    virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
    virtual TStatId GetStatId() const override;
    // ~FTickableGameObject

private:
    bool bComputeFallbackGameThreadTime = false;
    FNetworkStats NetworkStats;
    FMemoryStats MemoryStats;
};
