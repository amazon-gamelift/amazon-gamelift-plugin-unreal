// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SClientBuildModal.h"

#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "SWidgets/SPathInput.h"
#include "SWidgets/SNamedRow.h"

#define LOCTEXT_NAMESPACE "SClientBuildModal"

void SClientBuildModal::Construct(const FArguments& InArgs)
{
    ClientBuildExecutablePath = InArgs._DefaultClientBuildExecutablePath;
    ClientBuildLauncherArguments = InArgs._DefaultClientBuildLauncherArguments;
    OnStartClientClickedDelegate = InArgs._OnStartClientClickedDelegate;
    ParentWidget = InArgs._ParentWidget;

    ChildSlot
        [
            SAssignNew(OwnerWindow, SWindow)
                .IsPopupWindow(true)
                .SupportsMinimize(false)
                .SupportsMaximize(false)
                .bDragAnywhere(true)
                .SizingRule(ESizingRule::Autosized)
                .HasCloseButton(true)
                .Content()
                [
                    SNew(SBox)
                        .MinDesiredWidth(ModalWindowWidth)
                        [
                            SNew(SVerticalBox)
                                // Client modal title and close button
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
                                                .Text(Menu::DeployCommon::kClientBuildModalHeader)
                                                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                                        ]
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .HAlign(HAlign_Right)
                                        .VAlign(VAlign_Center)
                                        [
                                            SNew(SButton)
                                                .OnClicked(this, &SClientBuildModal::OnCloseButtonClicked)
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
                                            SNew(SVerticalBox)
                                                // Client modal description
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(SPadding::Top_Bottom)
                                                [
                                                    SNew(SRichTextBlock)
                                                        .Text(Menu::DeployCommon::kClientBuildModalDescription)
                                                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                                                        .AutoWrapText(true)

                                                ]
                                                // Game client build executable input
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(SPadding::Top_Bottom2x)
                                                [
                                                    CreateGameClientPathInput()
                                                ]
                                                // Game client launcher args input
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(SPadding::Top_Bottom2x)
                                                [
                                                    CreateGameClientLauncherArgumentsInput()
                                                ]
                                                // Buttons
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(SPadding::Top2x_Bottom2x)
                                                [
                                                    SNew(SHorizontalBox)
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
                                                                .OnClicked(this, &SClientBuildModal::OnCloseButtonClicked)
                                                        ]
                                                        + SHorizontalBox::Slot()
                                                        .AutoWidth()
                                                        .HAlign(HAlign_Right)
                                                        .VAlign(VAlign_Center)
                                                        [
                                                            SAssignNew(StartClient, SButton)
                                                                .Text(Menu::DeployCommon::kStartClientButtonText)
                                                                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
                                                                .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
                                                                .ContentPadding(SPadding::LaunchBarButtonPadding)
                                                                .OnClicked_Raw(this, &SClientBuildModal::OnStartClientClicked)
                                                        ]
                                                ]
                                        ]
                                ]
                        ]
                ]
        ];
        
    SetParentWindow();
    ClientBuildExecutablePathToUpdate = ClientBuildExecutablePath;
    StartClient->SetEnabled(!ClientBuildExecutablePathToUpdate.IsEmpty());
    SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChangedMultiDelegate.AddSP(this, &SClientBuildModal::OnBootstrapStatusChanged);
}

void SClientBuildModal::SetParentWindow()
{
    TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (TopLevelWindow.IsValid())
    {
        OwnerWindow->SetAsModalWindow();

        FSlateApplication::Get().AddWindowAsNativeChild(OwnerWindow.ToSharedRef(), TopLevelWindow.ToSharedRef(), false);
    }
}

void SClientBuildModal::ShowModal()
{
    OwnerWindow->ShowWindow();

    if (ParentWidget.IsValid())
    {
        ParentWidget.Pin()->SetEnabled(false);
    }
    GameClientExecutablePathInput->SetSelectedPath(ClientBuildExecutablePath);
    GameClientLauncherArgumentsInput->SetText(FText::FromString(ClientBuildLauncherArguments));
}

void SClientBuildModal::CloseModal()
{
    OwnerWindow->HideWindow();

    if (ParentWidget.IsValid())
    {
        ParentWidget.Pin()->SetEnabled(true);
    }
}

FReply SClientBuildModal::OnStartClientClicked() {
    CloseModal();

    ClientBuildExecutablePath = ClientBuildExecutablePathToUpdate;
    ClientBuildLauncherArguments = GameClientLauncherArgumentsInput.Get()->GetText().ToString();
    OnStartClientClickedDelegate.ExecuteIfBound(ClientBuildExecutablePath, ClientBuildLauncherArguments);

    return FReply::Handled();
}

void SClientBuildModal::OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu*)
{
    CloseModal();
}

TSharedRef<SWidget> SClientBuildModal::CreateGameClientPathInput()
{
    SAssignNew(GameClientExecutablePathInput, SPathInput)
        .Title(Menu::DeployCommon::kGameClientPathTitle)
        .PathHint(Menu::DeployCommon::kGameClientPathHint)
        .IsFileSelection(true)
        .ToolTipText(Menu::DeployCommon::kGameClientPathTooltip)
        .OnPathUpdated(this, &SClientBuildModal::OnGameClientExecutablePathInputUpdated);

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::Top)
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(SPadding::Right2x)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployCommon::kGameClientPathTitle)
                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    GameClientExecutablePathInput.ToSharedRef()
                ]
        ];
}

TSharedRef<SWidget> SClientBuildModal::CreateGameClientLauncherArgumentsInput()
{
    SAssignNew(GameClientLauncherArgumentsInput, SEditableTextBox)
        .HintText(Menu::DeployCommon::kGameClientLauncherArgumentsHint)
        .ToolTipText(Menu::DeployCommon::kGameClientLauncherArgumentsTooltip);

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::Top)
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(SPadding::Right2x)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployCommon::kGameClientLauncherArgumentsTitle)
                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    GameClientLauncherArgumentsInput.ToSharedRef()
                ]
        ];
}

void SClientBuildModal::OnGameClientExecutablePathInputUpdated(const FString& NewPath)
{
    ClientBuildExecutablePathToUpdate = NewPath;

    StartClient->SetEnabled(!ClientBuildExecutablePathToUpdate.IsEmpty());

    OwnerWindow->BringToFront(false);
}

FReply SClientBuildModal::OnCloseButtonClicked()
{
    CloseModal();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE