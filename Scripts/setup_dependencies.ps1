# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

$ScriptBasePath = Get-Location

$dependencies = @(
    [PSCustomObject]@{
        Name = "spdlog"
        Url = "https://github.com/gabime/spdlog/archive/refs/tags/v1.14.0.zip"
        TargetFolder = "spdlog"
        DependencyRootCopyFrom = "spdlog-1.14.0"
    },
    [PSCustomObject]@{
        Name = "rapidjson"
        Url = "https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.zip"
        TargetFolder = "rapidjson"
        DependencyRootCopyFrom = "rapidjson-1.1.0"
    },
    [PSCustomObject]@{
        Name = "asio-asio"
        Url = "https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-20-0.zip"
        TargetFolder = "asio"
        DependencyRootCopyFrom = "asio-asio-1-20-0\asio"
    },
    [PSCustomObject]@{
        Name = "websocketpp"
        Url = "https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.zip"
        TargetFolder = "websocketpp"
        DependencyRootCopyFrom = "websocketpp-0.8.2"
    }
)

$TempDir = Join-Path $ScriptBasePath "Temp"
$DownloadDir = Join-Path $TempDir "downloads"
$ExtractDir = Join-Path $TempDir "extracted"
$FinalDir = Join-Path $ScriptBasePath "GameLiftServerSDK\ThirdParty"

if (-not (Test-Path $FinalDir)) {
    Write-Host "ERROR: Target directory '$FinalDir' does not exist!"
    exit 1
}

New-Item -ItemType Directory -Path $DownloadDir -Force | Out-Null
New-Item -ItemType Directory -Path $ExtractDir -Force | Out-Null

function Download-And-ExtractDependency {
    param ($Dependency)

    $zipPath = Join-Path $DownloadDir "$($Dependency.Name).zip"
    $extractPath = Join-Path $ExtractDir $Dependency.Name
    $targetPath = Join-Path $FinalDir $Dependency.TargetFolder

    Write-Host "Processing: $($Dependency.Name) ..."

    try {
        if (-not (Test-Path $zipPath)) {
            Write-Host "Downloading $($Dependency.Url) to $zipPath"
            Invoke-WebRequest -Uri $Dependency.Url -OutFile $zipPath
        } else {
            Write-Host "Zip file already exists, skipping download."
        }

        if (Test-Path $extractPath) {
            Write-Host "Removing old extraction folder: $extractPath"
            Remove-Item -Path $extractPath -Recurse -Force -ErrorAction SilentlyContinue
        }

        Write-Host "Extracting $zipPath to $extractPath"
        Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force

        New-Item -ItemType Directory -Path $targetPath -Force | Out-Null

        $dependencyRootPath = Join-Path $extractPath $Dependency.DependencyRootCopyFrom
        if (Test-Path $dependencyRootPath) {
            Write-Host "Copying contents from $dependencyRootPath to $targetPath"
            Copy-Item -Path (Join-Path $dependencyRootPath "*") -Destination $targetPath -Recurse -Force
        } else {
            throw "ERROR: Dependency root path $dependencyRootPath does not exist!"
        }

        Write-Host "Processing of $($Dependency.Name) completed.`n"

    } catch {
        Write-Host "ERROR: Failed to process $($Dependency.Name). Reason: $_"
        throw $_
    }
}

try {
    foreach ($dependency in $dependencies) {
        Download-And-ExtractDependency -Dependency $dependency
    }

    Write-Host "All dependencies processed successfully."
    exit 0
} catch {
    Write-Host "Installation failed!"
} finally {
    if (Test-Path $ExtractDir) {
        Write-Host "Cleaning up extracted files..."
        Remove-Item -Path $ExtractDir -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "Extracted files cleaned up successfully."
    }
}
