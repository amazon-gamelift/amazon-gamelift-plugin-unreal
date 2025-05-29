#!/bin/bash
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
SCRIPT_BASE_PATH=$(pwd)

rm -rf "$SCRIPT_BASE_PATH/Out"
find "$SCRIPT_BASE_PATH/Temp" -mindepth 1 -type d ! -name 'downloads' -exec rm -rf {} +
find "$SCRIPT_BASE_PATH/Temp/downloads" -mindepth 1 ! -name 'GameLift-Cpp-ServerSDK.zip' -exec rm -rf {} +
rm -rf "$SCRIPT_BASE_PATH/GameLiftServerSDK/Source/GameLiftServerSDK/private/aws"
find "$SCRIPT_BASE_PATH/GameLiftServerSDK/ThirdParty" -mindepth 1 ! -name '.gitkeep' -exec rm -rf {} +
find "$SCRIPT_BASE_PATH/GameLiftPlugin/Source/GameLiftServer/ThirdParty" -mindepth 1 ! -name '.gitkeep' -exec rm -rf {} +
find "$SCRIPT_BASE_PATH/GameLiftPlugin/Source/GameLiftServer" -mindepth 1 -type d ! -name 'ThirdParty' -exec rm -rf {} +
find "$SCRIPT_BASE_PATH/GameLiftPlugin/Source/GameLiftServer" -maxdepth 1 -type f -exec rm -f {} +
