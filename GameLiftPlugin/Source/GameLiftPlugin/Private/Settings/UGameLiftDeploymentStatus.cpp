// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "UGameLiftDeploymentStatus.h"

#include "GameLiftPluginConstants.h"

#include "Utils/StringPaths.h"

#include "Types/EDeploymentMessageState.h"

UGameLiftDeploymentStatus::UGameLiftDeploymentStatus(const FObjectInitializer& ObjectInitializer)
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
	SetDefault(Scenario, Menu::DeployManagedEC2::kNeedDeployment);
	SetDefault(ApiGatewayEndpoint, Menu::DeployManagedEC2::kNeedDeployment);
	SetDefault(CognitoClientId, Menu::DeployManagedEC2::kNeedDeployment);
	SetDefault(ExtraServerResourcesPath, Paths::DefaultExtraServerResourcesPath());
	SetDefault(OutConfigFilePath, Paths::DefaultOutConfigFilePath());

	SaveConfig();
}

void UGameLiftDeploymentStatus::ResetStatus()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	DeploySettings->BuildName = FText::GetEmpty();
	DeploySettings->BuildOperatingSystem = FText::GetEmpty();
	DeploySettings->BuildFolderPath = FText::GetEmpty();
	DeploySettings->BuildFilePath = FText::GetEmpty();
	DeploySettings->ExtraServerResourcesPath = FText::FromString(Paths::DefaultExtraServerResourcesPath());
	DeploySettings->OutConfigFilePath = FText::FromString(Paths::DefaultOutConfigFilePath());
	DeploySettings->GameClientFilePath = FText::GetEmpty();
	DeploySettings->GameClientLauncherArguments = FText::GetEmpty();
	DeploySettings->DeployedRegion = FText::GetEmpty();
	DeploySettings->ApiGatewayEndpoint = FText::GetEmpty();
	DeploySettings->CognitoClientId = FText::GetEmpty();
	DeploySettings->Scenario = FText::GetEmpty();
	DeploySettings->Status = FText::FromString(EDeploymentMessageStateToString(EDeploymentMessageState::NoDeploymentMessage));
	DeploySettings->LatestError = FText::GetEmpty();
	DeploySettings->StackIdentifier.Empty();
	DeploySettings->SaveConfig();
}
