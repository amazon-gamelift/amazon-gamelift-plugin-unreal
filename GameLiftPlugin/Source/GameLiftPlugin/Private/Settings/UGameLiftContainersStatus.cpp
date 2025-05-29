// Copyright Amazon.com, Inc. or its affiliates.All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "UGameLiftContainersStatus.h"
#include <Types/EContainersDeploymentScenario.h>
#include <Types/EDeploymentMessageState.h>

UGameLiftContainersStatus::UGameLiftContainersStatus(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    LoadConfig();

    auto SetDefault = [&](FText& Field, auto&& Definition)
    {
        if (Field.IsEmpty())
        {
            Field = FText::FromString(Definition);
        }
    };

    SetDefault(Status, Menu::DeployManagedEC2::kNoDeployment);

    SaveConfig();
}

void UGameLiftContainersStatus::ResetStatus()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->GameClientExecutablePath.Empty();
    ContainersStatus->GameClientLauncherArguments.Empty();
    ContainersStatus->GameServerBuildPath.Empty();
    ContainersStatus->GameServerExecutablePath.Empty();
    ContainersStatus->ECRRepoName = (Menu::DeployContainers::kECRRepoNameDefault).ToString();
    ContainersStatus->ECRRepoUri.Empty();
    ContainersStatus->ECRImageDigest.Empty();
    ContainersStatus->DockerRepository = FText::GetEmpty();
    ContainersStatus->ImageID = FText::GetEmpty();
    ContainersStatus->CgdStatus.Empty();
    ContainersStatus->CgdName.Empty();
    ContainersStatus->CgdVersion.Empty();
    ContainersStatus->FleetId.Empty();
    ContainersStatus->FleetStatus.Empty();
    ContainersStatus->FleetType.Empty();
    ContainersStatus->InstanceType.Empty();
    ContainersStatus->StackIdentifier.Empty();
    ContainersStatus->ErrorMessageCreatingECRRepo = FText::GetEmpty();
    ContainersStatus->ErrorMessageCreatingContainerImage = FText::GetEmpty();
    ContainersStatus->ErrorMessageCreatingContainerGroupSection = FText::GetEmpty();
    ContainersStatus->ErrorMessageCreatingContainerFleet = FText::GetEmpty();
    ContainersStatus->ApiGatewayEndpoint = FText::GetEmpty();
    ContainersStatus->BuildOperatingSystem = FText::GetEmpty();
    ContainersStatus->CognitoClientId = FText::GetEmpty();
    ContainersStatus->ContainerImageName = FText::GetEmpty();
    ContainersStatus->ContainerImageURI = FText::GetEmpty();
    ContainersStatus->DeployedRegion = FText::GetEmpty();
    ContainersStatus->GameClientFilePath = FText::GetEmpty();
    ContainersStatus->GameName = Menu::DeployContainers::kGameNameDefault;
    ContainersStatus->IntraContainerLaunchPath.Empty();
    ContainersStatus->LatestError = FText::GetEmpty();
    ContainersStatus->OutConfigFilePath = FText::GetEmpty();
    ContainersStatus->Scenario = FText::GetEmpty();
    ContainersStatus->Status = FText::FromString(EDeploymentMessageStateToString(EDeploymentMessageState::NoDeploymentMessage));

    ContainersStatus->IsContainerFolderSetup = false;
    ContainersStatus->ErrorCreatingContainerImage = false;
    ContainersStatus->ErrorCreatingECRRepo = false;
    ContainersStatus->ErrorCreatingContainerFleet = false;
    ContainersStatus->ErrorRunningBuildAndPushScript = false;
    ContainersStatus->ErrorCreatingContainerGroupSection = false;
    ContainersStatus->IsCgdFailed = false;
    ContainersStatus->IsCgdConfigurationFinished = false;
    ContainersStatus->IsUpdateContainerFleetModalOpen = false;
    ContainersStatus->IsResetContainerFleetModalOpen = false;
    ContainersStatus->IsSetupECRRepoStepFinished = false;
    ContainersStatus->IsContainerImagePushed = false;
    ContainersStatus->IsLastStepComplete = false;

    ContainersStatus->HasExistingContainerImage = 0;
    ContainersStatus->UseExistingECRRepo = 0;
    ContainersStatus->ContainerImageChoice = 0;
    ContainersStatus->DeploymentScenario = 0;
    ContainersStatus->DeploymentDetailRegionChoice = 0;
    ContainersStatus->ContainerFleetDeploymentStatus = 0;

    ContainersStatus->ConnectionPortRange = Menu::DeployContainers::kConnectionPortRangeDefault;
    ContainersStatus->TotalMemoryLimit = Menu::DeployContainers::kTotalMemoryLimitDefault;
    ContainersStatus->TotalVCPULimit = Menu::DeployContainers::kTotalVCPULimitDefault;
    ContainersStatus->ImageTag = Menu::DeployContainers::kImageTagDefault;

    ContainersStatus->SaveConfig();
}