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

#include "GameLiftMetricsConfig.generated.h"

/**
 * GameLift Metrics module configuration.
 *
 * Can be configured in `DefaultGame.ini`:
 * ```
 * [/Script/GameLiftMetricsUnreal.GameLiftMetricsConfig]
 * CollectorHost = "127.0.0.1"
 * CollectorPort = 8125
 * MaxPacketSize = 2048
 * CaptureIntervalSeconds = 10
 * bEnableMetrics = true
 * ```
 */
UCLASS(config=Game, defaultconfig)
class GAMELIFTMETRICS_API UGameLiftMetricsConfig : public UObject
{
    GENERATED_BODY()

public:
    UGameLiftMetricsConfig() = default;

    /**
     * Returns the current configuration.
     */
    static const UGameLiftMetricsConfig& Get();

    /**
     * Whether metrics collection is enabled.
     * When disabled, no metrics will be collected or sent.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="GameLift Metrics")
    bool bEnableMetrics = true;

    /**
     * Host to send metrics to.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="GameLift Metrics")
    FString CollectorHost = TEXT("127.0.0.1");

    /**
     * UDP port number to send metrics to.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="GameLift Metrics")
    int32 CollectorPort = 8125;

    /**
     * Metrics capture interval before sending data to the collector.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="GameLift Metrics")
    float CaptureIntervalSeconds = 10.0f;

    /**
     * Maximum UDP packet size.
     *
     * Typical MTU for Internet packets is 1500 bytes.
     * We set the default to 1472, leaving room for headers.
     *
     * For use with collector on same machine or with AWS jumbo packets this value can be bumped up past 1500 bytes.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="GameLift Metrics")
    int32 MaxPacketSize = 1472;
};
