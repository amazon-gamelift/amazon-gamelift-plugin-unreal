# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

$ErrorActionPreference = "Stop"

$dependencyScript = "Scripts/setup_dependencies.ps1"
$serverSdkScript = "Scripts/setup_server_sdk.ps1"

function Run-Script {
    param (
        [string]$scriptPath
    )

    Write-Host "Executing: $scriptPath"
    try {
        & $scriptPath
        if ($LASTEXITCODE -ne 0) {
            throw "Script $scriptPath failed with exit code $LASTEXITCODE"
        }
        Write-Host "Success: $scriptPath"
    } catch {
        Write-Host "Error: $_"
        exit 1
    }
}

Run-Script $dependencyScript
Run-Script $serverSdkScript

$ScriptBasePath = Get-Location
$LightWeightPluginDirectory = Join-Path $ScriptBasePath "GameLiftServerSDK"
$StandaloneServerSDKModuleDirectory = Join-Path $ScriptBasePath "GameLiftPlugin\Source\GameLiftServer"

Write-Host "Copying the Server SDK Module into the Standalone Plugin..."
Copy-Item -Path "$LightWeightPluginDirectory\*" -Destination $StandaloneServerSDKModuleDirectory -Recurse -Force
Remove-Item -Path "$StandaloneServerSDKModuleDirectory\GameLiftServerSDK.uplugin" -Force

Write-Host "All setup scripts completed successfully!"
