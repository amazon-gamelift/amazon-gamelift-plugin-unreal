# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

$sdkUrl = "https://github.com/amazon-gamelift/amazon-gamelift-servers-cpp-server-sdk/releases/download/v5.4.0/GameLift-Cpp-ServerSDK-5.4.0.zip"
$rootDir = Resolve-Path "$PSScriptRoot\.."
$tempFolder = Join-Path $rootDir "Temp"
$downloadFolder = Join-Path $tempFolder "downloads"
$zipFile = Join-Path $downloadFolder "GameLift-Cpp-ServerSDK.zip"
$extractPath = Join-Path $tempFolder "GameLiftServerSDK"
$sourceInclude = Join-Path $extractPath "gamelift-server-sdk\include\"
$sourceSource = Join-Path $extractPath "gamelift-server-sdk\source\"
$destinationInclude = Join-Path $rootDir "GameLiftServerSDK\Source\GameLiftServerSDK\Public\"
$destinationSource = Join-Path $rootDir "GameLiftServerSDK\Source\GameLiftServerSDK\Private\"

try {
    if (!(Test-Path $destinationInclude)) {
        throw "Target folder: $destinationInclude does not exist."
    }
    if (!(Test-Path $destinationSource)) {
        throw "Target folder: $destinationSource does not exist."
    }
    if (!(Test-Path $downloadFolder)) {
        New-Item -ItemType Directory -Path $downloadFolder -Force | Out-Null
    }
    if (!(Test-Path $tempFolder)) {
        New-Item -ItemType Directory -Path $tempFolder -Force | Out-Null
    }

    if (!(Test-Path $zipFile)) {
        Write-Host "Downloading GameLift SDK..."
        Invoke-WebRequest -Uri $sdkUrl -OutFile $zipFile -ErrorAction Stop
    } else {
        Write-Host "SDK zip file already exists in Temp/downloads folder, skipping download."
    }

    if (Test-Path $extractPath) {
        Remove-Item -Recurse -Force $extractPath -ErrorAction Stop
    }
    Expand-Archive -Path $zipFile -DestinationPath $extractPath -Force -ErrorAction Stop

    Write-Host "Copying include files..."
    New-Item -ItemType Directory -Path $destinationInclude -Force | Out-Null
    Copy-Item -Path "$sourceInclude*" -Destination $destinationInclude -Recurse -Force -ErrorAction Stop

    Write-Host "Copying source files..."
    New-Item -ItemType Directory -Path $destinationSource -Force | Out-Null
    Copy-Item -Path "$sourceSource*" -Destination $destinationSource -Recurse -Force -ErrorAction Stop

    Write-Host "Cleaning up extracted files..."
    Remove-Item -Recurse -Force $extractPath -ErrorAction Stop

    Write-Host "Done."
    exit 0

} catch {
    Write-Host "ERROR: $_"
    exit 1
}
