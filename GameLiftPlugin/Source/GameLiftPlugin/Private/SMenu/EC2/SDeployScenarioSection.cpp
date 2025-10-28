// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SDeployScenarioSection.h"

#include <Widgets/Text/SRichTextBlock.h>
#include "Widgets/Layout/SWidgetSwitcher.h"
#include <Framework/MetaData/DriverMetaData.h>

#include "SWidgets/SBootstrapStatus.h"
#include "SWidgets/SDeploymentFields.h"
#include "SWidgets/SDeploymentStatus.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SOnlineHyperlink.h"

#include "Settings/UGameLiftDeploymentStatus.h"
#include "Settings/UGameLiftSettings.h"

#include "Utils/Misc.h"
#include "Utils/Notifier.h"
#include "Utils/StringPaths.h"
#include <Async/Async.h>

#include "IGameLiftCoreModule.h"
#include "GameLiftPlugin.h"
#include "GameLiftPluginStyle.h"

#define LOCTEXT_NAMESPACE "SDeployScenarioSection"

FDeploymentProgress SDeployScenarioSection::OnEC2DeploymentProgressChangedMultiDelegate;
void SDeployScenarioSection::Construct(const FArguments& InArgs)
{
	DeploymentFields = InArgs._SetDeploymentFields;
	DeploymentStatus = SNew(SDeploymentStatus);

	SSectionStep::Construct(
		SSectionStep::FArguments()
		.HeaderTitle(Menu::DeployManagedEC2::kDeploymentScenarioHeader)
		.HeaderDescription(Menu::DeployManagedEC2::kDeploymentStatusInfoRichText)
		.BodyContent()
		[
			SNew(SVerticalBox)
				// Deployment status
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					DeploymentStatus.ToSharedRef()
				]
				// Buttons
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(SPadding::Top3x)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						[
							SNew(SWidgetSwitcher)
								.WidgetIndex_Lambda([&]
									{
										UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
										EDeploymentMessageState State = EDeploymentMessageStateFromString(DeploySettings->Status.ToString());
										if (State == EDeploymentMessageState::InProgressMessage || State == EDeploymentMessageState::AbortInProgressMessage)
										{
											// Stop deployment
											return 2;
										}
										else if (State == EDeploymentMessageState::FailureMessage || State == EDeploymentMessageState::NoDeploymentMessage)
										{
											// Deploy scenario
											return 0;
										}
										else {
											// Redeploy scenario
											return 1;
										}
									})
								+ SWidgetSwitcher::Slot()
								[
									SNew(SButton)
										.Text(Menu::DeployManagedEC2::kDeployCloudFormationButtonText)
										.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
										.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
										.OnClicked_Raw(this, &SDeployScenarioSection::DeployCloudFormation)
								]
								+ SWidgetSwitcher::Slot()
								[
									SNew(SButton)
										.Text(Menu::DeployManagedEC2::kRedeployCloudFormationButtonText)
										.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
										.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
										.OnClicked_Raw(this, &SDeployScenarioSection::DeployCloudFormation)
								]
								+ SWidgetSwitcher::Slot()
								[
									// Button to stop
									SNew(SButton)
										.Text(Menu::DeployManagedEC2::kStopCloudFormationButtonText)
										.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kErrorButtonStyleName)
										.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
										.ToolTipText_Lambda([&]()
											{
												return HandleStopButtonState().second;
											})
										.OnClicked_Raw(this, &SDeployScenarioSection::StopDeploying)
										.IsEnabled_Lambda([&]()
											{
												return HandleStopButtonState().first;
											})
								]
						]
						// Hyperlink
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Right)
						.Padding(SPadding::Right) // move the link left a bit
						[
							CreateCloudFormationInConsoleHyperLink()
						]
				]
		]);

		SetDefaultValues();
		UpdateUIBasedOnCurrentState();

		// Only collapsed if no deployment on initialization
		UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
		EDeploymentMessageState State = EDeploymentMessageStateFromString(DeploySettings->Status.ToString());

		if (State == EDeploymentMessageState::NoDeploymentMessage)
		{
			SetExpansion(false);
		}
}

bool SDeployScenarioSection::IsDeploymentStatusActive()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	return EDeploymentMessageStateFromString(DeploySettings->Status.ToString()) == EDeploymentMessageState::ActiveMessage;
}

std::pair<bool, FText> SDeployScenarioSection::HandleStopButtonState()
{
	bool IsDeploymentInProgress = AsSDeploymentStatusRef(DeploymentStatus)->GetCurrentState() == EDeploymentMessageState::InProgressMessage;
	if (IsDeploymentInProgress)
	{
		return { true, Menu::DeployManagedEC2::kStopDeploymentEnabledTooltip };
	}
	else
	{
		return { false, Menu::DeployManagedEC2::kStopDeploymentDisabledTooltip };
	}
}

std::pair<bool, FText> SDeployScenarioSection::HandleDeployButtonState()
{
	if (AsSDeploymentFieldsRef(DeploymentFields)->GetBuildName().IsEmptyOrWhitespace())
	{
		return { false, Menu::DeployManagedEC2::kDeploymentDisabledBuildNameEmptyTooltip };
	}

	if (AsSDeploymentFieldsRef(DeploymentFields)->GetBuildName().ToString().Len() > Menu::DeployManagedEC2::kMaxGameNameLength)
	{
		return { false, Menu::DeployManagedEC2::kDeploymentDisabledBuildNameTooLongTooltip };
	}

	if (AsSDeploymentFieldsRef(DeploymentFields)->GetBuildFilePath().IsEmptyOrWhitespace() || AsSDeploymentFieldsRef(DeploymentFields)->GetBuildFolderPath().IsEmptyOrWhitespace())
	{
		return { false, Menu::DeployManagedEC2::kDeploymentDisabledServerBuildPathNotSetTooltip };
	}

	bool IsDeploymentInProgress = AsSDeploymentStatusRef(DeploymentStatus)->GetCurrentState() == EDeploymentMessageState::InProgressMessage;
	if (IsDeploymentInProgress)
	{
		return { false, Menu::DeployManagedEC2::kDeploymentDisabledAlreadyActiveTooltip };
	}

	auto* Settings = GetMutableDefault<UGameLiftSettings>();
	bool IsBootstrapped = EBootstrapMessageStateFromInt(Settings->BootstrapStatus) == EBootstrapMessageState::ActiveMessage;
	if (!IsBootstrapped)
	{
		return { false, Menu::DeployCommon::kDeploymentDisabledNeedBootstrapTooltip };
	}

	return { true, Menu::DeployCommon::kDeploymentEnabledTooltip };
}

void SDeployScenarioSection::SetUGameLiftDeploymentStatus(EDeploymentMessageState State)
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	DeploySettings->Status = FText::FromString(EDeploymentMessageStateToString(State));
	DeploySettings->SaveConfig();
}

void SDeployScenarioSection::StartSection()
{
	SetUGameLiftDeploymentStatus(EDeploymentMessageState::NoDeploymentMessage);
}

FReply SDeployScenarioSection::DeployCloudFormation()
{
	if (Paths::DefaultOutConfigFilePath() != AsSDeploymentFieldsRef(DeploymentFields)->GetOutConfigFilePath().ToString())
	{
		UE_LOG(GameLiftPluginLog, Warning, TEXT("%s"), Menu::DeployManagedEC2::Logs::kOutputPathChangedWarning);
	}

	if (IsDeploymentStatusActive())
	{
		auto choice = FMessageDialog::Open(EAppMsgType::YesNo, Menu::DeployManagedEC2::kConfirmReploymentText);
		if (choice != EAppReturnType::Yes) {
			return FReply::Handled();
		}
	}

	SetProgressBarState(SProgressBar::EProgressBarUIState::InProgress);
	AsSDeploymentStatusRef(DeploymentStatus)->OnDeploy();	

	SetUGameLiftDeploymentStatus(EDeploymentMessageState::InProgressMessage);
	OnEC2DeploymentProgressChangedMultiDelegate.Broadcast();

	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	DeploySettings->BuildName = DeploymentInfo->GetBuildName();
	DeploySettings->BuildOperatingSystem = DeploymentInfo->GetBuildOperatingSystem();
	DeploySettings->BuildFolderPath = DeploymentInfo->GetBuildFolderPath();
	DeploySettings->BuildFilePath = DeploymentInfo->GetBuildFilePath();
	DeploySettings->ExtraServerResourcesPath = DeploymentInfo->GetExtraServerResourcesPath();
	DeploySettings->OutConfigFilePath = DeploymentInfo->GetOutConfigFilePath();
	DeploySettings->Scenario = DeploymentInfo->GetDeploymentScenario();

	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	DeploySettings->DeployedRegion = Settings->AwsRegion;
	DeploySettings->ApiGatewayEndpoint = FText::GetEmpty();
	DeploySettings->CognitoClientId = FText::GetEmpty();
	DeploySettings->SaveConfig();

	FText NotifyMessage = FText::GetEmpty();

	NotifyMessage = FText::Format(LOCTEXT("DeploymentNotificationAws", "{0}\n{1}"),
		FText::FromString(Menu::DeployManagedEC2::kDeploymentStartedAwsScenarioNotification), DeploySettings->Scenario);

	NotificationItem = Notifier::CreateNotification(NotifyMessage);

	Async(EAsyncExecution::Thread, [this, NotificationItem = NotificationItem] () mutable
		{
			UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
			auto& deployer = IGameLiftCoreModule::Get().GetScenarioDeployer();

			// Pass in StackIdentifier for the GameName field in the deployer as a quick workaround
			// to decouple containers and ec2 stack deployments and fix describe container fleet calls
			FString HostingSolution = "-ManagedEC2";
			DeploySettings->StackIdentifier = DeploySettings->BuildName.ToString() + HostingSolution;

			bool IsDeployed = deployer.DeployManagedEC2Scenario(
				DeploySettings->Scenario,
				IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance(),
				DeploySettings->StackIdentifier,
				DeploySettings->BuildOperatingSystem.ToString(),
				DeploySettings->BuildFolderPath.ToString(),
				DeploySettings->BuildFilePath.ToString(),
				DeploySettings->OutConfigFilePath.ToString(),
				DeploySettings->ExtraServerResourcesPath.ToString(),
				DeploySettings->EnableMetrics
			);

			if (IsDeployed)
			{
				UE_LOG(GameLiftPluginLog, Display, TEXT("%s"), Menu::DeployManagedEC2::Logs::kDeploymentSucceed);

				DeploySettings->ApiGatewayEndpoint = FText::FromString(deployer.GetLastApiGatewayEndpoint());
				DeploySettings->CognitoClientId = FText::FromString(deployer.GetLastCognitoClientId());
				SetUGameLiftDeploymentStatus(EDeploymentMessageState::ActiveMessage);

				Notifier::CompleteWithSuccess(NotificationItem, Menu::DeployManagedEC2::kDeploymentCompletedNotification);
			}
			else
			{
				UE_LOG(GameLiftPluginLog, Display, TEXT("%s"), Menu::DeployManagedEC2::Logs::kDeploymentFailed);

				DeploySettings->LatestError = Utils::BuildLastestErrorMessage(deployer);
				SetUGameLiftDeploymentStatus(EDeploymentMessageState::FailureMessage);

				Notifier::CompleteWithFailure(NotificationItem, Menu::DeployManagedEC2::kDeploymentFailedNotification);
			}

			DeploySettings->SaveConfig();

			Async(EAsyncExecution::TaskGraphMainThread,
				[this]
				{
					UpdateUIBasedOnCurrentState();
				});
		});

		return FReply::Handled();
}

FReply SDeployScenarioSection::StopDeploying()
{
	auto choice = FMessageDialog::Open(EAppMsgType::YesNo, Menu::DeployManagedEC2::kConfirmStopDeploymentText);

	if (choice != EAppReturnType::Yes)
	{
		return FReply::Handled();
	}

	Async(EAsyncExecution::Thread, [this, Status = DeploymentStatus, NotificationItem = NotificationItem]()
		{
			Notifier::UpdateTextNotification(NotificationItem, Menu::DeployManagedEC2::kDeploymentAbortingNotification);
			SetUGameLiftDeploymentStatus(EDeploymentMessageState::AbortInProgressMessage);

			auto& deployer = IGameLiftCoreModule::Get().GetScenarioDeployer();
			
			if (deployer.StopDeployment(IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance()))
			{
				AsSDeploymentStatusRef(Status)->OnAbort();
				Notifier::CompleteWithSuccess(NotificationItem, Menu::DeployManagedEC2::kDeploymentAbortedNotification);
				StartSection();
			}
			else
			{
				UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
				DeploySettings->LatestError = Utils::BuildLastestErrorMessage(deployer);
				AsSDeploymentStatusRef(Status)->OnFailure();
				SetUGameLiftDeploymentStatus(EDeploymentMessageState::FailureMessage);
				Notifier::CompleteWithFailure(NotificationItem, Menu::DeployManagedEC2::kDeploymentAbortFailedNotification);
			}

			Async(EAsyncExecution::TaskGraphMainThread,
				[this]
				{
					UpdateUIBasedOnCurrentState();
				});

		});

	return FReply::Handled();
}

TSharedRef<SWidget> SDeployScenarioSection::CreateCloudFormationInConsoleHyperLink()
{
	return SNew(SOnlineHyperlink)
		.Text(Menu::DeployManagedEC2::kViewInCloudFormationConsoleHyperLinkText)
		.Visibility_Lambda([&]
		{
				UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
				EDeploymentMessageState State = EDeploymentMessageStateFromString(DeploySettings->Status.ToString());
				return State != EDeploymentMessageState::NoDeploymentMessage ? EVisibility::Visible : EVisibility::Hidden;
		})
		.Link_Lambda([&]
			{
				// Update link with the region associated with the currently selected profile
				FString CurrentAwsRegion(TEXT("us-west-2"));
				UGameLiftDeploymentStatus* DeploymentStatus = GetMutableDefault<UGameLiftDeploymentStatus>();
				if (DeploymentStatus && !DeploymentStatus->DeployedRegion.IsEmpty())
				{
					CurrentAwsRegion = DeploymentStatus->DeployedRegion.ToString();
				}
				return FString::Format(Menu::DeployManagedEC2::kViewInCloudFormationHyperLinkUrlFormat, { CurrentAwsRegion });
			});
}

void SDeployScenarioSection::SetDefaultValues()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();

	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	DeploymentInfo->SetBuildName(DeploySettings->BuildName);
	DeploymentInfo->SetBuildOperatingSystem(DeploySettings->BuildOperatingSystem);
	DeploymentInfo->SetBuildFolderPath(DeploySettings->BuildFolderPath);
	DeploymentInfo->SetBuildFilePath(DeploySettings->BuildFilePath);
	DeploymentInfo->SetExtraServerResourcesPath(DeploySettings->ExtraServerResourcesPath);
	DeploymentInfo->SetOutConfigFilePath(DeploySettings->OutConfigFilePath);

	AsSDeploymentStatusRef(DeploymentStatus)->SetState(EDeploymentMessageStateFromString(DeploySettings->Status.ToString()));
}

void SDeployScenarioSection::UpdateUIBasedOnCurrentState()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	EDeploymentMessageState State = EDeploymentMessageStateFromString(DeploySettings->Status.ToString());

	if (State == EDeploymentMessageState::NoDeploymentMessage)
	{
		SetProgressBarState(SProgressBar::EProgressBarUIState::NotStart);
	}
	else 
	{
		OnEC2DeploymentProgressChangedMultiDelegate.Broadcast();
		SetExpansion(true);

		if (State == EDeploymentMessageState::InProgressMessage || State == EDeploymentMessageState::AbortInProgressMessage)
		{
			SetProgressBarState(SProgressBar::EProgressBarUIState::InProgress);
		}
		else if (State == EDeploymentMessageState::FailureMessage)
		{
			SetProgressBarState(SProgressBar::EProgressBarUIState::ProgressError);
		}
		else if (State == EDeploymentMessageState::ActiveMessage)
		{
			SetProgressBarState(SProgressBar::EProgressBarUIState::ProgressComplete);
		}
	}
}

#undef LOCTEXT_NAMESPACE