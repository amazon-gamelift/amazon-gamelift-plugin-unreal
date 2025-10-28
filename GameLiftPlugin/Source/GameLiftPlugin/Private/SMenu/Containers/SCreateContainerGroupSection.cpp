// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SCreateContainerGroupSection.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/SSetupMessage.h"

#include <GameLiftPluginConstants.h>
#include <GameLiftPluginStyle.h>
#include <Settings/UGameLiftContainersStatus.h>
#include <IGameLiftContainersHandler.h>
#include <IGameLiftCoreModule.h>
#include <Settings/UGameLiftDeploymentStatus.h>
#include <Settings/UGameLiftSettings.h>
#include <GameLiftCoreConstants.h>
#include <GameLiftPlugin.h>
#include <Types/EDeploymentMessageState.h>
#include <Types/EContainersDeploymentScenario.h>


#define LOCTEXT_NAMESPACE "SCreateContainerGroupSection"

FConfigureCgd SCreateContainerGroupSection::OnConfigureCgdMultiDelegate;
FDeploymentComplete SCreateContainerGroupSection::OnDeploymentCompleteMultiDelegate;

void SCreateContainerGroupSection::Construct(const FArguments& InArgs)
{
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
				.Padding(SPadding::Bottom5x + SPadding::Right2x)
				.FillWidth(1.0f)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(SPadding::SectionTitleTopPadding)
						[
							SNew(STextBlock)
								.Text(Menu::DeployContainers::kCreateContainerGroupHeader)
								.AutoWrapText(true)
								.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(SPadding::Top3x)
						[
							SNew(STextBlock)
								.Text(Menu::DeployContainers::kCreateContainerGroupDescription)
								.AutoWrapText(true)
								.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
						]
						// Error text box
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(SPadding::Top2x)
						[
							CreateShowErrorUI()
						]
						// CGD details
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							CreateCgdDetails()
						]
						// Retry button
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						.Padding(SPadding::Top3x)
						[
							SNew(SButton)
								.OnClicked_Lambda([&]()
									{
										ResetUIToNotStarted();
										StartSectionWorkflow();
										return FReply::Handled();
									})
								.Visibility_Lambda([&]() -> EVisibility
									{
										UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
										return ContainersStatus->ErrorCreatingContainerGroupSection ? EVisibility::Visible : EVisibility::Collapsed;
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
		];
	UpdateUIBasedOnCurrentState();
}

TSharedRef<SWidget> SCreateContainerGroupSection::CreateCgdDetails()
{
	return SAssignNew(CgdDetails, SVerticalBox)
		// Group definition status row
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(SPadding::Top2x)
		[
			SAssignNew(GroupDefinitionStatusRow, SNamedRow)
				.NameText(Menu::DeployContainers::kContainerGroupDefinitionStatusTitle)
				.RowWidget(
					SAssignNew(GroupDefinitionStatusTextBlock, STextStatus)
					.StatusTextStyle(Style::Text::kFieldBold)
				)
				.SecondaryColumnLeftPadding(true)
		]
		// Group definition name row
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(SPadding::Top2x)
		[
			SAssignNew(GroupDefinitionNameRow, SNamedRow)
				.NameText(Menu::DeployContainers::kContainerGroupDefinitionNameTitle)
				.RowWidget(
					SAssignNew(GroupDefinitionNameTextBlock, STextBlock)
					.AutoWrapText(true)
					.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				)
				.SecondaryColumnLeftPadding(true)
		]
		// cgd version row
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(SPadding::Top2x)
		[
			SAssignNew(CgdVersionRow, SNamedRow)
				.NameText(Menu::DeployContainers::kContainerGroupDefinitionVersionTitle)
				.RowWidget(
					SAssignNew(CgdVersionTextBlock, STextBlock)
					.AutoWrapText(true)
					.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				)
				.SecondaryColumnLeftPadding(true)
		]
		// Total memory limit row
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(SPadding::Top2x)
		[
			SAssignNew(TotalMemoryLimitRow, SNamedRow)
				.NameText(Menu::DeployContainers::kTotalMemoryLimitText)
				.RowWidget(
					SAssignNew(TotalMemoryLimitTextBlock, STextBlock)
					.AutoWrapText(true)
					.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				)
				.SecondaryColumnLeftPadding(true)
		]
		// Total cpu limit row
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(SPadding::Top2x)
		[
			SAssignNew(TotalVcpuLimitRow, SNamedRow)
				.NameText(Menu::DeployContainers::kTotalVCPULimitText)
				.RowWidget(
					SAssignNew(TotalVcpuLimitTextBlock, STextBlock)
					.AutoWrapText(true)
					.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				)
				.SecondaryColumnLeftPadding(true)
		];

	CgdDetails->SetVisibility(EVisibility::Collapsed);
}

TSharedRef<SWidget> SCreateContainerGroupSection::CreateShowErrorUI()
{
	TSharedPtr<SHorizontalBox> RowWidget = SNew(SHorizontalBox);
	RowWidget->AddSlot()
		.FillWidth(1.0f)
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
	RowWidget->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(SPadding::All)
		[
			CreateViewInCFNButton()
		];

	return SNew(SSetupMessage)
		.Visibility_Lambda([&]() -> EVisibility
			{
				UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
				return ContainersStatus->ErrorCreatingContainerGroupSection ? EVisibility::Visible : EVisibility::Collapsed;
			})
		.FailureRowWidget(RowWidget)
		.SetState(ESetupMessageState::FailureMessage);
}

TSharedRef<SWidget> SCreateContainerGroupSection::CreateViewInCFNButton()
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

void SCreateContainerGroupSection::UpdateUIBasedOnCurrentState()
{
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	if (ContainersStatus->ErrorCreatingContainerGroupSection) {
		ErrorTextBlock->SetText(ContainersStatus->ErrorMessageCreatingContainerImage);
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressError);
		UpdateCgdStatusTextBlock(FText::FromString(ContainersStatus->CgdStatus));
		PopulateCgdDetails();
	}
	else if (ContainersStatus->IsCgdConfigurationFinished) {
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressComplete);
		UpdateCgdStatusTextBlock(FText::FromString(ContainersStatus->CgdStatus));
		PopulateCgdDetails();
	}
	else if (ShouldStartSectionWorkflow()) {
		StartSectionWorkflow();
	} else {
		ResetUIToNotStarted();
	}
}

bool SCreateContainerGroupSection::ShouldStartSectionWorkflow() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	if (ContainersStatus->DeploymentScenario == (int)EContainersDeploymentScenario::HaveContainerImageInEcr) {
		if (ContainersStatus->ContainerFleetDeploymentStatus == (int)EContainerFleetDeploymentStatus::DeploymentInProgress && !ContainersStatus->IsCgdConfigurationFinished) {
			return true;
		}
		return false;
	} else {
		if (ContainersStatus->ContainerFleetDeploymentStatus == (int)EContainerFleetDeploymentStatus::DeploymentInProgress && !ContainersStatus->IsCgdConfigurationFinished && ContainersStatus->IsContainerImagePushed) {
			return true;
		}
		return false;
	}
}

void SCreateContainerGroupSection::SetFieldInputsToEmptyStatuses()
{
	// Set field inputs to empty status
	FString EmptyStatus = "-";
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ContainersStatus->CgdName = EmptyStatus;
	ContainersStatus->CgdVersion = EmptyStatus;
	ContainersStatus->SaveConfig();
}

void SCreateContainerGroupSection::PopulateCgdDetails() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	TotalMemoryLimitTextBlock->SetText(FText::Format(FText::FromString("{0} {1}"), ContainersStatus->TotalMemoryLimit, Menu::DeployContainers::kTotalMemoryLimitUnit));
	TotalVcpuLimitTextBlock->SetText(FText::Format(FText::FromString("{0} {1}"), ContainersStatus->TotalVCPULimit, Menu::DeployContainers::kTotalVCPULimitUnit));
	GroupDefinitionNameTextBlock->SetText(FText::FromString(ContainersStatus->CgdName));
	CgdVersionTextBlock->SetText(FText::FromString(ContainersStatus->CgdVersion));
	CgdDetails->SetVisibility(EVisibility::Visible);
}

void SCreateContainerGroupSection::StartSectionWorkflow() {
	
	SetFieldInputsToEmptyStatuses();
	UpdateCgdStatusTextBlock(FText::GetEmpty());
	PopulateCgdDetails();
	DeployCloudFormation();
}

bool SCreateContainerGroupSection::IsContainerDeploymentStatusActive()
{
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return EDeploymentMessageStateFromString(ContainersStatus->Status.ToString()) == EDeploymentMessageState::ActiveMessage;
}

void SCreateContainerGroupSection::PopulateDeploymentStatus()
{
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ContainersStatus->CgdName = ContainersStatus->GameName.ToString();
	ContainersStatus->ContainerImageName = FText::FromString(ContainersStatus->GameName.ToString() + FString("Container"));
	ContainersStatus->ContainerImageURI = FText::FromString(ContainersStatus->ECRRepoUri + FString(":") + ContainersStatus->ImageTag.ToString());
	ContainersStatus->IntraContainerLaunchPath = ContainersStatus->GameServerExecutablePath;

	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	ContainersStatus->DeployedRegion = Settings->AwsRegion;
	ContainersStatus->ApiGatewayEndpoint = FText::GetEmpty();
	ContainersStatus->CognitoClientId = FText::GetEmpty();
	ContainersStatus->Scenario = ContainersStatus->DeploymentDetailRegionChoice ? Menu::DeploymentServer::kContainersFlexMatch : Menu::DeploymentServer::kContainersSingleRegionFleet;
	ContainersStatus->SaveConfig();
}

void SCreateContainerGroupSection::UpdateCgdStatusTextBlock(const FText Status)
{
	if (Status.ToLower().EqualTo(Menu::DeployContainers::kReady.ToLower()))
	{
		GroupDefinitionStatusTextBlock->SetState(STextStatus::EIconState::Success, Menu::DeployContainers::kReady);
	}
	else if (Status.ToLower().EqualTo(Menu::DeployContainers::kCopying.ToLower()))
	{
		GroupDefinitionStatusTextBlock->SetState(STextStatus::EIconState::Loading, Menu::DeployContainers::kCopying);
	}
	else if (Status.ToLower().EqualTo(Menu::DeployContainers::kFailed.ToLower()))
	{
		GroupDefinitionStatusTextBlock->SetState(STextStatus::EIconState::Error, Menu::DeployContainers::kFailed);
	}
	else
	{
		GroupDefinitionStatusTextBlock->SetState(STextStatus::EIconState::Inactive, Menu::DeployContainers::kNoDeployment);
	}
}

void SCreateContainerGroupSection::PollDescribeContainerGroupDefinition()
{
	Async(EAsyncExecution::Thread,
		[this]() mutable
		{
			UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
			int i = 0;
			GameLiftDescribeCgdResult DescribeCGDResult;
			bool HasSucceeded = false;
			// Setting a 2 hour timeout for pulling cfn deployment result
			for (; i < WaitTimesCount; ++i) {
				FPlatformProcess::Sleep(WaitIntervalSeconds);

				// First detect the stack deployment status
				if (EDeploymentMessageStateFromString(ContainersStatus->Status.ToString()) == EDeploymentMessageState::FailureMessage) {
					Async(EAsyncExecution::TaskGraphMainThread,
						[this, HasSucceeded]() mutable
						{
							// Manually update status to FAILED if stack has failed and we previously succeeded
							// ie. Stack may have deleted CGD successfully before we detected CGD failure
							if (HasSucceeded)
							{
								UpdateCgdStatusTextBlock(Menu::DeployContainers::kFailed);
							}
						});
					break;
				}

				// Describe the CGD
				IGameLiftContainersHandler& Handler = IGameLiftCoreModule::Get().GetGameLiftContainersHandler();
				DescribeCGDResult = Handler.DescribeCgd(ContainersStatus->CgdName);
				if (DescribeCGDResult.bIsSuccessful) {
					HasSucceeded = true;
					Async(EAsyncExecution::TaskGraphMainThread,
						[this, DescribeCGDResult]() mutable
						{
							UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
							UpdateCgdStatusTextBlock(FText::FromString(DescribeCGDResult.CgdStatus));
							GroupDefinitionNameTextBlock->SetText(FText::FromString(ContainersStatus->CgdName));
							CgdVersionTextBlock->SetText(FText::FromString(DescribeCGDResult.CgdVersion));
							ContainersStatus->CgdVersion = DescribeCGDResult.CgdVersion;
							ContainersStatus->CgdStatus = DescribeCGDResult.CgdStatus;
							ContainersStatus->SaveConfig();
						});

					if (DescribeCGDResult.CgdStatus.Equals("READY")) {
						Async(EAsyncExecution::TaskGraphMainThread,
							[this]()
							{
								HandleCgdReady();
							});
						break;
					}
					else if (DescribeCGDResult.CgdStatus == "FAILED") {
						HandleCgdFailed();
						break;
					}
				}
			}
			if (i == WaitTimesCount) {
				bool success = DescribeCGDResult.bIsSuccessful;
				FString ErrorMessage = DescribeCGDResult.ErrorMessage;
				Async(EAsyncExecution::TaskGraphMainThread,
					[this, success = MoveTemp(success), ErrorMessage = MoveTemp(ErrorMessage)]() mutable
					{
						UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
						ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressError);
						ContainersStatus->ErrorCreatingContainerGroupSection = true;
						ContainersStatus->IsCgdConfigurationFinished = false;
						ErrorTextBlock->SetText(Menu::DeployContainers::kTimedOutPollingCgdStatusErrorMessage);
						ContainersStatus->ErrorMessageCreatingContainerImage = Menu::DeployContainers::kTimedOutPollingCgdStatusErrorMessage;
						if (!success) {
							UE_LOG(GameLiftPluginLog, Display, TEXT("Timed out describing container group definition, %s"), *ErrorMessage);
						}
						else {
							UE_LOG(GameLiftPluginLog, Display, TEXT("Timed out waiting for container group definition to enter READY or FAILED status"));
						}
						ContainersStatus->SaveConfig();
					});
			}
			ContainersStatus->SaveConfig();
		});
}

FReply SCreateContainerGroupSection::DeployCloudFormation()
{
	ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::InProgress);
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ContainersStatus->ErrorCreatingContainerGroupSection = false;
	ContainersStatus->IsCgdFailed = false;
	ContainersStatus->SaveConfig();
	if (IsContainerDeploymentStatusActive())
	{
		auto choice = FMessageDialog::Open(
			EAppMsgType::YesNo, Menu::DeployManagedEC2::kConfirmReploymentText);
		if (choice != EAppReturnType::Yes) {
			return FReply::Handled();
		}
	}
	PopulateDeploymentStatus();

	FText NotifyMessage = FText::GetEmpty();
	NotifyMessage = FText::Format(LOCTEXT("DeploymentNotificationAws", "{0}\n{1}"),
		FText::FromString(Menu::DeployContainers::kDeploymentStartedAwsScenarioNotification), ContainersStatus->Scenario);
	NotificationItem = Notifier::CreateNotification(NotifyMessage);
	ContainersStatus->Status = FText::FromString(EDeploymentMessageStateToString(EDeploymentMessageState::InProgressMessage));
	ContainersStatus->SaveConfig();

	// Start polling before deploying
	PollDescribeContainerGroupDefinition();

	Async(EAsyncExecution::Thread, 
		[this, NotificationItem = NotificationItem] () mutable
		{
			UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
			auto& deployer = IGameLiftCoreModule::Get().GetScenarioDeployer();

			// Pass in StackIdentifier for the GameName field in the deployer as a quick workaround
			// to decouple containers and ec2 stack deployments and fix describe container fleet calls
			FString HostingSolution = "-Containers";
			ContainersStatus->StackIdentifier = ContainersStatus->GameName.ToString() + HostingSolution;

			bool IsDeployed = deployer.DeployContainerScenario(
				ContainersStatus->Scenario,
				IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance(),
				ContainersStatus->CgdName,
				ContainersStatus->ContainerImageName.ToString(),
				ContainersStatus->ContainerImageURI.ToString(),
				ContainersStatus->IntraContainerLaunchPath,
				ContainersStatus->StackIdentifier,
				ContainersStatus->OutConfigFilePath.ToString(),
				ContainersStatus->ConnectionPortRange,
				ContainersStatus->TotalVCPULimit.ToString(),
				ContainersStatus->TotalMemoryLimit.ToString(),
				ContainersStatus->EnableMetrics
			);
			if (IsDeployed)
			{
				UE_LOG(GameLiftPluginLog, Display, TEXT("%s"), Menu::DeployContainers::kDeploymentSucceed);
				ContainersStatus->IsLastStepComplete = true;
				ContainersStatus->ApiGatewayEndpoint = FText::FromString(deployer.GetLastApiGatewayEndpoint());
				ContainersStatus->CognitoClientId = FText::FromString(deployer.GetLastCognitoClientId());
				ContainersStatus->Status = FText::FromString(EDeploymentMessageStateToString(EDeploymentMessageState::ActiveMessage));
			}
			else
			{
				UE_LOG(GameLiftPluginLog, Display, TEXT("%s"), Menu::DeployContainers::kDeploymentFailed);
				if (!ContainersStatus->IsCgdConfigurationFinished)
				{
					ContainersStatus->ErrorCreatingContainerGroupSection = true;
					if (ContainersStatus->IsCgdFailed)
					{
						ContainersStatus->ErrorMessageCreatingContainerImage = Menu::DeployContainers::kCgdStatusFailedErrorMessage;
						ContainersStatus->LatestError = Menu::DeployContainers::kCgdStatusFailedErrorMessage;
					}
					ContainersStatus->LatestError = Menu::DeployContainers::kDeploymentFailedMessage;
				}
				else 
				{
					ContainersStatus->ErrorCreatingContainerFleet = true;
					ContainersStatus->ErrorMessageCreatingContainerFleet = Menu::DeployContainers::kDeploymentFailedMessage;
					ContainersStatus->LatestError = ContainersStatus->ErrorMessageCreatingContainerFleet;
				}
				ContainersStatus->Status = FText::FromString(EDeploymentMessageStateToString(EDeploymentMessageState::FailureMessage));
			}
			ContainersStatus->SaveConfig();

			Async(EAsyncExecution::TaskGraphMainThread,
				[this, NotificationItem = NotificationItem]
				{
					UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
					if (ContainersStatus->ErrorCreatingContainerGroupSection)
					{
						ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressError);
						ErrorTextBlock->SetText(ContainersStatus->LatestError);
					}
					if (ContainersStatus->IsLastStepComplete)
					{
						Notifier::CompleteWithSuccess(NotificationItem, Menu::DeployContainers::kDeploymentCompletedNotification);
					}
					else
					{
						Notifier::CompleteWithFailure(NotificationItem, Menu::DeployContainers::kDeploymentFailedNotification);
					}
					OnDeploymentCompleteMultiDelegate.Broadcast();
				});
		});

		return FReply::Handled();
}

void SCreateContainerGroupSection::HandleCgdReady() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressComplete);
	ContainersStatus->IsCgdConfigurationFinished = true;
	ContainersStatus->SaveConfig();
	OnConfigureCgdMultiDelegate.Broadcast();
}

void SCreateContainerGroupSection::HandleCgdFailed() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ContainersStatus->IsCgdFailed = true;
	ContainersStatus->IsCgdConfigurationFinished = false;
	ContainersStatus->SaveConfig();
}

void SCreateContainerGroupSection::ResetUIToNotStarted() {
	ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::NotStart);
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ContainersStatus->ErrorCreatingContainerGroupSection = false;
	ContainersStatus->IsCgdFailed = false;
	ContainersStatus->SaveConfig();
	CgdDetails->SetVisibility(EVisibility::Collapsed);
}

#undef LOCTEXT_NAMESPACE
