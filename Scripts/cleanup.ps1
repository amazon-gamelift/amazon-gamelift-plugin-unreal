# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

$rootDir = Resolve-Path "$PSScriptRoot\.."

Remove-Item -Path "$rootDir\Out" -Recurse -Force -ErrorAction SilentlyContinue

Get-ChildItem -Path "$rootDir\Temp" -Directory | Where-Object { $_.Name -ne 'downloads' } | ForEach-Object {
    Remove-Item -Path $_.FullName -Recurse -Force -ErrorAction SilentlyContinue
}

Get-ChildItem -Path "$rootDir\Temp\downloads" | Where-Object { $_.Name -ne 'GameLift-Cpp-ServerSDK.zip' } | ForEach-Object {
    Remove-Item -Path $_.FullName -Recurse -Force -ErrorAction SilentlyContinue
}

Remove-Item -Path "$rootDir\GameLiftServerSDK\Source\GameLiftServerSDK\private\aws" -Recurse -Force -ErrorAction SilentlyContinue

Get-ChildItem -Path "$rootDir\GameLiftServerSDK\ThirdParty" | Where-Object { $_.Name -ne '.gitkeep' } | ForEach-Object {
    Remove-Item -Path $_.FullName -Recurse -Force -ErrorAction SilentlyContinue
}

Get-ChildItem -Path "$rootDir\GameLiftPlugin\Source\GameLiftServer\ThirdParty" | Where-Object { $_.Name -ne '.gitkeep' } | ForEach-Object {
    Remove-Item -Path $_.FullName -Recurse -Force -ErrorAction SilentlyContinue
}

Get-ChildItem -Path "$rootDir\GameLiftPlugin\Source\GameLiftServer" -Directory | Where-Object { $_.Name -ne 'ThirdParty' } | ForEach-Object {
    Remove-Item -Path $_.FullName -Recurse -Force -ErrorAction SilentlyContinue
}

Get-ChildItem -Path "$rootDir\GameLiftPlugin\Source\GameLiftServer" -File | ForEach-Object {
    Remove-Item -Path $_.FullName -Force -ErrorAction SilentlyContinue
}
