// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SUserInputSection.h"
#include "SWidgets/SSectionsWithHeaders.h"
#include "SResetDeploymentModal.h"

#include <Types/EBootstrapMessageState.h>
#include <GameLiftPluginConstants.h>
#include <GameLiftPluginStyle.h>

#include <Async/Async.h>
#include "IGameLiftCoreModule.h"
#include <Settings/UGameLiftContainersStatus.h>
#include <Settings/UGameLiftDeploymentStatus.h>
#include <Settings/UGameLiftSettings.h>

#define LOCTEXT_NAMESPACE "SUserInputSection"

void SUserInputSection::Construct(const FArguments& InArgs)
{
    TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom2x)
        [
            CreateMetricsInfoMessage()
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Top_Bottom)
        [
            CreateMetricsCheckBox()
        ];

    VerticalBox->AddSlot()
        [
            SNew(SBox)
                .Visibility_Lambda([this]()
                    {
                        return ShowGameServerPathInput();
                    })
                [
                    CreateGameServerPathInput()
                ]
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom2x)
        [
            CreateOutConfigFilePathInput()
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom2x)
        [
            SNew(SBox)
                .Visibility_Lambda([this]()
                    {
                        return ShowExistingECRRepoDropdown();
                    })
                [
                    CreateExistingECRRepoDropdown()
                ]
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom2x)
        [
            SNew(SBox)
                .Visibility_Lambda([this]()
                    {
                        return ShowExistingECRImageDropdown();
                    })
                [
                    CreateExistingECRImageDropdown()
                ]
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom2x)
        [
            SNew(SBox)
                .Visibility_Lambda([this]()
                    {
                        return ShowDockerRepoInput();
                    })
                [
                    CreateDockerRepoInput()
                ]
        ];

    // Deployment Scenario Choice
    VerticalBox->AddSlot()
        .AutoHeight()
        [
            CreateDeploymentScenarioChoice()
        ];


    // Default Settings
    TSharedPtr<SSectionsWithHeaders> SectionsWithHeaders = SNew(SSectionsWithHeaders).ShowDefaultColor(true);
    SectionsWithHeaders->AddSection(Menu::DeployContainers::kDefaultSettingsHeader, CreateDefaultSettings(), true);

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Top + SPadding::SectionWithHeaderLeftPadding)
        [
            SectionsWithHeaders.ToSharedRef()
        ];

    ChildSlot
        .Padding(SPadding::Top_Bottom)
        [
            VerticalBox
        ];

    if (CanLoadECRRepoDropDown()) {
        LoadRepoInfoAsync();
    }

    if (CanLoadECRImageDropDown()) {
        LoadImageInfoAsync();
    }

    SIntakeQuestionnaireSection::OnDeploymentScenarioStateChangedMultiDelegate.AddSP(this, &SUserInputSection::OnDeploymentScenarioStateChanged);
    SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChangedMultiDelegate.AddSP(this, &SUserInputSection::OnBootstrapStatusChanged);
    SResetDeploymentModal::OnResetDeploymentMultiDelegate.AddSP(this, &SUserInputSection::OnRefreshUI);

    UpdateInitialUI();
}

TSharedRef<SWidget> SUserInputSection::CreateMetricsInfoMessage()
{
    MetricsInfoMessage = SNew(SSetupMessage)
        .InfoText(Menu::DeployCommon::kEnableMetricsInfoText)
        .InfoButtonStyle(Style::Button::kCloseButtonStyleName)
        .OnButtonClicked_Lambda([this]()
            {
                MetricsInfoMessage->SetVisibility(EVisibility::Collapsed);
            })
        .SetState(ESetupMessageState::InfoMessage);
    
        return MetricsInfoMessage.ToSharedRef();
}

TSharedRef<SWidget> SUserInputSection::CreateMetricsCheckBox()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    EnableMetricsCheckBox = SNew(SCheckBox)
        .IsChecked(ContainersStatus->EnableMetrics ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
        .OnCheckStateChanged_Raw(this, &SUserInputSection::OnEnableMetricsChanged);
    
    return SNew(SNamedRow)
        .NameText(Menu::DeployCommon::kEnableMetricsTitle)
        .NameTooltipText(Menu::DeployCommon::kEnableMetricsTooltip)
        .PrimaryColumnWidthOverride(OverridePrimaryColumnWidth)
        .RowWidget(EnableMetricsCheckBox.ToSharedRef());
}

TSharedRef<SWidget> SUserInputSection::CreateGameServerPathInput()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    SAssignNew(GameServerBuildPathInput, SPathInput)
        .Title(Menu::DeployContainers::kGameServerBuildLabel)
        .PathHint(Menu::DeployContainers::kGameServerBuildHint)
        .IsFileSelection(false)
        .ToolTipText(Menu::DeployContainers::kGameServerBuildTooltip)
        .OnPathUpdated(this, &SUserInputSection::OnGameServerBuildPathInputUpdated);
    GameServerBuildPathInput->SetSelectedPath(ContainersStatus->GameServerBuildPath);

    SAssignNew(GameServerExecutablePathInput, SPathInput)
        .Title(Menu::DeployContainers::kGameServerExecutableLabel)
        .PathHint(Menu::DeployContainers::kGameServerExecutableHint)
        .IsFileSelection(true)
        .ToolTipText(Menu::DeployContainers::kGameServerExecutableTooltip)
        .OnPathUpdated(this, &SUserInputSection::OnGameServerExecutablePathInputUpdated);
    GameServerExecutablePathInput->SetSelectedPath(ContainersStatus->GameServerExecutablePath);

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::Bottom)
        [
            SNew(SNamedRow)
                .NameText(Menu::DeployContainers::kGameServerBuildLabel)
                .RowWidget(
                    GameServerBuildPathInput.ToSharedRef()
                )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::Top)
        [
            SNew(SNamedRow)
                .NameText(Menu::DeployContainers::kGameServerExecutableLabel)
                .RowWidget(
                    GameServerExecutablePathInput.ToSharedRef()
                )
        ];
}

TSharedRef<SWidget> SUserInputSection::CreateOutConfigFilePathInput()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    SAssignNew(OutConfigFilePathInput, SPathInput)
        .Title(Menu::DeployCommon::kOutConfigFilePathTitle)
        .PathHint(Menu::DeployCommon::kOutConfigFilePathHint)
        .IsFileSelection(false)
        .ToolTipText(Menu::DeployCommon::kOutConfigFilePathTooltip)
        .OnPathUpdated(this, &SUserInputSection::OnOutConfigFilePathInputUpdated);
    OutConfigFilePathInput->SetSelectedPath(ContainersStatus->OutConfigFilePath);

    return SNew(SNamedRow)
        .NameText(Menu::DeployCommon::kOutConfigFilePathTitle)
        .NameTooltipText(Menu::DeployCommon::kOutConfigFilePathTooltip)
        .RowWidget(OutConfigFilePathInput);
}

TSharedRef<SWidget> SUserInputSection::CreateExistingECRRepoDropdown()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SNamedRow)
                .NameText(Menu::DeployContainers::kSelectECRRepoLabel)
                .RowWidget(
                    SAssignNew(ExistingRepoDropdown, SSelectionComboBox)
                    .UnselectedText((Menu::DeployContainers::kUnselecedRepoDropdownText))
                    .OnListBuilding_Raw(this, &SUserInputSection::OnBuildExistingRepoValues)
                    .OnItemSelected_Raw(this, &SUserInputSection::OnExistingRepoSelected)
                    .IsEnabled_Raw(this, &SUserInputSection::CanLoadECRRepoDropDown)
                    .HideDescription(true)
                )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::Top2x)
        [
            SAssignNew(ErrorRow, SNamedRow)
                .RowWidget(
                    SAssignNew(ErrorTextBlock, STextBlock)
                    .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kNote)
                    .AutoWrapText(true)
                    .ColorAndOpacity(FGameLiftPluginStyle::Get().GetColor(Style::Color::kError))
                )
                .Visibility(EVisibility::Collapsed)
        ];
}

TSharedRef<SWidget> SUserInputSection::CreateExistingECRImageDropdown()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SNamedRow)
                .NameText(Menu::DeployContainers::kSelectECRImageLabel)
                .RowWidget(
                    SAssignNew(ExistingImageDropdown, SSelectionComboBox)
                    .UnselectedText((Menu::DeployContainers::kUnselecedImageDropdownText))
                    .OnListBuilding_Raw(this, &SUserInputSection::OnBuildExistingImageValues)
                    .OnItemSelected_Raw(this, &SUserInputSection::OnExistingImageSelected)
                    .IsEnabled_Raw(this, &SUserInputSection::CanLoadECRImageDropDown)
                )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::Top2x)
        [
            SAssignNew(ImageErrorRow, SNamedRow)
                .RowWidget(
                    SAssignNew(ImageErrorTextBlock, STextBlock)
                    .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kNote)
                    .AutoWrapText(true)
                    .ColorAndOpacity(FGameLiftPluginStyle::Get().GetColor(Style::Color::kError))
                )
                .Visibility(EVisibility::Collapsed)
        ];
}

TSharedRef<SWidget> SUserInputSection::CreateDockerRepoInput()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    SAssignNew(ImageIDInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kImageIDTextHint)
        .OnTextCommitted_Raw(this, &SUserInputSection::OnImageIDInputCommitted);
    ImageIDInput->SetText(ContainersStatus->ImageID);

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::Top)
        [
            SNew(SNamedRow)
                .NameText(Menu::DeployContainers::kImageIDText)
                .RowWidget(
                    ImageIDInput.ToSharedRef()
                )
        ];
}

TSharedRef<SWidget> SUserInputSection::CreateDeploymentScenarioChoice()
{
    TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom)
        [
            SNew(SCheckBox)
                .Style(FCoreStyle::Get(), "RadioButton")
                .IsChecked(this, &SUserInputSection::IsRadioChecked, EDeploymentScenarioFleetRegionType::SingleRegion)
                .OnCheckStateChanged(this, &SUserInputSection::OnRadioChanged, EDeploymentScenarioFleetRegionType::SingleRegion)
                .Content()
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployContainers::kSingleLocationText)
                ]
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom2x)
        [
            SNew(STextBlock)
                .Text(Menu::DeployContainers::kSingleLocationDescription)
                .AutoWrapText(true)
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom)
        [
            SNew(SCheckBox)
                .Style(FCoreStyle::Get(), "RadioButton")
                .IsChecked(this, &SUserInputSection::IsRadioChecked, EDeploymentScenarioFleetRegionType::MultiRegionFlexMatch)
                .OnCheckStateChanged(this, &SUserInputSection::OnRadioChanged, EDeploymentScenarioFleetRegionType::MultiRegionFlexMatch)
                .Content()
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployContainers::kMultiLocationFlexMatchText)
                ]
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom2x)
        [
            SNew(STextBlock)
                .Text(Menu::DeployContainers::kMultiLocationFlexMatchDescription)
                .AutoWrapText(true)
        ];

    return SNew(SNamedRow)
        .NameText(Menu::DeployContainers::kDeploymentScenarioText)
        .FirstColumnVAlignTop(true)
        .RowWidget(
            VerticalBox
        );
}

TSharedRef<SWidget> SUserInputSection::CreateDefaultSettings()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    SAssignNew(GameNameInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kGameNameTextHint)
        .OnTextCommitted_Raw(this, &SUserInputSection::OnGameNameInputCommitted);
    GameNameInput->SetText(ContainersStatus->GameName);

    SAssignNew(ConnectionPortRangeInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kConnectionPortRangeTextHint)
        .OnTextCommitted_Raw(this, &SUserInputSection::OnConnectionPortRangeInputCommitted);
    ConnectionPortRangeInput->SetText(ContainersStatus->ConnectionPortRange);

    SAssignNew(TotalMemoryLimitInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kTotalMemoryLimitTextHint)
        .OnTextCommitted_Raw(this, &SUserInputSection::OnTotalMemoryLimitInputCommitted);
    TotalMemoryLimitInput->SetText(ContainersStatus->TotalMemoryLimit);

    SAssignNew(TotalVCPULimitInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kTotalVCPULimitTextHint)
        .OnTextCommitted_Raw(this, &SUserInputSection::OnTotalVCPULimitInputCommitted);
    TotalVCPULimitInput->SetText(ContainersStatus->TotalVCPULimit);

    SAssignNew(ImageTagInput, SEditableTextBox)
        .OnTextCommitted_Raw(this, &SUserInputSection::OnImageTagInputCommitted);
    ImageTagInput->SetText(ContainersStatus->ImageTag);

    SAssignNew(ECRRepoInput, SEditableTextBox)
        .OnTextCommitted_Raw(this, &SUserInputSection::OnECRRepoInputCommitted);
    ECRRepoInput->SetText(FText::FromString(ContainersStatus->ECRRepoName));

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(SPadding::Top_Bottom)
        .AutoHeight()
        [
            SNew(STextBlock)
                .Text(Menu::DeployContainers::kDefaultSettingsDescription)
                .AutoWrapText(true)
        ]

        + SVerticalBox::Slot()
        .Padding(SPadding::Top_Bottom)
        .AutoHeight()
        [
            SNew(SNamedRow)
                .NameText(Menu::DeployContainers::kGameNameText)
                .PrimaryColumnWidthOverride(OverridePrimaryColumnWidth)
                .RowWidget(
                    GameNameInput.ToSharedRef()
                )
        ]

        + SVerticalBox::Slot()
        .Padding(SPadding::Top_Bottom)
        .AutoHeight()
        [
            SNew(SNamedRow)
                .NameText(Menu::DeployContainers::kConnectionPortRangeText)
                .PrimaryColumnWidthOverride(OverridePrimaryColumnWidth)
                .RowWidget(
                    ConnectionPortRangeInput.ToSharedRef()
                )
        ]

        + SVerticalBox::Slot()
        .Padding(SPadding::Top_Bottom)
        .AutoHeight()
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(SPadding::Right2x)
                [
                    SNew(SNamedRow)
                        .NameText(Menu::DeployContainers::kTotalMemoryLimitText)
                        .PrimaryColumnWidthOverride(OverridePrimaryColumnWidth)
                        .SecondaryColumnWidthOverride(OverrideSecondaryColumnWidth)
                        .RowWidget(
                            TotalMemoryLimitInput.ToSharedRef()
                        )
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(SPadding::Right2x)
                [
                    SNew(STextBlock).Text(Menu::DeployContainers::kTotalMemoryLimitUnit)
                ]
        ]

        + SVerticalBox::Slot()
        .Padding(SPadding::Top_Bottom)
        .AutoHeight()
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(SPadding::Right2x)
                [
                    SNew(SNamedRow)
                        .NameText(Menu::DeployContainers::kTotalVCPULimitText)
                        .PrimaryColumnWidthOverride(OverridePrimaryColumnWidth)
                        .SecondaryColumnWidthOverride(OverrideSecondaryColumnWidth)
                        .RowWidget(
                            TotalVCPULimitInput.ToSharedRef()
                        )
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(SPadding::Right2x)
                [
                    SNew(STextBlock).Text(Menu::DeployContainers::kTotalVCPULimitUnit)
                ]
        ]

        + SVerticalBox::Slot()
        .Padding(SPadding::Top_Bottom)
        .AutoHeight()
        [
            SNew(SBox)
                .Visibility_Lambda([&]()
                    {
                        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                        return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr ? EVisibility::Collapsed : EVisibility::Visible;
                    })
                [
                    SNew(SNamedRow)
                        .NameText(Menu::DeployContainers::kImageTagText)
                        .PrimaryColumnWidthOverride(OverridePrimaryColumnWidth)
                        .RowWidget(
                            ImageTagInput.ToSharedRef()
                        )
                ]
        ]

        + SVerticalBox::Slot()
        .Padding(SPadding::Top_Bottom)
        .AutoHeight()
        [
            SNew(SBox)
                .Visibility_Lambda([&]()
                    {
                        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                        bool IsCollapsed = (ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo ||
                            ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr);
                        return IsCollapsed ? EVisibility::Collapsed : EVisibility::Visible;
                    })
                [
                    SNew(SNamedRow)
                        .NameText(Menu::DeployContainers::kECRRepoNameText)
                        .PrimaryColumnWidthOverride(OverridePrimaryColumnWidth)
                        .RowWidget(
                            ECRRepoInput.ToSharedRef()
                        )
                ]
        ];
}

void SUserInputSection::OnGameServerBuildPathInputUpdated(const FString& NewPath)
{
    if (!NewPath.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->GameServerBuildPath = NewPath;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnGameServerExecutablePathInputUpdated(const FString& NewPath)
{
    if (!NewPath.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->GameServerExecutablePath = NewPath;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnOutConfigFilePathInputUpdated(const FString& NewPath)
{
    if (!NewPath.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->OutConfigFilePath = FText::FromString(NewPath);
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnBuildExistingRepoValues(TArray<FTextIntPair>& Items)
{
    Items.Reset();
    for (int Index = 0; Index < ExistingRepoList.Num(); Index++)
    {
        Items.Add(FTextIntPair(ExistingRepoList[Index].Text, Index + 1, ExistingRepoList[Index].Description));
    }
}

void SUserInputSection::OnExistingRepoSelected(int SelectionId, const FTextIntPair& Item)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ECRRepoName = Item.Text.ToString();
    ContainersStatus->ECRRepoUri = Item.Description.ToString();
    ContainersStatus->ECRImageDigest.Empty();
    ContainersStatus->SaveConfig();

    ErrorRow->SetVisibility(EVisibility::Collapsed);
    ErrorTextBlock->SetText(FText::GetEmpty());
    ExistingRepoDropdown->RequestToRebuildList();
    ExistingRepoDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRRepoName));

    // Reload image dropdown when repo selection changed
    if (CanLoadECRImageDropDown())
    {
        LoadImageInfoAsync();
    }
}

void SUserInputSection::LoadRepoInfoAsync()
{
    IsLoadingRepoInfo = true;
    ErrorRow->SetVisibility(EVisibility::Collapsed);
    ErrorTextBlock->SetText(FText::GetEmpty());
    ExistingRepoDropdown->SetUnselectedText(Menu::DeployContainers::kUnselecedRepoDropdownText);

    Async(EAsyncExecution::Thread,
        [this]()
        {
            // Get list of existing ecr repos
            IGameLiftContainersHandler& Handler = IGameLiftCoreModule::Get().GetGameLiftContainersHandler();
            GameLiftECRDescribeRepositoriesResult DescribeRepoResult = Handler.DescribeRepositories();

            // Update UI on the main thread to avoid Slate errors such as SLATE_CROSS_THREAD_CHECK failure.
            Async(EAsyncExecution::TaskGraphMainThread,
                [this, DescribeRepoResult = MoveTemp(DescribeRepoResult)]
                {
                    OnLoadRepoInfoComplete(DescribeRepoResult);
                });
        });
}

void SUserInputSection::OnLoadRepoInfoComplete(const GameLiftECRDescribeRepositoriesResult& DescribeRepoResult)
{
    IsLoadingRepoInfo = false;
    if (DescribeRepoResult.bIsSuccessful)
    {
        OnAvailableReposChanged(DescribeRepoResult.Repos);
    }
    else
    {
        // Show error row
        ErrorTextBlock->SetText(FText::FromString(DescribeRepoResult.ErrorMessage));
        ErrorRow->SetVisibility(EVisibility::Visible);
    }
}

void SUserInputSection::OnAvailableReposChanged(const TArray<GameLiftECRDescribeRepositoriesResult::RepositoryAttributes>& Repos)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ExistingRepoNameList.Reset();
    ExistingRepoList.Reset();
    for (int Index = 0; Index < Repos.Num(); Index++)
    {
        ExistingRepoList.Add(FTextIntPair(FText::FromString(Repos[Index].RepositoryName), Index + 1, FText::FromString(Repos[Index].RepositoryUri)));
        ContainersStatus->ExistingRepoNameList.Add(FText::FromString(Repos[Index].RepositoryName));
    }
    ExistingRepoDropdown->RequestToRebuildList();
    ExistingRepoDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRRepoName));
    ContainersStatus->SaveConfig();
}

void SUserInputSection::OnBuildExistingImageValues(TArray<FTextIntPair>& Items)
{
    Items.Reset();
    for (int Index = 0; Index < ExistingImageList.Num(); Index++)
    {
        Items.Add(FTextIntPair(ExistingImageList[Index].Text, Index + 1, ExistingImageList[Index].Description));
    }
}

void SUserInputSection::OnExistingImageSelected(int SelectionId, const FTextIntPair& Item)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ECRImageDigest = Item.Text.ToString();
    ContainersStatus->ImageTag = Item.Description;
    ContainersStatus->SaveConfig();
    ImageErrorRow->SetVisibility(EVisibility::Collapsed);
    ImageErrorTextBlock->SetText(FText::GetEmpty());
    ExistingImageDropdown->RequestToRebuildList();
    ExistingImageDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRImageDigest));
}

void SUserInputSection::LoadImageInfoAsync()
{
    IsLoadingImageInfo = true;
    ImageErrorRow->SetVisibility(EVisibility::Collapsed);
    ImageErrorTextBlock->SetText(FText::GetEmpty());
    ExistingImageDropdown->SetUnselectedText(Menu::DeployContainers::kUnselecedImageDropdownText);

    Async(EAsyncExecution::Thread,
        [this]()
        {
            UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
            if (!ContainersStatus->ECRRepoName.IsEmpty())
            {
                // Get list of existing ecr images
                IGameLiftContainersHandler& Handler = IGameLiftCoreModule::Get().GetGameLiftContainersHandler();
                GameLiftECRListImagesResult ListImageResult = Handler.ListImages(ContainersStatus->ECRRepoName);

                Async(EAsyncExecution::TaskGraphMainThread,
                    [this, ListImageResult = MoveTemp(ListImageResult)]
                    {
                        OnLoadImageInfoComplete(ListImageResult);
                    });
            }
        });
}

void SUserInputSection::OnLoadImageInfoComplete(const GameLiftECRListImagesResult& ListImageResult)
{
    IsLoadingImageInfo = false;
    if (ListImageResult.bIsSuccessful)
    {
        OnAvailableImagesChanged(ListImageResult.Images);
    }
    else
    {
        ImageErrorTextBlock->SetText(FText::FromString(ListImageResult.ErrorMessage));
        ImageErrorRow->SetVisibility(EVisibility::Visible);
    }
}

void SUserInputSection::OnAvailableImagesChanged(const TArray<GameLiftECRListImagesResult::ImageDetailAttributes>& Images)
{
    ExistingImageList.Reset();
    for (int Index = 0; Index < Images.Num(); Index++)
    {
        ExistingImageList.Add(FTextIntPair(FText::FromString(Images[Index].ImageDigest), Index + 1, FText::FromString(Images[Index].ImageTag)));
    }
    ExistingImageDropdown->RequestToRebuildList();
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ExistingImageDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRImageDigest));
}

bool SUserInputSection::CanLoadECRImageDropDown() const
{
    UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    return EBootstrapMessageStateFromInt(Settings->BootstrapStatus) == EBootstrapMessageState::ActiveMessage &&
        ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr &&
        !ContainersStatus->ECRRepoName.IsEmpty();
}


void SUserInputSection::OnDeploymentScenarioStateChanged(const SIntakeQuestionnaireSection*)
{
    // Reset ECRRepoName field to force select existing ECR Repo and avoid using
    // customer provided/default input, leading to RepositoryNotFound exceptions
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    if (CanLoadECRRepoDropDown())
    {
        ContainersStatus->ECRRepoName.Empty();
        ContainersStatus->ECRRepoUri.Empty();
        ContainersStatus->ECRImageDigest.Empty();
    }
    else
    {
        ContainersStatus->ECRRepoName = (Menu::DeployContainers::kECRRepoNameDefault).ToString();
    }
    ContainersStatus->SaveConfig();

    OnRefreshUI();
}

void SUserInputSection::OnRefreshUI()
{
    UpdateInitialUI();

    if (CanLoadECRRepoDropDown())
    {
        LoadRepoInfoAsync();
    }

    if (CanLoadECRImageDropDown())
    {
        LoadImageInfoAsync();
    }
}

void SUserInputSection::OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu*)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ExistingRepoList.Reset();
    ContainersStatus->ExistingRepoNameList.Reset();
    ContainersStatus->SaveConfig();
    ExistingRepoDropdown->ClearSelection();
    ExistingRepoDropdown->RequestToRebuildList();
    ExistingRepoDropdown->SetUnselectedText(Menu::DeployContainers::kUnselecedRepoDropdownText);

    ExistingImageList.Reset();
    ExistingImageDropdown->ClearSelection();
    ExistingImageDropdown->RequestToRebuildList();
    ExistingImageDropdown->SetUnselectedText(Menu::DeployContainers::kUnselecedImageDropdownText);

    UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
    EBootstrapMessageState BootstrapState = EBootstrapMessageStateFromInt(Settings->BootstrapStatus);
    if (BootstrapState == EBootstrapMessageState::NoBootstrappedMessage
        || BootstrapState == EBootstrapMessageState::ActiveMessage)
    {
        if (CanLoadECRRepoDropDown())
        {
            LoadRepoInfoAsync();
        }

        if (CanLoadECRImageDropDown())
        {
            LoadImageInfoAsync();
        }
    }
}

bool SUserInputSection::CanLoadECRRepoDropDown() const
{
    UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    return EBootstrapMessageStateFromInt(Settings->BootstrapStatus) == EBootstrapMessageState::ActiveMessage &&
        (ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo ||
            ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr);
}

void SUserInputSection::OnImageIDInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    if (!NewInput.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->ImageID = NewInput;
        ContainersStatus->SaveConfig();
    }
}

ECheckBoxState SUserInputSection::IsRadioChecked(EDeploymentScenarioFleetRegionType RadioButtonChoice) const
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return (ContainersStatus->DeploymentDetailRegionChoice == (int)RadioButtonChoice) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SUserInputSection::OnRadioChanged(ECheckBoxState NewRadioState, EDeploymentScenarioFleetRegionType RadioButtonChoice)
{
    if (NewRadioState == ECheckBoxState::Checked)
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->DeploymentDetailRegionChoice = (int)RadioButtonChoice;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnGameNameInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    if (!NewInput.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->GameName = NewInput;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnConnectionPortRangeInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    if (!NewInput.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->ConnectionPortRange = NewInput;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnTotalMemoryLimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    if (!NewInput.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->TotalMemoryLimit = NewInput;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnTotalVCPULimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    if (!NewInput.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->TotalVCPULimit = NewInput;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnImageTagInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    if (!NewInput.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->ImageTag = NewInput;
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnECRRepoInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    if (!NewInput.IsEmpty())
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->ECRRepoName = NewInput.ToString();
        ContainersStatus->SaveConfig();
    }
}

void SUserInputSection::OnEnableMetricsChanged(ECheckBoxState NewState)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->EnableMetrics = (NewState == ECheckBoxState::Checked);
    ContainersStatus->SaveConfig();
}

void SUserInputSection::UpdateInitialUI()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    GameServerBuildPathInput->SetSelectedPath(ContainersStatus->GameServerBuildPath);
    GameServerExecutablePathInput->SetSelectedPath(ContainersStatus->GameServerExecutablePath);
    GameNameInput->SetText(ContainersStatus->GameName);
    ConnectionPortRangeInput->SetText(ContainersStatus->ConnectionPortRange);
    TotalVCPULimitInput->SetText(ContainersStatus->TotalVCPULimit);
    TotalMemoryLimitInput->SetText(ContainersStatus->TotalMemoryLimit);
    ImageTagInput->SetText(ContainersStatus->ImageTag);
    ECRRepoInput->SetText(FText::FromString(ContainersStatus->ECRRepoName));
    EnableMetricsCheckBox->SetIsChecked(ContainersStatus->EnableMetrics ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

    ErrorRow->SetVisibility(EVisibility::Collapsed);
    ErrorTextBlock->SetText(FText::GetEmpty());
    ExistingRepoDropdown->ClearSelection();
    ExistingRepoDropdown->SetUnselectedText(Menu::DeployContainers::kUnselecedRepoDropdownText);

    ImageErrorRow->SetVisibility(EVisibility::Collapsed);
    ImageErrorTextBlock->SetText(FText::GetEmpty());
    ExistingImageDropdown->ClearSelection();
    ExistingImageDropdown->SetUnselectedText(Menu::DeployContainers::kUnselecedImageDropdownText);

    ExistingRepoDropdown->RequestToRebuildList();
    ExistingRepoDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRRepoName));

    ExistingImageDropdown->RequestToRebuildList();
    ExistingImageDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRImageDigest));
}

EVisibility SUserInputSection::ShowGameServerPathInput()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo ||
        ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SUserInputSection::ShowExistingECRRepoDropdown()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr ||
        ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SUserInputSection::ShowDockerRepoInput()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInDocker ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SUserInputSection::ShowExistingECRImageDropdown()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE