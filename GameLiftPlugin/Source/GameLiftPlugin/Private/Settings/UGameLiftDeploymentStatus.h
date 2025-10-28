// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "GameLiftPluginConstants.h"

#include "UGameLiftDeploymentStatus.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class UGameLiftDeploymentStatus
	: public UObject
{
	GENERATED_UCLASS_BODY()

public:
	static void ResetStatus();

public:
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText Status;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText Scenario;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	bool EnableMetrics;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText BuildName;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText BuildOperatingSystem;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText BuildFolderPath;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText BuildFilePath;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText ExtraServerResourcesPath;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText OutConfigFilePath;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText GameClientFilePath;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText GameClientLauncherArguments;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FDateTime LastUpdated;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText CognitoClientId;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText ApiGatewayEndpoint;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText LatestError;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FText DeployedRegion;
	UPROPERTY(config, EditAnywhere, Category = "Deployment Status")
	FString StackIdentifier;

};
