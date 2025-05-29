// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SBuildAndPushToECRSection.h"
#include "SWidgets/SProgressBar.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"


class SBootstrapStatus;

DECLARE_DELEGATE(FOnECRUpdate);

class SSetupECRRepositorySection : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SSetupECRRepositorySection) {}
	SLATE_EVENT(FOnECRUpdate, OnECRUpdateDelegate)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void StartSectionWorkflow();
	void ResetUIToNotStarted();

private:
	TTuple<bool, FString> CreateRepository(FString RepositoryName);
	TSharedRef<SWidget> CreateShowErrorUI();
	FText GetECRRepoNameToDisplay();
	void OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu*);
	bool ScenarioNoContainerImageNoExistingEcrRepoNotFinished();
	bool ScenarioNoContainerImageUseExistingEcrRepoNotFinished();
	bool ScenarioNoContainerImageNoExistingEcrRepoFinished();
	bool ScenarioNoContainerImageUseExistingEcrRepoFinished();
	void UpdateUIBasedOnCurrentState();
	bool IsScenario(int Scenario);
	bool ScenarioNoContainerImageFinished();
	void HandleSetupRepoSucceed(FText ECRRepoNameToDisplay);
	void HandleSetupRepoFailed(FText ECRRepoNameToDisplay, FString ErrorMessage);
	

private:
	TSharedPtr<STextBlock> ErrorTextBlock;
	TSharedPtr<SNamedRow> ErrorRow;
	TSharedPtr<SNamedRow> RepoNameRow;
	TSharedPtr<STextBlock> RepoNameTextBlock;
	TSharedPtr<SProgressBar> ProgressBar;
	FOnECRUpdate OnECRUpdateDelegate;
};
