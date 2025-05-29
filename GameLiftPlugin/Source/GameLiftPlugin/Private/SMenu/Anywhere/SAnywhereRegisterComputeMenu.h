// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SWidgets/SSectionStep.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"

class SEditableTextBox;
class STextBlock;
class SWindow;
class SWidgetSwitcher;
class SIPAddressTextBox;
class SNamedRow;
class SBootstrapStatus;

struct FComputeInfo
{
	FString ComputeName;
	FString RegisteredIPAddress;
	FString LinkedFleetId;
};

DECLARE_DELEGATE(FOnComputeChanged);

class SAnywhereRegisterComputeMenu : public SSectionStep
{
	SLATE_BEGIN_ARGS(SAnywhereRegisterComputeMenu) {}

	SLATE_EVENT(FOnComputeChanged, OnComputeChangedDelegate)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

	void InvalidateCompute();

private:
	// UI states displayed one at a time
	enum class EComputeUIState
	{
		CreateNewCompute,
		ShowExistingCompute
	};

	// Helpers for creating the widgets
	TSharedRef<SWidget> CreateShowExistingComputeUI();
	TSharedRef<SWidget> CreateAddNewComputeUI();
	TSharedRef<SWidget> CreateComputeStatusRow();

	// Helpers for updating the widgets
	void ChangeComputeSelectionUIState(EComputeUIState InState);
	void UpdateContentForCurrentUIState();

	// UI handler functions
	FReply OnRegisterFirstComputeButtonClicked();
	FReply OnRegisterNewComputeButtonClicked();
	FReply OnCancelComputeButtonClicked();

	TTuple<bool, FString> RegisterCompute(const FString& InComputeName, const FString& InIPAddress);
	TTuple<FString, FString> CreateCustomLocation();

	void AddNewCompute(const FString& InComputeName, const FString& InIPAddress);
	FText GetDefaultComputeName() const;
	void ValidateComputeSettings();

	bool CanEditComputeSettings() const;
	bool CanRegisterCompute() const;
	bool HaveExistingCompute() const;

	void UpdateErrorMessage(const FText& ErrorMessage);

	int32 ComputeUIStateToInt(EComputeUIState State)
	{
		return (int32)State;
	}

	void OnComputeRegistered();
	void OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu* /* Sender */);

	void StartSection();

private:
	// Widgets that change content based on state
	TSharedPtr<SEditableTextBox> ComputeNameTextInput;
	TSharedPtr<SIPAddressTextBox> ComputeIPTextInput;
	TSharedPtr<SEditableText> ComputeNameTextView;
	TSharedPtr<SEditableText> ComputeIPTextView;
	TSharedPtr<SButton> RegisterComputeButton;
	TSharedPtr<SWidgetSwitcher> ComputeWidgetSwitcher;
	TSharedPtr<SWidget> CancelButton;
	// Rows that show/hide based on state
	TSharedPtr<SWidget> ComputeStatusRow;

	TSharedPtr<SNamedRow> RegisterComputeErrorRow;
	TSharedPtr<STextBlock> RegisterComputeErrorTextBlock;

	// Delegates
	FOnComputeChanged OnComputeChangedDelegate;

	// Compute data
	FComputeInfo SelectedCompute;

	bool IsLoading = false;
};
