// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SGameLiftSettingsHelpMenu.h"

#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"
#include "SWidgets/SOnlineHyperlink.h"

#define LOCTEXT_NAMESPACE "SGameLiftSettingsHelpMenu"

void SGameLiftSettingsHelpMenu::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		MakeHelpWidget()
	];
}

TSharedRef<SWidget> SGameLiftSettingsHelpMenu::MakeHelpWidget() const
{
	TSharedRef<SWidget> SetupAccountCard = MakeHelpCardWidget(
		Url::kHelpSetupAWSAccountLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kDocumentationIcon,
		Settings::Help::kHelpSetupAWSAccountLinkHeader,
		Settings::Help::kHelpSetupAWSAccountLinkText
	);

	TSharedRef<SWidget> PluginGuideCard = MakeHelpCardWidget(
		Url::kHelpPluginGuideLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kDocumentationIcon,
		Settings::Help::kHelpPluginGuideLinkHeader,
		Settings::Help::kHelpPluginGuideLinkText
	);

	TSharedRef<SWidget> GettingStartedCard = MakeHelpCardWidget(
		Url::kHelpGettingStartedLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kGuidanceIcon,
		Settings::Help::kHelpGettingStartedLinkHeader,
		FText::GetEmpty()
	);

	TSharedRef<SWidget> OrganizingEnvironmentCard = MakeHelpCardWidget(
		Url::kHelpOrganizingEnvironmentLinkUrl,
		Style::kWhitePaperIconWidth,
		Style::Brush::kWhitepaperIcon,
		Settings::Help::kHelpOrganizingEnvironmentLinkHeader,
		FText::GetEmpty()
	);

	TSharedRef<SWidget> GetAccessKeyCard = MakeHelpCardWidget(
		Url::kHelpGetAccessKeyLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kDocumentationIcon,
		Settings::Help::kHelpGetAccessKeyLinkHeader,
		Settings::Help::kHelpGetAccessKeyLinkText
	);

	TSharedRef<SWidget> GameliftLocationsCard = MakeHelpCardWidget(
		Url::kHelpServiceLocationsLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kDocumentationIcon,
		Settings::Help::kHelpServiceLocationsLinkHeader,
		Settings::Help::kHelpServiceLocationsLinkText
	);

	TSharedRef<SWidget> ManageAccessKeyCard = MakeHelpCardWidget(
		Url::kHelpManageAccessKeysLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kDocumentationIcon,
		Settings::Help::kHelpManageAccessKeysLinkHeader,
		Settings::Help::kHelpManageAccessKeysLinkText
	);

	TSharedRef<SWidget> HostingSolutionsCard = MakeHelpCardWidget(
		Url::kHelpGameLiftHostingSolutionsLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kDocumentationIcon,
		Settings::Help::kHelpGameLiftHostingSolutionsLinkHeader,
		Settings::Help::kHelpGameLiftHostingSolutionsLinkText
	);

	TSharedRef<SWidget> SetupFleetCard = MakeHelpCardWidget(
		Url::kHelpSettingupHostingFleetLinkUrl,
		Style::kDocumentationIconWidth,
		Style::Brush::kDocumentationIcon,
		Settings::Help::kHelpSettingupHostingFleetLinkHeader,
		Settings::Help::kHelpSettingupHostingFleetLinkText
	);

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(SPadding::Top2x_Bottom)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right2x)
				[
					SetupAccountCard
				]
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right2x)
				[
					PluginGuideCard
				]
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right4x)
				[
					GettingStartedCard
				]
		]

		+SVerticalBox::Slot()
			.Padding(SPadding::Top2x_Bottom)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(SPadding::Right2x)
					[
						OrganizingEnvironmentCard
					]
					+ SHorizontalBox::Slot()
					.Padding(SPadding::Right2x)
					[
						GetAccessKeyCard
					]
					+ SHorizontalBox::Slot()
					.Padding(SPadding::Right4x)
					[
						GameliftLocationsCard
					]
			]

		+ SVerticalBox::Slot()
		.Padding(SPadding::Top2x_Bottom2x + SPadding::Bottom5x)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right2x)
				[
					ManageAccessKeyCard
				]
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right2x)
				[
					HostingSolutionsCard
				]
				+ SHorizontalBox::Slot()
				.Padding(SPadding::Right4x)
				[
					SetupFleetCard
				]
		];
}

TSharedRef<SWidget> SGameLiftSettingsHelpMenu::MakeHelpCardWidget(const TCHAR* UrlLink, int IconWidth, const FString& IconName, const FText TitleText, const FText DescriptionText) const
{
	return SNew(SButton)
		.OnClicked_Lambda([UrlLink]()
			{
				FPlatformProcess::LaunchURL(UrlLink, nullptr, nullptr);
				return FReply::Handled();
			})
		.Content()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						.Padding(SPadding::Top2x_Bottom2x + SPadding::Top)
						[
							SNew(SBox)
								.HeightOverride(Style::kHelpfulResourceIconHeight)
								.WidthOverride(IconWidth)
								[
									SNew(SImage)
										.Image(FGameLiftPluginStyle::Get().GetBrush(FName(IconName)))
								]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						.Padding(SPadding::Top2x)
						[
							SNew(SBox)
								.WidthOverride(Style::kHelpfulResourceCardWidth)
								[
									SNew(STextBlock)
										.Text(TitleText)
										.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kSubtitleStyleName)
										.AutoWrapText(true)
								]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						.Padding(SPadding::Top2x_Bottom2x)
						[
							SNew(SBox)
								.WidthOverride(Style::kHelpfulResourceCardWidth)
								[
									SNew(STextBlock)
										.Text(DescriptionText)
										.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kParagraph)
										.AutoWrapText(true)
								]
						]
				]
		];
}

#undef LOCTEXT_NAMESPACE
