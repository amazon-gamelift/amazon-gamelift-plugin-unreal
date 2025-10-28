#!/bin/bash
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

set -e

SDK_URL="https://github.com/amazon-gamelift/amazon-gamelift-servers-cpp-server-sdk/releases/download/v5.4.0/GameLift-Cpp-ServerSDK-5.4.0.zip"
CURRENT_DIR="$(pwd)"
TEMP_FOLDER="$CURRENT_DIR/Temp"
DOWNLOAD_FOLDER="$TEMP_FOLDER/downloads"
ZIP_FILE="$DOWNLOAD_FOLDER/GameLift-Cpp-ServerSDK.zip"
EXTRACT_PATH="$TEMP_FOLDER/GameLiftServerSDK"
SOURCE_INCLUDE="$EXTRACT_PATH/gamelift-server-sdk/include/"
SOURCE_SOURCE="$EXTRACT_PATH/gamelift-server-sdk/source/"
DESTINATION_INCLUDE="$CURRENT_DIR/GameLiftServerSDK/Source/GameLiftServerSDK/Public/"
DESTINATION_SOURCE="$CURRENT_DIR/GameLiftServerSDK/Source/GameLiftServerSDK/Private/"

if [[ ! -d "$DESTINATION_INCLUDE" ]]; then
    echo "Error: Target folder '$DESTINATION_INCLUDE' does not exist." >&2
    exit 1
fi

if [[ ! -d "$DESTINATION_SOURCE" ]]; then
    echo "Error: Target folder '$DESTINATION_SOURCE' does not exist." >&2
    exit 1
fi

mkdir -p "$DOWNLOAD_FOLDER"
mkdir -p "$TEMP_FOLDER"

if [[ ! -f "$ZIP_FILE" ]]; then
    echo "Downloading GameLift SDK..."
    curl -L -o "$ZIP_FILE" "$SDK_URL"
else
    echo "SDK zip file already exists in Temp/downloads folder, skipping download."
fi

if [[ -d "$EXTRACT_PATH" ]]; then
    rm -rf "$EXTRACT_PATH"
fi
mkdir -p "$EXTRACT_PATH"
unzip -q "$ZIP_FILE" -d "$EXTRACT_PATH"

echo "Copying include files..."
cp -r "$SOURCE_INCLUDE"/* "$DESTINATION_INCLUDE"

echo "Copying source files..."
cp -r "$SOURCE_SOURCE"/* "$DESTINATION_SOURCE"

echo "Cleaning up extracted files..."
rm -rf "$EXTRACT_PATH"

echo "Done."
exit 0
