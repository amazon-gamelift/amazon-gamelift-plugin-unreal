// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SGameParametersSection.h"

#include "Settings/UGameLiftDeploymentStatus.h"
#include "Settings/UGameLiftSettings.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/STextSeparator.h"
#include "SWidgets/SDeploymentFields.h"
#include "SWidgets/SNamedRow.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#include "Types/EManagedEC2DeploymentScenario.h"
#include "Types/EDeploymentMessageState.h"

#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "IGameLiftCoreModule.h"

#include "SGameLiftDeployManagedEC2Menu.h"
#include "SDeployScenarioSection.h"

#define LOCTEXT_NAMESPACE "SGameParametersSection"

void SGameParametersSection::Construct(const FArguments& InArgs)
{
	DeploymentFields = InArgs._SetDeploymentFields;

	SSectionStep::Construct(
		SSectionStep::FArguments()
		.HeaderTitle(Menu::DeployManagedEC2::kGameParametersHeader)
		.HeaderDescription(Menu::DeployManagedEC2::kGameParametersDescription)
		.BodyContent()
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(SPadding::Top_Bottom)
				[
					CreateMetricsInfoMessage()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(SPadding::Top_Bottom)
				[
					CreateMetricsCheckBox()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					DeploymentFields.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(SPadding::Top_Bottom)
				[
					SAssignNew(EnablePlayerGatewayRow, SBox)
					[
						CreatePlayerGatewayCheckBox()
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(SPadding::Top_Bottom)
				[
					SAssignNew(GameServerIpProtocolRow, SBox)
					[
						CreateGameServerIpProtocolRadioButtons()
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				.Padding(SPadding::Top3x)
				[
					SAssignNew(SectionSwitcher, SWidgetSwitcher)
						+ SWidgetSwitcher::Slot()
						[
							CreateSubmissionButton()
						]
						+ SWidgetSwitcher::Slot()
						[
							CreateModifyButton()
						]
				]
		]);

	SDeployScenarioSection::OnEC2DeploymentProgressChangedMultiDelegate.AddSP(this, &SGameParametersSection::UpdateUIBasedOnCurrentState);
	UpdateUIBasedOnCurrentState();

	// Subscribe to OS changes and set initial visibility
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	DeploymentInfo->OnOperatingSystemChanged.AddSP(this, &SGameParametersSection::UpdatePlayerGatewayVisibility);
	UpdatePlayerGatewayVisibility();
}

SGameParametersSection::~SGameParametersSection()
{
	if (DeploymentFields.IsValid())
	{
		auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
		DeploymentInfo->OnOperatingSystemChanged.RemoveAll(this);
	}
}

TSharedRef<SWidget> SGameParametersSection::CreateMetricsInfoMessage()
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

TSharedRef<SWidget> SGameParametersSection::CreateMetricsCheckBox()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	EnableMetricsCheckBox = SNew(SCheckBox)
		.IsChecked(DeploySettings->EnableMetrics ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged_Raw(this, &SGameParametersSection::OnEnableMetricsChanged);
	
	return SNew(SNamedRow)
		.NameText(Menu::DeployCommon::kEnableMetricsTitle)
		.NameTooltipText(Menu::DeployCommon::kEnableMetricsTooltip)
		.RowWidget(EnableMetricsCheckBox.ToSharedRef());

}

TSharedRef<SWidget> SGameParametersSection::CreatePlayerGatewayCheckBox()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	EnablePlayerGatewayCheckBox = SNew(SCheckBox)
		.IsChecked(DeploySettings->EnablePlayerGateway ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged_Raw(this, &SGameParametersSection::OnEnablePlayerGatewayChanged);

	return SNew(SNamedRow)
		.NameText(Menu::DeployCommon::kEnablePlayerGatewayTitle)
		.NameTooltipText(Menu::DeployCommon::kEnablePlayerGatewayTooltip)
		.RowWidget(EnablePlayerGatewayCheckBox.ToSharedRef());
}

TSharedRef<SWidget> SGameParametersSection::CreateGameServerIpProtocolRadioButtons()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	if (DeploySettings->GameServerIpProtocol.IsEmpty())
	{
		DeploySettings->GameServerIpProtocol = Menu::DeployManagedEC2::kGameServerIpProtocolIpv4;
	}

	return SNew(SNamedRow)
		.NameText(Menu::DeployManagedEC2::kServerIpProtocolTitle)
		.NameTooltipText(Menu::DeployManagedEC2::kServerIpProtocolTooltip)
		.RowWidget(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0, 0, 10, 0)
			[
				SNew(SCheckBox)
				.Style(FAppStyle::Get(), "RadioButton")
				.IsChecked(this, &SGameParametersSection::IsGameServerIpProtocolChecked, EIpProtocol::IPv4)
				.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState)
				{
					OnGameServerIpProtocolChanged(EIpProtocol::IPv4, NewState);
				})
				[
					SNew(STextBlock)
					.Text(Menu::DeployManagedEC2::kIpv4RadioButtonText)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.Style(FAppStyle::Get(), "RadioButton")
				.IsChecked(this, &SGameParametersSection::IsGameServerIpProtocolChecked, EIpProtocol::DualStack)
				.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState)
				{
					OnGameServerIpProtocolChanged(EIpProtocol::DualStack, NewState);
				})
				[
					SNew(STextBlock)
					.Text(Menu::DeployManagedEC2::kDualStackRadioButtonText)
				]
			]
		);
}

TSharedRef<SWidget> SGameParametersSection::CreateSubmissionButton()
{
	return SAssignNew(SubmissionButton, SButton)
		.Text(Menu::DeployManagedEC2::kSetParametersForDeploymentButtonText)
		.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
		.OnClicked_Raw(this, &SGameParametersSection::OnSubmissionButtonClicked)
		.IsEnabled_Lambda([&]()
			{
				return AreDeploySettingsComplete();
			});
}

TSharedRef<SWidget> SGameParametersSection::CreateModifyButton()
{
	return SAssignNew(ModifyButton, SButton)
		.Text(Menu::DeployManagedEC2::kModifyParametersButtonText)
		.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
		.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
		.OnClicked_Raw(this, &SGameParametersSection::OnModifyButtonClicked);
}

FReply SGameParametersSection::OnSubmissionButtonClicked()
{
	CompleteSection();
	StartNextSection();

	return FReply::Handled();
}

bool SGameParametersSection::AreDeploySettingsComplete()
{
	// ExtraServerResourcesPath field isn't visible so we exclude it here
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	return !DeploymentInfo->GetBuildName().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetBuildOperatingSystem().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetBuildFolderPath().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetBuildFilePath().IsEmptyOrWhitespace() &&
		!DeploymentInfo->GetOutConfigFilePath().IsEmptyOrWhitespace();
}

FReply SGameParametersSection::OnModifyButtonClicked()
{
	ResetAndCollapseNextSections();

	return FReply::Handled();
}

void SGameParametersSection::OnEnableMetricsChanged(ECheckBoxState NewState)
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	DeploySettings->EnableMetrics = (NewState == ECheckBoxState::Checked);
	DeploySettings->SaveConfig();
}

void SGameParametersSection::OnEnablePlayerGatewayChanged(ECheckBoxState NewState)
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	DeploySettings->EnablePlayerGateway = (NewState == ECheckBoxState::Checked);
	DeploySettings->SaveConfig();

	// Show/hide IP protocol options based on player gateway state
	if (GameServerIpProtocolRow.IsValid())
	{
		GameServerIpProtocolRow->SetVisibility(
			DeploySettings->EnablePlayerGateway ? EVisibility::Visible : EVisibility::Collapsed
		);
	}
}

ECheckBoxState SGameParametersSection::IsGameServerIpProtocolChecked(EIpProtocol Protocol) const
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	FString ProtocolString = (Protocol == EIpProtocol::DualStack) ? Menu::DeployManagedEC2::kGameServerIpProtocolDualStack : Menu::DeployManagedEC2::kGameServerIpProtocolIpv4;
	return (DeploySettings->GameServerIpProtocol == ProtocolString)
		? ECheckBoxState::Checked
		: ECheckBoxState::Unchecked;
}

void SGameParametersSection::OnGameServerIpProtocolChanged(EIpProtocol Protocol, ECheckBoxState NewState)
{
	if (NewState == ECheckBoxState::Checked)
	{
		UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
		DeploySettings->GameServerIpProtocol = (Protocol == EIpProtocol::DualStack) ? Menu::DeployManagedEC2::kGameServerIpProtocolDualStack : Menu::DeployManagedEC2::kGameServerIpProtocolIpv4;
		DeploySettings->SaveConfig();
	}
}

void SGameParametersSection::UpdateUIBasedOnCurrentState()
{
	// TODO: Add state for each section completion
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	EDeploymentMessageState State = EDeploymentMessageStateFromString(DeploySettings->Status.ToString());

	ModifyButton->SetEnabled(State != EDeploymentMessageState::InProgressMessage);

	if (State != EDeploymentMessageState::NoDeploymentMessage)
	{
		CompleteSection();
	}
	else
	{
		ResetAndCollapseSection();
	}
}

void SGameParametersSection::CompleteSection()
{
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	DeploymentInfo->SetAllFieldsReadOnly(true);
	EnableMetricsCheckBox->SetEnabled(false);
	EnablePlayerGatewayCheckBox->SetEnabled(false);
	GameServerIpProtocolRow->SetEnabled(false);
	SectionSwitcher->SetActiveWidgetIndex((int32)ESectionUIState::Complete);
	SetProgressBarState(SProgressBar::EProgressBarUIState::ProgressComplete);
	UpdatePlayerGatewayVisibility();
}

void SGameParametersSection::StartSection()
{
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	DeploymentInfo->SetAllFieldsReadOnly(false);
	EnableMetricsCheckBox->SetEnabled(true);
	EnablePlayerGatewayCheckBox->SetEnabled(true);
	GameServerIpProtocolRow->SetEnabled(true);
	SectionSwitcher->SetActiveWidgetIndex((int32)ESectionUIState::InComplete);
	UpdatePlayerGatewayVisibility();
}

void SGameParametersSection::UpdatePlayerGatewayVisibility()
{
	if (!EnablePlayerGatewayRow.IsValid() || !DeploymentFields.IsValid())
	{
		return;
	}

	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	auto DeploymentInfo = AsSDeploymentFieldsRef(DeploymentFields);
	FText OS = DeploymentInfo->GetBuildOperatingSystem();
	
	bool bIsWindows = OS.EqualTo(Menu::DeployManagedEC2::kWindowsServer2016Value) || 
	                  OS.EqualTo(Menu::DeployManagedEC2::kWindowsServer2022Value);
	
	EnablePlayerGatewayRow->SetVisibility(bIsWindows ? EVisibility::Collapsed : EVisibility::Visible);
	GameServerIpProtocolRow->SetVisibility(bIsWindows || !DeploySettings->EnablePlayerGateway ? EVisibility::Collapsed : EVisibility::Visible);
}

#undef LOCTEXT_NAMESPACE
