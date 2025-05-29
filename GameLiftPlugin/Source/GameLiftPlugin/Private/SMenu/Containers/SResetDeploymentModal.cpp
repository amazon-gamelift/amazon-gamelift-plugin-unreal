// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SResetDeploymentModal.h"

#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/SSetupMessage.h"
#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "SMenu/Containers/SUserInputSection.h"
#include <Settings/UGameLiftDeploymentStatus.h>
#include <Settings/UGameLiftSettings.h>
#include <Settings/UGameLiftContainersStatus.h>
#include "SMenu/Containers/SGameLiftDeployContainersMenu.h"

#define LOCTEXT_NAMESPACE "SResetDeploymentModal"

FResetDeployment SResetDeploymentModal::OnResetDeploymentMultiDelegate;

void SResetDeploymentModal::Construct(const FArguments& InArgs)
{
    OwnerWindow = InArgs._OwnerWindow;

    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

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
                                .Text(Menu::DeployContainers::kResetFleetDeploymentTitle)
                                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Center)
                        [
                            SNew(SButton)
                                .OnClicked(this, &SResetDeploymentModal::OnCloseButtonClicked)
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
                            // Reset deployment Description
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(STextBlock)
                                    .Text(Menu::DeployContainers::kResetFleetDeploymentDescription)
                                    .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                                    .AutoWrapText(true)

                            ]
                            // Reset deployment setup message warning
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                CreateResetFleetDeploymentWarningMessage()
                            ]
                            // Reset deployment Explanantion
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(SPadding::Top_Bottom)
                            [
                                SNew(STextBlock)
                                    .Text(Menu::DeployContainers::kResetFleetDeploymentExplanantion)
                                    .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                                    .AutoWrapText(true)

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
                                            .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
                                            .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
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
                                            .Text(Menu::DeployContainers::kResetButtonText)
                                            .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
                                            .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kErrorButtonStyleName)
                                            .ContentPadding(SPadding::LaunchBarButtonPadding)
                                            .OnClicked_Lambda([this]
                                                {
                                                    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                                                    ContainersStatus->GameName = Menu::DeployContainers::kGameNameDefault;
                                                    ContainersStatus->ContainerFleetDeploymentStatus = (int)EContainerFleetDeploymentStatus::DeploymentNotStart;
                                                    ContainersStatus->ResetStatus();
                                                    ContainersStatus->SaveConfig();
                                                    OnResetDeploymentMultiDelegate.Broadcast();
                                                    OnCloseButtonClicked();
                                                    return FReply::Handled();
                                                })
                                    ]
                            ]
                        ]
                ]
        ];
}

TSharedRef<SWidget> SResetDeploymentModal::CreateResetFleetDeploymentWarningMessage() {
    TSharedPtr<SHorizontalBox> RowWidget = SNew(SHorizontalBox);
    RowWidget->AddSlot()
        .FillWidth(1.0f)
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
                .Text(Menu::DeployContainers::kResetWarningText)
                .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
                .AutoWrapText(true)
        ];

    RowWidget->AddSlot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .Padding(SPadding::All)
        [
            CreateAWSConsoleButton()
        ];

    return SAssignNew(ResetFleetDeploymentSetupMessage, SSetupMessage)
        .WarningRowWidget(RowWidget)
        .WarningButtonStyle(Style::Button::kCloseButtonStyleName)
        .OnButtonClicked_Lambda([&]
            {
                ResetFleetDeploymentSetupMessage->SetVisibility(EVisibility::Collapsed);
            });
}

TSharedRef<SWidget> SResetDeploymentModal::CreateAWSConsoleButton()
{
    return SNew(SButton)
        .ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
        .OnClicked_Lambda([]
            {
                UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
                FString CurrentAwsRegion = Settings->AwsRegion.ToString().IsEmpty() ? FString("us-west-2") : Settings->AwsRegion.ToString();
                FPlatformProcess::LaunchURL(*FString::Format(Menu::DeployManagedEC2::kViewInCloudFormationHyperLinkUrlFormat, { CurrentAwsRegion }), nullptr, nullptr);
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
                                ]
                        ]
                ]
        ];
}



FReply SResetDeploymentModal::OnCloseButtonClicked()
{
    if (OwnerWindow.IsValid())
    {
        OwnerWindow.Pin()->RequestDestroyWindow();
    }
   
    SGameLiftDeployContainersMenu::OnModalClosed.ExecuteIfBound();

    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->IsResetContainerFleetModalOpen = false;
    ContainersStatus->SaveConfig();
    return FReply::Handled();
}

void SResetDeploymentModal::SetOwnerWindow(TSharedPtr<SWindow> NewOwnerWindow)
{
    OwnerWindow = NewOwnerWindow;
}

#undef LOCTEXT_NAMESPACE