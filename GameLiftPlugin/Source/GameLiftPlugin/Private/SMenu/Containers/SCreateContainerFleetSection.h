// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SWidgets/STextStatus.h"
#include "SWidgets/SProgressBar.h"
#include "Types/EDeploymentMessageState.h"

DECLARE_DELEGATE(FOnRetryStep);

class SNamedRow;
class SCreateContainerFleetSection : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCreateContainerFleetSection) {}
		SLATE_EVENT(FOnRetryStep, OnRetryStepDelegate)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void StartSectionWorkflow();
	void ResetUIToNotStarted();
	void OnDeploymentComplete();

private:
	void UpdateUIBasedOnCurrentState();
	TSharedRef<SWidget> CreateShowErrorUI();
	TSharedRef<SWidget> CreateViewInCFNButton();
	bool ShowFleetDetails();
	void PopulateFleetDetails();
	bool ShouldStartSectionWorkflow();
	void SetFieldInputsToEmptyStatuses();
	void UpdateStepStatusUI(EDeploymentMessageState DeploymentState);
	void PollDescribeContainerFleet();
	
private:
	FOnRetryStep OnRetryStepDelegate;
	TSharedPtr<SProgressBar> ProgressBar;
	TSharedPtr<SNamedRow> FleetIdRow;
	TSharedPtr<STextBlock> FleetIdTextBlock;
	TSharedPtr<SNamedRow> FleetStatusRow;
	TSharedPtr<STextStatus> FleetStatusTextBlock;
	TSharedPtr<SNamedRow> FleetTypeRow;
	TSharedPtr<STextBlock> FleetTypeTextBlock;
	TSharedPtr<SNamedRow> InstanceTypeRow;
	TSharedPtr<STextBlock> InstanceTypeTextBlock;
	TSharedPtr<STextBlock> ErrorTextBlock;
	TSharedPtr<SNamedRow> ErrorRow;
	const int OverrideButtonHeight = 20;
	const int WaitIntervalSeconds = 30;
	const int WaitTimesCount = 240;
};

