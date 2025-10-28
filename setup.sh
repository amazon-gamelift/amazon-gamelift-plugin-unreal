#!/bin/bash
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

set -euo pipefail

cleanup_script="Scripts/cleanup.sh"
dependency_script="Scripts/setup_dependencies.sh"
server_sdk_script="Scripts/setup_server_sdk.sh"

run_script() {
    local script_path="$1"
    echo "Executing: $script_path"
    
    if ! bash "$script_path"; then
        echo "Error: Script $script_path failed"
        exit 1
    fi
    
    echo "Success: $script_path"
}

run_script "$cleanup_script"
run_script "$dependency_script"
run_script "$server_sdk_script"

SCRIPT_BASE_PATH=$(pwd)
LIGHTWEIGHT_PLUGIN_DIR="$SCRIPT_BASE_PATH/GameLiftServerSDK"
STANDALONE_SERVER_SDK_MODULE_DIR="$SCRIPT_BASE_PATH/GameLiftPlugin/Source/GameLiftServer"

echo "Copying the Server SDK Module into the Standalone Plugin..."
cp -r "$LIGHTWEIGHT_PLUGIN_DIR"/* "$STANDALONE_SERVER_SDK_MODULE_DIR"
rm -rf "$STANDALONE_SERVER_SDK_MODULE_DIR/GameLiftServerSDK.uplugin"

echo "All setup scripts completed successfully!"
