/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
using UnrealBuildTool;

public class GameLiftMetrics : ModuleRules
{
    public GameLiftMetrics(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;
        bUseRTTI = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameLiftServerSDK",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Core",
                "Engine",
                "Sockets",
                "Networking",
            }
        );
    }
}
