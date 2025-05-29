// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SLaunchBar.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include <GameLiftPluginConstants.h>
#include <GameLiftPluginStyle.h>
#include "Settings/UGameLiftContainersStatus.h"
#include "SMenu/Containers/SUpdateDeploymentModal.h"
#include "Interfaces/IMainFrameModule.h"
#include "SMenu/Containers/SGameLiftDeployContainersMenu.h"

#define LOCTEXT_NAMESPACE "SLaunchBar"

void SLaunchBar::Construct(const FArguments& InArgs) 
{
	ClientBuildExecutablePath = InArgs._DefaultClientBuildExecutablePath;
	ClientBuildLauncherArguments = InArgs._DefaultClientBuildLauncherArguments;
	OnStartClientButtonClicked = InArgs._OnStartClientButtonClicked;
	OnStartClientButtonClickedWith2Players = InArgs._OnStartClientButtonClickedWith2Players;
	OnStartServerButtonClicked = InArgs._OnStartServerButtonClicked;
	IsStartServerButtonEnabled = InArgs._IsStartServerButtonEnabled;
	IsStartClientButtonEnabled = InArgs._IsStartClientButtonEnabled;
	IsUpdateDeploymentButtonEnabled = InArgs._IsUpdateDeploymentButtonEnabled;
	ParentWidget = InArgs._ParentWidget;
	MenuType = InArgs._MenuType;

	if (MenuType == EMenuType::Anywhere)
	{
		ShowStartServerButton = true;
	}
	else if (MenuType == EMenuType::Containers)
	{
		ShowUpdateDeploymentButton = true;
	}

	ChildSlot
		[
			SAssignNew(LaunchBar, SBorder)
				.BorderBackgroundColor(FColor::Black)
				.BorderImage(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kBorderImageName))
				.Padding(SPadding::LaunchBarPadding)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Left)
						[
							CreateUpdateDeploymentButton()
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Right)
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(SPadding::Right2x)
								[
									CreateTest2PlayerClientCheckBox()
								]
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(SPadding::Right2x)
								[
									CreateStartServerInstruction()
								]
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(SPadding::Right2x)
								[
									SAssignNew(StartServerButtonWidgetSwitcher, SWidgetSwitcher)
										+ SWidgetSwitcher::Slot()
										[
											CreateStartServerButton()
										]
										+ SWidgetSwitcher::Slot()
										[
											CreateLoadingButton()
										]
								]
								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SAssignNew(StartClientButtonWidgetSwitcher, SWidgetSwitcher)
										+ SWidgetSwitcher::Slot()
										[
											CreateStartClientButton()
										]
										+ SWidgetSwitcher::Slot()
										[
											CreateLoadingButton()
										]
								]
						]
				]
		];
}

TSharedRef<SWidget> SLaunchBar::CreateUpdateDeploymentButton() 
{
	return SNew(SButton)
		.Visibility(ShowUpdateDeploymentButton ? EVisibility::Visible : EVisibility::Collapsed)
		.IsEnabled(IsUpdateDeploymentButtonEnabled)
		.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kLaunchBarNormalButtonStyle)
		.ContentPadding(SPadding::LaunchBarButtonPadding)
		.OnClicked_Lambda([this]
			{
				DisplayUpdateModal();
				return FReply::Handled();
			})
		.Content()
		[
			SNew(SBox)
				.HAlign(HAlign_Center)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						.Padding(SPadding::Right)
						[
							SNew(SBox)
								.WidthOverride(Style::kResetIconSize)
								.HeightOverride(Style::kResetIconSize)
								[
									SNew(SImage)
										.Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kResetIconName))
								]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
								.Text(Menu::DeployCommon::kUpdateDeploymentButtonText)
								.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
						]
				]
		];
}

void SLaunchBar::DisplayUpdateModal()
{
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (TopLevelWindow.IsValid() && !ContainersStatus->IsUpdateContainerFleetModalOpen)
	{
		TSharedRef<SWindow> ModalWindow = SNew(SWindow)
			.IsPopupWindow(true)
			.SupportsMinimize(false)
			.SupportsMaximize(false)
			.bDragAnywhere(true)
			.SizingRule(ESizingRule::Autosized);

		TSharedRef<SUpdateDeploymentModal> ModalContent = SNew(SUpdateDeploymentModal)
			.OwnerWindow(ModalWindow);

		ModalWindow->SetContent(ModalContent);
		ModalWindow->SetAsModalWindow();

		FSlateApplication::Get().AddWindowAsNativeChild(ModalWindow, TopLevelWindow.ToSharedRef(), true);

		ContainersStatus->IsUpdateContainerFleetModalOpen = true;
		ContainersStatus->SaveConfig();

		SGameLiftDeployContainersMenu::OnModalOpened.ExecuteIfBound();
	}
}

TSharedRef<SClientBuildModal> SLaunchBar::CreateClientBuildModal()
{
	return SAssignNew(ClientBuildModal, SClientBuildModal)
		.ParentWidget(ParentWidget)
		.DefaultClientBuildExecutablePath(ClientBuildExecutablePath.Get())
		.DefaultClientBuildLauncherArguments(ClientBuildLauncherArguments.Get())
		.OnStartClientClickedDelegate(FStartClient::CreateRaw(this, &SLaunchBar::OnStartClientButtonPressed));
}

TSharedRef<SWidget> SLaunchBar::CreateTest2PlayerClientCheckBox()
{
	return SNew(SCheckBox)
		.Visibility(ShowTwoPlayerClientCheckBox ? EVisibility::Visible : EVisibility::Collapsed)
		.Padding(SPadding::LaunchBarCheckboxPadding)
		.OnCheckStateChanged(this, &SLaunchBar::OnCheckboxStateChanged)
		[
			SNew(STextBlock)
				.Text(Menu::DeployCommon::kTestTwoPlayerCheckBoxText)
		];
}

void SLaunchBar::OnCheckboxStateChanged(ECheckBoxState NewState)
{
	IsTwoPlayerClientSelected = (NewState == ECheckBoxState::Checked);
}

void SLaunchBar::OnStartClientButtonPressed(FString ClientBuildPathExecutable, FString LauncherArgs)
{
	ClientBuildExecutablePath = ClientBuildPathExecutable;
	ClientBuildLauncherArguments = LauncherArgs;

	if (IsTwoPlayerClientSelected)
	{
		OnStartClientButtonClickedWith2Players.ExecuteIfBound(ClientBuildPathExecutable, LauncherArgs);
	}
	else
	{
		OnStartClientButtonClicked.ExecuteIfBound(ClientBuildPathExecutable, LauncherArgs);
	}
}

FReply SLaunchBar::OnStartServerButtonPressed()
{
	OnStartServerButtonClicked.ExecuteIfBound();
	return FReply::Handled();
}

TSharedRef<SButton> SLaunchBar::CreateLoadingButton()
{
	return SNew(SButton)
		.IsEnabled(false)
		.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kLaunchBarNormalButtonStyle)
		.ContentPadding(SPadding::LaunchBarButtonPadding)
		.Content()
		[
			SNew(SBox)
				.HAlign(HAlign_Center)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						.Padding(SPadding::Right)
						[
							SNew(SBox)
								.WidthOverride(Style::kStartIconSize)
								.HeightOverride(Style::kStartIconSize)
								[
									SNew(SImage)
										.Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kStartIconName))
								]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
								.Text(Menu::DeployCommon::kStartButtonLoadingText)
								.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
						]
				]
		];
}

TSharedRef<SButton> SLaunchBar::CreateStartClientButton() 
{
	return SNew(SButton)
		.IsEnabled(IsStartClientButtonEnabled)
		.ButtonStyle(FGameLiftPluginStyle::Get(), ShowStartServerButton ? Style::Button::kLaunchBarNormalButtonStyle : Style::Button::kLaunchBarSuccessButtonStyle)
		.ContentPadding(SPadding::LaunchBarButtonPadding)
		.OnClicked_Lambda([&] 
			{
				// This ensures client build modal is built before calling ShowModal everytime
				// preventing different order of operations causing ensure condition failed exceptions
				CreateClientBuildModal();
				ClientBuildModal->ShowModal();
				return FReply::Handled();
			})
		.Content()
		[
			SNew(SBox)
				.HAlign(HAlign_Center)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						.Padding(SPadding::Right)
						[
							SNew(SBox)
								.WidthOverride(Style::kStartIconSize)
								.HeightOverride(Style::kStartIconSize)
								[
									SNew(SImage)
										.Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kStartIconName))
								]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
								.Text(Menu::DeployCommon::kStartClientButtonText)
								.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
						]
				]
		];
}

TSharedRef<SWidget> SLaunchBar::CreateStartServerButton()
{
	return SNew(SButton)
		.Visibility(ShowStartServerButton ? EVisibility::Visible : EVisibility::Collapsed)
		.IsEnabled(IsStartServerButtonEnabled)
		.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kLaunchBarSuccessButtonStyle)
		.ContentPadding(SPadding::LaunchBarButtonPadding)
		.OnClicked_Raw(this, &SLaunchBar::OnStartServerButtonPressed)
		.Content()
		[
			SNew(SBox)
				.HAlign(HAlign_Center)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						.Padding(SPadding::Right)
						[
							SNew(SBox)
								.WidthOverride(Style::kStartIconSize)
								.HeightOverride(Style::kStartIconSize)
								[
									SNew(SImage)
										.Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kStartIconName))
								]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(Menu::DeployCommon::kStartServerButtonText)
							.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
						]
				]
		];
}

TSharedRef<SWidget> SLaunchBar::CreateStartServerInstruction()
{
	return SNew(SBox)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
				.Visibility(ShowStartServerButton ? EVisibility::Visible : EVisibility::Collapsed)
				.Text(Menu::DeployAnywhere::kGameLaunchNoteText)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.AutoWrapText(true)
		];
}

void SLaunchBar::ChangeStartClientButtonUIState(EStartButtonUIState InState)
{
	StartClientButtonWidgetSwitcher->SetActiveWidgetIndex(LaunchBarUIStateToInt(InState));
}

void SLaunchBar::ChangeStartServerButtonUIState(EStartButtonUIState InState)
{
	StartServerButtonWidgetSwitcher->SetActiveWidgetIndex(LaunchBarUIStateToInt(InState));
}

#undef LOCTEXT_NAMESPACE