// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SSetupECRRepositorySection.h"

#include <Async/Async.h>
#include <iostream>

#include "Widgets/Layout/SWidgetSwitcher.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SSelectionComboBox.h"
#include "SWidgets/SSetupMessage.h"
#include "SWidgets/SInputTextBox.h"

#include "IGameLiftCoreModule.h"

#include "Settings/UGameLiftSettings.h"
#include "Settings/UGameLiftContainersStatus.h"
#include "Types/EBootstrapMessageState.h"
#include "Types/EContainersDeploymentScenario.h"
#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"
#include "SMenu/Containers/SControlBarSection.h"



#define LOCTEXT_NAMESPACE "SSetupECRRepositorySection"

void SSetupECRRepositorySection::Construct(const FArguments& InArgs)
{
	OnECRUpdateDelegate = InArgs._OnECRUpdateDelegate;

	ChildSlot
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right2x)
				.AutoWidth()
				[
					SAssignNew(ProgressBar, SProgressBar)
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(SPadding::Bottom5x + SPadding::Right2x)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SWidgetSwitcher)
								.WidgetIndex_Lambda([&]()
									{
										UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
										return ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo || ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInDocker ? 0 : 1;
									})
								+ SWidgetSwitcher::Slot()
								[
									SNew(SVerticalBox)
										+ SVerticalBox::Slot()
										.AutoHeight()
										.Padding(SPadding::SectionTitleTopPadding)
										[
											SNew(STextBlock)
												.Text(Menu::DeployContainers::kCreateECRRepositoryHeader)
												.AutoWrapText(true)
												.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
										]
										+ SVerticalBox::Slot()
										.AutoHeight()
										.Padding(SPadding::Top3x)
										[
											SNew(STextBlock)
												.Text(Menu::DeployContainers::kCreateECRRepositoryDescription)
												.AutoWrapText(true)
												.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
										]
								]
								+ SWidgetSwitcher::Slot()
								[
									SNew(SVerticalBox)
										+ SVerticalBox::Slot()
										.AutoHeight()
										.Padding(SPadding::SectionTitleTopPadding)
										[
											SNew(STextBlock)
												.Text(Menu::DeployContainers::kSetupECRRepositoryHeader)
												.AutoWrapText(true)
												.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
										]
										+ SVerticalBox::Slot()
										.AutoHeight()
										.Padding(SPadding::Top3x)
										[
											SNew(STextBlock)
												.Text(Menu::DeployContainers::kSetupECRRepositoryDescription)
												.AutoWrapText(true)
												.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
										]
								]
						]
						// External links
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(SPadding::Top2x)
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.HAlign(HAlign_Left)
								[
									SNew(SOnlineHyperlink)
										.Text(Menu::DeployContainers::kLearnMoreAboutPrivateRepositoryLabel)
										.Link(Url::kAmazonECRUserGuideLinkUrl)
								]
						]
						// Error text box
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(SPadding::Top2x)
						[
							CreateShowErrorUI()
						]
						// Repository name row
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.Padding(SPadding::Top2x)
						[
							SAssignNew(RepoNameRow, SNamedRow)
								.NameText(Menu::DeployContainers::kRepositoryNameTitle)
								.RowWidget(
									SAssignNew(RepoNameTextBlock, STextBlock)
									.AutoWrapText(true)
									.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
								)
								.SecondaryColumnLeftPadding(true)
								.Visibility(EVisibility::Collapsed)

						]
						// Retry button
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						[
							SNew(SBox)
								.Padding(SPadding::Top3x)
								.Visibility_Lambda([&]() -> EVisibility
									{
										UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
										return ContainersStatus->ErrorCreatingECRRepo ? EVisibility::Visible : EVisibility::Collapsed;
									})
								[
									SNew(SButton)
										.OnClicked_Lambda([&]()
											{
												ResetUIToNotStarted();
												StartSectionWorkflow();
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
																.Text(Menu::DeployContainers::kRetryButtonLabel)
																.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
														]
												]
										]
								]
						]
				]
		];
	SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChangedMultiDelegate.AddSP(this, &SSetupECRRepositorySection::OnBootstrapStatusChanged);
	UpdateUIBasedOnCurrentState();
}

TSharedRef<SWidget> SSetupECRRepositorySection::CreateShowErrorUI()
{
	TSharedPtr<SHorizontalBox> RowWidget = SNew(SHorizontalBox);
	RowWidget->AddSlot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
				.WidthOverride(Style::kMessageTextBoxWidth)
				[
					SAssignNew(ErrorTextBlock, STextBlock)
						.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
						.AutoWrapText(true)
				]
		];

	return SNew(SSetupMessage)
		.Visibility_Lambda([&]() -> EVisibility
			{
				UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
				return  ContainersStatus->ErrorCreatingECRRepo ? EVisibility::Visible : EVisibility::Collapsed;
			})
		.FailureRowWidget(RowWidget)
		.SetState(ESetupMessageState::FailureMessage);
}

void SSetupECRRepositorySection::OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu* /* Sender */)
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	EBootstrapMessageState BootstrapState = EBootstrapMessageStateFromInt(Settings->BootstrapStatus);
	if (BootstrapState == EBootstrapMessageState::NoBootstrappedMessage
		|| BootstrapState == EBootstrapMessageState::ActiveMessage)
	{
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::NotStart);
	}
}

void SSetupECRRepositorySection::UpdateUIBasedOnCurrentState()
{
	FText ECRRepoNameToDisplay = GetECRRepoNameToDisplay();
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	if (ScenarioNoContainerImageNoExistingEcrRepoNotFinished() || ScenarioNoContainerImageUseExistingEcrRepoNotFinished()) {
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::NotStart);
	}
	else if (ContainersStatus->ErrorCreatingECRRepo) {
		RepoNameTextBlock->SetText(ECRRepoNameToDisplay);
		ErrorTextBlock->SetText(ContainersStatus->ErrorMessageCreatingECRRepo);
		RepoNameRow->SetVisibility(EVisibility::Visible);
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressError);
	}
	else if (ContainersStatus->IsSetupECRRepoStepFinished)
	{
		RepoNameTextBlock->SetText(ECRRepoNameToDisplay);
		RepoNameRow->SetVisibility(EVisibility::Visible);
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressComplete);
	}
	else
	{
		ResetUIToNotStarted();
	}
}

FText SSetupECRRepositorySection::GetECRRepoNameToDisplay() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	if (ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo || ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo) {
		return FText::FromString(ContainersStatus->ECRRepoName); 
	}
	else {
		return FText::FromString("gamelift_container_fleet");
	}
}

bool SSetupECRRepositorySection::ScenarioNoContainerImageFinished() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return ContainersStatus->IsContainerFolderSetup;
}

bool SSetupECRRepositorySection::IsScenario(int Scenario) {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return ContainersStatus->DeploymentScenario == Scenario;
}

bool SSetupECRRepositorySection::ScenarioNoContainerImageNoExistingEcrRepoNotFinished() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return IsScenario((int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo) && !ScenarioNoContainerImageFinished();
}

bool SSetupECRRepositorySection::ScenarioNoContainerImageUseExistingEcrRepoNotFinished() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return IsScenario((int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo) && !ScenarioNoContainerImageFinished();
}

bool SSetupECRRepositorySection::ScenarioNoContainerImageNoExistingEcrRepoFinished() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return IsScenario((int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo) && ScenarioNoContainerImageFinished();
}

bool SSetupECRRepositorySection::ScenarioNoContainerImageUseExistingEcrRepoFinished() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return IsScenario((int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo) && ScenarioNoContainerImageFinished();
}

TTuple<bool, FString> SSetupECRRepositorySection::CreateRepository(FString RepositoryName)
{
	IGameLiftContainersHandler& Handler = IGameLiftCoreModule::Get().GetGameLiftContainersHandler();
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	GameLiftECRCreateRepositoryResult CreateRepoResult = Handler.CreateRepository(RepositoryName);
	if (CreateRepoResult.bIsSuccessful)
	{
		ContainersStatus->ECRRepoUri = CreateRepoResult.RepoUri;
		ContainersStatus->ECRRepoName = CreateRepoResult.RepositoryName;
		ContainersStatus->SaveConfig();
		return MakeTuple(true, TEXT(""));
	}
	else if (CreateRepoResult.ErrorMessage.Contains("RepositoryAlreadyExistsException")) {
		GameLiftECRDescribeRepositoriesResult result = Handler.DescribeRepositories();
		FString ECRRegistryUrl;
		FString FirstRepo;
		result.Repos[0].RepositoryUri.Split(TEXT("/"), &ECRRegistryUrl, &FirstRepo);
		ECRRegistryUrl += "/";
		ECRRegistryUrl += RepositoryName;
		ContainersStatus->ECRRepoUri = ECRRegistryUrl;
		ContainersStatus->ECRRepoName = RepositoryName;
		ContainersStatus->SaveConfig();
		return MakeTuple(true, TEXT(""));
	}
	else
	{
		return MakeTuple(false, CreateRepoResult.ErrorMessage);
	}
}

void SSetupECRRepositorySection::StartSectionWorkflow() {
	FText ECRRepoNameToDisplay = GetECRRepoNameToDisplay();
	RepoNameTextBlock->SetText(ECRRepoNameToDisplay);
	RepoNameRow->SetVisibility(EVisibility::Visible);
	TTuple<bool, FString> result = CreateRepository(ECRRepoNameToDisplay.ToString());
	if (result.Get<0>()) {
		HandleSetupRepoSucceed(ECRRepoNameToDisplay);
	} else {
		HandleSetupRepoFailed(ECRRepoNameToDisplay, result.Get<1>());
	}
}

void SSetupECRRepositorySection::HandleSetupRepoSucceed(FText ECRRepoNameToDisplay) {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	RepoNameTextBlock->SetText(ECRRepoNameToDisplay);
	RepoNameRow->SetVisibility(EVisibility::Visible);
	ContainersStatus->IsSetupECRRepoStepFinished = true;
	ContainersStatus->ErrorCreatingECRRepo = false;
	ContainersStatus->ErrorMessageCreatingECRRepo = FText::GetEmpty();
	ContainersStatus->SaveConfig();
	ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressComplete);
	OnECRUpdateDelegate.ExecuteIfBound();
}

void SSetupECRRepositorySection::HandleSetupRepoFailed(FText ECRRepoNameToDisplay, FString ErrorMessage) {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	RepoNameTextBlock->SetText(ECRRepoNameToDisplay);
	RepoNameRow->SetVisibility(EVisibility::Visible);
	ContainersStatus->IsSetupECRRepoStepFinished = false;
	ContainersStatus->ErrorCreatingECRRepo = true;
	ContainersStatus->ErrorMessageCreatingECRRepo = FText::FromString(ErrorMessage);
	ContainersStatus->SaveConfig();
	ErrorTextBlock->SetText(FText::FromString(ErrorMessage));
	ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressError);
}

void SSetupECRRepositorySection::ResetUIToNotStarted() {
	ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::NotStart);
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ContainersStatus->ErrorCreatingECRRepo = false;
	RepoNameRow->SetVisibility(EVisibility::Collapsed);
	ContainersStatus->SaveConfig();
}

#undef LOCTEXT_NAMESPACE