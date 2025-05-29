// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "IGameLiftContainersHandler.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "GameLiftPluginConstants.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"
#include "Types/EContainersDeploymentScenario.h"
#include "SWidgets/SPathInput.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SSetupMessage.h"
#include "SWidgets/SSelectionComboBox.h"

class SWindow;
class SUserInputSection;
class SControlBarSection;
DECLARE_MULTICAST_DELEGATE(FUpdateDeploymemt);

class SUpdateDeploymentModal : public SCompoundWidget
{

    SLATE_BEGIN_ARGS(SUpdateDeploymentModal) {}
        SLATE_ARGUMENT(FText, ModalText)
        SLATE_ARGUMENT(TWeakPtr<SWindow>, OwnerWindow) // Use TWeakPtr to avoid ownership issues
    SLATE_END_ARGS()
public:
    void Construct(const FArguments& InArgs);
    void SetOwnerWindow(TSharedPtr<SWindow> OwnerWindow);
    static FUpdateDeploymemt OnUpdateDeploymentMultiDelegate;

private:
    FText ModalText;
    TSharedPtr<SPathInput> GameServerBuildPathInput;
    TSharedPtr<SPathInput> GameServerExecutablePathInput;
    TSharedPtr<SUserInputSection> UserInputSection;
    TSharedPtr<SControlBarSection> ControlBarSection;
    TSharedPtr<SEditableTextBox> DockerRepoInput;
    TSharedPtr<SEditableTextBox> ImageIDInput;
    TWeakPtr<SWindow> OwnerWindow;
    TSharedPtr<SEditableTextBox> ImageTagInput;
    TSharedPtr<SEditableTextBox> ConnectionPortRangeInput;
    TSharedPtr<SEditableTextBox> TotalMemoryLimitInput;
    TSharedPtr<SEditableTextBox> TotalVCPULimitInput;
    TSharedPtr<SSelectionComboBox> ExistingRepoDropdown;
    TSharedPtr<SCheckBox> SingleRegionCheckBox;
    TSharedPtr<SCheckBox> MultiRegionFlexMatchCheckBox;
    TSharedPtr<SSetupMessage> UpdateFleetDeploymentSetupMessage;
    TArray<FTextIntPair> CurrentECRRepoImagesList;
    TSharedPtr<STextBlock> ImageErrorTextBlock;
    TSharedPtr<SNamedRow> ImageErrorRow;
    TSharedPtr<SSelectionComboBox> ExistingImageDropdown;
    TArray<FTextIntPair> ExistingRepoList;
    bool IsLoadingRepoInfo = false;
    bool IsLoadingImageInfo = false;
    TSharedPtr<STextBlock> ErrorTextBlock;
    TSharedPtr<SNamedRow> ErrorRow;

private:
    FReply OnCloseButtonClicked();
    void UpdateDeploymentConfigurations();
    void ResetAllUpdateFields();
    void OnUpdateDeploymentClicked();
    void OnLoadImageInfoComplete(const GameLiftECRListImagesResult& ListImageResult);
    void LoadImageInfoAsync();
    void OnAvailableReposChanged(const TArray<GameLiftECRDescribeRepositoriesResult::RepositoryAttributes>& Repos);
    void OnLoadRepoInfoComplete(const GameLiftECRDescribeRepositoriesResult& DescribeRepoResult);
    void OnAvailableImagesChanged(const TArray<GameLiftECRListImagesResult::ImageDetailAttributes>& Images);
    void LoadRepoInfoAsync();
    bool CanLoadECRRepoDropDown() const;
    void OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu*);
    TSharedRef<SWidget> CreateExistingECRImageDropdown();
    void OnExistingImageSelected(int SelectionId, const FTextIntPair& Item);
    bool CanLoadECRImageDropDown() const;
    void OnBuildImagesListForCurrentRepo(TArray<FTextIntPair>& Items);
    void OnImageTagInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnConnectionPortRangeInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnTotalMemoryLimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnTotalVCPULimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnImageIDInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnGameServerBuildPathInputUpdated(const FString& NewPath);
    void OnGameServerExecutablePathInputUpdated(const FString& NewPath);
    void OnExistingRepoSelected(int SelectionId, const FTextIntPair& Item);
    void OnRadioChanged(ECheckBoxState NewRadioState, EDeploymentScenarioFleetRegionType RadioButtonChoice);
    void OnBuildExistingRepoValues(TArray<FTextIntPair>& Items);
    void SetDeploymentRegionChoice();
    TSharedRef<SWidget> CreateExistingECRRepoDropdown();
    TSharedRef<SWidget> CreateUpdateFleetDeploymentWarningMessage();
    TSharedRef<SWidget> CreateAWSConsoleButton();
    TSharedRef<SWidget> CreateGameServerPathInput();
    TSharedRef<SWidget> CreateDeploymentScenarioChoice();
    ECheckBoxState IsRadioChecked(EDeploymentScenarioFleetRegionType RadioButtonChoice) const;
    const int OverrideSecondaryColumnWidth = 100;
    const int OverrideButtonHeight = 20;

};