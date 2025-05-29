// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SBootstrapModal.h"

#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "Widgets/Text/STextBlock.h"
#include <Widgets/Input/SHyperlink.h>
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "SWidgets/SPathInput.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SSetupMessage.h"
#include "SWidgets/SOnlineHyperlink.h"

#include "Settings/UGameLiftSettings.h"

#define LOCTEXT_NAMESPACE "SBootstrapModal"

void SBootstrapModal::Construct(const FArguments& InArgs)
{
    S3BucketName = InArgs._DefaultS3BucketName;
    OnBootstrapProfileClickedDelegate = InArgs._OnBootstrapProfileClickedDelegate;
    ParentWidget = InArgs._ParentWidget;

    UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
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
                        .MinDesiredWidth(MinModalWindowWidth)
                        .MaxDesiredWidth(MaxModalWindowWidth)
                        [
                            SNew(SVerticalBox)
                                // Bootstrap modal title and close button
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
                                                .Text(FText::FromString(FString::Format(TEXT("Bootstrap {0}"), { Settings->CurrentProfileName })))
                                                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                                        ]
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .HAlign(HAlign_Right)
                                        .VAlign(VAlign_Center)
                                        [
                                            SNew(SButton)
                                                .OnClicked(this, &SBootstrapModal::OnCloseButtonClicked)
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
                                                // AWS Tier warning message
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(SPadding::Top_Bottom2x)
                                                [
                                                    SNew(SSetupMessage)
                                                        .WarningRowWidget(
                                                            SNew(SVerticalBox)
                                                            + SVerticalBox::Slot()
                                                            .AutoHeight()
                                                            [
                                                                SNew(SHorizontalBox)
                                                                    + SHorizontalBox::Slot()
                                                                    .FillWidth(1.0f)
                                                                    .HAlign(HAlign_Left)
                                                                    .VAlign(VAlign_Center)
                                                                    [
                                                                        SNew(SRichTextBlock)
                                                                            .Text(Settings::AwsAccount::kBootstrapProfileModalWarningMessage)
                                                                            .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                                                                            .AutoWrapText(true)
                                                                    ]
                                                            ]
                                                            + SVerticalBox::Slot()
                                                            .AutoHeight()
                                                            .Padding(SPadding::Top2x)
                                                            [
                                                                SNew(SOnlineHyperlink)
                                                                    .Text(Settings::AwsAccount::kAWSFreeTierLinkText)
                                                                    .Link(Settings::AwsAccount::kAWSFreeTierLinkUrl)
                                                            ]
                                                        )
                                                ]
                                                // Bootstrap modal description
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(SPadding::Top_Bottom2x)
                                                [
                                                    SNew(STextBlock)
                                                        .Text(Settings::AwsAccount::kBootstrapProfileModalDescription)
                                                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                                                        .AutoWrapText(true)

                                                ]
                                                // S3 bucket name input
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(SPadding::Top_Bottom2x)
                                                [
                                                    CreateS3BucketNameInput()
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
                                                                .OnClicked(this, &SBootstrapModal::OnCloseButtonClicked)
                                                        ]
                                                        + SHorizontalBox::Slot()
                                                        .AutoWidth()
                                                        .HAlign(HAlign_Right)
                                                        .VAlign(VAlign_Center)
                                                        [
                                                            SAssignNew(BootstrapProfile, SButton)
                                                                .Text(Settings::AwsAccount::kBootstrapProfileButtonText)
                                                                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
                                                                .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
                                                                .ContentPadding(SPadding::LaunchBarButtonPadding)
                                                                .OnClicked_Raw(this, &SBootstrapModal::OnBootstrapProfileClicked)
                                                        ]
                                                ]
                                        ]
                                ]
                        ]
                ]
        ];

    SetParentWindow();
    S3BucketNameToUpdate = S3BucketName;
    BootstrapProfile->SetEnabled(!S3BucketNameToUpdate.IsEmpty());
}

void SBootstrapModal::SetParentWindow()
{
    TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (TopLevelWindow.IsValid())
    {
        OwnerWindow->SetAsModalWindow();

        FSlateApplication::Get().AddWindowAsNativeChild(OwnerWindow.ToSharedRef(), TopLevelWindow.ToSharedRef(), false);
    }
}

void SBootstrapModal::ShowModal()
{
    if (OwnerWindow.IsValid())
    {
        OwnerWindow->ShowWindow();
    }

    if (ParentWidget.IsValid())
    {
        ParentWidget.Pin()->SetEnabled(false);
    }
    S3BucketNameInput->SetSelectedPath(S3BucketName);
}

void SBootstrapModal::CloseModal()
{
    if (OwnerWindow.IsValid())
    {
        OwnerWindow->RequestDestroyWindow();

    }

    if (ParentWidget.IsValid())
    {
        ParentWidget.Pin()->SetEnabled(true);
    }
}

FReply SBootstrapModal::OnBootstrapProfileClicked() {
    S3BucketName = S3BucketNameToUpdate;
    OnBootstrapProfileClickedDelegate.ExecuteIfBound(S3BucketName);
    CloseModal();

    return FReply::Handled();
}

TSharedRef<SWidget> SBootstrapModal::CreateS3BucketNameInput()
{
    SAssignNew(S3BucketNameInput, SPathInput)
        .Title(Settings::AwsAccount::kS3BucketNameTitle)
        .ButtonVisibility(EVisibility::Collapsed)
        .ToolTipText(Settings::AwsAccount::kS3BucketNameTooltip)
        .OnPathUpdated(this, &SBootstrapModal::OnS3BucketNameInputUpdated);

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
                        .Text(Settings::AwsAccount::kS3BucketNameTitle)
                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    S3BucketNameInput.ToSharedRef()
                ]
        ];
}

void SBootstrapModal::OnS3BucketNameInputUpdated(const FString& NewPath)
{
    S3BucketNameToUpdate = NewPath;

    BootstrapProfile->SetEnabled(!S3BucketNameToUpdate.IsEmpty());

    OwnerWindow->BringToFront(false);
}

FReply SBootstrapModal::OnCloseButtonClicked()
{
    CloseModal();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE