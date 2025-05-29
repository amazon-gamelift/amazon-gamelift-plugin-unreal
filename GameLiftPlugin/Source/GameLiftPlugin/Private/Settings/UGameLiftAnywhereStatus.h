// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "GameLiftPluginConstants.h"

#include "UGameLiftAnywhereStatus.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class UGameLiftAnywhereStatus
	: public UObject
{
	GENERATED_UCLASS_BODY()

public:
	static void ResetStatus();
	static bool IsAnywhereReady();

public:
	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString DeployedRegion;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString CustomLocation;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString FleetName;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString FleetId;
	
	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	bool IsFleetDeployed;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString ComputeName;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString ComputeIPAddress;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	bool IsComputeRegistered;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString AuthToken;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FDateTime AuthTokenExpirationTime;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString PathToServerBuild;

	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString PathToClientBuild;
	
	UPROPERTY(config, EditAnywhere, Category = "Anywhere Status")
	FString ClientBuildLauncherArguments;
};
