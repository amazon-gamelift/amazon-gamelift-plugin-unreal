// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "IGameLiftContainersHandler.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SPathInput.h"
#include "SWidgets/SSelectionComboBox.h"
#include "SWidgets/SSetupMessage.h"
#include "Types/EContainersDeploymentScenario.h"
#include "SIntakeQuestionnaireSection.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"

class SUserInputSection : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SUserInputSection) { }

    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs);

private:
    TSharedRef<SWidget> CreateMetricsInfoMessage();
    TSharedRef<SWidget> CreateMetricsCheckBox();
    TSharedRef<SWidget> CreateGameServerPathInput();
    TSharedRef<SWidget> CreateOutConfigFilePathInput();
    TSharedRef<SWidget> CreateExistingECRRepoDropdown();
    TSharedRef<SWidget> CreateExistingECRImageDropdown();
    TSharedRef<SWidget> CreateDockerRepoInput();
    TSharedRef<SWidget> CreateDeploymentScenarioChoice();
    TSharedRef<SWidget> CreateDefaultSettings();

    void OnGameServerBuildPathInputUpdated(const FString& NewPath);
    void OnGameServerExecutablePathInputUpdated(const FString& NewPath);
    void OnOutConfigFilePathInputUpdated(const FString& NewPath);

    void OnBuildExistingRepoValues(TArray<FTextIntPair>& Items);
    void OnExistingRepoSelected(int SelectionId, const FTextIntPair& Item);
    void LoadRepoInfoAsync();
    void OnLoadRepoInfoComplete(const GameLiftECRDescribeRepositoriesResult& DescribeRepoResult);
    void OnAvailableReposChanged(const TArray<GameLiftECRDescribeRepositoriesResult::RepositoryAttributes>& Repos);
    bool CanLoadECRRepoDropDown() const;

    void OnBuildExistingImageValues(TArray<FTextIntPair>& Items);
    void OnExistingImageSelected(int SelectionId, const FTextIntPair& Item);
    void LoadImageInfoAsync();
    void OnLoadImageInfoComplete(const GameLiftECRListImagesResult& ListImageResult);
    void OnAvailableImagesChanged(const TArray<GameLiftECRListImagesResult::ImageDetailAttributes>& Images);
    bool CanLoadECRImageDropDown() const;

    void OnDeploymentScenarioStateChanged(const SIntakeQuestionnaireSection*);
    void OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu*);
    void OnRefreshUI();

    void OnImageIDInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);

    ECheckBoxState IsRadioChecked(EDeploymentScenarioFleetRegionType RadioButtonChoice) const;
    void OnRadioChanged(ECheckBoxState NewRadioState, EDeploymentScenarioFleetRegionType RadioButtonChoice);

    void OnGameNameInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnConnectionPortRangeInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnTotalMemoryLimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnTotalVCPULimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnImageTagInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnECRRepoInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo);
    void OnEnableMetricsChanged(ECheckBoxState NewState);

    void UpdateInitialUI();
    EVisibility ShowGameServerPathInput();
    EVisibility ShowExistingECRRepoDropdown();
    EVisibility ShowDockerRepoInput();
    EVisibility ShowExistingECRImageDropdown();

private:
    TSharedPtr<SPathInput> GameServerBuildPathInput;
    TSharedPtr<SPathInput> GameServerExecutablePathInput;
    TSharedPtr<SPathInput> OutConfigFilePathInput;

    TSharedPtr<SSelectionComboBox> ExistingRepoDropdown;
    TArray<FTextIntPair> ExistingRepoList;
    bool IsLoadingRepoInfo = false;
    TSharedPtr<STextBlock> ErrorTextBlock;
    TSharedPtr<SNamedRow> ErrorRow;

    TSharedPtr<SSelectionComboBox> ExistingImageDropdown;
    TArray<FTextIntPair> ExistingImageList;
    bool IsLoadingImageInfo = false;
    TSharedPtr<STextBlock> ImageErrorTextBlock;
    TSharedPtr<SNamedRow> ImageErrorRow;

    TSharedPtr<SEditableTextBox> ImageIDInput;

    // optional settings
    TSharedPtr<SEditableTextBox> GameNameInput;
    TSharedPtr<SEditableTextBox> ConnectionPortRangeInput;
    TSharedPtr<SEditableTextBox> TotalMemoryLimitInput;
    TSharedPtr<SEditableTextBox> TotalVCPULimitInput;
    TSharedPtr<SEditableTextBox> ImageTagInput;
    TSharedPtr<SEditableTextBox> ECRRepoInput;
    TSharedPtr<SCheckBox> EnableMetricsCheckBox;
    TSharedPtr<SSetupMessage> MetricsInfoMessage;
    const int OverridePrimaryColumnWidth = 200;
    const int OverrideSecondaryColumnWidth = 100;
};
