// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SWidgets/SProgressBar.h"
#include "SWidgets/SNamedRow.h"

DECLARE_DELEGATE(FOnContainersImageUpdate);

class SCreateContainerImageSection : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCreateContainerImageSection) {}
	SLATE_EVENT(FOnContainersImageUpdate, OnContainersImageUpdateDelegate)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void ResetUIToNotStarted();
	FReply StartSectionWorkflow();

private:
	void UpdateUIBasedOnCurrentState();
	void UpdateUIToInProgress();
	TSharedRef<SWidget> CreateShowErrorUI();
	FString DockerFileTemplatePath;
	
private:
	TSharedPtr<STextBlock> GameServerBuildTextBlock;
	TSharedPtr<STextBlock> GameServerExecutableTextBlock;
	TSharedPtr<SNamedRow> GameServerBuildRow;
	TSharedPtr<SNamedRow> GameServerExecutableRow;
	TSharedPtr<STextBlock> ErrorTextBlock;
	TSharedPtr<SProgressBar> ProgressBar;
	FOnContainersImageUpdate OnContainersImageUpdateDelegate;
};

