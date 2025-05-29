// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SWidgets/SProgressBar.h"
#include "SWidgets/STextStatus.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SSelectionComboBox.h"
#include "Utils/Notifier.h"

DECLARE_MULTICAST_DELEGATE(FConfigureCgd);
DECLARE_MULTICAST_DELEGATE(FDeploymentComplete);

class SBootstrapStatus;

class SCreateContainerGroupSection : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCreateContainerGroupSection) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void StartSectionWorkflow();
	static FConfigureCgd OnConfigureCgdMultiDelegate;
	static FDeploymentComplete OnDeploymentCompleteMultiDelegate;
	FReply DeployCloudFormation();
	void ResetUIToNotStarted();
	void SetFieldInputsToEmptyStatuses();

private:
	void UpdateUIBasedOnCurrentState();
	bool IsContainerDeploymentStatusActive();
	void PopulateCgdDetails();
	bool ShouldStartSectionWorkflow();
	TSharedRef<SWidget> CreateCgdDetails();
	TSharedRef<SWidget> CreateShowErrorUI();
	TSharedRef<SWidget> CreateViewInCFNButton();
	void HandleCgdReady();
	void HandleCgdFailed();
	void PopulateDeploymentStatus();
	void PollDescribeContainerGroupDefinition();
	void UpdateCgdStatusTextBlock(const FText Status);

private:
	TSharedPtr<SWidget> CgdNameTextInput;
	TSharedPtr<SWidgetSwitcher> CreateCgdWidgetSwitcher;
	TSharedPtr<SEditableText> CgdNameTextView;
	TSharedPtr<SSelectionComboBox> ExistingCgdDropdown;
	TSharedPtr<STextBlock> ErrorTextBlock;
	TSharedPtr<SNamedRow> GroupDefinitionStatusRow;
	TSharedPtr<STextStatus> GroupDefinitionStatusTextBlock;
	TSharedPtr<SNamedRow> GroupDefinitionNameRow;
	TSharedPtr<STextBlock> GroupDefinitionNameTextBlock;
	TSharedPtr<SNamedRow> CgdVersionRow;
	TSharedPtr<STextBlock> CgdVersionTextBlock;
	TSharedPtr<SNamedRow> TotalMemoryLimitRow;
	TSharedPtr<STextBlock> TotalMemoryLimitTextBlock;
	TSharedPtr<SNamedRow> TotalVcpuLimitRow;
	TSharedPtr<STextBlock> TotalVcpuLimitTextBlock;
	TSharedPtr<SWidget> CgdDetails;
	TSharedPtr<SProgressBar> ProgressBar;
	TSharedPtr<SNotificationItem> NotificationItem;
	FText CgdName;
	TSharedPtr<SWidget> DeploymentStatus;
	bool IsLoading = false;
	bool IsLoadingCgdInfo = false;
	const int WaitIntervalSeconds = 10;
	const int WaitTimesCount = 720;
	const int OverrideButtonHeight = 20;
};
