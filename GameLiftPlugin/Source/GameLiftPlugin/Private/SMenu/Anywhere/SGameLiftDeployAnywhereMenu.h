// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SWidgets/SSectionStep.h"
#include "SWidgets/SLaunchBar.h"

#include "Types/FTextIntPair.h"

class SWindow;
class SSetProfileSection;
class SIntegrateGameSection;
class SConnectAnywhereFleetMenu;
class SAnywhereRegisterComputeMenu;
class SAnywhereGenerateAuthTokenMenu;
class SPathInput;

class SGameLiftDeployAnywhereMenu : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SGameLiftDeployAnywhereMenu) { }
	
	SLATE_ARGUMENT(TWeakPtr<SWindow>, ContextWindow)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> CreateDivider();
	TSharedRef<SSectionStep> CreateIntegrateGameSection();
	TSharedRef<SSectionStep> CreateConnectToFleetSection();
	TSharedRef<SSectionStep> CreateRegisterComputeSection();
	TSharedRef<SSectionStep> CreateAuthTokenSection();
	TSharedRef<SLaunchBar> CreateLaunchBar();

	bool CanLaunchServer() const;
	bool CanLaunchClient() const;
	void UpdateLaunchServerButtonState();
	void UpdateLaunchClientButtonState();

	void OnFleetChanged();
	void OnComputeChanged();

	void OnLaunchServerButtonClicked();
	void OnLaunchClientButtonClicked(FString GameClientPath, FString LauncherArgs);

private:
	TSharedPtr<SIntegrateGameSection> IntegrateGameSection;
	TSharedPtr<SConnectAnywhereFleetMenu> AnywhereConnectFleetSection;
	TSharedPtr<SAnywhereRegisterComputeMenu> AnywhereRegisterComputeSection;
	TSharedPtr<SAnywhereGenerateAuthTokenMenu> AnywhereGenerateAuthTokenSection;
	TSharedPtr<SLaunchBar> LaunchBar;

	TWeakPtr<SWindow> ContextWindow;

	bool IsLaunchingGameServer = false;
	bool IsLaunchingGameClient = false;
	bool HasLaunchedGameServer = false;
};
