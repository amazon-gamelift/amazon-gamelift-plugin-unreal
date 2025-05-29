#include "SCreateContainerFleetSection.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/SSetupMessage.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SDeploymentStatus.h"

#include <GameLiftPluginConstants.h>
#include <GameLiftPluginStyle.h>
#include <GameLiftPlugin.h>
#include "SMenu/Containers/SCreateContainerGroupSection.h"
#include <IGameLiftContainersHandler.h>
#include <IGameLiftCoreModule.h>
#include <Settings/UGameLiftContainersStatus.h>
#include <Settings/UGameLiftDeploymentStatus.h>
#include <Settings/UGameLiftSettings.h>
#include <Types/EContainersDeploymentScenario.h>
#include <Types/EDeploymentMessageState.h>

#define LOCTEXT_NAMESPACE "SCreateContainerFleetSection"

void SCreateContainerFleetSection::Construct(const FArguments& InArgs)
{
	OnRetryStepDelegate = InArgs._OnRetryStepDelegate;
	
	ChildSlot
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right2x)
				.AutoWidth()
				[
					SAssignNew(ProgressBar, SProgressBar)
						.IsLastComponent(true)
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
								.Text(Menu::DeployContainers::kCreateContainerFleetHeader)
								.AutoWrapText(true)
								.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(SPadding::Top3x)
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(STextBlock)
										.Text(Menu::DeployContainers::kCreateContainerFleetDescription)
										.AutoWrapText(true)
										.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(SPadding::Top3x)
								[
									SNew(STextBlock)
										.Text(Menu::DeployContainers::kCreateContainerFleetDescriptionPart2)
										.AutoWrapText(true)
										.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
								]
						]
						// Error text box
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(SPadding::Top2x)
						[
							CreateShowErrorUI()
						]
						// Fleet status row
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.Padding(SPadding::Top2x)
						[
							SAssignNew(FleetStatusRow, SNamedRow)
								.NameText(Menu::DeployContainers::kContainerFleetStatusTitle)
								.RowWidget(
									SAssignNew(FleetStatusTextBlock, STextStatus)
										.StatusTextStyle(Style::Text::kFieldBold)
								)
								.SecondaryColumnLeftPadding(true)
								.Visibility_Lambda([&]() -> EVisibility
									{
										return ShowFleetDetails() ? EVisibility::Visible : EVisibility::Collapsed;
									})
						]
						// Fleet id row
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.Padding(SPadding::Top2x)
						[
							SAssignNew(FleetIdRow, SNamedRow)
								.NameText(Menu::DeployContainers::kContainerFleetIdTitle)
								.RowWidget(
									SAssignNew(FleetIdTextBlock, STextBlock)
									.AutoWrapText(true)
									.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
								)
								.SecondaryColumnLeftPadding(true)
								.Visibility_Lambda([&]() -> EVisibility
									{
										return ShowFleetDetails() ? EVisibility::Visible : EVisibility::Collapsed;
									})
						]
						// Fleet type row
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.Padding(SPadding::Top2x)
						[
							SAssignNew(FleetTypeRow, SNamedRow)
								.NameText(Menu::DeployContainers::kContainerFleetTypeTitle)
								.RowWidget(
									SAssignNew(FleetTypeTextBlock, STextBlock)
									.AutoWrapText(true)
									.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
								)
								.SecondaryColumnLeftPadding(true)
								.Visibility_Lambda([&]() -> EVisibility
									{
										return ShowFleetDetails() ? EVisibility::Visible : EVisibility::Collapsed;
									})
						]
						// Instance type row
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.Padding(SPadding::Top2x)
						[
							SAssignNew(InstanceTypeRow, SNamedRow)
								.NameText(Menu::DeployContainers::kContainerInstanceTypeTitle)
								.RowWidget(
									SAssignNew(InstanceTypeTextBlock, STextBlock)
									.AutoWrapText(true)
									.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
								)
								.SecondaryColumnLeftPadding(true)
								.Visibility_Lambda([&]() -> EVisibility
									{
										return ShowFleetDetails() ? EVisibility::Visible : EVisibility::Collapsed;
									})
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
										OnRetryStepDelegate.ExecuteIfBound();
										return FReply::Handled();
									})
								.Visibility_Lambda([&]() -> EVisibility
									{
										UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
										return ContainersStatus->ErrorCreatingContainerFleet && ContainersStatus->IsCgdConfigurationFinished ? EVisibility::Visible : EVisibility::Collapsed;
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
						// View in Gamelift Console button
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						.Padding(SPadding::Top3x)
						[
							SNew(SButton)
								.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
								.Visibility_Lambda([&]() -> EVisibility
									{
										UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
										// On error, we will hide this because the error banner will have a button linking to CFN
										return !ContainersStatus->ErrorCreatingContainerFleet && ContainersStatus->IsCgdConfigurationFinished ? EVisibility::Visible : EVisibility::Collapsed;
									})
								.OnClicked_Lambda([]
									{
										UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
										FString CurrentAwsRegion = Settings->AwsRegion.ToString().IsEmpty() ? FString("us-west-2") : Settings->AwsRegion.ToString();
										FPlatformProcess::LaunchURL(*FString::Format(Url::kAWSConsoleUrlContainerFleetFormat, { CurrentAwsRegion }), nullptr, nullptr);
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
								]
						]
				]
		];
	UpdateUIBasedOnCurrentState();
	SCreateContainerGroupSection::OnConfigureCgdMultiDelegate.AddSP(this, &SCreateContainerFleetSection::StartSectionWorkflow);
	SCreateContainerGroupSection::OnDeploymentCompleteMultiDelegate.AddSP(this, &SCreateContainerFleetSection::OnDeploymentComplete);
}

TSharedRef<SWidget> SCreateContainerFleetSection::CreateViewInCFNButton()
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

TSharedRef<SWidget> SCreateContainerFleetSection::CreateShowErrorUI()
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
				return ContainersStatus->ErrorCreatingContainerFleet && ContainersStatus->IsCgdConfigurationFinished ? EVisibility::Visible : EVisibility::Collapsed;
			})
		.FailureRowWidget(RowWidget)
		.SetState(ESetupMessageState::FailureMessage);
}

bool SCreateContainerFleetSection::ShowFleetDetails() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	return ContainersStatus->IsCgdConfigurationFinished;
}

void SCreateContainerFleetSection::UpdateUIBasedOnCurrentState()
{
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	if (ContainersStatus->ErrorCreatingContainerFleet) {
		if (ContainersStatus->IsCgdConfigurationFinished) {
			ErrorTextBlock->SetText(ContainersStatus->ErrorMessageCreatingContainerFleet);
			UpdateStepStatusUI(EDeploymentMessageState::FailureMessage);
			PopulateFleetDetails();
		}
	}
	else if (ContainersStatus->IsLastStepComplete) {
		UpdateStepStatusUI(EDeploymentMessageState::ActiveMessage);
		PopulateFleetDetails();
	}
	else if (ShouldStartSectionWorkflow()) {
		StartSectionWorkflow();
	}
	else
	{
		ResetUIToNotStarted();
	}
}

bool SCreateContainerFleetSection::ShouldStartSectionWorkflow() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	if (ContainersStatus->ContainerFleetDeploymentStatus == (int)EContainerFleetDeploymentStatus::DeploymentInProgress && ContainersStatus->IsCgdConfigurationFinished) {
		return true;
	}
	return false;
}

void SCreateContainerFleetSection::ResetUIToNotStarted() {
	UpdateStepStatusUI(EDeploymentMessageState::NoDeploymentMessage);
	SetFieldInputsToEmptyStatuses();
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	ContainersStatus->ErrorCreatingContainerFleet = false;
	ContainersStatus->SaveConfig();
}

void SCreateContainerFleetSection::PopulateFleetDetails() {
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	FleetIdTextBlock->SetText(FText::FromString(ContainersStatus->FleetId));
	FleetTypeTextBlock->SetText(FText::FromString(ContainersStatus->FleetType));
	InstanceTypeTextBlock->SetText(FText::FromString(ContainersStatus->InstanceType));
}

void SCreateContainerFleetSection::SetFieldInputsToEmptyStatuses()
{
	// Set field inputs to empty status
	FString EmptyStatus = "-";
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	FleetIdTextBlock->SetText(FText::FromString(EmptyStatus));
	FleetTypeTextBlock->SetText(FText::FromString(EmptyStatus));
	InstanceTypeTextBlock->SetText(FText::FromString(EmptyStatus));

	ContainersStatus->FleetId = EmptyStatus;
	ContainersStatus->FleetType = EmptyStatus;
	ContainersStatus->InstanceType = EmptyStatus;
	ContainersStatus->SaveConfig();
}

void SCreateContainerFleetSection::StartSectionWorkflow()
{
	SetFieldInputsToEmptyStatuses();
	UpdateStepStatusUI(EDeploymentMessageState::InProgressMessage);
	PollDescribeContainerFleet();
}

void SCreateContainerFleetSection::PollDescribeContainerFleet()
{
	Async(EAsyncExecution::Thread,
		[this]() mutable
		{
			UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
			FString StackIdentifier = ContainersStatus->StackIdentifier;

			int i = 0;
			for (; i < WaitTimesCount; ++i) {
				// If the cgd is not setup, it means the whole thing is reset
				auto ContainerState = EDeploymentMessageStateFromString(ContainersStatus->Status.ToString());
				if (!ContainersStatus->IsCgdConfigurationFinished || ContainerState != EDeploymentMessageState::InProgressMessage) {
					return;
				}

				IGameLiftContainersHandler& Handler = IGameLiftCoreModule::Get().GetGameLiftContainersHandler();
				// Pass in StackIdentifier for the GameName field in the deployer as a quick workaround
				// to decouple containers and ec2 stack deployments and fix describe container fleet calls
				GameLiftDescribeContainerFleetResult DescribeContainerFleetResult = Handler.DescribeContainerFleet(StackIdentifier);
				Async(EAsyncExecution::TaskGraphMainThread,
					[this, DescribeContainerFleetResult = MoveTemp(DescribeContainerFleetResult)]() mutable
					{
						UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
						// Populate fields only when describe call is successful
						if (DescribeContainerFleetResult.bIsSuccessful) {
							ContainersStatus->FleetId = DescribeContainerFleetResult.FleetId;
							ContainersStatus->FleetStatus = DescribeContainerFleetResult.FleetStatus;
							ContainersStatus->FleetType = DescribeContainerFleetResult.FleetType;
							ContainersStatus->InstanceType = DescribeContainerFleetResult.InstanceType;
							PopulateFleetDetails();
							ContainersStatus->SaveConfig();
						}
					});

				if (DescribeContainerFleetResult.bIsSuccessful) {
					return;
				}

				FPlatformProcess::Sleep(WaitIntervalSeconds);
			}
			if (i == WaitTimesCount) {
				Async(EAsyncExecution::TaskGraphMainThread,
					[this]() mutable
					{
						UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
						UpdateStepStatusUI(EDeploymentMessageState::FailureMessage);
						ContainersStatus->ErrorCreatingContainerFleet = true;
						ErrorTextBlock->SetText(Menu::DeployContainers::kTimedOutPollingDeploymentStatusErrorMessage);
						ContainersStatus->ErrorMessageCreatingContainerFleet = Menu::DeployContainers::kTimedOutPollingDeploymentStatusErrorMessage;
						UE_LOG(GameLiftPluginLog, Display, TEXT("Timed out polling for deployment status"));
						ContainersStatus->SaveConfig();
					});
			}
		});
}

void SCreateContainerFleetSection::OnDeploymentComplete()
{
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
	if (ContainersStatus->IsCgdConfigurationFinished) {
		auto ContainerState = EDeploymentMessageStateFromString(ContainersStatus->Status.ToString());
		if (ContainerState == EDeploymentMessageState::FailureMessage) {
			ErrorTextBlock->SetText(ContainersStatus->LatestError);
		}
		UpdateStepStatusUI(ContainerState);
	}
	
}

void SCreateContainerFleetSection::UpdateStepStatusUI(EDeploymentMessageState DeploymentState)
{
	if (DeploymentState == EDeploymentMessageState::NoDeploymentMessage)
	{
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::NotStart);
		FleetStatusTextBlock->SetState(STextStatus::EIconState::Inactive, Menu::DeployContainers::kNoDeployment);
	}
	else if (DeploymentState == EDeploymentMessageState::InProgressMessage)
	{
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::InProgress);
		FleetStatusTextBlock->SetState(STextStatus::EIconState::Loading, Menu::DeployContainers::kInProgressDeployment);
	}
	else if (DeploymentState == EDeploymentMessageState::ActiveMessage)
	{
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressComplete);
		FleetStatusTextBlock->SetState(STextStatus::EIconState::Success, Menu::DeployContainers::kActiveDeployment);
	}
	else if (DeploymentState == EDeploymentMessageState::FailureMessage)
	{
		ProgressBar->ChangeProgressBarUIState(SProgressBar::EProgressBarUIState::ProgressError);
		FleetStatusTextBlock->SetState(STextStatus::EIconState::Error, Menu::DeployContainers::kFailedDeployment);
	}
}

#undef LOCTEXT_NAMESPACE