// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SBootstrapStatus.h"

#include <EditorStyleSet.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/Layout/SBorder.h>
#include <Widgets/Layout/SWidgetSwitcher.h>

#include "SWidgets/SNamedRow.h"
#include "SWidgets/STextStatus.h"
#include "Settings/UGameLiftSettings.h"

#include "GameLiftPluginStyle.h"

namespace Internal
{
	TSharedRef<SWidget> MakeNoBootstrapStatusWidget()
	{
		TSharedRef<STextStatus> TextStatus = SNew(STextStatus)
			.StatusText(Settings::AwsAccount::kNoBootstrapMessage)
			.StatusTextStyle(Style::Text::kFieldBold)
			.IconState(STextStatus::EIconState::Inactive);

		TextStatus->SetStatusTextColor(FGameLiftPluginStyle::Get().GetColor(Style::Color::kInactive));

		return TextStatus;
	}

	TSharedRef<SWidget> MakeInProgressStatusWidget()
	{
		TSharedRef<STextStatus> TextStatus = SNew(STextStatus)
			.StatusText(Settings::AwsAccount::kBootstrapInProgressMessage)
			.StatusTextStyle(Style::Text::kFieldBold)
			.IconState(STextStatus::EIconState::Loading);

		TextStatus->SetStatusTextColor(FGameLiftPluginStyle::Get().GetColor(Style::Color::kLoading));

		return TextStatus;
	}

	TSharedRef<SWidget> MakeActiveStatusWidget()
	{
		TSharedRef<STextStatus> TextStatus = SNew(STextStatus)
			.StatusText(Settings::AwsAccount::kActiveBootstrapMessage)
			.StatusTextStyle(Style::Text::kFieldBold)
			.IconState(STextStatus::EIconState::Success);

		TextStatus->SetStatusTextColor(FGameLiftPluginStyle::Get().GetColor(Style::Color::kSuccess));

		return TextStatus;
	}

	TSharedRef<SWidget> MakeFailureStatusWidget()
	{
		TSharedRef<STextStatus> TextStatus = SNew(STextStatus)
			.StatusText(Settings::AwsAccount::kFailedBootstrapMessage)
			.StatusTextStyle(Style::Text::kFieldBold)
			.IconState(STextStatus::EIconState::Error);

		TextStatus->SetStatusTextColor(FGameLiftPluginStyle::Get().GetColor(Style::Color::kError));

		return TextStatus;
	}
} // namespace Internal

#define LOCTEXT_NAMESPACE "SBootstrapStatus"


void SBootstrapStatus::Construct(const FArguments& InArgs)
{
	TSharedRef<SWidget> NeedDeploymentWidget = Internal::MakeNoBootstrapStatusWidget();
	TSharedRef<SWidget> InProgressWidget = Internal::MakeInProgressStatusWidget();
	TSharedRef<SWidget> ActiveWidget = Internal::MakeActiveStatusWidget();
	TSharedRef<SWidget> FailureWidget = Internal::MakeFailureStatusWidget();

	UGameLiftSettings* GameLiftSettings = GetMutableDefault<UGameLiftSettings>();
	BootstrapState = EBootstrapMessageState::NoBootstrappedMessage;

	ChildSlot
	[
		SNew(SWidgetSwitcher)
		.WidgetIndex(this, &SBootstrapStatus::GetSetupStateAsInt)
		+ SWidgetSwitcher::Slot()
		[
			NeedDeploymentWidget
		]
		+ SWidgetSwitcher::Slot()
		[
			InProgressWidget
		]
		+ SWidgetSwitcher::Slot()
		[
			FailureWidget
		]
		+ SWidgetSwitcher::Slot()
		[
			ActiveWidget
		]
	];
}

void SBootstrapStatus::OnNeedBootstrap()
{
	UpdateState(int(EBootstrapMessageState::NoBootstrappedMessage));
}

void SBootstrapStatus::OnBootstrap()
{
	UpdateState(int(EBootstrapMessageState::InProgressMessage));
}

void SBootstrapStatus::OnFailure()
{
	UpdateState(int(EBootstrapMessageState::FailureMessage));
}

void SBootstrapStatus::OnSucceeded()
{
	UpdateState(int(EBootstrapMessageState::ActiveMessage));
}

int32 SBootstrapStatus::GetSetupStateAsInt() const
{
	return (int32)BootstrapState;
}

void SBootstrapStatus::UpdateState(int NewState)
{
	BootstrapState = EBootstrapMessageStateFromInt(NewState);
}

#undef LOCTEXT_NAMESPACE
