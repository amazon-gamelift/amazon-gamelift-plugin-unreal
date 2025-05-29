// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SGameLiftSettingsAwsAccountMenu.h"

#include <Async/Async.h>
#include <DetailLayoutBuilder.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/Layout/SWidgetSwitcher.h>
#include <Widgets/Text/SRichTextBlock.h>

#include "IAWSConfigFileProfile.h"
#include "IGameLiftCoreModule.h"

#include "GameLiftPlugin.h"
#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"

#include "Widgets/Layout/SSeparator.h"
#include "SWidgets/SAccountStartupCard.h"
#include "SWidgets/SBootstrapStatus.h"
#include "SWidgets/SInputTextBox.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/SSelectionComboBox.h"
#include "SWidgets/SSetupMessage.h"
#include "SWidgets/STextSeparator.h"
#include "SWidgets/STitleBlock.h"
#include "SWidgets/STextStatus.h"
#include "SWidgets/SExternalLinkButton.h"
#include "SWidgets/SSectionsWithHeaders.h"
#include "SMenu/SBootstrapModal.h"

#include "Settings/FGameLiftSettingsCustomization.h"
#include "Settings/UGameLiftSettings.h"
#include "Settings/FUserProfileInfo.h"
#include "Types/EAwsRegions.h"
#include "Utils/Misc.h"

#define LOCTEXT_NAMESPACE "SGameLiftSettingsAwsAccountMenu"

FOnProfileSelectionChanged SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChangedMultiDelegate;

void SGameLiftSettingsAwsAccountMenu::Construct(const FArguments& InArgs)
{
	ContextWindow = InArgs._ContextWindow;
	ProfileManagementEnabled = InArgs._ProfileManagementEnabled;
	HideButtonsWhenBootstrapped = InArgs._HideButtonsWhenBootstrapped;
	ShowBootstrapStatusOnly = InArgs._ShowBootstrapStatusOnly;

	ProfileListContainer = SNew(SVerticalBox);

	// Make sure current profile settings are still valid based on available profiles
	SyncFromCredentialsFile();

	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	if (!ShowBootstrapStatusOnly)
	{
		VerticalBox->AddSlot()
			.AutoHeight()
			.Padding(SPadding::Bottom)
			[
				MakeHeaderWidget()
			];

		VerticalBox->AddSlot()
			.AutoHeight()
			.Padding(SPadding::Top_Bottom2x + SPadding::Right2x)
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SImage).Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kGameLiftDivider))
					]
			];
	}

	// The message widget still needs to be initialized since there is a member variable pointing to the widget.
	// If ShowBootstrapStatusOnly is true, the visibility of the message widget would be 'collapsed'.
	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Top_Bottom + SPadding::Right2x)
		[
			MakeSetupMessageWidget()
		];

	// Initialize help message widget before using inside profile switcher widget
	MakeAccessCredentialsHelpWidget();

	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Top)
		[
			MakeProfileSwitcherWidget()
		];

	if (ShowBootstrapStatusOnly) {
		VerticalBox->AddSlot()
			.AutoHeight()
			.Padding(SPadding::Top4x)
			[
				MakeDocumentationLinkWidget(InArgs._ReadDeveloperGuideLink)
			];
	}

	ChildSlot
	[
		VerticalBox
	];

	// Listen to "profile selection change" notifications to update the UI when the change comes from another instance.
	// Keeps a weak reference to the object
	OnProfileSelectionChangedMultiDelegate.AddSP(this, &SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChanged);

	SetupMenu();
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeHeaderWidget()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(SPadding::Top_Bottom)
		[
			SNew(STitleBlock)
			.Text(Settings::AwsAccount::kHeaderTitleText)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(SPadding::Top_Bottom + SPadding::Right2x)
		[
			SNew(STextBlock)
			.Text(Settings::AwsAccount::kHeaderDescriptionText)
			.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
			.AutoWrapText(true)
		];
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeSetupMessageWidget()
{
	return SAssignNew(AwsProfileSetupMessage, SSetupMessage)
		.WarningText(Settings::AwsAccount::kNotConfiguredText)
		.FailureText(Settings::AwsAccount::kConfigurationFailedText)
		.ReadyToGoText(Settings::AwsAccount::kConfiguredText)
		.OnUpdateState_Raw(this, &SGameLiftSettingsAwsAccountMenu::DetectBootstrapSetup)
		.WarningButtonStyle(Style::Button::kCloseButtonStyleName)
		.OnButtonClicked_Lambda([&]
			{
				AwsProfileSetupMessage->SetVisibility(EVisibility::Collapsed);
			})
		.Visibility(ShowBootstrapStatusOnly ? EVisibility::Collapsed : EVisibility::Visible);
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeProfileSwitcherWidget()
{
	TSharedRef<SWidget> StartupWidget = MakeProfileStartupWidget();
	TSharedRef<SWidget> ModifyAccount = MakeProfileModificationWidget();
	TSharedRef<SWidget> BootstrapAccount = MakeProfileBootstrappingWidget();

	return SNew(SWidgetSwitcher)
		.WidgetIndex(this, &SGameLiftSettingsAwsAccountMenu::GetAccountManagementStateAsInt)
		+ SWidgetSwitcher::Slot()
		[
			StartupWidget
		]
		+ SWidgetSwitcher::Slot()
		[
			ModifyAccount
		]
		+ SWidgetSwitcher::Slot()
		[
			BootstrapAccount
		];
}

ESetupMessageState SGameLiftSettingsAwsAccountMenu::DetectBootstrapSetup(ESetupMessageState CurrentState)
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	if (Settings->CurrentProfileName.IsEmpty() || !Settings->UserProfileInfoMap.Contains(Settings->CurrentProfileName)) {
		return ESetupMessageState::WarningMessage;
	}
	switch (Settings->UserProfileInfoMap[Settings->CurrentProfileName].BootstrapStatus)
	{
		case (int)EBootstrapMessageState::ActiveMessage:
			return ESetupMessageState::ReadyToGoMessage;
		case (int)EBootstrapMessageState::FailureMessage:
			return ESetupMessageState::FailureMessage;
		default:
			return ESetupMessageState::WarningMessage;
	}
}

void SGameLiftSettingsAwsAccountMenu::OnBootstrapStateChanged()
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	ProfileSelectedActive = Settings->BootstrapStatus == int(EBootstrapMessageState::ActiveMessage);
	RefreshProfileList();
	AwsProfileSetupMessage->Tick();
	// Broadcast profile selection change notification
	OnProfileSelectionChangedMultiDelegate.Broadcast(this);
}

void SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChanged(const SGameLiftSettingsAwsAccountMenu* Sender)
{
	if (this != Sender)
	{
		// Profile selection has changed externally, udpate the UI accordingly
		SetupMenu();
	}
}

void SGameLiftSettingsAwsAccountMenu::SetupMenu()
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	TSharedRef<IAWSConfigFileProfile> Configurator = IGameLiftCoreModule::Get().MakeAWSConfigFileProfile();
	const TArray<FString>& AwsAccountsNames = Configurator->GetProfileNames();
	ProfileSelectedActive = Settings->BootstrapStatus == int(EBootstrapMessageState::ActiveMessage);
	ResetModificationUI();

	if (ShowBootstrapStatusOnly && Settings->CurrentProfileName.IsEmpty())
	{
		// No profile selected
		UpdateAccountManagementState(EAccountManagementState::NoAccount);
		return;
	}
	if (AwsAccountsNames.Num() == 0)
	{
		// Users don't have any account profile.
		UpdateAccountManagementState(EAccountManagementState::NoAccount);
	}
	else
	{
		UpdateAccountManagementState(EAccountManagementState::BootstrapAccount);
		RefreshProfileList();
	}
}

void SGameLiftSettingsAwsAccountMenu::RefreshProfileList()
{
	ProfileListContainer->ClearChildren();

	TSharedRef<IAWSConfigFileProfile> Configurator = IGameLiftCoreModule::Get().MakeAWSConfigFileProfile();
	const TArray<FString>& AwsAccountsNames = Configurator->GetProfileNames();

	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	FString SelectedProfileName = Settings->CurrentProfileName;

	for (int32 Index = 0; Index < AwsAccountsNames.Num(); ++Index)
	{
		const FString& ProfileName = AwsAccountsNames[Index];
		FString Region = Configurator->GetRegion(ProfileName);
		FString Bucket = "-";

		bool IsSelectedProfile = ProfileName.Equals(SelectedProfileName);
		bool ProfileVisible = !ShowBootstrapStatusOnly || IsSelectedProfile;

		if (Settings->UserProfileInfoMap.Contains(ProfileName) &&
			Settings->UserProfileInfoMap[ProfileName].AwsRegion == Region &&
			!Settings->UserProfileInfoMap[ProfileName].S3Bucket.IsEmpty())
		{
			Bucket = Settings->UserProfileInfoMap[ProfileName].S3Bucket;
		}
		if (IsSelectedProfile && RadioButtonSelectedProfile.IsEmpty()) {
			RadioButtonSelectedProfile = ProfileName;
		}
		TSharedRef<SWidget> RowWidget = CreateProfileRow(ProfileName, Region, Bucket, IsSelectedProfile, ProfileVisible);
		ProfileListContainer->AddSlot()
			.Padding(SPadding::Right2x)
			.AutoHeight()
		[RowWidget];
	}
}

void SGameLiftSettingsAwsAccountMenu::UpdateAccountManagementState(EAccountManagementState NewState)
{
	AccountManagementState = NewState;
}

int32 SGameLiftSettingsAwsAccountMenu::GetAccountManagementStateAsInt() const
{
	return (int32)AccountManagementState;
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeProfileDescriptionChildWidget(const FText& HeaderText, const FText& DescriptionText)
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(SPadding::Top_Bottom)
		[
			SNew(SSubtitleBlock)
			.Text(HeaderText)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(SPadding::Top_Bottom)
		[
			SNew(STextBlock)
			.Text(DescriptionText)
			.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
			.AutoWrapText(true)
		];
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeProfileStartupWidget()
{
	if (ShowBootstrapStatusOnly) {
		return SNew(SSetupMessage)
			.WarningText(Settings::AwsAccount::kNoDeploymentProfileText)
			.WarningButtonText(Settings::AwsAccount::kNoDeploymentProfileButtonText)
			.WarningButtonStyle(Style::Button::kSuccessButtonStyleName)
			.OnButtonClicked_Lambda([&]
				{
					FGameLiftPluginModule* Module = FModuleManager::GetModulePtr<FGameLiftPluginModule>(Plugin::GameLiftPluginTabName);
					Module->OpenSettingsButtonClicked();
				})
			.SetState(ESetupMessageState::WarningMessage);
	}

	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(SPadding::Top2x_Bottom2x)
		[
			SNew(SAccountStartupCard)
			.Icon(Style::Brush::kAccountMissingIconName)
			.DescriptionText(Settings::AwsAccount::kProfileStartupAccountMissingText)
			.ButtonText(Settings::AwsAccount::kProfileStartupAccountMissingButtonText)
			.OnButtonClicked_Lambda([]
			{
				FPlatformProcess::LaunchURL(*FString(Url::kAwsAccountCreateAwsAccountUrl), nullptr, nullptr);
			})
			.IsHyperlinkButton(true)
			.MiddleDescriptionText(Settings::AwsAccount::kProfileStartupAccountMissingDescriptionText)
		]
		+ SHorizontalBox::Slot()
		.Padding(SPadding::Top2x_Bottom2x + SPadding::Left2x_Right2x)
		[
			SNew(SAccountStartupCard)
			.Icon(Style::Brush::kAccountIconName)
			.DescriptionText(Settings::AwsAccount::kProfileStartupAccountText)
			.ButtonText(Settings::AwsAccount::kProfileStartupAccountButtonText)
			.UseSuccessButton(true)
			.OnButtonClicked_Lambda([&]
			{
				UpdateAccountManagementState(EAccountManagementState::ModifyAccount);
			})
			.MiddleDescriptionText(Settings::AwsAccount::kProfileStartupAccountDescriptionText)
		];
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeProfileModificationWidget()
{
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	if (!ShowBootstrapStatusOnly)
	{
		VerticalBox->AddSlot()
			.AutoHeight()
			.Padding(SPadding::Bottom + SPadding::Right2x)
			[
				MakeProfileDescriptionChildWidget(
					Settings::AwsAccount::kProfileDetailsTitleText,
					Settings::AwsAccount::kProfileModificationDescriptionText)
			];
	}

	VerticalBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(SPadding::Top_Bottom + SPadding::Right2x)
		[
			MakeAddProfileWidget(false)
		];

	return VerticalBox;
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeContainersUnsupportedWidget()
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	TSharedRef<SHorizontalBox> ContainersUnsupportedWidget = SNew(SHorizontalBox);
	ContainersUnsupportedWidget->AddSlot()
		.FillWidth(1)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
				.WidthOverride(Style::kMessageTextBoxWidth)
				[
					SNew(STextBlock)
						.Text(Settings::AwsAccount::kContainersRegionUnsupportedText)
						.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
						.AutoWrapText(true)
				]
		];

	ContainersUnsupportedWidget->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(SPadding::All)
		[
			SNew(SExternalLinkButton)
				.Text(Settings::AwsAccount::kContainersRegionSupportButtonText)
				.Link(Url::kContainerSupportedRegionsUrl)
		];

	return ContainersUnsupportedWidget;
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeProfileBootstrappingWidget()
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	SAssignNew(CurrentBootstrapStatusWidget, SBootstrapStatus);
	StaticCastSharedPtr<SBootstrapStatus>(CurrentBootstrapStatusWidget)->UpdateState(Settings->BootstrapStatus);

	// Incomplete profile warning message
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	VerticalBox->AddSlot()
	.AutoHeight()
	.Padding(SPadding::Bottom2x + SPadding::Right2x)
	[
		SNew(SBox)
		.Visibility_Lambda([&]() -> EVisibility
			{
				// If region is supported but inactive profile, only then display incomplete profile message.
				return (ShowBootstrapStatusOnly && (!ProfileSelectedActive) && ContainersRegionSupported) ? EVisibility::Visible : EVisibility::Collapsed;
			})
		[
			SNew(SSetupMessage)
			.WarningText(Settings::AwsAccount::kIncompleteDeploymentProfileText)
			.SetState(ESetupMessageState::WarningMessage)
		]
	];

	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Bottom2x + SPadding::Right2x)
		[
			SNew(SBox)
				.Visibility_Lambda([&]() -> EVisibility
					{
						// Only display warning message if on containers page and the region is unsupported.
						return (ShowBootstrapStatusOnly && !ContainersRegionSupported) ? EVisibility::Visible : EVisibility::Collapsed;
					})
				[
					SAssignNew(ContainersUnsupportedMessage, SSetupMessage)
						.WarningRowWidget(MakeContainersUnsupportedWidget())
				]
		];

	SAssignNew(ProfileHeaderSwitcher, SWidgetSwitcher)
		+ SWidgetSwitcher::Slot()
		[
			MakeProfileDescriptionChildWidget(
				Settings::AwsAccount::kAWSProfilesTitleText,
				Settings::AwsAccount::kAWSProfilesDescriptionText)
		]
		+ SWidgetSwitcher::Slot()
		[
			MakeProfileDescriptionChildWidget(
				Settings::AwsAccount::kProfileDetailsTitleText,
				Settings::AwsAccount::kProfileBootstrappedDescriptionText)
		];

	if (!ShowBootstrapStatusOnly)
	{
		VerticalBox->AddSlot()
			.AutoHeight()
			.Padding(SPadding::Bottom + SPadding::Right2x)
			[
				ProfileHeaderSwitcher.ToSharedRef()
			];
	}

	VerticalBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Right)
		.Padding(SPadding::Top_Bottom + SPadding::Right2x)
		[
			SNew(SHorizontalBox)
			.Visibility(ShowBootstrapStatusOnly ? EVisibility::Collapsed : EVisibility::Visible)

			// Set as selected button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(SPadding::Bottom + SPadding::Right2x)
			[
				SNew(SButton)
				.Text(Settings::AwsAccount::kSetProfileAsSelectedButtonText)
				.OnClicked_Raw(this, &SGameLiftSettingsAwsAccountMenu::OnSetProfileAsSelectedButtonClicked)
				.IsEnabled_Lambda([&]
				{
					UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
					return Settings->CurrentProfileName != RadioButtonSelectedProfile;
				})
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
			]

			// bootstrap button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(SPadding::Bottom + SPadding::Right2x)
			[
				SNew(SButton)
				.Text(Settings::AwsAccount::kBootstrapProfileButtonText)
				.OnClicked_Lambda([&]()
				{
					MakeBootstrapModal();
					BootstrapModal->ShowModal();
					return FReply::Handled();
				})
				.IsEnabled_Lambda([&]
				{
					UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
					if (Settings->CurrentProfileName.IsEmpty() || !Settings->UserProfileInfoMap.Contains(Settings->CurrentProfileName)
						|| Settings->CurrentProfileName != RadioButtonSelectedProfile) // Only enable the Bootstrap profile button when the radio button is on the selected profile.
					{
						return false;
					}
					int BootstrapStatusInt = Settings->UserProfileInfoMap[Settings->CurrentProfileName].BootstrapStatus;
					return EBootstrapMessageStateFromInt(BootstrapStatusInt) != EBootstrapMessageState::InProgressMessage &&
						EBootstrapMessageStateFromInt(BootstrapStatusInt) != EBootstrapMessageState::ActiveMessage;
				})
				.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
			]

			//  What is bootstrapping link
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Fill)
			.Padding(SPadding::Top_Bottom + SPadding::Right2x)
			[
				SNew(SOnlineHyperlink)
				.Text(Settings::AwsAccount::kWhatIsBootstrappingHyperLinkText)
				.Link(Settings::AwsAccount::kWhatIsBootstrappingHyperLinkUrl)
			]
		];

	VerticalBox->AddSlot()
		.Padding(SPadding::Right2x)
		.AutoHeight()
		[
			MakeProfileTableHeader()
		];

	// Table Rows
	VerticalBox->AddSlot()
    .AutoHeight()
    [
		ProfileListContainer.ToSharedRef()
	];

	RefreshProfileList();


	VerticalBox->AddSlot()
    .Padding(SPadding::Bottom2x + SPadding::Right2x)
	[
		SNew(SBox)
		.HeightOverride(35)
		.Visibility(ShowBootstrapStatusOnly ? EVisibility::Collapsed : EVisibility::Visible)
		[
			SNew(SButton)
			.ContentPadding(SPadding::Left)
			.VAlign(VAlign_Center)
			.Text(Settings::AwsAccount::kAddAnotherProfileButtonText)
			.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonNormal)
			.OnClicked_Lambda([&]
			{
				ResetAddProfileUI(true);
				ShowAddProfile = true;
				ProfileHeaderSwitcher->SetActiveWidgetIndex(1);
				return FReply::Handled();
			})
		]
	];

	VerticalBox->AddSlot()
	.AutoHeight()
	.Padding(SPadding::Top2x)
	[
		SNew(SBox)
		.Visibility_Lambda([&]() -> EVisibility {
			return (ShowAddProfile && !ShowBootstrapStatusOnly) ? EVisibility::Visible : EVisibility::Collapsed;
		})
		[
			MakeAddProfileWidget(true)
		]
	];

	if (ShowBootstrapStatusOnly) {
		VerticalBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.Padding(SPadding::Top2x_Bottom2x)
		[
			SNew(STextBlock)
			.Text(Settings::AwsAccount::kCredentialProfileDescriptionText)
		];

		VerticalBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.Padding(SPadding::Top_Bottom + SPadding::Right2x)
		[
			MakeOpenCredentialsPageButtonSwitcher()
		];
	} else {
		// Getting profile file path
		// Tried to use AWS SDK function Aws::Auth::ProfileConfigFileAWSCredentialsProvider::GetCredentialsProfileFilename(), but it crashes randomly sometimes.
	    FString EnvOverridePath = FPlatformMisc::GetEnvironmentVariable(TEXT("AWS_SHARED_CREDENTIALS_FILE"));
        FString DefaultPath = FPaths::Combine(*FString(FPlatformProcess::UserDir()).Replace(TEXT("/Documents"), TEXT("")), TEXT(".aws/credentials"));
        FString	CredentialPath = EnvOverridePath.IsEmpty() ? DefaultPath : EnvOverridePath;
		VerticalBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			.Padding(SPadding::Top2x_Bottom2x)
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Left)
					.Padding(SPadding::Top_Bottom + SPadding::Right2x)
					[
						SNew(STextBlock)
							.Text(Settings::AwsAccount::kAWSCredentialsFileLabel)
							.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kField)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Left)
					.Padding(SPadding::Top_Bottom + SPadding::Right2x)
					[
						SNew(STextBlock)
							.Text(FText::FromString(CredentialPath))
							.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kField)
					]
					// Open file button
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Left)
					.Padding(SPadding::Right2x)
					[
						SNew(SExternalLinkButton)
							.Text(Settings::AwsAccount::kOpenFileButtonText)
							.ButtonTooltipText(Settings::AwsAccount::kOpenFileButtonHoverText)
							.FilePath(CredentialPath)
					]
			];
	}

	return VerticalBox;
}

TSharedRef<SBootstrapModal> SGameLiftSettingsAwsAccountMenu::MakeBootstrapModal()
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	auto& Bootstrapper = IGameLiftCoreModule::Get().GetProfileBootstrap();

	FString DefaultBucketName = Bootstrapper.GetDefaultBucketName(Settings->CurrentProfileName,
		IGameLiftCoreModule::Get().MakeAWSConfigFileProfile());

	return SAssignNew(BootstrapModal, SBootstrapModal)
		.ParentWidget(this->AsWeak())
		.DefaultS3BucketName(DefaultBucketName)
		.OnBootstrapProfileClickedDelegate(FBootstrapProfile::CreateRaw(this, &SGameLiftSettingsAwsAccountMenu::BootstrapAccount));
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeProfileTableHeader() {
	return SNew(SBorder)
		.BorderBackgroundColor(FGameLiftPluginStyle::Get().GetSlateColor(Style::Color::kTableSeparator))
		.BorderImage(FGameLiftPluginStyle::GetAppStyle().GetBrush("WhiteTexture"))
		.Padding(SPadding::Border_Bottom)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FGameLiftPluginStyle::Get().GetSlateColor(Style::Color::kTableHeaderBackground))
			.BorderImage(FGameLiftPluginStyle::GetAppStyle().GetBrush("WhiteTexture"))
			.Padding(SPadding::Left_Right)
			[
				SNew(SBox)
				.HeightOverride(30)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.03f)
					+ SHorizontalBox::Slot()
					.FillWidth(0.20f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Profile name"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.12f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString("AWS Region"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.40f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString("S3 bucket name"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.25f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Bootstrap status"))
					]
				]
			]
		];
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeAddProfileWidget(bool BootstrapPage)
{
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	VerticalBox->AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	.Padding(SPadding::Top_Bottom + SPadding::Right2x)
	[
		SNew(SNamedRow)
		.NameText(Settings::AwsAccount::kProfileModificationProfileNameText)
		.RowWidget(
			SAssignNew(BootstrapPage ? BootstrapPageAwsProfileNameInput : AwsProfileNameInput, SInputTextBox)
			.ButtonVisibility(EVisibility::Hidden)
		)
	];

	// Access credentials help documentation
	VerticalBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(SPadding::Top_Bottom + SPadding::Right2x)
		[
			AccessCredentialsHelpMessage.ToSharedRef()
		];

	// AWS access key
	VerticalBox->AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	.Padding(SPadding::Top_Bottom + SPadding::Right2x)
	[
		SNew(SNamedRow)
		.NameText(Settings::AwsAccount::kProfileModificationAccessKeyText)
		.RowWidget(SAssignNew(BootstrapPage ? BootstrapPageAwsAccessKeyIdInput : AwsAccessKeyIdInput, SInputTextBox))
	];

	// AWS secret key
	VerticalBox->AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	.Padding(SPadding::Top_Bottom + SPadding::Right2x)
	[
		SNew(SNamedRow)
		.NameText(Settings::AwsAccount::kProfileModificationSecretKeyText)
		.RowWidget(SAssignNew(BootstrapPage ? BootstrapPageAwsSecretKeyInput : AwsSecretKeyInput, SInputTextBox))
	];

	// AWS region
	VerticalBox->AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	.Padding(SPadding::Top_Bottom + SPadding::Right2x)
	[
		SNew(SNamedRow)
		.NameText(Settings::AwsAccount::kProfileModificationRegionText)
		.RowWidget(
			SAssignNew(BootstrapPage? BootstrapPageAwsRegionInput : AwsRegionInput, SSelectionComboBox)
			.OnListBuilding_Raw(this, &SGameLiftSettingsAwsAccountMenu::BuildRegionValues)
			.OnItemSelected_Raw(this, &SGameLiftSettingsAwsAccountMenu::OnGetRegion)
		)
	];

	// Buttons and hyperlink
	VerticalBox->AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	.Padding(SPadding::Top_Bottom + SPadding::Right2x)
	[
		SNew(SNamedRow)
		.RowWidget(
			SNew(SHorizontalBox)
			// Button to cancel creating profile
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.Padding(SPadding::Right)
			[
				SNew(SButton)
				.Text(Settings::AwsAccount::kCancelButtonText)
				.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
				.OnClicked_Lambda([this, BootstrapPage]
					{
						ResetAddProfileUI(BootstrapPage);
						ShowAddProfile = false;
						ProfileHeaderSwitcher->SetActiveWidgetIndex(0);
						return FReply::Handled();
					})
			]
			// Button to create profile
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.Padding(SPadding::Right)
			[
				SNew(SButton)
				.Text(Settings::AwsAccount::kCreateProfileButtonText)
				.OnClicked_Raw(this, &SGameLiftSettingsAwsAccountMenu::CreateAccount, BootstrapPage)
				.IsEnabled_Raw(this, &SGameLiftSettingsAwsAccountMenu::CanCreateAccountProfile, BootstrapPage)
				.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
			]
		)
	];

	return VerticalBox;
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeOpenCredentialsPageButtonSwitcher() {
	return SNew(SWidgetSwitcher)
		.WidgetIndex_Lambda([&]
			{
				return ProfileSelectedActive ? 1 : 0;
			})
		+ SWidgetSwitcher::Slot()
		[
			SNew(SButton)
			.Text(Settings::AwsAccount::kIncompleteDeploymentProfileButtonText)
			.OnClicked_Lambda([&]()
				{
					FGameLiftPluginModule* Module = FModuleManager::GetModulePtr<FGameLiftPluginModule>(Plugin::GameLiftPluginTabName);
					Module->OpenSettingsButtonClicked();
					return FReply::Handled();
				})
			.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kSuccessButtonStyleName)
			.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
		]
		+ SWidgetSwitcher::Slot()
		[
			SNew(SButton)
			.Text(Settings::AwsAccount::kCompleteDeploymentProfileButtonText)
			.OnClicked_Lambda([&]()
				{
					FGameLiftPluginModule* Module = FModuleManager::GetModulePtr<FGameLiftPluginModule>(Plugin::GameLiftPluginTabName);
					Module->OpenSettingsButtonClicked();
					return FReply::Handled();
				})
			.ButtonStyle(FGameLiftPluginStyle::Get(), Style::Button::kNormalButtonStyleName)
			.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kButtonLight)
		];
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::CreateAccessCredentialsInstructionsSection()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top_Bottom2x)
		.AutoHeight()
		[
			SNew(SRichTextBlock)
				.Text(Settings::AwsAccount::kProfileHelpIntroText)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.DecoratorStyleSet(&FGameLiftPluginStyle::Get())
				.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top)
		.AutoHeight()
		[
			SNew(SRichTextBlock)
				.Text(Settings::AwsAccount::kProfileHelpStep0)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.DecoratorStyleSet(&FGameLiftPluginStyle::Get())
				.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top)
		.AutoHeight()
		[
			SNew(SRichTextBlock)
				.Text(Settings::AwsAccount::kProfileHelpStep1)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.DecoratorStyleSet(&FGameLiftPluginStyle::Get())
				.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top)
		.AutoHeight()
		[
			SNew(SRichTextBlock)
				.Text(Settings::AwsAccount::kProfileHelpStep2)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.DecoratorStyleSet(&FGameLiftPluginStyle::Get())
				.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top)
		.AutoHeight()
		[
			SNew(SRichTextBlock)
				.Text(Settings::AwsAccount::kProfileHelpStep3)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.DecoratorStyleSet(&FGameLiftPluginStyle::Get())
				.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top)
		.AutoHeight()
		[
			SNew(SRichTextBlock)
				.Text(Settings::AwsAccount::kProfileHelpStep4)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.DecoratorStyleSet(&FGameLiftPluginStyle::Get())
				.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top_Bottom)
		.AutoHeight()
		[
			SNew(STextBlock)
				.Text(Settings::AwsAccount::kProfileHelpDescriptionText)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.AutoWrapText(true)
		];

}

void SGameLiftSettingsAwsAccountMenu::MakeAccessCredentialsHelpWidget()
{
	TSharedPtr<SVerticalBox> RowWidget = SNew(SVerticalBox);
	RowWidget->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Bottom2x)
		[
			SNew(STextBlock)
				.Text(Settings::AwsAccount::kProfileHelpTitleText)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kSubtitleStyleName)
				.AutoWrapText(true)
		];

	RowWidget->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Bottom2x)
		[
			SNew(STextBlock)
				.Text(Settings::AwsAccount::kProfileHelpSubTitleText)
				.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
				.AutoWrapText(true)
		];

	TSharedPtr<SSectionsWithHeaders> SectionsWithHeaders = SNew(SSectionsWithHeaders).ShowDefaultColor(true);
	SectionsWithHeaders->AddSection(Settings::AwsAccount::kProfileHelpHeaderText, CreateAccessCredentialsInstructionsSection(), true);

	RowWidget->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Bottom2x)
		[
			SectionsWithHeaders.ToSharedRef()
		];

	RowWidget->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Bottom2x)
		[
			SNew(SOnlineHyperlink)
				.Text(Settings::AwsAccount::kViewIAMConsoleHyperLinkText)
				.Link(Settings::AwsAccount::kViewIAMConsoleHyperLinkUrl)
			
		];
	
	 SAssignNew(AccessCredentialsHelpMessage, SSetupMessage)
		.HelpRowWidget(RowWidget)
		.HelpButtonStyle(Style::Button::kCloseButtonStyleName)
		.IconVAlignTop(true)
		.ButtonVAlignTop(true)
		.OnButtonClicked_Lambda([&]
			{
				AccessCredentialsHelpMessage->SetVisibility(EVisibility::Collapsed);
			})
		.SetState(ESetupMessageState::HelpMessage);
}

void SGameLiftSettingsAwsAccountMenu::ResetModificationUI()
{
	SInputTextBox::CastToSharedRef(AwsProfileNameInput)->SetInputText("");
	SInputTextBox::CastToSharedRef(AwsAccessKeyIdInput)->SetInputText("");
	SInputTextBox::CastToSharedRef(AwsSecretKeyInput)->SetInputText("");
	SSelectionComboBox::CastToSharedRef(AwsRegionInput)->SetSelectedByName(FText::FromString(RegionToString(Aws::ERegions::us_west_2)));
}

void SGameLiftSettingsAwsAccountMenu::UpdateModificationUI(const FText& ProfileName)
{
	TSharedRef<IAWSConfigFileProfile> Configurator = IGameLiftCoreModule::Get().MakeAWSConfigFileProfile();
	const FString& AccessKey = Configurator->GetAccessKey(ProfileName.ToString());
	const FString& SecretKey = Configurator->GetSecretAccessKey(ProfileName.ToString());
	const FString& Region = Configurator->GetRegion(ProfileName.ToString());

	SInputTextBox::CastToSharedRef(AwsProfileNameInput)->SetInputText(ProfileName);
	SInputTextBox::CastToSharedRef(AwsAccessKeyIdInput)->SetInputText(AccessKey);
	SInputTextBox::CastToSharedRef(AwsSecretKeyInput)->SetInputText(SecretKey);
	SSelectionComboBox::CastToSharedRef(AwsRegionInput)->SetSelectedByName(FText::FromString(Region));
}

void SGameLiftSettingsAwsAccountMenu::BuildRegionValues(TArray<FTextIntPair>& Items)
{
	Items.Reset();

	int id = 0;
	auto AddToList = [&](Aws::ERegions InRegion)
	{
		Items.Add(FTextIntPair(
			FText::FromString(Aws::RegionToString(InRegion)),
			id++, 
			FText::FromString(Aws::RegionToNameString(InRegion))
		));
	};

	AddToList(Aws::ERegions::us_east_2);
	AddToList(Aws::ERegions::us_east_1);
	AddToList(Aws::ERegions::us_west_1);
	AddToList(Aws::ERegions::us_west_2);
	//AddToList(Aws::ERegions::af_south_1);
	//AddToList(Aws::ERegions::ap_east_1);
	//AddToList(Aws::ERegions::ap_southeast_3);
	AddToList(Aws::ERegions::ap_south_1);
	//AddToList(Aws::ERegions::ap_northeast_3);
	AddToList(Aws::ERegions::ap_northeast_2);
	AddToList(Aws::ERegions::ap_southeast_1);
	AddToList(Aws::ERegions::ap_southeast_2);
	AddToList(Aws::ERegions::ap_northeast_1);
	AddToList(Aws::ERegions::ca_central_1);
	AddToList(Aws::ERegions::eu_central_1);
	AddToList(Aws::ERegions::eu_west_1);
	AddToList(Aws::ERegions::eu_west_2);
	//AddToList(Aws::ERegions::eu_south_1);
	//AddToList(Aws::ERegions::eu_west_3);
	//AddToList(Aws::ERegions::eu_north_1);
	//AddToList(Aws::ERegions::me_south_1);
	AddToList(Aws::ERegions::sa_east_1);

	Items.Sort([](const FTextIntPair& LeftValue, const FTextIntPair& RightValue)
	{
		return LeftValue.Text.CompareTo(RightValue.Text) < 0;
	});
}

void SGameLiftSettingsAwsAccountMenu::OnGetRegion(int SelectionId, const FTextIntPair& Item)
{
	/**/
}

void SGameLiftSettingsAwsAccountMenu::SetContainersRegionSupported(bool ContainersSupported)
{
	ContainersRegionSupported = ContainersSupported;
}

void SGameLiftSettingsAwsAccountMenu::ModifyAwsAccountProfile(const FString& ProfileName, const FString& AccessKey, const FString& SecretKey, const FString& Region)
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	bool IsNameChanged = Settings->CurrentProfileName != ProfileName;

	{
		TSharedRef<IAWSConfigFileProfile> Configurator = IGameLiftCoreModule::Get().MakeAWSConfigFileProfile();

		if (IsNameChanged)
		{
			Configurator->RenameProfile(Settings->CurrentProfileName, ProfileName);
		}

		Configurator->SetAccessKey(ProfileName, AccessKey);
		Configurator->SetSecretAccessKey(ProfileName, SecretKey);
		Configurator->SetRegion(ProfileName, Region);
		Configurator->Save();

		Settings->UserProfileInfoMap.Remove(Settings->CurrentProfileName);
		FUserProfileInfo ProfileInfo = FUserProfileInfo();
		ProfileInfo.AwsRegion = Region;
		Settings->UserProfileInfoMap.Add(ProfileName, ProfileInfo);
		Settings->CurrentProfileName = ProfileName;

		Settings->SaveConfig();
	}
	CopyProfileInfoFromMapToCurrentSettings();
	OnBootstrapStateChanged();
}

void SGameLiftSettingsAwsAccountMenu::CreateAwsAccountProfile(const FString& ProfileName, const FString& AccessKey, const FString& SecretKey, const FString& Region)
{
	TSharedRef<IAWSConfigFileProfile> Configurator = IGameLiftCoreModule::Get().MakeAWSConfigFileProfile();

	Configurator->CreateProfile(ProfileName);
	Configurator->SetAccessKey(ProfileName, AccessKey);
	Configurator->SetSecretAccessKey(ProfileName, SecretKey);
	Configurator->SetRegion(ProfileName, Region);
	Configurator->Save();

	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	FUserProfileInfo ProfileInfo = FUserProfileInfo();
	ProfileInfo.AwsRegion = Region;
	Settings->UserProfileInfoMap.Add(ProfileName, ProfileInfo);
	Settings->SaveConfig();
}

void SGameLiftSettingsAwsAccountMenu::SyncFromCredentialsFile()
{
	// Check if current profile map is consistent with aws credentails file. Add/remove accordingly.
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	TSharedRef<IAWSConfigFileProfile> Configurator = IGameLiftCoreModule::Get().MakeAWSConfigFileProfile();
	const TArray<FString>& AwsAccountsNames = Configurator->GetProfileNames();

	// First copy the map, and empty the original map
	TMap<FString, FUserProfileInfo> OldProfilesMap = MoveTemp(Settings->UserProfileInfoMap); // MoveTemp will empty the source map

	// Add every item in the credentails file to the map
	for (int Index = 0; Index < AwsAccountsNames.Num(); Index++)
	{
		FString CurrProfile = AwsAccountsNames[Index];
		FString CurrRegion = Configurator->GetRegion(CurrProfile);
		if (OldProfilesMap.Contains(CurrProfile) && OldProfilesMap[CurrProfile].AwsRegion == CurrRegion)
		{
			// Add profile from the old map if it already exists
			Settings->UserProfileInfoMap.Emplace(CurrProfile, OldProfilesMap[CurrProfile]);
		}
		else
		{
			// Add brand new profile if it doesn't exist
			FUserProfileInfo ProfileInfo = FUserProfileInfo();
			ProfileInfo.AwsRegion = CurrRegion;
			Settings->UserProfileInfoMap.Add(CurrProfile, ProfileInfo);
		}
	}

	Settings->SaveConfig();
	CopyProfileInfoFromMapToCurrentSettings();
}

bool SGameLiftSettingsAwsAccountMenu::CanCreateAccountProfile(bool BootstrapPage) const
{
	const auto& Name = SInputTextBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsProfileNameInput : AwsProfileNameInput)->GetInputText();
	const auto& AccessKey = SInputTextBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsAccessKeyIdInput : AwsAccessKeyIdInput)->GetInputText();
	const auto& SecretKey = SInputTextBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsSecretKeyInput : AwsSecretKeyInput)->GetInputText();
	return !(Name.IsEmptyOrWhitespace() || SecretKey.IsEmptyOrWhitespace() || AccessKey.IsEmptyOrWhitespace());
}

FReply SGameLiftSettingsAwsAccountMenu::CreateAccount(bool BootstrapPage)
{
	auto ManageAccount = [&](auto& Name, auto& AccessKey, auto& SecretKey, auto& Region)
	{
		TSharedRef<IAWSConfigFileProfile> Configurator = IGameLiftCoreModule::Get().MakeAWSConfigFileProfile();
		const TArray<FString>& AwsAccountsNames = Configurator->GetProfileNames();

		if (!AwsAccountsNames.Contains(Name))
		{
			CreateAwsAccountProfile(Name, AccessKey, SecretKey, Region);
		}
		else
		{
			ModifyAwsAccountProfile(Name, AccessKey, SecretKey, Region);
		}

		ResetModificationUI();
	};

	FText ProfileName = SInputTextBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsProfileNameInput : AwsProfileNameInput)->GetInputText();
	FText AwsKeyAccessId = SInputTextBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsAccessKeyIdInput : AwsAccessKeyIdInput)->GetInputText();
	FText AwsSecretKey = SInputTextBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsSecretKeyInput : AwsSecretKeyInput)->GetInputText();
	FText AwsRegion = SSelectionComboBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsRegionInput : AwsRegionInput)->GetSelectedPair().Text;

	ManageAccount(ProfileName.ToString(), AwsKeyAccessId.ToString(), AwsSecretKey.ToString(), AwsRegion.ToString());
	// go to the bootstrap page once click create
	UpdateAccountManagementState(EAccountManagementState::BootstrapAccount);
	RefreshProfileList();
	ShowAddProfile = false;
	ProfileHeaderSwitcher->SetActiveWidgetIndex(0);

	FGameLiftSettingsCustomization::OnProfileNumberUpdated.ExecuteIfBound();

	return FReply::Handled();
}

void SGameLiftSettingsAwsAccountMenu::ResetAddProfileUI(bool BootstrapPage)
{
	SInputTextBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsProfileNameInput : AwsProfileNameInput)->SetInputText("");
	SInputTextBox::CastToSharedRef(BootstrapPage? BootstrapPageAwsAccessKeyIdInput : AwsAccessKeyIdInput)->SetInputText("");
	SInputTextBox::CastToSharedRef(BootstrapPage? BootstrapPageAwsSecretKeyInput : AwsSecretKeyInput)->SetInputText("");
	SSelectionComboBox::CastToSharedRef(BootstrapPage ? BootstrapPageAwsRegionInput : AwsRegionInput)->
										SetSelectedByName(FText::FromString(RegionToString(Aws::ERegions::us_west_2)));
}

void SGameLiftSettingsAwsAccountMenu::BootstrapAccount(FString BucketName)
{
	SyncFromCredentialsFile(); // validate here in case the credential file is updated outside of plugin
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	if (Settings->CurrentProfileName.IsEmpty())
	{
		UE_LOG(GameLiftPluginLog, Log, TEXT("Credential deleted outside of plugin, resetting UI and skipping bootstrap"));
		SetupMenu();
		// Broadcast profile selection change notification
		OnProfileSelectionChangedMultiDelegate.Broadcast(this);
		return;
	}

	TSharedPtr<SBootstrapStatus> BootstrapStatusPtr = StaticCastSharedPtr<SBootstrapStatus>(CurrentBootstrapStatusWidget);
	BootstrapStatusPtr->OnBootstrap();
	Async(EAsyncExecution::Thread, [
		BootstrapStatus = std::move(BootstrapStatusPtr), Settings, BucketName, this
	]() {
			auto& Bootstrapper = IGameLiftCoreModule::Get().GetProfileBootstrap();
			auto IsConfigured = Bootstrapper.ConfigureAccount(Settings->CurrentProfileName,
				IGameLiftCoreModule::Get().MakeAWSConfigFileProfile());

			if (IsConfigured)
			{
				auto IsBootstrapped = Bootstrapper.Bootstrap(BucketName);
				if (IsBootstrapped)
				{
					UE_LOG(GameLiftPluginLog, Log, TEXT("%s"), Settings::Logs::kBootstrapSucceed);

					Async(EAsyncExecution::TaskGraphMainThread,
						[BootstrapStatus, BucketName, this]()
						{
							UGameLiftSettings* GameLiftSettings = GetMutableDefault<UGameLiftSettings>();
							GameLiftSettings->UserProfileInfoMap[GameLiftSettings->CurrentProfileName].S3Bucket = BucketName;
							GameLiftSettings->UserProfileInfoMap[GameLiftSettings->CurrentProfileName].BootstrapStatus = int(EBootstrapMessageState::ActiveMessage);
							BootstrapStatus->OnSucceeded();
							CopyProfileInfoFromMapToCurrentSettings();
							OnBootstrapStateChanged();
						});
					return;
				}
			}
			UE_LOG(GameLiftPluginLog, Log, TEXT("%s"), Settings::Logs::kBootstrapFailed);
			const FText ErrorMessage = Utils::BuildLastestErrorMessage(Bootstrapper);
			Async(EAsyncExecution::TaskGraphMainThread,
				[BootstrapStatus, ErrorMessage, this]()
				{
					UGameLiftSettings* GameLiftSettings = GetMutableDefault<UGameLiftSettings>();
					GameLiftSettings->UserProfileInfoMap[GameLiftSettings->CurrentProfileName].BootstrapError = ErrorMessage.ToString();
					GameLiftSettings->UserProfileInfoMap[GameLiftSettings->CurrentProfileName].BootstrapStatus = int(EBootstrapMessageState::FailureMessage);
					GameLiftSettings->UserProfileInfoMap[GameLiftSettings->CurrentProfileName].S3Bucket.Empty(); // in case credential changed after successfully bootstrapped
					BootstrapStatus->OnFailure();
					CopyProfileInfoFromMapToCurrentSettings();
					OnBootstrapStateChanged();
				});
	});
}

ECheckBoxState SGameLiftSettingsAwsAccountMenu::IsRadioChecked(FString ProfileName) const
{
	return ProfileName == RadioButtonSelectedProfile ? 
		ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FReply SGameLiftSettingsAwsAccountMenu::OnSetProfileAsSelectedButtonClicked()
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	Settings->CurrentProfileName = RadioButtonSelectedProfile;
	Settings->SaveConfig();
	SyncFromCredentialsFile(); // validate here in case the credential file is updated outside of plugin
	if (Settings->UserProfileInfoMap.Num() == 0)
	{
		// all credentials are deleted outside of plugin, resetting UI
		SetupMenu();
		// Broadcast profile selection change notification
		OnProfileSelectionChangedMultiDelegate.Broadcast(this);
		return FReply::Handled();
	}
	CopyProfileInfoFromMapToCurrentSettings();
	if (Settings->UserProfileInfoMap.Contains(Settings->CurrentProfileName) &&
		Settings->UserProfileInfoMap[Settings->CurrentProfileName].BootstrapStatus == int(EBootstrapMessageState::ActiveMessage))
	{
		// If the new profile status is active, call RestoreAccount for GameLiftCore/CoreSDK to pick up the new profile,
		// also in case the credential is modified out of the plugin
		RestoreAccount();
	}

	StaticCastSharedPtr<SBootstrapStatus>(CurrentBootstrapStatusWidget)->UpdateState(Settings->BootstrapStatus);
	OnBootstrapStateChanged();
	return FReply::Handled();
}

void SGameLiftSettingsAwsAccountMenu::OnRadioChanged(ECheckBoxState NewRadioState, FString ProfileName)
{
	if (NewRadioState == ECheckBoxState::Checked)
	{
		RadioButtonSelectedProfile = ProfileName;
	}
}

void SGameLiftSettingsAwsAccountMenu::RestoreAccount()
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();

	if (Settings->CurrentProfileName.IsEmpty() || !Settings->UserProfileInfoMap.Contains(Settings->CurrentProfileName))
	{
		return; // No account to restore
	}

	auto status = EBootstrapMessageStateFromInt(Settings->BootstrapStatus);

	if (status == EBootstrapMessageState::ActiveMessage)
	{
		auto& Bootstrapper = IGameLiftCoreModule::Get().GetProfileBootstrap();
		auto IsConfigured = Bootstrapper.ConfigureAccount(Settings->CurrentProfileName,
			IGameLiftCoreModule::Get().MakeAWSConfigFileProfile(), Settings->S3Bucket.ToString());
		if (!IsConfigured)
		{
			Settings->UserProfileInfoMap[Settings->CurrentProfileName].BootstrapStatus = int(EBootstrapMessageState::FailureMessage);
			CopyProfileInfoFromMapToCurrentSettings();
		}
	}
}

void SGameLiftSettingsAwsAccountMenu::CopyProfileInfoFromMapToCurrentSettings()
{

	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	if (Settings->CurrentProfileName.IsEmpty() || !Settings->UserProfileInfoMap.Contains(Settings->CurrentProfileName))
	{
		UE_LOG(GameLiftPluginLog, Log, TEXT("No credentials found in the AWS credential file."));
		Settings->CurrentProfileName.Empty();
		Settings->AwsRegion = FText::GetEmpty();
		Settings->S3Bucket = FText::GetEmpty();
		Settings->BootstrapStatus = int(EBootstrapMessageState::NoBootstrappedMessage);
		Settings->BootstrapError = FText::GetEmpty();
		Settings->SaveConfig();
		return;
	}
	FUserProfileInfo ProfileInfo = Settings->UserProfileInfoMap[Settings->CurrentProfileName];
	Settings->AwsRegion = FText::FromString(ProfileInfo.AwsRegion);
	Settings->S3Bucket = FText::FromString(ProfileInfo.S3Bucket);
	Settings->BootstrapStatus = ProfileInfo.BootstrapStatus;
	Settings->BootstrapError = FText::FromString(ProfileInfo.BootstrapError);
	Settings->SaveConfig();
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::CreateStatusWidget(FString ProfileName) const
{
	UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();
	if (!Settings->UserProfileInfoMap.Contains(ProfileName))
	{
		// This should never happen ideally. If it does, return inactive (default) status.
		return SNew(SBootstrapStatus);
	}
	int status = Settings->UserProfileInfoMap[ProfileName].BootstrapStatus;
	if (ShowBootstrapStatusOnly || !ProfileName.Equals(Settings->CurrentProfileName)) {
		// For profiles other than the current one, create a new status widget for each one
		TSharedPtr<SBootstrapStatus> BootstrapStatusWidget;
		SAssignNew(BootstrapStatusWidget, SBootstrapStatus);
		StaticCastSharedPtr<SBootstrapStatus>(BootstrapStatusWidget)->UpdateState(status);
		return BootstrapStatusWidget.ToSharedRef();
	} else {
		// For current profile, reuse the CurrentBootstrapStatusWidget. 
		// Reusing because we needed a reference to the widget so we can update elsewhere when bootstrapping.
		StaticCastSharedPtr<SBootstrapStatus>(CurrentBootstrapStatusWidget)->UpdateState(status);
		return CurrentBootstrapStatusWidget.ToSharedRef();
	}
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::CreateProfileRow(
		const FString& ProfileName, const FString& AwsRegion, const FString& BucketName, bool IsSelectedProfile, bool ShowProfileRow)
{
	return SNew(SBorder)
		.Visibility(ShowProfileRow ? EVisibility::Visible : EVisibility::Collapsed)
		.BorderBackgroundColor(FGameLiftPluginStyle::Get().GetSlateColor(Style::Color::kTableSeparator))
		.BorderImage(FGameLiftPluginStyle::GetAppStyle().GetBrush("WhiteTexture"))
		.Padding(SPadding::Border_Bottom)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FGameLiftPluginStyle::Get().GetSlateColor(Style::Color::kTableRowBackground))
			.BorderImage(FGameLiftPluginStyle::GetAppStyle().GetBrush("WhiteTexture"))
			.Padding(SPadding::Left_Right)
			[
				SNew(SBox)
				.HeightOverride(30)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.03f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(""))
						.Visibility(ShowBootstrapStatusOnly ? EVisibility::Visible : EVisibility::Collapsed)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.03f)
					[
						SNew(SBox)
						.Visibility(ShowBootstrapStatusOnly ? EVisibility::Collapsed : EVisibility::Visible)
						[
							SNew(SCheckBox)
							.Style(FCoreStyle::Get(), "RadioButton")
							.IsChecked(this, &SGameLiftSettingsAwsAccountMenu::IsRadioChecked, ProfileName)
							.OnCheckStateChanged(this, &SGameLiftSettingsAwsAccountMenu::OnRadioChanged, ProfileName)
						]
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.20f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kNoteLight)
						.Text(FText::FromString(ProfileName + (IsSelectedProfile ? " [selected]" : "")))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.12f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kNoteLight)
						.Text(FText::FromString(AwsRegion))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.40f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kNoteLight)
						.Text(FText::FromString(BucketName))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.25f)
					.VAlign(VAlign_Center)
					[
						CreateStatusWidget(ProfileName)
					]
				]
			]
		];
}

TSharedRef<SWidget> SGameLiftSettingsAwsAccountMenu::MakeDocumentationLinkWidget(FString ReadDeveloperGuideLink) {
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			SNew(SOnlineHyperlink)
				.Text(Menu::DeployCommon::kReadDeveloperGuideText)
				.Link(ReadDeveloperGuideLink)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		[
			SNew(STextSeparator).Separator(TEXT("|"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			SNew(SOnlineHyperlink)
				.Text(Settings::AwsAccount::kHeaderForumLinkText)
				.Link(Url::kOpenAwsGameTechForumUrl)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		[
			SNew(STextSeparator).Separator(TEXT("|"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			SNew(SOnlineHyperlink)
				.Text(Menu::DeployCommon::kReportIssuesText)
				.Link(Url::kReportIssuesUrl)
		];
}

#undef LOCTEXT_NAMESPACE
