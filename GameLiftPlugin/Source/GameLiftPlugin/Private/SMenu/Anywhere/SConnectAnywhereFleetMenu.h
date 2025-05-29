// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "IGameLiftAnywhereHandler.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SWidgets/SSectionStep.h"
#include "SWidgets/SNamedRow.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"

#include "Types/FTextIntPair.h"

class SBootstrapStatus;
class SEditableTextBox;
class SErrorBanner;
class SSelectionComboBox;
class STextBlock;
class SWindow;
class SWidgetSwitcher;

DECLARE_DELEGATE(FOnFleetChanged);

class SConnectAnywhereFleetMenu : public SSectionStep
{
	SLATE_BEGIN_ARGS(SConnectAnywhereFleetMenu) {}

	SLATE_EVENT(FOnFleetChanged, OnFleetChangedDelegate)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	// UI states displayed one at a time
	enum class EFleetSelectionUIState
	{
		CreateNewFleet,
		ChooseExistingFleet
	};

	// Helpers for creating the widgets
	TSharedRef<SWidget> CreateChooseExistingFleetUI();
	TSharedRef<SWidget> CreateAddNewFleetUI();
	TSharedRef<SWidget> CreateFleetsInConsoleHyperLink();

	// Helpers for updating the widgets
	void ChangeFleetSelectionUIState(EFleetSelectionUIState InState);
	void UpdateContentForCurrentUIState();

	// UI handler functions
	void OnBuildExistingFleetValues(TArray<FTextIntPair>& Items);
	void OnExistingFleetSelected(int SelectionId, const FTextIntPair& Item);
	FReply OnCreateFleetButtonClicked();
	FReply OnGoToCreateFleetButtonClicked();
	FReply OnCancelFleetButtonClicked();
	FReply OnModifyButtonClicked();

	void LoadFleetInfoAsync();
	void OnLoadFleetInfoComplete(const GameLiftAnywhereDescribeFleetsResult& ListFleetResult);
	void OnRetryLoadingFleetInfo();
	void OnAvailableFleetsChanged(const TArray<GameLiftAnywhereDescribeFleetsResult::FleetAttributes>& Fleets);
	
	void OnSelectedFleetChanged();

	void OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu* /* Sender */);

	TTuple<FString, FString> CreateCustomLocation();
	TTuple<FString, FString> CreateCustomLocationAndFleet(const FString& InFleetName);

	void AddNewAnywhereFleet(const FString& InFleetName, const FString& InFleetId);

	bool CanCreateFleet() const;
	bool CanEnableFleetSelectionUI() const;
	bool CanEnableUI() const;
	bool HaveExistingFleets() const;
	bool IsFleetSelected() const;
	void SaveSelectedFleet(const FString& InFleetName, const FString& InFleetId);
	void ClearSelectedFleet();
	void ValidateCurrentlySelectedFleet();

	void UpdateErrorMessage(const FText& ErrorMessage);

	int32 FleetSelectionUIStateToInt(EFleetSelectionUIState State)
	{
		return (int32)State;
	}

	FText GetDefaultFleetName() const;

	void StartSection();

private:
	// Widgets that change content based on state
	TSharedPtr<SEditableTextBox> FleetNameTextInput;
	TSharedPtr<SSelectionComboBox> ExistingFleetsComboBox;
	TSharedPtr<SEditableText> FleetIDTextView;
	TSharedPtr<SButton> AddFleetButton;
	TSharedPtr<SWidgetSwitcher> ConnectToFleetWidgetSwitcher;
	TSharedPtr<SWidgetSwitcher> ModifyCreateButtonSwitcher;
	TSharedPtr<SWidget> CancelButton;
	TSharedPtr<SWidget> ModifyButton;
	// Rows that show/hide based on state
	TSharedPtr<SWidget> FleetIDRow;

	TSharedPtr<SErrorBanner> DeployFleetErrorBanner;
	TSharedPtr<SNamedRow> DeployFleetErrorRow;
	TSharedPtr<STextBlock> DeployFleetErrorTextBlock;

	// Delegates
	FOnFleetChanged OnFleetChangedDelegate;

	// Fleet data
	TArray<FTextIntPair> ExistingFleetList;

	bool IsLoading = false;
	bool IsLoadingFleetInfo = false;
};
