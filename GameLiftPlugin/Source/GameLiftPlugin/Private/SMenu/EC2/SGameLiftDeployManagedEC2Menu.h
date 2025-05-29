// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SWidgets/SExpandableSection.h"
#include "SWidgets/SLaunchBar.h"
#include "SWidgets/SSectionStep.h"
#include "SWidgets/SSetupMessage.h"
#include "SWidgets/SSectionsWithProgressBars.h"

struct FTextIntPair;
class SWindow;
class SPathInput;

class SIntegrateGameSection;
class SSelectDeploymentScenarioSection;
class SDeployScenarioSection;
class SGameParametersSection;
class SSectionStep;
class SSectionsWithProgressBars;

class SGameLiftDeployManagedEC2Menu : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SGameLiftDeployManagedEC2Menu) { }
	
	SLATE_ARGUMENT(TWeakPtr<SWindow>, ContextWindow)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void ResetFlow();

private:
	TSharedRef<SWidget> CreateDivider();
	TSharedRef<SSectionStep> CreateIntegrateGameSection();
	TSharedRef<SSectionStep> CreateSelectDeploymentScenarioSection();
	TSharedRef<SSectionStep> CreateGameParametersSection();
	TSharedRef<SSectionStep> CreateDeployScenarioSection();
	TSharedRef<SWidget> CreateLaunchBar();

	void SwitchDeploymentFields();
	bool CanLaunchGameClient() const;

	FText TooltipLaunchGameClient() const;
	void OnLaunchClientButtonClicked(FString GameClientPath, FString LauncherArgs);

private:
	TSharedPtr<SSectionsWithProgressBars> SectionsWithProgressBars;
	TSharedPtr<SWidget> DeploymentFields;
	TSharedPtr<SWidget> DeploymentStatus;
	TSharedPtr<SLaunchBar> LaunchBar;
	TSharedPtr<SSetupMessage> GameClientMessage;
	TWeakPtr<SWindow> ContextWindow;

	bool IsLaunchingGameClient = false;
};

