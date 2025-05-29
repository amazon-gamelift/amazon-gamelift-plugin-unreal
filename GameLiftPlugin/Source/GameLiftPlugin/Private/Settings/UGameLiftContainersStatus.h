// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 // SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "GameLiftPluginConstants.h"

#include "UGameLiftContainersStatus.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class UGameLiftContainersStatus
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    //UGameLiftContainersStatus(const FObjectInitializer& ObjectInitializer);
    static void ResetStatus();

public:
    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    int HasExistingContainerImage = 0;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    int UseExistingECRRepo = 0;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    int ContainerImageChoice = 0;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    int DeploymentScenario = 0;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    int DeploymentDetailRegionChoice;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ConnectionPortRange = Menu::DeployContainers::kConnectionPortRangeDefault;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText TotalMemoryLimit = Menu::DeployContainers::kTotalMemoryLimitDefault;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText TotalVCPULimit = Menu::DeployContainers::kTotalVCPULimitDefault;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ImageTag = Menu::DeployContainers::kImageTagDefault;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString GameClientExecutablePath;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString GameClientLauncherArguments;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString GameServerBuildPath;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString GameServerExecutablePath;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString ECRRepoName = (Menu::DeployContainers::kECRRepoNameDefault).ToString();

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString ECRRepoUri;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString ECRImageDigest;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText DockerRepository;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ImageID;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString CgdStatus;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString CgdName;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString CgdVersion;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsContainerFolderSetup;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsCgdConfigurationFinished;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsSetupECRRepoStepFinished;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsContainerImagePushed;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    int ContainerFleetDeploymentStatus;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsLastStepComplete;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool ErrorCreatingContainerImage;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ErrorMessageCreatingContainerImage;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool ErrorCreatingECRRepo;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ErrorMessageCreatingECRRepo;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool ErrorRunningBuildAndPushScript;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool ErrorCreatingContainerFleet;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ErrorMessageCreatingContainerFleet;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    int DeploymentDetailRegionChoiceToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ConnectionPortRangeToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText TotalMemoryLimitToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText TotalVCPULimitToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ImageTagToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString ECRImageDigestToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString GameServerBuildPathToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString GameServerExecutablePathToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString ECRRepoNameToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString ECRRepoUriToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ImageIDToUpdate;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsUpdateContainerFleetModalOpen;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsResetContainerFleetModalOpen;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    TArray<FText> ExistingRepoNameList;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool ErrorCreatingContainerGroupSection;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    bool IsCgdFailed;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ErrorMessageCreatingContainerGroupSection;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString FleetId;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString FleetStatus;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString FleetType;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString InstanceType;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ContainerImageName;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ContainerImageURI;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString IntraContainerLaunchPath;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText Status;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText BuildOperatingSystem;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText OutConfigFilePath;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText GameClientFilePath;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText CognitoClientId;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText ApiGatewayEndpoint;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText LatestError;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText DeployedRegion;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText GameName = Menu::DeployContainers::kGameNameDefault;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FString StackIdentifier;

    UPROPERTY(config, EditAnywhere, Category = "Containers Status")
    FText Scenario;
};
