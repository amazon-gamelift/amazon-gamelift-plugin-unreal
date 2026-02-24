#!/bin/bash
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

# Apply compatibility fixes for newer compilers and Unreal Engine versions

SCRIPT_BASE_PATH=$(pwd)
THIRD_PARTY="$SCRIPT_BASE_PATH/GameLiftServerSDK/ThirdParty"

echo "Applying compatibility fixes..."

# Fix RapidJSON const assignment issue for Clang 20 (for UE 5.7+ Linux cross-compilation)
RAPIDJSON_DOCUMENT="$THIRD_PARTY/rapidjson/include/rapidjson/document.h"
if [ -f "$RAPIDJSON_DOCUMENT" ]; then
    echo "Fixing RapidJSON const issue..."
    sed -i.bak 's/const SizeType length;/SizeType length;/' "$RAPIDJSON_DOCUMENT"
    rm -f "$RAPIDJSON_DOCUMENT.bak"
    echo "RapidJSON fix applied."
else
    echo "WARNING: $RAPIDJSON_DOCUMENT not found, skipping fix."
fi

# Fix spdlog FMT_STRING consteval issue for Clang 20 (for UE 5.7+ Linux cross-compilation)
# Upgrading to spdlog 1.17+ resolves this but we're using 1.14.0 for old engine compatibility
SPDLOG_COMMON="$THIRD_PARTY/spdlog/include/spdlog/common.h"
if [ -f "$SPDLOG_COMMON" ]; then
    echo "Fixing spdlog FMT_STRING issue..."
    sed -i.bak 's/#define SPDLOG_FMT_STRING(format_string) FMT_STRING(format_string)/#define SPDLOG_FMT_STRING(format_string) format_string/' "$SPDLOG_COMMON"
    rm -f "$SPDLOG_COMMON.bak"
    echo "spdlog fix applied."
else
    echo "WARNING: $SPDLOG_COMMON not found, skipping fix."
fi

# Fix fmt library FMT_STRING calls in format-inl.h for Clang 20 (for UE 5.7+ Linux cross-compilation)
FMT_FORMAT_INL="$THIRD_PARTY/spdlog/include/spdlog/fmt/bundled/format-inl.h"
if [ -f "$FMT_FORMAT_INL" ]; then
    echo "Fixing fmt format-inl.h FMT_STRING calls..."
    sed -i.bak \
        -e 's/FMT_STRING("{}{}")/"{}{}"/' \
        -e 's/FMT_STRING("{:x}")/"{:x}"/' \
        -e 's/FMT_STRING("{:08x}")/"{:08x}"/' \
        -e 's/FMT_STRING("p{}")/"p{}"/' \
        "$FMT_FORMAT_INL"
    rm -f "$FMT_FORMAT_INL.bak"
    echo "fmt format-inl.h fix applied."
else
    echo "WARNING: $FMT_FORMAT_INL not found, skipping fix."
fi

echo "Compatibility fixes applied successfully."
