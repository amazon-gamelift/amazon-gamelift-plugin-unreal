// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SUpdateDeploymentModal.h"

#include "IGameLiftCoreModule.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "SGameLiftDeployContainersMenu.h"
#include "Settings/UGameLiftContainersStatus.h"
#include "Settings/UGameLiftSettings.h"
#include "Types/FTextIntPair.h"

#define LOCTEXT_NAMESPACE "SUpdateDeploymentModal"

FUpdateDeploymemt SUpdateDeploymentModal::OnUpdateDeploymentMultiDelegate;

void SUpdateDeploymentModal::Construct(const FArguments& InArgs)
{
    OwnerWindow = InArgs._OwnerWindow;

    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    SAssignNew(ImageTagInput, SEditableTextBox)
        .OnTextCommitted_Raw(this, &SUpdateDeploymentModal::OnImageTagInputCommitted);
    ImageTagInput->SetText(ContainersStatus->ImageTag);
    SAssignNew(ConnectionPortRangeInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kConnectionPortRangeTextHint)
        .OnTextCommitted_Raw(this, &SUpdateDeploymentModal::OnConnectionPortRangeInputCommitted);
    ConnectionPortRangeInput->SetText(ContainersStatus->ConnectionPortRange);
    SAssignNew(TotalMemoryLimitInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kTotalMemoryLimitTextHint)
        .OnTextCommitted_Raw(this, &SUpdateDeploymentModal::OnTotalMemoryLimitInputCommitted);
    TotalMemoryLimitInput->SetText(ContainersStatus->TotalMemoryLimit);
    SAssignNew(TotalVCPULimitInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kTotalVCPULimitTextHint)
        .OnTextCommitted_Raw(this, &SUpdateDeploymentModal::OnTotalVCPULimitInputCommitted);
    TotalVCPULimitInput->SetText(ContainersStatus->TotalVCPULimit);
    SAssignNew(ImageIDInput, SEditableTextBox)
        .HintText(Menu::DeployContainers::kImageIDTextHint)
        .OnTextCommitted_Raw(this, &SUpdateDeploymentModal::OnImageIDInputCommitted);
    ImageIDInput->SetText(ContainersStatus->ImageID);

    ChildSlot
        [
            SNew(SVerticalBox)
                // Modal title and close button
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(SPadding::Left4x + SPadding::Right4x + SPadding::Top2x_Bottom2x)
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .HAlign(HAlign_Left)
                        [
                            SNew(STextBlock)
                                .Text(Menu::DeployContainers::kUpdateFleetDeploymentTitle)
                                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Center)
                        [
                            SNew(SButton)
                                .OnClicked(this, &SUpdateDeploymentModal::OnCloseButtonClicked)
                                .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kCloseButtonStyleName)
                        ]
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBorder)
                        .BorderBackgroundColor(FColor::Black)
                        .BorderImage(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kBorderImageName))
                        .Padding(SPadding::Left4x + SPadding::Right4x + SPadding::Top2x_Bottom2x)
                        [
                            // Update deployment description
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(STextBlock)
                                    .Text(Menu::DeployContainers::kUpdateFleetDeploymentDescription)
                                    .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                                    .AutoWrapText(true)

                            ]
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(SOnlineHyperlink)
                                    .Text(Settings::AwsAccount::kAWSFreeTierLinkText)
                                    .Link(Settings::AwsAccount::kAWSFreeTierLinkUrl)

                            ]
                            // Update deployment setup message warning
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top2x_Bottom2x)
                            [
                                CreateUpdateFleetDeploymentWarningMessage()
                            ]
                            // deployment details title
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(STextBlock)
                                    .Text(Menu::DeployContainers::kDeploymentDetailsTitle)
                                    .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                                    .AutoWrapText(true)
                            ]
                            // Game server directory and game server executable input 
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(SBox)
                                    .Visibility_Lambda([&]()
                                        {
                                            UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                                            return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo || ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo ? EVisibility::Visible : EVisibility::Collapsed;
                                        })
                                    [
                                        CreateGameServerPathInput()
                                    ]
                            ]
                            // Image id input
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(SNamedRow)
                                    .NameText(Menu::DeployContainers::kImageIDText)
                                    .Visibility_Lambda([&]()
                                        {
                                            UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                                            return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInDocker ? EVisibility::Visible : EVisibility::Collapsed;
                                        })
                                    .RowWidget(
                                        ImageIDInput.ToSharedRef()
                                    )
                            ]
                            // Select ecr repo input
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                CreateExistingECRRepoDropdown()
                            ]
                            // Select image from ecr repo input
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                CreateExistingECRImageDropdown()
                            ]
                            // Deployment scenario choice input
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                CreateDeploymentScenarioChoice()
                            ]
                            // Connection port range input
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(SNamedRow)
                                    .NameText(Menu::DeployContainers::kConnectionPortRangeText)
                                    .RowWidget(
                                        ConnectionPortRangeInput.ToSharedRef()
                                    )
                            ]
                            // Total memory limit input
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
                            // Total cpu limit input
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
                            // Image tag input
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom2x)
                            [
                                SNew(SNamedRow)
                                    .NameText(Menu::DeployContainers::kImageTagText)
                                    .RowWidget(
                                        ImageTagInput.ToSharedRef()
                                    )
                                    .Visibility_Lambda([&]()
                                        {
                                            UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                                            return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr ? EVisibility::Collapsed : EVisibility::Visible;
                                        })
                            ]
                            // Buttons
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top2x_Bottom2x)
                            [
                                SNew(SHorizontalBox)
                                    + SHorizontalBox::Slot()
                                    .AutoWidth()
                                    .HAlign(HAlign_Left)
                                    [
                                        CreateAWSConsoleButton()
                                    ]
                                    + SHorizontalBox::Slot()
                                    .FillWidth(1)
                                    .HAlign(HAlign_Right)
                                    .VAlign(VAlign_Center)
                                    .Padding(SPadding::Right2x)
                                    [
                                        SNew(SButton)
                                            .Text(Settings::AwsAccount::kCancelButtonText)
                                            .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
                                            .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
                                            .ContentPadding(SPadding::LaunchBarButtonPadding)
                                            .OnClicked_Lambda([&]
                                                {
                                                    OnCloseButtonClicked();
                                                    return FReply::Handled();
                                                })
                                    ]
                                    + SHorizontalBox::Slot()
                                    .AutoWidth()
                                    .HAlign(HAlign_Right)
                                    .VAlign(VAlign_Center)
                                    [
                                        SNew(SButton)
                                            .Text(Menu::DeployContainers::kUpdateButtonText)
                                            .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
                                            .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
                                            .ContentPadding(SPadding::LaunchBarButtonPadding)
                                            .OnClicked_Lambda([this]
                                                {
                                                    OnUpdateDeploymentClicked();
                                                    return FReply::Handled();
                                                })
                                    ]
                            ]
                        ]
                ]
        ];
    ResetAllUpdateFields();
    SetDeploymentRegionChoice();
    SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChangedMultiDelegate.AddSP(this, &SUpdateDeploymentModal::OnBootstrapStatusChanged);
    if (CanLoadECRRepoDropDown()) {
        LoadRepoInfoAsync();
    }
    if (CanLoadECRImageDropDown()) {
        LoadImageInfoAsync();
    }
}

TSharedRef<SWidget> SUpdateDeploymentModal::CreateUpdateFleetDeploymentWarningMessage() {
    TSharedPtr<SHorizontalBox> RowWidget = SNew(SHorizontalBox);
    RowWidget->AddSlot()
        .FillWidth(1.0f)
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
                .Text(Menu::DeployContainers::kUpdateWarningText)
                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                .AutoWrapText(true)
        ];

    RowWidget->AddSlot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .Padding(SPadding::All)
        [
            SNew(SButton)
                .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
                .OnClicked_Lambda([]
                    {
                        FPlatformProcess::LaunchURL(*FString(Url::kLearnMoreAboutContainerFleetUpdateLinkUrl), nullptr, nullptr);
                        return FReply::Handled();
                    })
                .Content()
                [
                    SNew(SBox)
                        .HeightOverride(OverrideButtonHeight)
                        .HAlign(HAlign_Center)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock)
                                        .Text(Settings::UseCase::kUseCaseLearnMoreText)
                                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
                                ]
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(SPadding::Left)
                                .HAlign(HAlign_Left)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(SBox)
                                        .WidthOverride(Style::kLinkIconSize)
                                        .HeightOverride(Style::kLinkIconSize)
                                        [
                                            SNew(SImage)
                                                .Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kExternalLinkIconName))
#if(ENGINE_MAJOR_VERSION == 4)
                                                .ColorAndOpacity(FColor::Black)
#endif
                                        ]
                                ]
                        ]
                ]
        ];

    return SAssignNew(UpdateFleetDeploymentSetupMessage, SSetupMessage)
        .WarningRowWidget(RowWidget)
        .WarningButtonStyle(Style::Button::kCloseButtonStyleName)
        .OnButtonClicked_Lambda([&]
            {
                UpdateFleetDeploymentSetupMessage->SetVisibility(EVisibility::Collapsed);
            });
}

void SUpdateDeploymentModal::OnUpdateDeploymentClicked() {
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    UpdateDeploymentConfigurations();
    if (OwnerWindow.IsValid())
    {
        OwnerWindow.Pin()->RequestDestroyWindow();
    }
    ContainersStatus->IsUpdateContainerFleetModalOpen = false;
    ContainersStatus->SaveConfig();
    OnUpdateDeploymentMultiDelegate.Broadcast();
    SGameLiftDeployContainersMenu::OnModalClosed.ExecuteIfBound();
}

void SUpdateDeploymentModal::UpdateDeploymentConfigurations() {
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->GameServerBuildPath = ContainersStatus->GameServerBuildPathToUpdate;
    ContainersStatus->GameServerExecutablePath = ContainersStatus->GameServerExecutablePathToUpdate;
    ContainersStatus->ImageID = ContainersStatus->ImageIDToUpdate;
    ContainersStatus->ECRRepoName = ContainersStatus->ECRRepoNameToUpdate;
    ContainersStatus->ECRRepoUri = ContainersStatus->ECRRepoUriToUpdate;
    ContainersStatus->ECRImageDigest = ContainersStatus->ECRImageDigestToUpdate;
    ContainersStatus->ImageTag = ContainersStatus->ImageTagToUpdate;
    ContainersStatus->ConnectionPortRange = ContainersStatus->ConnectionPortRangeToUpdate;
    ContainersStatus->TotalMemoryLimit = ContainersStatus->TotalMemoryLimitToUpdate;
    ContainersStatus->TotalVCPULimit = ContainersStatus->TotalVCPULimitToUpdate;
    ContainersStatus->DeploymentDetailRegionChoice = ContainersStatus->DeploymentDetailRegionChoiceToUpdate;
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::ResetAllUpdateFields() {
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->GameServerBuildPathToUpdate = ContainersStatus->GameServerBuildPath;
    ContainersStatus->GameServerExecutablePathToUpdate = ContainersStatus->GameServerExecutablePath;
    ContainersStatus->ImageIDToUpdate = ContainersStatus->ImageID;
    ContainersStatus->ECRRepoNameToUpdate = ContainersStatus->ECRRepoName;
    ContainersStatus->ECRRepoUriToUpdate = ContainersStatus->ECRRepoUri;
    ContainersStatus->ECRImageDigestToUpdate = ContainersStatus->ECRImageDigest;
    ContainersStatus->ImageTagToUpdate = ContainersStatus->ImageTag;
    ContainersStatus->ConnectionPortRangeToUpdate = ContainersStatus->ConnectionPortRange;
    ContainersStatus->TotalMemoryLimitToUpdate = ContainersStatus->TotalMemoryLimit;
    ContainersStatus->TotalVCPULimitToUpdate = ContainersStatus->TotalVCPULimit;
    ContainersStatus->DeploymentDetailRegionChoiceToUpdate = ContainersStatus->DeploymentDetailRegionChoice;
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::SetDeploymentRegionChoice() {
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    if (ContainersStatus->DeploymentDetailRegionChoice == 0) {
        SingleRegionCheckBox->SetIsChecked(ECheckBoxState::Checked);
        MultiRegionFlexMatchCheckBox->SetIsChecked(ECheckBoxState::Unchecked);
    }
    else {
        SingleRegionCheckBox->SetIsChecked(ECheckBoxState::Unchecked);
        MultiRegionFlexMatchCheckBox->SetIsChecked(ECheckBoxState::Checked);
    }
    ContainersStatus->DeploymentDetailRegionChoiceToUpdate = ContainersStatus->DeploymentDetailRegionChoice;
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::OnBuildImagesListForCurrentRepo(TArray<FTextIntPair>& Items)
{
    // Items are entries to display for the selection combo box, populate the array with CurrentECRRepoImagesList
    Items.Reset();
    for (int Index = 0; Index < CurrentECRRepoImagesList.Num(); Index++)
    {
        // Text is set to image digest, and Description is set to image tag. Both will be displayed in the selection dropdown
        Items.Add(FTextIntPair(CurrentECRRepoImagesList[Index].Text, Index + 1, CurrentECRRepoImagesList[Index].Description));
    }
}

void SUpdateDeploymentModal::OnExistingImageSelected(int SelectionId, const FTextIntPair& Item)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ECRImageDigestToUpdate = Item.Text.ToString();
    ContainersStatus->ImageTagToUpdate = Item.Description;
    ContainersStatus->SaveConfig();

    ImageErrorRow->SetVisibility(EVisibility::Collapsed);
    ImageErrorTextBlock->SetText(FText::GetEmpty());
    ExistingImageDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRImageDigestToUpdate));
}

bool SUpdateDeploymentModal::CanLoadECRImageDropDown() const
{
    UGameLiftSettings* GameLiftSettings = GetMutableDefault<UGameLiftSettings>();
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    return EBootstrapMessageStateFromInt(GameLiftSettings->BootstrapStatus) == EBootstrapMessageState::ActiveMessage &&
        ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr &&
        !ContainersStatus->ECRRepoName.IsEmpty();
}

TSharedRef<SWidget> SUpdateDeploymentModal::CreateExistingECRImageDropdown()
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
                    .OnListBuilding_Raw(this, &SUpdateDeploymentModal::OnBuildImagesListForCurrentRepo)
                    .OnItemSelected_Raw(this, &SUpdateDeploymentModal::OnExistingImageSelected)
                    .IsEnabled_Raw(this, &SUpdateDeploymentModal::CanLoadECRImageDropDown)
                )
                .Visibility_Lambda([&]()
                    {
                        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                        return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr ? EVisibility::Visible : EVisibility::Collapsed;
                    })
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

TSharedRef<SWidget> SUpdateDeploymentModal::CreateDeploymentScenarioChoice()
{
    TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);
    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom)
        [
            SAssignNew(SingleRegionCheckBox, SCheckBox)
                .Style(FCoreStyle::Get(), "RadioButton")
                .IsChecked(this, &SUpdateDeploymentModal::IsRadioChecked, EDeploymentScenarioFleetRegionType::SingleRegion)
                .OnCheckStateChanged(this, &SUpdateDeploymentModal::OnRadioChanged, EDeploymentScenarioFleetRegionType::SingleRegion)
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
        ];

    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Bottom)
        [
            SAssignNew(MultiRegionFlexMatchCheckBox, SCheckBox)
                .Style(FCoreStyle::Get(), "RadioButton")
                .IsChecked(this, &SUpdateDeploymentModal::IsRadioChecked, EDeploymentScenarioFleetRegionType::MultiRegionFlexMatch)
                .OnCheckStateChanged(this, &SUpdateDeploymentModal::OnRadioChanged, EDeploymentScenarioFleetRegionType::MultiRegionFlexMatch)
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
        ];

    return SNew(SNamedRow)
        .NameText(Menu::DeployContainers::kDeploymentScenarioText)
        .FirstColumnVAlignTop(true)
        .RowWidget(
            VerticalBox
        );
}

ECheckBoxState SUpdateDeploymentModal::IsRadioChecked(EDeploymentScenarioFleetRegionType RadioButtonChoice) const
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return (ContainersStatus->DeploymentDetailRegionChoiceToUpdate == (int)RadioButtonChoice) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool SUpdateDeploymentModal::CanLoadECRRepoDropDown() const
{
    UGameLiftSettings* GameLiftSettings = GetMutableDefault<UGameLiftSettings>();
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    return EBootstrapMessageStateFromInt(GameLiftSettings->BootstrapStatus) == EBootstrapMessageState::ActiveMessage &&
        (ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo ||
            ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr);
}

void SUpdateDeploymentModal::OnLoadRepoInfoComplete(const GameLiftECRDescribeRepositoriesResult& DescribeRepoResult)
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

void SUpdateDeploymentModal::OnAvailableReposChanged(const TArray<GameLiftECRDescribeRepositoriesResult::RepositoryAttributes>& Repos)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ExistingRepoList.Reset();
    for (int Index = 0; Index < Repos.Num(); Index++)
    {
        ExistingRepoList.Add(FTextIntPair(FText::FromString(Repos[Index].RepositoryName), Index + 1, FText::FromString(Repos[Index].RepositoryUri)));
    }
    ExistingRepoDropdown->RequestToRebuildList();
    ExistingRepoDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRRepoNameToUpdate));
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::LoadRepoInfoAsync()
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

void SUpdateDeploymentModal::OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu*)
{
    OnCloseButtonClicked();
}

void SUpdateDeploymentModal::LoadImageInfoAsync()
{
    IsLoadingImageInfo = true;
    ImageErrorRow->SetVisibility(EVisibility::Collapsed);
    ImageErrorTextBlock->SetText(FText::GetEmpty());
    ExistingImageDropdown->SetUnselectedText(Menu::DeployContainers::kUnselecedImageDropdownText);

    Async(EAsyncExecution::Thread,
        [this]()
        {
            UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
            if (!ContainersStatus->ECRRepoNameToUpdate.IsEmpty())
            {
                // Get list of existing ecr images
                IGameLiftContainersHandler& Handler = IGameLiftCoreModule::Get().GetGameLiftContainersHandler();
                GameLiftECRListImagesResult ListImageResult = Handler.ListImages(ContainersStatus->ECRRepoNameToUpdate);

                Async(EAsyncExecution::TaskGraphMainThread,
                    [this, ListImageResult = MoveTemp(ListImageResult)]
                    {
                        OnLoadImageInfoComplete(ListImageResult);
                    });
            }
        });
}

void SUpdateDeploymentModal::OnLoadImageInfoComplete(const GameLiftECRListImagesResult& ListImageResult)
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

void SUpdateDeploymentModal::OnAvailableImagesChanged(const TArray<GameLiftECRListImagesResult::ImageDetailAttributes>& Images)
{
    CurrentECRRepoImagesList.Reset();
    for (int Index = 0; Index < Images.Num(); Index++)
    {
        CurrentECRRepoImagesList.Add(FTextIntPair(FText::FromString(Images[Index].ImageDigest), Index + 1, FText::FromString(Images[Index].ImageTag)));
    }
    ExistingImageDropdown->RequestToRebuildList();
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ExistingImageDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRImageDigestToUpdate));
}

void SUpdateDeploymentModal::OnRadioChanged(ECheckBoxState NewRadioState, EDeploymentScenarioFleetRegionType RadioButtonChoice)
{
    if (NewRadioState == ECheckBoxState::Checked)
    {
        SingleRegionCheckBox->SetIsChecked(ECheckBoxState::Unchecked);
        MultiRegionFlexMatchCheckBox->SetIsChecked(ECheckBoxState::Unchecked);
        if (RadioButtonChoice == EDeploymentScenarioFleetRegionType::SingleRegion) {
            SingleRegionCheckBox->SetIsChecked(ECheckBoxState::Checked);
        }
        else {
            MultiRegionFlexMatchCheckBox->SetIsChecked(ECheckBoxState::Checked);
        }
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->DeploymentDetailRegionChoiceToUpdate = (int)RadioButtonChoice;
        ContainersStatus->SaveConfig();
    }
}

TSharedRef<SWidget> SUpdateDeploymentModal::CreateExistingECRRepoDropdown()
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
                    .OnListBuilding_Raw(this, &SUpdateDeploymentModal::OnBuildExistingRepoValues)
                    .OnItemSelected_Raw(this, &SUpdateDeploymentModal::OnExistingRepoSelected)
                    .HideDescription(true)
                )
                .Visibility_Lambda([&]()
                    {
                        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                        return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr || ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo ? EVisibility::Visible : EVisibility::Collapsed;
                    })
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

void SUpdateDeploymentModal::OnBuildExistingRepoValues(TArray<FTextIntPair>& Items)
{
    Items.Reset();
    for (int Index = 0; Index < ExistingRepoList.Num(); Index++)
    {
        Items.Add(FTextIntPair(ExistingRepoList[Index].Text, Index + 1, ExistingRepoList[Index].Description));
    }
}

void SUpdateDeploymentModal::OnExistingRepoSelected(int SelectionId, const FTextIntPair& Item)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ECRRepoNameToUpdate = Item.Text.ToString();
    FString ECRRegistryUrl;
    FString OriginalRepo;
    ContainersStatus->ECRRepoUri.Split(TEXT("/"), &ECRRegistryUrl, &OriginalRepo);
    ECRRegistryUrl += "/";
    ECRRegistryUrl += Item.Text.ToString();
    ContainersStatus->ECRRepoUriToUpdate = ECRRegistryUrl;
    ContainersStatus->SaveConfig();

    ExistingRepoDropdown->SetSelectedByName(FText::FromString(ContainersStatus->ECRRepoNameToUpdate));

    if (CanLoadECRImageDropDown())
    {
        LoadImageInfoAsync();
    }
}

TSharedRef<SWidget> SUpdateDeploymentModal::CreateGameServerPathInput()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

    SAssignNew(GameServerBuildPathInput, SPathInput)
        .Title(Menu::DeployContainers::kGameServerBuildLabel)
        .PathHint(Menu::DeployContainers::kGameServerBuildHint)
        .IsFileSelection(false)
        .ToolTipText(Menu::DeployContainers::kGameServerBuildTooltip)
        .OnPathUpdated(this, &SUpdateDeploymentModal::OnGameServerBuildPathInputUpdated);
    GameServerBuildPathInput->SetSelectedPath(ContainersStatus->GameServerBuildPath);

    SAssignNew(GameServerExecutablePathInput, SPathInput)
        .Title(Menu::DeployContainers::kGameServerExecutableLabel)
        .PathHint(Menu::DeployContainers::kGameServerExecutableHint)
        .IsFileSelection(true)
        .ToolTipText(Menu::DeployContainers::kGameServerExecutableTooltip)
        .OnPathUpdated(this, &SUpdateDeploymentModal::OnGameServerExecutablePathInputUpdated);
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

void SUpdateDeploymentModal::OnImageIDInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ImageIDToUpdate = NewInput;
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::OnGameServerBuildPathInputUpdated(const FString& NewPath)
{
    
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->GameServerBuildPathToUpdate = NewPath;
    ContainersStatus->SaveConfig();
   
    OwnerWindow.Pin()->BringToFront(false);
}

void SUpdateDeploymentModal::OnGameServerExecutablePathInputUpdated(const FString& NewPath)
{
    
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->GameServerExecutablePathToUpdate = NewPath;
    ContainersStatus->SaveConfig();
    
    OwnerWindow.Pin()->BringToFront(false);
}



TSharedRef<SWidget> SUpdateDeploymentModal::CreateAWSConsoleButton()
{
    return SNew(SButton)
        .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
        .OnClicked_Lambda([]
            {
                UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
                FPlatformProcess::LaunchURL(*FString::Format(Url::kAWSConsoleUrlContainerFleetFormat, { Settings->AwsRegion.ToString() }), nullptr, nullptr);
                return FReply::Handled();
            })
        .Content()
        [
            SNew(SBox)
                .HeightOverride(OverrideButtonHeight)
                .HAlign(HAlign_Center)
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                                .Text(Menu::DeployContainers::kAWSConsoleButtonText)
                                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(SPadding::Left)
                        .HAlign(HAlign_Left)
                        .VAlign(VAlign_Center)
                        [
                            SNew(SBox)
                                .WidthOverride(Style::kLinkIconSize)
                                .HeightOverride(Style::kLinkIconSize)
                                [
                                    SNew(SImage)
                                        .Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kExternalLinkIconName))
#if(ENGINE_MAJOR_VERSION == 4)
                                        .ColorAndOpacity(FColor::Black)
#endif
                                ]
                        ]
                ]
        ];
}


void SUpdateDeploymentModal::OnImageTagInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ImageTagToUpdate = NewInput;
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::OnConnectionPortRangeInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->ConnectionPortRangeToUpdate = NewInput;
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::OnTotalMemoryLimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->TotalMemoryLimitToUpdate = NewInput;
    ContainersStatus->SaveConfig();
}

void SUpdateDeploymentModal::OnTotalVCPULimitInputCommitted(const FText& NewInput, ETextCommit::Type InCommitInfo)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->TotalVCPULimitToUpdate = NewInput;
    ContainersStatus->SaveConfig();
}

FReply SUpdateDeploymentModal::OnCloseButtonClicked()
{
    if (OwnerWindow.IsValid())
    {
        OwnerWindow.Pin()->RequestDestroyWindow();
    }

    SGameLiftDeployContainersMenu::OnModalClosed.ExecuteIfBound();

    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ResetAllUpdateFields();
    ContainersStatus->IsUpdateContainerFleetModalOpen = false;
    ContainersStatus->SaveConfig();
    return FReply::Handled();
}

void SUpdateDeploymentModal::SetOwnerWindow(TSharedPtr<SWindow> NewOwnerWindow)
{
    OwnerWindow = NewOwnerWindow;
}

#undef LOCTEXT_NAMESPACE