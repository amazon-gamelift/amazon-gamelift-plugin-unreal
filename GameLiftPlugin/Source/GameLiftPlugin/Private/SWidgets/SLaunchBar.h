// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "SMenu/SClientBuildModal.h"

enum class EMenuType
{
	Anywhere,
	EC2,
	Containers
};

class SLaunchBar : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SLaunchBar) { }

	SLATE_ARGUMENT(EMenuType, MenuType)

	SLATE_ARGUMENT(TWeakPtr<SWidget>, ParentWidget)

	SLATE_ATTRIBUTE(FString, DefaultClientBuildExecutablePath)

	SLATE_ATTRIBUTE(FString, DefaultClientBuildLauncherArguments)

	SLATE_EVENT(FSimpleDelegate, OnStartServerButtonClicked)

	SLATE_EVENT(FStartClient, OnStartClientButtonClicked)

	SLATE_EVENT(FStartClient, OnStartClientButtonClickedWith2Players)

	SLATE_ATTRIBUTE(bool, IsStartServerButtonEnabled)

	SLATE_ATTRIBUTE(bool, IsStartClientButtonEnabled)

	SLATE_ATTRIBUTE(bool, IsUpdateDeploymentButtonEnabled)

	SLATE_END_ARGS()

public:
	enum class EStartButtonUIState
	{
		Start,
		Loading,
	};
	void Construct(const FArguments& InArgs);
	
	void ChangeStartClientButtonUIState(EStartButtonUIState InState);
	void ChangeStartServerButtonUIState(EStartButtonUIState InState);

private:
	TSharedRef<SWidget> CreateUpdateDeploymentButton();
	TSharedRef<SWidget> CreateTest2PlayerClientCheckBox();
	TSharedRef<SButton> CreateStartClientButton();
	TSharedRef<SWidget> CreateStartServerButton();
	TSharedRef<SWidget> CreateStartServerInstruction();
	TSharedRef<SButton> CreateLoadingButton();
	TSharedRef<SClientBuildModal> CreateClientBuildModal();
	void DisplayUpdateModal();
	void OnCheckboxStateChanged(ECheckBoxState NewState);
	void OnStartClientButtonPressed(FString ClientBuildPathExecutable, FString LauncherArgs);
	FReply OnStartServerButtonPressed();
	int32 LaunchBarUIStateToInt(EStartButtonUIState State)
	{
		return (int32)State;
	};

private:
	const int OverrideButtonHeight = 20;
	bool ShowUpdateDeploymentButton = false;
	bool ShowTwoPlayerClientCheckBox = false;
	bool ShowStartServerButton = false;
	bool IsTwoPlayerClientSelected = false;

	TAttribute<bool> IsStartServerButtonEnabled = false;
	TAttribute<bool> IsStartClientButtonEnabled = false;
	TAttribute<bool> IsUpdateDeploymentButtonEnabled = false;
	TAttribute<FString> ClientBuildExecutablePath;
	TAttribute<FString> ClientBuildLauncherArguments;

	EMenuType MenuType;
	FStartClient OnStartClientButtonClicked;
	FStartClient OnStartClientButtonClickedWith2Players;
	FSimpleDelegate OnStartServerButtonClicked;
	TSharedPtr<SWidgetSwitcher> StartClientButtonWidgetSwitcher;
	TSharedPtr<SWidgetSwitcher> StartServerButtonWidgetSwitcher;
	TSharedPtr<SClientBuildModal> ClientBuildModal;
	TWeakPtr<SWidget> ParentWidget;
	TSharedPtr<SWidget> LaunchBar;
};