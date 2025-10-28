#!/bin/bash
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

SCRIPT_BASE_PATH=$(pwd)

DEPENDENCY_NAMES=("spdlog" "rapidjson" "asio-asio" "websocketpp" "concurrentqueue")
DEPENDENCY_URLS=("https://github.com/gabime/spdlog/archive/refs/tags/v1.14.0.zip" 
                 "https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.zip"
                 "https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-20-0.zip"
                 "https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.zip"
                 "https://github.com/cameron314/concurrentqueue/archive/refs/tags/v1.0.4.zip")
DEPENDENCY_TARGET_FOLDERS=("spdlog" "rapidjson" "asio" "websocketpp" "concurrentqueue")
DEPENDENCY_ROOT_COPY_FROM=("spdlog-1.14.0" "rapidjson-1.1.0" "asio-asio-1-20-0/asio" "websocketpp-0.8.2" "concurrentqueue-1.0.4")

TEMP_DIR="$SCRIPT_BASE_PATH/Temp"
DOWNLOAD_DIR="$TEMP_DIR/downloads"
EXTRACT_DIR="$TEMP_DIR/extracted"
FINAL_DIR="$SCRIPT_BASE_PATH/GameLiftServerSDK/ThirdParty"

if [ ! -d "$FINAL_DIR" ]; then
    echo "ERROR: Target directory '$FINAL_DIR' does not exist!"
    exit 1
fi

mkdir -p "$DOWNLOAD_DIR"
mkdir -p "$EXTRACT_DIR"

function download_and_extract_dependency {
    DEP_NAME=$1
    DEP_URL=$2
    TARGET_FOLDER=$3
    DEP_ROOT_COPY_FROM=$4

    ZIP_PATH="$DOWNLOAD_DIR/$DEP_NAME.zip"
    EXTRACT_PATH="$EXTRACT_DIR/$DEP_NAME"
    TARGET_PATH="$FINAL_DIR/$TARGET_FOLDER"

    echo "Processing: $DEP_NAME ..."

    if [ ! -f "$ZIP_PATH" ]; then
        echo "Downloading $DEP_URL to $ZIP_PATH"
        curl -L "$DEP_URL" -o "$ZIP_PATH"
    else
        echo "Zip file already exists, skipping download."
    fi

    if [ -d "$EXTRACT_PATH" ]; then
        echo "Removing old extraction folder: $EXTRACT_PATH"
        rm -rf "$EXTRACT_PATH"
    fi

    echo "Extracting $ZIP_PATH to $EXTRACT_PATH"
    unzip -q "$ZIP_PATH" -d "$EXTRACT_PATH"

    mkdir -p "$TARGET_PATH"

    DEP_ROOT_PATH="$EXTRACT_PATH/$DEP_ROOT_COPY_FROM"
    if [ -d "$DEP_ROOT_PATH" ]; then
        echo "Copying contents from $DEP_ROOT_PATH to $TARGET_PATH"
        cp -r "$DEP_ROOT_PATH"/* "$TARGET_PATH"
    else
        echo "ERROR: Dependency root path $DEP_ROOT_PATH does not exist!"
        exit 1
    fi

    echo "Processing of $DEP_NAME completed."
}

for i in "${!DEPENDENCY_NAMES[@]}"; do
    DEP_NAME="${DEPENDENCY_NAMES[$i]}"
    DEP_URL="${DEPENDENCY_URLS[$i]}"
    TARGET_FOLDER="${DEPENDENCY_TARGET_FOLDERS[$i]}"
    DEP_ROOT_COPY_FROM="${DEPENDENCY_ROOT_COPY_FROM[$i]}"

    download_and_extract_dependency "$DEP_NAME" "$DEP_URL" "$TARGET_FOLDER" "$DEP_ROOT_COPY_FROM"
done

echo "All dependencies processed successfully."

if [ -d "$EXTRACT_DIR" ]; then
    echo "Cleaning up extracted files..."
    rm -rf "$EXTRACT_DIR"
    echo "Extracted files cleaned up successfully."
fi
