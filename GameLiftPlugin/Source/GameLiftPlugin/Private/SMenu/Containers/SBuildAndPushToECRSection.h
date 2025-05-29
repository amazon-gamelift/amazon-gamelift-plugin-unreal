// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "SWidgets/SCopyPasteTextBox.h"
#include "SWidgets/SSectionsWithHeaders.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SWidgets/SExternalLinkButton.h"
#include "SWidgets/SProgressBar.h"
#include "SWidgets/SNamedRow.h"

DECLARE_DELEGATE(FOnBuildAndPushFinished);

class SBuildAndPushToECRSection : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SBuildAndPushToECRSection) {}
		SLATE_EVENT(FOnBuildAndPushFinished, OnBuildAndPushFinishedDelegate)
	SLATE_END_ARGS()

private:
	TSharedRef<SWidget> CreateShowCommandsUI();
	TSharedRef<SWidget> CreateShowErrorUI();	
	FReply OnContinueButtonClicked_CLISteps();
	void UpdateUIBasedOnCurrentState();
	FString GetScriptPath();
	bool RunScript();
	void SetCommandText();
	void HandleBuildAndPushSucceed();
	void HandleBuildAndPushFailed();
	bool ShouldStartSectionWorkflow();
	

public:
	void Construct(const FArguments& InArgs);
	FReply StartSectionWorkflow();
	void ResetUIToNotStarted();
	
private: 
	TSharedPtr<SCopyPasteTextBox> Command1Box;
	TSharedPtr<SCopyPasteTextBox> Command2Box;
	TSharedPtr<SCopyPasteTextBox> Command3Box;
	TSharedPtr<SCopyPasteTextBox> Command4Box;
	TSharedPtr<SProgressBar> ProgressBar;
	FOnBuildAndPushFinished OnBuildAndPushFinishedDelegate;
	TSharedPtr<SNamedRow> ImageTagRow;
	TSharedPtr<STextBlock> ImageTagTextBlock;
	TSharedPtr<STextBlock> ErrorTextBlock;
	TSharedPtr<SNamedRow> ErrorRow;
	TSharedPtr<SExternalLinkButton> ViewLogsButton;
	TSharedPtr<SButton> ContinueButton;
	bool ShowCommands;
	const int WaitIntervalSeconds = 3;
	const int WaitTimesCount = 600;
};

