// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "SWidgets/SDeploymentFields.h"
#include "SWidgets/SSectionStep.h"
#include "SWidgets/SSetupMessage.h"
#include "Widgets/Input/SCheckBox.h"

enum class EIpProtocol : uint8
{
	IPv4,
	DualStack
};

class SGameParametersSection : public SSectionStep
{
	SLATE_BEGIN_ARGS(SGameParametersSection) { }

	SLATE_ARGUMENT(TSharedPtr<SWidget>, SetDeploymentFields)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	virtual ~SGameParametersSection();

private:
	TSharedRef<SWidget> CreateMetricsInfoMessage();
	TSharedRef<SWidget> CreateMetricsCheckBox();
	TSharedRef<SWidget> CreatePlayerGatewayCheckBox();
	TSharedRef<SWidget> CreateGameServerIpProtocolRadioButtons();
	TSharedRef<SWidget> CreateSubmissionButton();
	TSharedRef<SWidget> CreateModifyButton();
	FReply OnSubmissionButtonClicked();
	FReply OnModifyButtonClicked();
	
	void UpdateUIBasedOnCurrentState();
	bool AreDeploySettingsComplete();
	void CompleteSection();
	void StartSection() override;
	void OnEnableMetricsChanged(ECheckBoxState NewState);
	void OnEnablePlayerGatewayChanged(ECheckBoxState NewState);
	ECheckBoxState IsGameServerIpProtocolChecked(EIpProtocol Protocol) const;
	void OnGameServerIpProtocolChanged(EIpProtocol Protocol, ECheckBoxState NewState);

private:
	/** Updates player gateway visibility based on OS selection. Player gateway is not supported on Windows platforms. */
	void UpdatePlayerGatewayVisibility();

	TSharedPtr<SWidget> DeploymentFields;
	TSharedPtr<SWidgetSwitcher> SectionSwitcher;
	TSharedPtr<SButton> SubmissionButton;
	TSharedPtr<SButton> ModifyButton;
	TSharedPtr<SCheckBox> EnableMetricsCheckBox;
	TSharedPtr<SSetupMessage> MetricsInfoMessage;
	TSharedPtr<SCheckBox> EnablePlayerGatewayCheckBox;
	TSharedPtr<SWidget> EnablePlayerGatewayRow; 
	TSharedPtr<SWidget> GameServerIpProtocolRow;
};
