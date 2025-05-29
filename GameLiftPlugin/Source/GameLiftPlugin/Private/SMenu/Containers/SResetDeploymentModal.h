// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "GameLiftPluginConstants.h"
#include "SWidgets/SSetupMessage.h"

class SWindow;
class SUserInputSection;
class SControlBarSection;
DECLARE_MULTICAST_DELEGATE(FResetDeployment);

class SResetDeploymentModal : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SResetDeploymentModal) {}
		SLATE_ARGUMENT(TWeakPtr<SWindow>, OwnerWindow)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void SetOwnerWindow(TSharedPtr<SWindow> OwnerWindow);
	static FResetDeployment OnResetDeploymentMultiDelegate;

private:
	TWeakPtr<SWindow> OwnerWindow;
	TSharedPtr<SSetupMessage> ResetFleetDeploymentSetupMessage;

private:
	FReply OnCloseButtonClicked();
	TSharedRef<SWidget> CreateResetFleetDeploymentWarningMessage();
	TSharedRef<SWidget> CreateAWSConsoleButton();
	const int OverrideColumnWidth = 200;
	const int OverrideButtonHeight = 20;

};