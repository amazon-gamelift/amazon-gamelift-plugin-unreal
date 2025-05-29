// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"

namespace Plugin
{
	static const auto sGameLiftPluginName = TEXT("GameLiftPlugin");
	static const FName GameLiftPluginTabName("GameLiftPlugin");

	static const auto kGameLiftProductName = NSLOCTEXT("Plugin", "GameLiftProductName", "Amazon GameLift");
} // namespace Plugin

namespace Settings
{
	static const auto kSettingsSectionName = "GameLiftPlugin";
	static const auto kSettingsSectionTooltip = NSLOCTEXT("Settings", "SectionTooltip", "Edit AWS User Profiles");

	static const auto kRichTextBlockNoteText = NSLOCTEXT("FGameLiftSettingsCustomization", "kRichTextBlockNoteText", "Note");

	namespace Category
	{
		static const auto kAwsProfileCategory = "AWS User Profiles";
		static const auto kAwsProfileCategoryText = NSLOCTEXT("FGameLiftSettingsCustomization", "kAwsProfileCategoryText", "AWS Authentication");

		static const auto kGameLiftOverviewCategory = "Getting Started";
		static const auto kGameLiftOverviewCategoryText = NSLOCTEXT("FGameLiftSettingsCustomization", "kGameLiftOverviewCategoryText", "Getting Started");

		static const auto kUseCaseCategory = "Games Using Amazon GameLift";
		static const auto kUseCaseCategoryText = NSLOCTEXT("FGameLiftSettingsCustomization", "kUseCaseCategoryText", "Games Using Amazon GameLift");

		static const auto kAdditionalResourcesCategory = "Additional Resources";
		static const auto kAdditionalResourcesCategoryText = NSLOCTEXT("FGameLiftSettingsCustomization", "kAdditionalResourcesCategoryText", "Helpful Resources");
	}

	namespace AwsAccount
	{
		// Setup message
		static const auto kNotConfiguredText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AwsAccountNotConfigured", "You will need to configure an AWS credentials profile to get started with Amazon GameLift.");
		static const auto kConfigurationFailedText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AwsAccountConfigurationFailed", "Failed to configure your AWS user profile.");
		static const auto kConfiguredText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AwsAccountConfigured", "Successfully configured your AWS user profile.");
		static const auto kSetPathInfoText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AwsAccountInfoMessageText", "Setup credentials used for account bootstrapping and CloudFormation deployment. Choose already existing AWS account or create a new one selecting \"Import or Update new credentials profile\" from the dropdown list.");
		static const auto kAddNewAccountInfoText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AwsAddNewAccountInfoText", "To Add a new AWS account you just need to copy credentials created through the AWS console");

		// Header
		static const auto kHeaderTitleText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "HeaderTitleText", "Set up your user profile");
		static const auto kHeaderDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "HeaderDescriptionText", "An AWS profile identifies a particular user in an AWS account. It secures the user's access to Amazon GameLift resources and related AWS services. You can have multiple profiles for different AWS accounts and users. Profiles are stored on your local device.");
		static const auto kHeaderDocumentationLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "HeaderDocumentationLinkText", "Documentation");
		static const auto kHeaderForumLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "HeaderForumLinkText", "AWS Game Tech forum");
		static const auto kHeaderPricePlanLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "HeaderPricePlanLinkText", "Amazon GameLift pricing");
		static const auto kHeaderCredentialsLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "HeaderCredentialsLinkText", "Read about user credentials");
		static const auto kHeaderIAMPermissionLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "HeaderIAMPermissionLinkText", "IAM permissions for Amazon GameLift");

		// Profile startup
		static const auto kProfileStartupAccountMissingText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileStartupAccountMissingText", "I need a new AWS account and/or user");
		static const auto kProfileStartupAccountMissingDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileStartupAccountMissingDescriptionText", "Set up AWS account and user in the AWS Management Console. Then return here to complete your profile.");
		static const auto kProfileStartupAccountMissingButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileStartupAccountMissingButtonText", "Go to AWS Management Console");

		static const auto kProfileStartupAccountText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileStartupAccountText", "I have an AWS account and user");
		static const auto kProfileStartupAccountDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileStartupAccountDescriptionText", "Create a profile with an AWS account and user.");
		static const auto kProfileStartupAccountButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileStartupAccountButtonText", "Add new profile");

		// Profile headers
		static const auto kProfileDetailsTitleText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileDetailsTitleText", "Profile details");
		static const auto kProfileModificationDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileModificationDescriptionText", "Provide the requested information to create an AWS profile for use with Amazon GameLift.");
		static const auto kProfileBootstrappedDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileBootstrappedDescriptionText", "By default, we will use settings found in the profile named default. To use alternate settings, you can create and set additional profiles.");

		static const auto kAWSProfilesTitleText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AWSProfilesTitleText", "User profiles");
		static const auto kAWSProfilesDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AWSProfilesDescriptionText", "These user profiles are configured for use with the plugin on this device. Select an existing profile to use during your current plugin session or create a new profile.");

		// Profile modification
		static const auto kProfileModificationProfileSelectionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileModificationProfileSelectionText", "Choose a user profile");
		static const auto kProfileModificationProfileNameText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileModificationProfileNameText", "Profile name");
		static const auto kProfileModificationAccessKeyText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileModificationAccessKeyText", "Access key ID");
		static const auto kProfileModificationSecretKeyText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileModificationSecretKeyText", "Secret access key");
		static const auto kProfileModificationRegionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileModificationRegionText", "Default AWS Region");

		// Profile help
		static const auto kProfileHelpTitleText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpTitleText", "AWS access credentials");
		static const auto kProfileHelpSubTitleText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpDescriptionText", "AWS uses two-part security credentials to authenticate your user and verify that you have permission to access the AWS resources that you're requesting.");
		static const auto kProfileHelpHeaderText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpSubtitleText", "Need your access key ID and secret access key?");
		static const auto kProfileHelpIntroText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpIntroText", "If you have security credentials for your AWS user, you should have them secured. Hint: you might have downloaded them as a .csv file or created an AWS CLI profile (look for the .aws folder on your local machine). If you don't have them available, create a new set.");
		static const auto kProfileHelpStep0 = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpStep0", "To create new security credentials:");
		static const auto kProfileHelpStep1 = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpStep1", "1. Sign in to the AWS Management console with your IAM user.");
		static const auto kProfileHelpStep2 = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpStep2", "2. In the console's upper right corner, choose your user name and select the dropdown item 'Security credentials'.");
		static const auto kProfileHelpStep3 = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpStep3", "3. In the 'Access keys' section, choose 'Create access key'. If asked for a use case, choose 'Other'.");
		static const auto kProfileHelpStep4 = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpStep4", "4. Take note of the two-part credentials or download as a .csv file.");
		static const auto kProfileHelpDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ProfileHelpDescriptionText", "Keep your secret access key safe--don't share it! If you lose it, create a new one.");

		static const auto kAddNewProfileSelectionId = 0;
		static const auto kAddNewProfileSelectionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AddNewProfileSelectionText", "<New profile>");

		// Profile bootstrap modal
		static const auto kBootstrapProfileModalHeader = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapProfileModalHeader", "Bootstrap");
		static const auto kBootstrapProfileModalDescription = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapProfileModalDescription", "Bootstrap your profile to prepare AWS resources for use with Amazon GameLift. This includes an Amazon S3 bucket to store project configurations, build artifacts, and other dependencies. Profiles do not share S3 buckets.");
		static const auto kBootstrapProfileModalWarningMessage = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapProfileModalWarningMessage", "This action creates AWS resources for your account. You might incur charges for resources and data storage unless your AWS account is eligible for <RichTextBlock.Bold>AWS Free Tier</> benefits.");
		static const auto kS3BucketNameTitle = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "S3BucketNameTitle", "S3 bucket name");
		static const auto kS3BucketNameTooltip = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "S3BucketNameTooltip", "Specify the S3 bucket name");

		// Buttons
		static const auto kSetProfileAsSelectedButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapAccountButtonText", "Select profile");
		static const auto kBootstrapProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapAccountButtonText", "Bootstrap profile");
		static const auto kCancelButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "CancelButtonText", "Cancel");
		static const auto kCreateProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "CreateProfileButtonText", "Create new profile");
		static const auto kAddAnotherProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AddAnotherProfileButtonText", "+ Add another profile");
		static const auto kBootstrapAndCreateProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapAndCreateAccountText", "Bootstrap and create profile");
		static const auto kBootstrapAndUpdateProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapAndUpdateAccountText", "Bootstrap and update profile");
		static const auto kAddNewProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AddNewProfileButtonText", "Add new profile");
		static const auto kManageYourProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ManageYourProfileButtonText", "Manage your profile");
		static const auto kOpenFileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "OpenFileButtonText", "Open file");
		static const auto kOpenFileButtonHoverText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "OpenFileButtonHoverText", "Open file to view or update the selected profile's settings");

		// Hyperlinks
		static const auto kWhatIsBootstrappingHyperLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "WhatIsBootstrappingHyperLinkText", "What is bootstrapping?");
		static const auto kWhatIsBootstrappingHyperLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-profiles-bootstrap.html");
		static const auto kViewS3ConsoleHyperLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapS3BucketHyperLinkText", "View Amazon S3 Console");
		static const auto kViewS3ConsoleHyperLinkUrl = TEXT("https://s3.console.aws.amazon.com/s3/");
		static const auto kViewIAMConsoleHyperLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "BootstrapS3BucketHyperLinkText", "Go to IAM Console");
		static const auto kViewIAMConsoleHyperLinkUrl = TEXT("https://console.aws.amazon.com/iam");
		static const auto kAWSFreeTierLinkText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AWSFreeTierLinkText", "Read more about AWS Free Tier benefits");
		static const auto kAWSFreeTierLinkUrl = TEXT("https://aws.amazon.com/free/");

		// Bootstrap status
		static const auto kNoBootstrapMessage = NSLOCTEXT("SBootstrapStatus", "NoBootstrapMessage", "Inactive");
		static const auto kBootstrapInProgressMessage = NSLOCTEXT("SBootstrapStatus", "BootstrapInProgressMessage", "In Progress  (typically 5-8 seconds)");
		static const auto kActiveBootstrapMessage = NSLOCTEXT("SBootstrapStatus", "ActiveBootstrapMessage", "Active");
		static const auto kFailedBootstrapMessage = NSLOCTEXT("SBootstrapStatus", "FailedBootstrapMessage", "Failed");

		static const auto kBootstrapStatusText = NSLOCTEXT("SBootstrapStatus", "BootstrapStatusText", "Bootstrap status");
		static const auto kLoadingStatusText = NSLOCTEXT("SBootstrapStatus", "LoadingStatusText", "Loading status");
		static const auto kRegionNameText = NSLOCTEXT("SBootstrapStatus", "RegionNameText", "AWS Region");
		static const auto kBucketNameText = NSLOCTEXT("SBootstrapStatus", "BucketNameText", "Amazon S3 bucket");
		static const auto kEmptyBucketNameText = NSLOCTEXT("SBootstrapStatus", "EmptyBucketNameText", "No bucket created");

		// Deployment Profile
		static const auto kNoDeploymentProfileText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "NoDeploymentProfileText", "You will need to configure an AWS credentials profile to get started.");
		static const auto kNoDeploymentProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "NoDeploymentProfileButtonText", "Open AWS Access Credentials");
		static const auto kIncompleteDeploymentProfileText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "IncompleteDeploymentProfileText", "Complete your credentials profile configuration to get started with Amazon GameLift.");
		static const auto kIncompleteDeploymentProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "IncompleteDeploymentProfileButtonText", "Configure in AWS Access Credentials");
		static const auto kCredentialProfileDescriptionText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "CredentialProfileDescriptionText", "This credential profile will be used to create your fleet, change your active profile in AWS Access Credentials.");
		static const auto kCompleteDeploymentProfileButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "CompleteDeploymentProfileText", "Open AWS Access Credentials");
		static const auto kContainersRegionUnsupportedText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "kContainersRegionUnsupportedText", "This region does not support containers. Select a different profile or create a new one that with a region that supports container fleets.");
		static const auto kContainersRegionSupportButtonText = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "ContainersRegionSupportButtonText", "View Supported Regions");
		static const auto kAWSCredentialsFileLabel = NSLOCTEXT("SGameLiftSettingsAwsAccountMenu", "AWSCredentialsFileLabel", "AWS credentials file");
	} // namespace AwsAccount

	namespace GameLift
	{
		static const auto kGameLiftIntroDescriptionText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftIntroDescriptionText",
			"<RichTextBlock.Bold>Amazon GameLift</> is a dedicated game server hosting solution that deploys, operates, and scales cloud servers for session-based, multiplayer games. Use as a fully managed solution or for specific features. Leveraging the power of AWS, Amazon GameLift delivers the lowest possible latency, low player wait times, and maximum cost savings.");

		static const auto kGameLiftGetStartedTitleText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftGetStartedTitleText", "Choose a hosting option");
		static const auto kGameLiftGetStartedDescriptionText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftGetStartedDescriptionText", "To get started, choose a hosting solution to work with. Each solution provides a step-by-step workflow to deploy your game server and run game sessions.");

		static const auto kGameLiftSetUpAnywhereTitleText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpAnywhereTitleText", "Anywhere");
		static const auto kGameLiftSetUpAnywhereButtonText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpAnywhereButtonText", "Test locally with Anywhere");
		static const auto kGameLiftSetUpAnywhereDescriptionText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpAnywhereDescriptionText", "Create an Anywhere fleet with your local workstation. Prepare your game server for hosting with Amazon GameLift and run it locally. Use a locally running game client to start a new game session and join to play the game.");

		static const auto kGameLiftSetUpManagedEC2TitleText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpManagedEC2TitleText", "Managed EC2");
		static const auto kGameLiftSetUpManagedEC2ButtonText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpManagedEC2ButtonText", "Cloud host with Managed EC2");
		static const auto kGameLiftSetUpManagedEC2DescriptionText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpManagedEC2DescriptionText", "Build a fleet of EC2 instances for cloud-based hosting managed by Amazon GameLift. Deploy your game server to the fleet, start a game session, and join from a game client on any supported device.");

		static const auto kGameLiftSetUpContainersTitleText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpContainersTitleText", "Managed Containers");
		static const auto kGameLiftSetUpContainersButtonText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpContainersButtonText", "Cloud host with Managed containers");
		static const auto kGameLiftSetUpContainersDescriptionText = NSLOCTEXT("SGameLiftSettingsGameLiftMenu", "GameLiftSetUpContainersDescriptionText", "Set up a container fleet with your game server for cloud-based hosting. Deploy your containerized game server to EC2 instances managed by Amazon GameLift. Start a game session and join from a game client on any supported device.");

		static const auto kGameLiftLogoFile = TEXT("Editor/Images/GameLiftLogo.png");
	}

	namespace UseCase
	{
		static const auto kUseCaseTitleFormatText = NSLOCTEXT("SGameLiftSettingsUseCaseMenu", "UseCaseTitleFormatText", "<RichTextBlock.Bold>{0}</> developed <RichTextBlock.Bold>[{1}]</> using Amazon GameLift");
		static const auto kUseCaseLearnMoreText = NSLOCTEXT("SGameLiftSettingsUseCaseMenu", "UseCaseLearnMoreText", "Learn more");
	}

	namespace Help
	{
		static const auto kHelpSetupAWSAccountLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpSetupAWSAccountLinkHeader", "Set up an AWS account");
		static const auto kHelpSetupAWSAccountLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpSetupAWSAccountLinkText", "Learn more about setting up an AWS account and user for use with Amazon GameLift.");
		static const auto kHelpPluginGuideLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpPluginGuideLinkHeader", "Plugin Guide: Create a user profile");
		static const auto kHelpPluginGuideLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpPluginGuideLinkText", "Get help with setting up a user profile for use with the Amazon GameLift plugin.");
		static const auto kHelpGettingStartedLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpGettingStartedLinkHeader", "Getting started with an AWS account");
		static const auto kHelpOrganizingEnvironmentLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpOrganizingEnvironmentLinkHeader", "Organizing your AWS environment using multiple accounts");
		static const auto kHelpGetAccessKeyLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpGetAccessKeyLinkHeader", "Get your access keys");
		static const auto kHelpGetAccessKeyLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpGetAccessKeyLinkText", "Learn how to generate a new set of security credentials for your AWS user.");
		static const auto kHelpServiceLocationsLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpServiceLocationsLinkHeader", "Amazon GameLift service locations");
		static const auto kHelpServiceLocationsLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpServiceLocationsLinkText", "Choose an AWS Region for your GameLift fleet type.");
		static const auto kHelpManageAccessKeysLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpManageAccessKeysLinkHeader", "Manage access keys for IAM users");
		static const auto kHelpManageAccessKeysLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpManageAccessKeysLinkText", "Learn how to manage and secure your access keys.");
		static const auto kHelpGameLiftHostingSolutionsLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpGameLiftHostingSolutionsLinkHeader", "Amazon GameLift hosting solutions");
		static const auto kHelpGameLiftHostingSolutionsLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpGameLiftHostingSolutionsLinkText", "Learn more about the different ways you can host your game servers. You can use the plugin to create managed EC2 fleets, managed container fleets, and Anywhere fleets.");
		static const auto kHelpSettingupHostingFleetLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpSettingupHostingFleetLinkHeader", "Setting up a hosting fleet with Amazon GameLift");
		static const auto kHelpSettingupHostingFleetLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpSettingupHostingFleetLinkText", "Learn about the role of a hosting fleet and how each fleet type is structured.");
		static const auto kHelpDeployContainerLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpDeployContainerLinkHeader", "Deploy your game to a managed container fleet");
		static const auto kHelpDeployContainerLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpDeployContainerLinkText", "Get help with using the plugin to build and deploy a fleet for your containerized game server.");
		static const auto kHelpDeployEC2LinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpDeployEC2LinkHeader", "Deploy your game to a managed EC2 fleet");
		static const auto kHelpDeployEC2LinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpDeployEC2LinkText", "Get help with using the plugin to build and deploy a cloud-based fleet for your game server.");
		static const auto kHelpDeployAnywhereLinkHeader = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpDeployAnywhereLinkHeader", "Deploy your game to an Anywhere fleet");
		static const auto kHelpDeployAnywhereLinkText = NSLOCTEXT("SGameLiftSettingsHelpMenu", "HelpDeployAnywhereLinkText", "Get help with using the plugin to set up an Anywhere fleet with your local device. Use the fleet to test your game server locally.");

	}

	namespace Logs
	{
		static const auto kBootstrapSucceed = TEXT("Bootstrap Account succeed");
		static const auto kBootstrapFailed = TEXT("Bootstrap Account failed");
	}

	static const auto kGoToSettingsButtonText = NSLOCTEXT("SSetupMessage", "GoToSettingsButtonText", "Manage Settings");
	static const auto kGoToDeploymentMenuText = NSLOCTEXT("SSetupMessage", "GoToDeploymentMenuText", "Deployment Menu");
} // namespace Settings

namespace ContextMenu
{
	// Amazon Section
	static const auto kAmazonSectionText = NSLOCTEXT("FGameLiftPluginModule", "AmazonSectionText", "Amazon");

	// Plugin Main Menu
	static const auto kPluginMainMenuText = NSLOCTEXT("FGameLiftPluginModule", "PluginMainMenuText", "GameLift Plugin");
	static const auto kPluginMainMenuTooltip = NSLOCTEXT("FGameLiftPluginModule", "PluginMainMenuTooltip", "Launch GameLift plugin");

	static const auto kDeploymentSectionText = NSLOCTEXT("FGameLiftPluginModule", "DeploymentSectionText", "Amazon GameLift");
	// Profile Setup Section
	static const auto kProfileSetupSectionText = NSLOCTEXT("FGameLiftPluginModule", "DeploymentSectionText", "Profile Setup");

	// Hosting Solutions Section
	static const auto kHostingSolutionsSectionText = NSLOCTEXT("FGameLiftPluginModule", "HostingSolutionsSectionText", "Hosting Solutions");

	// Help Section
	static const auto kHelpSectionText = NSLOCTEXT("FGameLiftPluginModule", "HelpSectionText", "Help");
	static const auto kHelpSubMenuText = NSLOCTEXT("FGameLiftPluginModule", "HelpSubMenuText", "Help");
	static const auto kHelpSubMenuTooltip = NSLOCTEXT("FGameLiftPluginModule", "HelpSubMenuTooltip", "Ask for help");

	static const auto kHelpDocumentationSectionText = NSLOCTEXT("FGameLiftPluginModule", "DocumentationSectionText", "Amazon GameLift");
	static const auto kHelpSdkSectionText = NSLOCTEXT("FGameLiftPluginModule", "SdkSectionText", "SDK");
}

namespace Menu
{
	static const auto kStatusMessageActiveTimeSec = 10;

	namespace DeployCommon
	{
		// Profile settings section
		static const auto kSetProfileHeader = NSLOCTEXT("SDeployCommon", "SetProfileHeader", "1. Set your profile");

		// Game integration section
		static const auto kIntegrateGameHeader = NSLOCTEXT("SDeployCommon", "IntegrateGameHeader", "Set up your game with Amazon GameLift");
		static const auto kIntegrateGameAnywhereDescription = NSLOCTEXT("SDeployCommon", "IntegrateGameAnywhereDescription", "Identify the game server build that you want to deploy to an Anywhere fleet. Your client and server components must be integrated and packaged with the Amazon GameLift SDKs. For guidance on integration and packaging, see the Plugin Guide for Unreal Engine.");
		static const auto kIntegrateGameEC2Description = NSLOCTEXT("SDeployCommon", "IntegrateGameEC2Description", "Integrate and package your game components with the Amazon GameLift SDKs. For guidance, see the Plugin Guide for Unreal Engine.");
		static const auto kSetServerBuildButtonText = NSLOCTEXT("SDeployCommon", "SetServerBuildButtonText", "Set server build path");
		static const auto kModifyBuildParameters = NSLOCTEXT("SDeployCommon", "ModifyBuildParameters", "Modify parameters");
		static const auto kUnrealSourceWarningMessageText = NSLOCTEXT("SDeployCommon", "kUnrealSourceWarningMessageText", "To package a server build for a multiplayer game, you need to use a source-built version of the Unreal Engine editor. If you're not using a source-build version, download the source code, build it, and then use it to open your game project.");
		static const auto kReadIntegrationGuideText = NSLOCTEXT("SDeployCommon", "ReadIntegrationGuide", "Read Integration Guide");
		static const auto kDownloadSourceText = NSLOCTEXT("SDeployCommon", "DownloadSourceText", "Download Source");

		// Server and Client paths
		static const auto kGameServerPathTitle = NSLOCTEXT("SDeployCommon", "GameServerPathTitle", "Path to server build");
		static const auto kGameServerPathHint = NSLOCTEXT("SDeployCommon", "GameServerPathHint", "Path to server build executable");
		static const auto kGameServerPathTooltip = NSLOCTEXT("SDeployCommon", "GameServerPathTooltip", "Specify path to server build executable");
		static const auto kOutConfigFilePathTitle = NSLOCTEXT("SDeployCommon", "ConfigFilePath_Title", "Client configuration output path");
		static const auto kOutConfigFilePathTooltip = NSLOCTEXT("SDeployCommon", "ConfigFilePath_ToolTip", "Amazon GameLift generates these files to store your hosting configuration based on the information you provide.");
		static const auto kOutConfigFilePathHint = NSLOCTEXT("SDeployCommon", "ConfigFilePath_Hint", "Path to client configuration output path");

		static const auto kGameClientPathTitle = NSLOCTEXT("SDeployCommon", "GameClientTitle", "Client build executable");
		static const auto kGameClientPathHint = NSLOCTEXT("SDeployCommon", "GameClientPathHint", "Path to client build executable");
		static const auto kGameClientPathTooltip = NSLOCTEXT("SDeployCommon", "GameClientPathTooltip", "Specify path to client build executable");
		static const auto kGameClientLauncherArgumentsTitle = NSLOCTEXT("SDeployCommon", "GameClientLauncherArgumentsTitle", "Command-line arguments - (optional) ");
		static const auto kGameClientLauncherArgumentsHint = NSLOCTEXT("SDeployCommon", "GameClientLauncherArgumentsHint", "Extra arguments to pass to game client");
		static const auto kGameClientLauncherArgumentsTooltip = NSLOCTEXT("SDeployCommon", "GameClientPathTooltip", "Specify additional command-line arguments when launching the client");

		static const auto kDeploymentShowOutputLog = NSLOCTEXT("SDeployCommon", "DeploymentShowOutputLog", "Show output log");

		// Launch Bar
		static const auto kUpdateDeploymentButtonText = NSLOCTEXT("SDeployCommon", "UpdateDeploymentButtonText", "Update Deployment");
		static const auto kTestTwoPlayerCheckBoxText = NSLOCTEXT("SDeployCommon", "TestTwoPlayerCheckBoxText", "Test with 2-player client?");
		static const auto kStartClientButtonText = NSLOCTEXT("SDeployCommon", "StartClientButtonText", "Start Client");
		static const auto kStartServerButtonText = NSLOCTEXT("SDeployCommon", "StartServerButtonText", "Start Server");
		static const auto kStartButtonLoadingText = NSLOCTEXT("SDeployCommon", "StartButtonLoadingText", "Starting...");
		static const auto kClientBuildModalHeader = NSLOCTEXT("SDeployCommon", "ClientBuildModalHeader", "Client build");
		static const auto kClientBuildModalDescription = NSLOCTEXT("SDeployCommon", "ClientBuildModalDescription", "Enter the local path to your game client executable and choose <RichTextBlock.Bold>Start Client</> to open an instance of your game client. \nFor multiplayer testing, choose <RichTextBlock.Bold>Start Client</> multiple times to run two or more game clients at the same time.");

		static const auto kLaunchedGameClientFailureMessage = NSLOCTEXT("SDeployCommon", "ClientFailureMessage", "Failed to launch game client");

		static const auto kStartClientLoadingButtonDelay = 1.0f;

		// Deployment tooltips
		static const auto kDeploymentEnabledTooltip = NSLOCTEXT("SDeployCommon", "DeploymentEnabledTooltip", "Deployment can be triggered");
		static const auto kDeploymentDisabledNeedBootstrapTooltip = NSLOCTEXT("SDeployCommon", "DeploymentDisabledNeedBootstrapTooltip", "Account needs to be configured first");
	
		// Documentation
		static const auto kReadDeveloperGuideText = NSLOCTEXT("SDeployCommon", "ReadDeveloperGuideText", "Read Developer Guide");
		static const auto kReportIssuesText = NSLOCTEXT("SDeployCommon", "ReportIssuesText", "Report Issues");
	}

	namespace DeployAnywhere
	{
		static const auto kConnectToFleetHeader = NSLOCTEXT("DeployAnywhere", "ConnectToFleetHeader", "Connect to an Anywhere fleet");
		static const auto kRegisterComputeHeader = NSLOCTEXT("DeployAnywhere", "RegisterComputeHeader", "Register your workstation");
		static const auto kGenerateAuthTokenHeader = NSLOCTEXT("DeployAnywhere", "GenerateAuthTokenHeader", "Generate Auth Token - (optional)");
		static const auto kModifyPath = NSLOCTEXT("DeployAnywhere", "ModifyPath", "Change server build path");

		// Connect section
		static const auto kFleetSectionDescription = NSLOCTEXT("SConnectAnywhereFleetMenu", "FleetSectionDescription", "Choose an existing Anywhere fleet or create a new fleet for your deployment. An Anywhere fleet is a collection of hosting resources that you managed (such as your local workstation). This action might create AWS resources and incur charges to your AWS account unless the account is eligible for AWS Free Tier benefits.");
		static const auto kNewFleetNameTitle = NSLOCTEXT("SConnectAnywhereFleetMenu", "AnywhereFleetNameTitle", "Fleet name");
		static const auto kUnselectedFleetDropDownText = NSLOCTEXT("SConnectAnywhereFleetMenu", "UnselectedFleetDropDownText", "Choose an existing Anywhere fleet");
		static const auto kLoadingFleetDropDownText = NSLOCTEXT("SConnectAnywhereFleetMenu", "LoadingFleetDropDownText", "Loading fleets...");
		static const auto kLoadingFleetErrorText = NSLOCTEXT("SConnectAnywhereFleetMenu", "LoadingFleetErrorText", "An error occurred when trying to load available fleets.");
		static const auto kLoadingFleetRetryText = NSLOCTEXT("SConnectAnywhereFleetMenu", "LoadingFleetRetryText", "Reload fleets");
		static const auto kSelectedFleetTitle = NSLOCTEXT("SConnectAnywhereFleetMenu", "AnywhereSelectedFleetTitle", "Fleet name");
		static const auto kSelectedFleetIDTitle = NSLOCTEXT("SConnectAnywhereFleetMenu", "AnywhereSelectedFleetIdTitle", "Fleet ID");
		static const auto kCreateFleetButtonText = NSLOCTEXT("SConnectAnywhereFleetMenu", "AnywhereCreateFleetButtonText", "Create Anywhere fleet");
		static const auto kGoToCreateFleetButtonText = NSLOCTEXT("SConnectAnywhereFleetMenu", "AnywhereGoToCreateFleetButtonText", "Create new Anywhere fleet");
		static const auto kFleetNameInstructionText = NSLOCTEXT("SConnectAnywhereFleetMenu", "FleetNameInstructionText", "The fleet name must have 1 - 1024 characters. Valid characters are A-Z, a-z, 0-9, _ and - (hyphen).");
		static const auto kAnywhereFleetActiveText = NSLOCTEXT("SConnectAnywhereFleetMenu", "AnywhereFleetActiveText", "Active");
		static const auto kNewFleetNameTextBoxHint = NSLOCTEXT("SConnectAnywhereFleetMenu", "NewFleetNameTextBoxHint", "Fleet name");
		static const auto kModifyAnywhereFleet = NSLOCTEXT("SConnectAnywhereFleetMenu", "ModifyAnywhereFleet", "Change fleet selection");


		// Auth token section
		static const auto kAuthTokenTitle = NSLOCTEXT("SDeployAnywhereMenu", "AuthTokenTitle", "Auth token");
		static const auto kAuthTokenNotGeneratedText = NSLOCTEXT("SDeployAnywhereMenu", "AuthTokenNotGeneratedText", "The auth token is generated when you launch your game.");
		static const auto kAuthTokenGenerateNoteText = NSLOCTEXT("SDeployAnywhereMenu", "AuthTokenGenerateNoteText", "Auth tokens are valid for 3 hours and must be refreshed.");
		static const auto kAuthTokenGenerateButtonText = NSLOCTEXT("SDeployAnywhereMenu", "AuthTokenGenerateButtonText", "Generate auth token");

		// Register compute section
		static const auto kRegisterWorkstationDescription = NSLOCTEXT("SAnywhereRegisterComputeMenu", "RegisterWorkstationDescription", "Register your local workstation as a compute resource in the Anywhere fleet. Amazon GameLift establishes communication with game server processes that are running on the compute. It sends prompts to start game sessions and other actions. \n\nAfter you've registered your workstation, you're ready to launch and play your multi-player game with Amazon GameLift! Start the server and then launch your project's game client to join a game session and start playing.");
		static const auto kComputeNameTitle = NSLOCTEXT("SAnywhereRegisterComputeMenu", "ComputeNameTitle", "Compute name");
		static const auto kComputeNameTextHint = NSLOCTEXT("SAnywhereRegisterComputeMenu", "ComputeNameTextHint", "Compute name");
		static const auto kComputeNameDefault = NSLOCTEXT("SAnywhereRegisterComputeMenu", "ComputeNameDefault", "AnywhereCompute");
		static const auto kComputeIPTitle = NSLOCTEXT("SAnywhereRegisterComputeMenu", "ComputeIPTitle", "IP address");
		static const auto kComputeIPTextHint = NSLOCTEXT("SAnywhereRegisterComputeMenu", "ComputeIPTextHint", "0 . 0 . 0 . 0");
		static const auto kComputeIPTextTooltip = NSLOCTEXT("SAnywhereRegisterComputeMenu", "ComputeIPTextTooltip", "Defaults to the local IP address. You can use localhost (127.0.0.1) as the IP address. If your machine is accessbile via a public IP address, change this value as appropriate.");
		static const auto kComputeStatusRegisteredText = NSLOCTEXT("SAnywhereRegisterComputeMenu", "ComputeStatusRegisteredText", "Registered");
		static const auto kRegisterNewComputeButtonText = NSLOCTEXT("SAnywhereRegisterComputeMenu", "RegisterNewComputeButtonText", "Register new compute");
		static const auto kRegisterFirstComputeButtonText = NSLOCTEXT("SAnywhereRegisterComputeMenu", "RegisterFirstComputeButtonText", "Register compute");

		// Launch game server and client section
		static const auto kGameServerLaunchTitle = NSLOCTEXT("SDeployAnywhereMenu", "GameServerLaunchTitle", "Launch game server");
		static const auto kGameServerLaunchButtonText = NSLOCTEXT("SDeployAnywhereMenu", "GameServerLaunchButtonText", "Run server");

		static const auto kGameClientLaunchTitle = NSLOCTEXT("SDeployAnywhereMenu", "GameClientLaunchTitle", "Launch game client");
		static const auto kGameClientLaunchButtonText = NSLOCTEXT("SDeployAnywhereMenu", "GameClientLaunchButtonText", "Run client");

		static const auto kGameLaunchNoteText = NSLOCTEXT("SDeployAnywhereMenu", "GameLaunchNoteText", "You may need to wait 30 secs after fleet creation.");

		static const auto kGameServerLaunchParameterLog = TEXT("-log");
		static const auto kGameServerLaunchParameterEnableAnywhere = TEXT("-glAnywhere");
		static const auto kGameServerLaunchParameterWebSocketUrl = TEXT("-glAnywhereWebSocketUrl={0}");
		static const auto kGameServerLaunchParameterFleetId = TEXT("-glAnywhereFleetId={0}");
		static const auto kGameServerLaunchParameterHostId = TEXT("-glAnywhereHostId={0}");
		static const auto kGameServerLaunchParameterAuthToken = TEXT("-glAnywhereAuthToken={0}");

		static const auto kGameClientLaunchParameterWindowMode = TEXT("-windowed");
		static const auto kGameClientLaunchParameterCredentialsName = TEXT("-glAnywhereClientCredentialsName={0}");
		static const auto kGameClientLaunchParameterFleetId = TEXT("-glAnywhereClientFleetId={0}");
		static const auto kGameClientLaunchParameterCustomLocation = TEXT("-glAnywhereClientCustomLocation={0}");

		static const auto kGameServerWebSocketUrlFormat = TEXT("wss://{0}.api.amazongamelift.com");

		// Hyperlinks
		static const auto kViewInGameLiftConsoleHyperLinkText = NSLOCTEXT("SDeployAnywhereMenu", "ViewInGameLiftConsoleHyperLinkText", "View in AWS Management Console");
		static const auto kViewInGameLiftConsoleHyperLinkUrlFormat = TEXT("https://{0}.console.aws.amazon.com/gamelift/fleets");

		static const auto kStatusTitle = NSLOCTEXT("SDeployAnywhereMenu", "StatusTitle", "Status");
		static const auto kCancelButtonText = NSLOCTEXT("SDeployAnywhereMenu", "CancelButtonText", "Cancel");

		// Misc
		static const auto kIPv4ValidationRegex = TEXT("^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])(\\.(?!$)|$)){4}$");
	} // namespace DeployAnywhere

	namespace DeployManagedEC2
	{
		static const auto kS3Bucket = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "S3Bucket_ToolTip", "Resources used by this plugin are stored in this Amazon S3 bucket");

		// Set section buttons
		static const auto kConfirmIntegrationCompleteButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "ConfirmIntegrationCompleteButtonText", "Amazon GameLift integration complete");
		static const auto kConfigureParametersButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "ConfigureParametersButtonText", "Select scenario");
		static const auto kSetParametersForDeploymentButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "SetParametersForDeploymentButtonText", "Set parameters for deployment");
		static const auto kViewFleetInAwsConsoleButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "ViewFleetInAwsConsoleButtonText", "View fleet in AWS Console");

		// Modify section buttons
		static const auto kModifyScenarioButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "ModifyScenarioButtonText", "Change scenario");
		static const auto kModifyParametersButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "ModifyParametersButtonText", "Modify parameters");
		
		// Select scenario
		static const auto kSelectScenarioHeader = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "SelectScenarioHeader", "Select deployment scenario");
		static const auto kSelectScenarioDescription = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "SelectScenarioDescription", "Choose the type of hosting solution you want to create. Each scenario generates a different collection of AWS resources. The charges to your AWS account, incurred when you deploy a fleet, vary depending on the scenario you select, unless the account is eligible for AWS Free Tier benefits.");
		static const auto kFleetTypeTitle = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "FleetTypeTitle", "Fleet Type");

		// Game parameters
		static const auto kGameParametersHeader = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "GameParametersHeader", "Set game parameters");
		static const auto kGameParametersDescription = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "GameParametersDescription", "Tell us about the game server build you want to deploy to this fleet. Use a server build that's been integrated and packaged with the Amazon GameLift SDKs. For guidance on integration and packaging, see the Plugin Guide for Unreal Engine.");

		static const auto kBuildName = NSLOCTEXT("SDeploymentFields", "BuildName_Text", "Server build name");
		static const auto kBuildNameHint = NSLOCTEXT("SDeploymentFields", "BuildName_Hint", "Build name");
		static const auto kBuildNameInstructionText = NSLOCTEXT("SDeploymentFields", "BuildName_InstructionText", "The build name must have 1-16 characters.");

		static const auto kBuildOperatingSystemTitle = NSLOCTEXT("SDeploymentFields", "BuildOperatingSystem_Text", "Server build OS");

		static const auto kBuildFolderPathTitle = NSLOCTEXT("SDeploymentFields", "BuildFolderPath_Title", "Server build folder");
		static const auto kBuildFolderPathTooltip = NSLOCTEXT("SDeploymentFields", "BuildFolderPath_ToolTip", "Specify the folder containing the packaged game server.");
		static const auto kBuildFolderPathHint = NSLOCTEXT("SDeploymentFields", "BuildFolderPath_Hint", "Path to server build folder");

		static const auto kBuildFilePathTitle = NSLOCTEXT("SDeploymentFields", "BuildFilePath_Title", "Server build executable");
		static const auto kBuildFilePathTooltip = NSLOCTEXT("SDeploymentFields", "BuildFilePath_ToolTip", "Specify the executable file of your game server within server folder path.");
		static const auto kBuildFilePathHint = NSLOCTEXT("SDeploymentFields", "BuildFilePath_Hint", "Path to server build executable");
		static const auto kBuildFilePathFilter = TEXT("Game Server|*.exe;*.sh;|All Files|*");

		static const auto kExtraServerResourcesPathTitle = NSLOCTEXT("SDeploymentFields", "ExtraFilesPath_Title", "Additional server resources path");
		static const auto kExtraServerResourcesPathTooltip = NSLOCTEXT("SDeploymentFields", "ExtraFilesPath_ToolTip", "Specify a folder which contains additional files for Gamelift server delivery.");

		static const auto kWindowsServer2016Name = NSLOCTEXT("EFleetOperatingSystem", "WindowsServer2016Name", "Windows Server 2016");
		static const auto kWindowsServer2016Value = NSLOCTEXT("EFleetOperatingSystem", "WindowsServer2016Value", "WINDOWS_2016");
		static const auto kAmazonLinux2Name = NSLOCTEXT("EFleetOperatingSystem", "AmazonLinux2Name", "Amazon Linux 2 (AL2)");
		static const auto kAmazonLinux2Value = NSLOCTEXT("EFleetOperatingSystem", "AmazonLinux2Value", "AMAZON_LINUX_2");
		static const auto kAmazonLinux2023Name = NSLOCTEXT("EFleetOperatingSystem", "AmazonLinux2023Name", "Amazon Linux 2023 (AL2023)");
		static const auto kAmazonLinux2023Value = NSLOCTEXT("EFleetOperatingSystem", "AmazonLinux2023Value", "AMAZON_LINUX_2023");

		// Deploy scenario
		static const auto kDeploymentScenarioHeader = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "DeploymentScenarioHeader", "Deploy scenario");
		static const auto kDeploymentStatusInfoRichText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "DeploymentStatusInfoRichText", "When you've selected a deployment scenario and set your game parameters, you're ready to deploy your complete hosting solution with managed EC2 fleets. This action creates AWS resources and incurs charges to your AWS account unless the account is eligible for AWS Free Tier benefits. \n\nDeployment can take 30 to 40 minutes. When deployment is complete, game servers are running and waiting for players. You can launch game clients from any supported device and use it to join a game session and start playing. Use the Start client button to launch a game client from your local workstation. If you deployed the FlexMatch scenario, you need at least two game clients requesting a match to start a game session.");

		static const auto kGameLiftHowToDeployYourFirstGameLinkText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "GameLiftHowToDeployYourFirstGameLinkText", "How to deploy your first game");
		static const auto kGameLiftChooseLocationLinkText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "GameLiftChooseLocationLinkText", "Choose a location for your fleet");
		static const auto kGameLiftPricingPlanLinkText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "GameLiftPricingPlanLinkText", "Amazon GameLift pricing");
		static const auto kDeploymentScenarioLearnMoreLinkText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "DeploymentScenarioLearnMoreLinkText ", "Learn more");

		static const auto kDeploymentStartedAwsScenarioNotification = TEXT("Deploying Amazon GameLift scenario...");
		static const auto kDeploymentStartedCustomScenarioNotification = TEXT("Deploying a custom Amazon GameLift scenario...");
		static const auto kDeploymentCompletedNotification = TEXT("Amazon GameLift deployment is complete.");
		static const auto kDeploymentFailedNotification = TEXT("Amazon GameLift deployment is failed.");

		static const auto kDeploymentAbortingNotification = TEXT("Aborting Amazon GameLift deployment...");
		static const auto kDeploymentAbortedNotification = TEXT("Amazon GameLift deployment is aborted.");
		static const auto kDeploymentAbortFailedNotification = TEXT("Failed to abort Amazon GameLift deployment.");

		static const auto kNeedDeployment = "Need Deployment";

		static const auto kNoDeployment = "Undeployed";
		static const auto kNoDeployment_Alternate = "No Deployment";
		static const auto kInProgressDeployment = "In Progress";
		static const auto kAbortInProgressDeployment = "Abort in Progress";
		static const auto kActiveDeployment = "Deployed";
		static const auto kActiveDeployment_Alternate = "Active";
		static const auto kFailedDeployment = "Failed";

		static const auto kDeploymentStoppedMessage = "Deployment has been cancelled";

		static const auto kConfirmStopDeploymentText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "ConfirmStopDeploymentText", "Do you really want to stop deployment?");
		static const auto kConfirmReploymentText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "ConfirmRedeploymentText", "Do you really want to redeploy stack?");

		static const auto kAwsResourceActionsText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "DeployCloudFormationTitle", "AWS resource actions");
		static const auto kDeployCloudFormationButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "DeployCloudFormationButtonText", "Deploy scenario");
		static const auto kRedeployCloudFormationButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "RedeployCloudFormationButtonText", "Redeploy scenario");
		static const auto kStopCloudFormationButtonText = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "StopCloudFormationButtonText", "Stop deployment");

		static const auto kDeploymentDisabledBuildNameEmptyTooltip = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "kDeploymentDisabledBuildNameEmptyTooltip", "Cannot deploy game name with empty build name");
		static const auto kDeploymentDisabledBuildNameTooLongTooltip = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "kDeploymentDisabledBuildNameTooLongTooltip", "Cannot deploy game name with more than 16 characters");
		static const auto kDeploymentDisabledServerBuildPathNotSetTooltip = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "kDeploymentDisabledServerBuildPathNotSetTooltip", "Cannot deploy with empty server build paths");
		static const auto kDeploymentDisabledAlreadyActiveTooltip = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "kDeploymentDisabledAlreadyActiveTooltip", "Deployment cannot be triggered once it is in progress state");

		static const auto kMaxGameNameLength = 16;

		static const auto kStopDeploymentEnabledTooltip = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "StopDeploymentEnabledTooltip", "Deployment can be stopped");
		static const auto kStopDeploymentDisabledTooltip = NSLOCTEXT("SGameLiftDeployManagedEC2Menu", "StopDeploymentDisabledTooltip", "Deployment can be stopped once it is in pending status");

		static const auto kViewInCloudFormationConsoleHyperLinkText = NSLOCTEXT("SDeploymentStatus", "ViewInCloudFormationConsoleHyperLinkText", "View in AWS Management Console");
		static const auto kViewInCloudFormationHyperLinkUrlFormat = TEXT("https://{0}.console.aws.amazon.com/cloudformation/home");

		static const auto kDeploymentStatusText = NSLOCTEXT("SGameLiftClientMenu", "DeploymentStatus", "Deployment status");
		static const auto kScenarioText = NSLOCTEXT("SGameLiftClientMenu", "Scenario", "Deployment scenario");
		static const auto kAwsRegionText = NSLOCTEXT("SDeploymentStatus", "AwsRegion", "AWS Region");
		static const auto kBuildNameText = NSLOCTEXT("SDeploymentStatus", "BuildName", "Server build name");

		static const auto kLastUpdatedText = NSLOCTEXT("SDeploymentStatus", "LastUpdatedText", "Last updated");
		static const auto kErrorMessageText = NSLOCTEXT("SDeploymentStatus", "ErrorMessageText", "Error message");

		static const auto kCognitoClientIdText = NSLOCTEXT("SDeploymentStatus", "CognitoClientId", "Cognito client ID");
		static const auto kCognitoClientIdTooltip = NSLOCTEXT("SDeploymentStatus", "CognitoClientId_Tooltip", "The Amazon Cognito service authenticates players for your game. Use this Client ID wherever you need to \ninteract with your hosting solution's Cognito user pool and manage player authentication.");

		static const auto kApiGatewayEndpointText = NSLOCTEXT("SDeploymentStatus", "ApiGatewayEndpoint", "API Gateway endpoint");
		static const auto kApiGatewayEndpointTooltip = NSLOCTEXT("SDeploymentStatus", "ApiGatewayEndpoint_Tooltip", "The Amazon API Gateway service enables client access to Amazon GameLift and other AWS resources via a WebSocket API \nand an authenticated player ID. Use this endpoint when you need to call an AWS service API on behalf of a game client.");

		// Client
		static const auto kRunDeploymentGameInfoText = NSLOCTEXT("SGameLiftClientMenu", "ClientInfoText", "To run a sample game you need to specify Client path that your previously built.");

		static const auto kNotDeployedMessage = NSLOCTEXT("SGameLiftClientMenu", "NotDeployedMessage", "To run a sample Game in Amazon Gamelift Testing Mode\nyou need to make deployment first.");

		static const auto kLaunchGameClientText = NSLOCTEXT("SGameLiftClientMenu", "LaunchGameClient", "Launch game client");
		static const auto kLaunchGameClientButtonText = NSLOCTEXT("SGameLiftClientMenu", "LaunchGameClientButtonText", "Run client");

		static const auto kCanLaunchGameClientTooltipText = NSLOCTEXT("SGameLiftClientMenu", "kCanLaunchGameClientTooltipText", "Game client can be launched");
		static const auto kCannotLaunchGameClientNoPathTooltipText = NSLOCTEXT("SGameLiftClientMenu", "kCannotLaunchGameClientNoPathTooltipText", "Specify a game path");
		static const auto kCannotLaunchGameClientNoDeploymentTooltipText = NSLOCTEXT("SGameLiftClientMenu", "kCannotLaunchGameClientNoDeploymentTooltipText", "Create a deployment before launching game client");

		namespace Logs
		{
			static const auto kOutputPathChangedWarning = TEXT("Client Config File Output Path is not default");
			static const auto kDeploymentSucceed = TEXT("Deployment succeed");
			static const auto kDeploymentFailed = TEXT("Deployment failed");
		}
	} // namespace DeployManagedEC2

	namespace DeployContainers
    {
		// Profile settings section
		static const auto kStepZero = NSLOCTEXT("DeployContainers", "StepZero", "Step 0");
		static const auto kAWSCredendentialsProfileTitle = NSLOCTEXT("DeployContainers", "AWSCredendentialsProfileTitle", "AWS Credentials Profile");

		// Intake Question
		static const auto kStepOne = NSLOCTEXT("DeployContainers", "StepOne", "Step 1");
		static const auto kIntakeQuestionnaireTitle = NSLOCTEXT("DeployContainers", "IntakeQuestionnaireTitle", "Assess Container Readiness");
		static const auto kExistingContainerImageQuestion = NSLOCTEXT("DeployContainers", "ExistingContainerImageQuestion", "Do you have an existing container image? If not, we'll create one for you.");
		static const auto kECRRepositoryQuestion = NSLOCTEXT("DeployContainers", "ECRRepositoryQuestion", "Do you want to store the new container image in an existing Amazon ECR repository? If not, we'll create a private repository for the image.");
		static const auto kWhereIsContainerImageQuestion = NSLOCTEXT("DeployContainers", "WhereIsContainerImageQuestion", "Where does your Container image live?");
		static const auto kAnswerYes = NSLOCTEXT("DeployContainers", "AnswerYes", " Yes");
		static const auto kAnswerNo = NSLOCTEXT("DeployContainers", "AnswerNo", " No");
		static const auto kAnswerDocker = NSLOCTEXT("DeployContainers", "AnswerDocker", " Local docker image");
		static const auto kAnswerAmazonECR = NSLOCTEXT("DeployContainers", "AnswerAmazonECR", " Amazon ECR private repo");

		// User Input
		static const auto kStepTwo = NSLOCTEXT("DeployContainers", "StepTwo", "Step 2");
		static const auto kUserInputSectionTitle = NSLOCTEXT("DeployContainers", "UserInputSectionTitle", "Configure Image Deployment");

		// User Input - Game Server Input
		static const auto kGameServerBuildLabel = NSLOCTEXT("DeployContainers", "GameServerBuildLabel", "Game server build directory");
		static const auto kGameServerBuildHint = NSLOCTEXT("DeployContainers", "GameServerBuildHint", "C:/LinuxGameArchiveBuild/LinuxServer");
		static const auto kGameServerBuildTooltip = NSLOCTEXT("DeployContainers", "GameServerBuildTooltip", "Specify path to server build directory");
		static const auto kGameServerExecutableLabel = NSLOCTEXT("DeployContainers", "GameServerExecutableLabel", "Game server executable");
		static const auto kGameServerExecutableHint = NSLOCTEXT("DeployContainers", "GameServerExecutableHint", "C:/LinuxGameArchiveBuild/LinuxServer/MyGameServer/Binaries/Linux/MyGameServer");
		static const auto kGameServerExecutableTooltip = NSLOCTEXT("DeployContainers", "GameServerExecutableTooltip", "Specify path to the server executable");

		// User Input - Docker Input
		static const auto kDockerRepoText = NSLOCTEXT("DeployContainers", "DockerRepoText", "Docker repository");
		static const auto kDockerRepoTextHint = NSLOCTEXT("DeployContainers", "DockerRepoTextHint", "<dockerRepositoryName>");
		static const auto kImageIDText = NSLOCTEXT("DeployContainers", "ImageIDText", "Image ID");
		static const auto kImageIDTextHint = NSLOCTEXT("DeployContainers", "ImageIDTextHint", "<dockerImageID>");

		// User Input - ECR Input
		static const auto kSelectECRRepoLabel = NSLOCTEXT("DeployContainers", "SelectECRRepoLabel", "Select ECR Repository");
		static const auto kUnselecedRepoDropdownText = NSLOCTEXT("DeployContainers", "UnselecedRepoDropdownText", "Choose an ECR Repo");
		static const auto kSelectECRImageLabel = NSLOCTEXT("DeployContainers", "SelectECRImageLabel", "Select Image");
		static const auto kUnselecedImageDropdownText = NSLOCTEXT("DeployContainers", "UnselecedImageDropdownText", "Choose an ECR Image");

		// User Input - Deployment Scenario Choice
		static const auto kDeploymentScenarioText = NSLOCTEXT("DeployContainers", "DeploymentScenarioText", "Deployment scenario");
		static const auto kSingleLocationText = NSLOCTEXT("DeployContainers", "SingleLocationText", " Single Location");
		static const auto kSingleLocationDescription = NSLOCTEXT("DeployContainers", "SingleLocationDescription", "Deploy to one On-Demand fleet and create a game backend to put players into games.");
		static const auto kMultiLocationFlexMatchText = NSLOCTEXT("DeployContainers", "MultiLocationFlexMatchText", " Multi-location Spot with FlexMatch");
		static const auto kMultiLocationFlexMatchDescription = NSLOCTEXT("DeployContainers", "MultiLocationFlexMatchDesciption", "Multi-location with FlexMatch Deploy to one On-Demand fleet with remote locations, and create a game backend, matchmaking, and placement queue to put players into games.");

		// User Input - Default Setting
		static const auto kDefaultSettingsHeader = NSLOCTEXT("DeployContainers", "DefaultSettingsHeader", "Container deployment settings - (optional)");
		static const auto kDefaultSettingsDescription = NSLOCTEXT("DeployContainers", "DefaultSettingsDescription", "Keep or modify these core settings for your container deployment. You can create a fully customized container definition by using the AWS Management Console or AWS CLI.");
		static const auto kGameNameText = NSLOCTEXT("DeployContainers", "GameNameText", "Game name");
		static const auto kGameNameDefault = NSLOCTEXT("DeployContainers", "GameNameDefault", "MyGame");
		static const auto kGameNameTextHint = NSLOCTEXT("DeployContainers", "GameNameTextHint", "The game name must have 1-16 characters. Valid characters are A-Z, a-z, 0-9 and - (hyphen).");
		static const auto kConnectionPortRangeText = NSLOCTEXT("DeployContainers", "ConnectionPortRangeText", "Connection port range");
		static const auto kConnectionPortRangeTextHint = NSLOCTEXT("DeployContainers", "ConnectionPortRangeTextHint", "Must be between 1 and 65535");
		static const auto kConnectionPortRangeDefault = NSLOCTEXT("DeployContainers", "ConnectionPortRangeDefault", "7770-7780");
		static const auto kTotalMemoryLimitText = NSLOCTEXT("DeployContainers", "TotalMemoryLimitText", "Game server memory limit");
		static const auto kTotalMemoryLimitTextHint = NSLOCTEXT("DeployContainers", "TotalMemoryLimitTextHint", "Must be between 4 and 1024000");
		static const auto kTotalMemoryLimitDefault = NSLOCTEXT("DeployContainers", "TotalMemoryLimitDefault", "4000");
		static const auto kTotalMemoryLimitUnit = NSLOCTEXT("DeployContainers", "TotalMemoryLimitUnit", "MiB");
		static const auto kTotalVCPULimitText = NSLOCTEXT("DeployContainers", "TotalVCPULimitText", "Game server vCPU limit");
		static const auto kTotalVCPULimitTextHint = NSLOCTEXT("DeployContainers", "TotalVCPULimitTextHint", "Game server vCPU limit");
		static const auto kTotalVCPULimitDefault = NSLOCTEXT("DeployContainers", "TotalVCPULimitDefault", "2");
		static const auto kTotalVCPULimitUnit = NSLOCTEXT("DeployContainers", "TotalVCPULimitUnit", "vCPUs");
		static const auto kImageTagText = NSLOCTEXT("DeployContainers", "ImageTagText", "Container image tag");
		static const auto kImageTagDefault = NSLOCTEXT("DeployContainers", "ImageTagDefault", "unreal-gamelift-plugin");
		static const auto kECRRepoNameText = NSLOCTEXT("DeployContainers", "ECRRepoNameText", "ECR Repository");
		static const auto kECRRepoNameDefault = NSLOCTEXT("DeployContainers", "ECRRepoNameDefault", "unreal-gamelift-plugin-ecr-repository");

		// Control Bar
		static const auto kMissingDockerWarningTitle = NSLOCTEXT("DeployContainers", "MissingDockerWarningTitle", "Missing Docker");
		static const auto kMissingDockerWarningDescription = NSLOCTEXT("DeployContainers", "MissingDockerWarningDescription", "The required tools are not detected on your system. You can install the tools and continue with fleet deployment. Or you can manually build your image and push it to an ECR private repository.");
		static const auto kInstallDockerText = NSLOCTEXT("DeployContainers", "InstallDockerText", "Install Docker Engine");
		static const auto kDeploymentTimeNotification = NSLOCTEXT("DeployContainers", "DeploymentTimeNotification", "Note: Fleet deployment can take 10-20 minutes to complete. Fleet status must be \"Active\" before you can start hosting game sessions. This action creates AWS resources and incurs charges to your AWS account unless the account is eligible for AWS Free Tier benefits.");
		static const auto kAWSConsoleButtonText = NSLOCTEXT("DeployContainers", "AWSConsoleButtonText", "View in AWS Management Console");
		static const auto kDeployContainerFleetButtonText = NSLOCTEXT("DeployContainers", "DeployContainerFleetButtonText", "Deploy Container Fleet");
		static const auto kResetDeploymentButtonText = NSLOCTEXT("DeployContainers", "ResetDeploymentButtonText", "Reset Deployment");

    	// Create container image section
    	static const auto kCreateContainerImageHeader = NSLOCTEXT("DeployContainers", "CreateContainerImageHeader", "Configuring container image");
    	static const auto kCreateContainerImageDescription = TEXT("We're creating a Dockerfile based on the information you provided. The Dockerfile is a blueprint for how to construct your container image. You can view the default Dockerfile template. When ready, your custom Dockerfile is saved to your local device in the {0} directory.");
    	static const auto kWhatIsDockerLinkLabel = NSLOCTEXT("DeployContainers", "WhatIsDockerLinkLabel", "What is Docker?");
    	static const auto kDockerDocumentationLinkLabel = NSLOCTEXT("DeployContainers", "DockerDocumentationLinkLabel", "Docker Installation Guide");

    	static const auto kGameLiftAgentLabel = NSLOCTEXT("DeployContainers", "GameLiftAgentLabel", "Amazon GameLift Agent");
    	static const auto kGameLiftAgentHint = NSLOCTEXT("DeployContainers", "GameLiftAgentHint", "C:/GameLiftAgent/GameLiftAgent-1.0.jar");
    	static const auto kGameLiftAgentTooltip = NSLOCTEXT("DeployContainers", "GameLiftAgentTooltip", "Specify path to GameLift Agent jar file");

    	static const auto kCreateImageButtonLabel = NSLOCTEXT("DeployContainers", "CreateImageButtonLabel", "Create image");
    	static const auto kOpenContainersFolderButtonLabel = NSLOCTEXT("DeployContainers", "OpenContainersFolderButtonLabel", "Open containers folder");
    	static const auto kViewDockerfileTemplateButtonLabel = NSLOCTEXT("DeployContainers", "ViewDockerfileTemplateButtonLabel", "View Dockerfile template");
    	static const auto kModifyImageButtonLabel = NSLOCTEXT("DeployContainers", "ModifyImageButtonLabel", "Modify image details");
    	static const auto kUpdateImageButtonLabel = NSLOCTEXT("DeployContainers", "UpdateImageButtonLabel", "Update image");
    	static const auto kCancelButtonText = NSLOCTEXT("DeployContainers", "CancelButtonText", "Cancel");

    	static const auto kOverwriteWarning = NSLOCTEXT("DeployContainers", "OverwriteWarning", "By updating the image, any changes in the GameLiftContainers folder will be lost. Backup any changes you may have.");

    	// Setup ECR repository section
		static const auto kCreateECRRepositoryHeader = NSLOCTEXT("DeployContainers", "CreateECRRepositoryHeader", "Creating an Amazon ECR repository");
		static const auto kCreateECRRepositoryDescription = NSLOCTEXT("DeployContainers", "CreateECRRepositoryDescription", "We're setting up a new private repository to store your container image. Amazon ECR is an AWS-managed container image registry service that provides a centralized hub for securely storing, managing, and deploying your container images.");
		static const auto kSetupECRRepositoryHeader = NSLOCTEXT("DeployContainers", "SetupECRRepositoryHeader", "Setting up existing Amazon ECR Repository");
    	static const auto kSetupECRRepositoryDescription = NSLOCTEXT("DeployContainers", "SetupECRRepositoryDescription", "We're preparing to store your container image based on the existing repository information you provided. Amazon ECR is an AWS-managed container image registry service that provides a centralized hub for securely storing, managing, and deploying your container images.");
    	static const auto kLearnMoreAboutPrivateRepositoryLabel = NSLOCTEXT("DeployContainers", "LearnMoreAboutPrivateRepositoryLabel", "Amazon ECR User Guide");
    	static const auto kSetupNewECRRepositoryDescription = NSLOCTEXT("DeployContainers", "SetupNewECRRepositoryDescription", "Alternatively, you can create your ECR repository in the AWS console...don't forget to come back to this flow.");

 		// Build and push section
 		static const auto kBuildAndPushToECRHeader = NSLOCTEXT("DeployContainers", "BuildAndPushToECRHeader", "Building image and pushing to Amazon ECR");
 		static const auto kBuildAndPushToECRDescription = NSLOCTEXT("DeployContainers", "BuildAndPushToECRDescription", "We're building your Docker container image using the Dockerfile with the information you provided. When the image build is complete, we'll push it to your ECR repository. When you create a container fleet, Amazon GameLift takes a snapshot of the stored image and deploys it across all instances in the fleet. You can view and update your ECR repositories in the AWS Console.");
 		static const auto kPushToRepoButtonLabel = NSLOCTEXT("DeployContainers", "PushToRepoButtonLabel", "Push image to repository");
 		static const auto kShowCommandsButtonLabel = NSLOCTEXT("DeployContainers", "ShowCommandButtonLabel", "Show commands");
 		static const auto kContinueButtonLabel = NSLOCTEXT("DeployContainers", "ContinueButtonLabel", "Continue");
 		static const auto kResetStepButtonLabel = NSLOCTEXT("DeployContainers", "ResetStepButtonLabel", "Reset step");
    	static const auto kSetupECRRepoLabel = NSLOCTEXT("DeployContainers", "SetupECRRepoLabel", "Set up ECR repository");
    	static const auto kRepositoryNameTitle = NSLOCTEXT("DeployContainers", "RepositoryNameTitle", "Repository name");

		// Build and Push section without docker installed
		static const auto kBuildAndPushToECRDescription_CLIHelperDescriptionPart1 = NSLOCTEXT("DeployContainers", "BuildAndPushToECRDescription", "We couldn't find Docker on your system. Install these required tools to complete this step on a Windows device. Alternatively, you can manually run the CLI commands on a Linux Docker platform with your container image folder.");
		static const auto kBuildAndPushToECRDescription_CLIHelperDescriptionPart2 = NSLOCTEXT("DeployContainers", "BuildAndPushToECRDescription", "To manually push a built image to your ECR repository complete the following steps. For alternative registry authentication methods, including using Amazon ECR credential helper, see Registry authentication.");
		
		// original two desciption things
		static const auto kBuildAndPushToECRDescription_CLIHelper = NSLOCTEXT("DeployContainers", "BuildAndPushToECRDescription", "The CLI commands should be executed in your Linux Docker platform where your container folder is located.");
		static const auto kBuildAndPushToECRDescription_CLIHelper2 = NSLOCTEXT("DeployContainers", "BuildAndPushToECRDescription2", "Use the following steps to authenticate and push an image to your repository. For additional registry authentication methods, including the Amazon ECR credential helper, see ");
		
		static const auto kBuildAndPushRerunCommandInfoText = NSLOCTEXT("DeployContainers", "BuildAndPushRerunCommandInfoText", "If you have installed Docker on your system, try again to automatically build your image and push it to Amazon ECR.");
		static const auto kBuildAndPushToECRDescription_CLIHelperHeader = NSLOCTEXT("DeployContainers", "BuildAndPushToECRViewCLICommands", "View CLI Push Commands");
		static const auto kBuildAndPushToECRDescription_CLIHelperLink = NSLOCTEXT("DeployContainers", "BuildAndPushToECRDescriptionLink", "Read more about Registry authentication.");
		static const auto kBuildAndPushToECRDescription_CLIHelper_Step1 = NSLOCTEXT("DeployContainers", "BuildAndPushToECRStep1", "1. Retrieve an authentication token and authenticate your Docker client to your registry. Use the AWS CLI or CloudShell in the AWS Console:");
		static const auto kBuildAndPushToECRDescription_CLIHelper_Step2 = NSLOCTEXT("DeployContainers", "BuildAndPushToECRStep2", "2. Build your Docker image using the following command. For information on building a Docker file from scratch, see the instructions here. You can skip this step if your image has already been built:");
		static const auto kBuildAndPushToECRDescription_CLIHelper_Step2Link = NSLOCTEXT("DeployContainers", "BuildAndPushToECRStep2Link", "For information on  building a Docker file from scratch, see the instructions here.");
		static const auto kBuildAndPushToECRDescription_CLIHelper_Step3 = NSLOCTEXT("DeployContainers", "BuildAndPushToECRStep3", "3. After the build is completed, tag your image so you can push the image to this repository:");
		static const auto kBuildAndPushToECRDescription_CLIHelper_Step4 = NSLOCTEXT("DeployContainers", "BuildAndPushToECRStep4", "4. Run the following command to push this image to your newly created AWS repository:");
		static const auto kBuildAndPushToECRDescription_CLIHelper_ContinueText = NSLOCTEXT("DeployContainers", "BuildAndPushToECRContinueText", "When you've successfully pushed your container image to the ECR repo, continue.  In the next step. Amazon GameLift will try to detect the new image.");
		static const auto kBuildAndPushToECRDescription_CLIHelper_ContinueButtonText = NSLOCTEXT("DeployContainers", "BuildAndPushToECRContinueButtonText", "Continue");
    	
    	// Create Container Group Definition section
    	static const auto kSetupCgdLabel = NSLOCTEXT("DeployContainers", "SetupCgdLabel", "Set up container definition");
    	static const auto kCreateContainerGroupHeader = NSLOCTEXT("DeployContainers", "CreateContainerGroupHeader", "Creating container group definition");
    	static const auto kCreateContainerGroupDescription = NSLOCTEXT("DeployContainers", "CreateContainerGroupDescription", "We're creating a container group definition for you, based on the information you provided. A container group definition describes how to deploy your container on each compute instance in a container fleet. Amazon GameLift uses container groups to manage sets of containers and distribute computing resources among them.");
		static const auto kContainerGroupDefinitionStatusTitle = NSLOCTEXT("DeployContainers", "ContainerGroupDefinitionStatusTitle", "Group definition status");
		static const auto kContainerGroupDefinitionNameTitle = NSLOCTEXT("DeployContainers", "ContainerGroupDefinitionNameTitle", "Group definition name");
		static const auto kUnselecedCgdDropdownText = NSLOCTEXT("DeployContainers", "UnselecedUnselecedCgdDropdownText", "Choose a container group definition");
		static const auto kContainerGroupDefinitionVersionTitle = NSLOCTEXT("DeployContainers", "ContainerGroupDefinitionVersionTitle", "Group definition version");
    	static const auto kHelpfulResourcesHeader = NSLOCTEXT("DeployContainers", "HelpfulResourcesHeader", "Helpful Resources");
		static const auto kCgdStatusFailedErrorMessage = NSLOCTEXT("DeployContainers", "CgdStatusFailedErrorMessage", "Container group definition entered FAILED status.");
		static const auto kTimedOutPollingCgdStatusErrorMessage = NSLOCTEXT("DeployContainers", "TimedOutPollingCgdStatusErrorMessage", "Timed out polling for container group definition status. For more information, see `Output Log`");

		// Cgd status for Create Container Group section
		static const auto kReady = NSLOCTEXT("DeployContainers", "ContainerGroupDefinitionStatus", "Ready");
		static const auto kCopying = NSLOCTEXT("DeployContainers", "ContainerGroupDefinitionStatus", "Copying");
		static const auto kFailed = NSLOCTEXT("DeployContainers", "ContainerGroupDefinitionStatus", "Failed");

		// Create Container Fleet section
		static const auto kContainerFleetIdTitle = NSLOCTEXT("DeployContainers", "ContainerFleetIdTitle", "Fleet ID");
		static const auto kContainerFleetStatusTitle = NSLOCTEXT("DeployContainers", "ContainerFleetStatusTitle", "Fleet status");
		static const auto kContainerFleetTypeTitle = NSLOCTEXT("DeployContainers", "ContainerFleetTypeTitle", "Fleet type");
		static const auto kContainerInstanceTypeTitle = NSLOCTEXT("DeployContainers", "ContainerInstanceTypeTitle", "Instance type");
		static const auto kCreateContainerFleetHeader = NSLOCTEXT("DeployContainers", "CreateContainerFleetHeader", "Creating a container fleet");
		static const auto kTimedOutPollingDeploymentStatusErrorMessage = NSLOCTEXT("DeployContainers", "TimedOutPollingDeploymentStatusErrorMessage", "Timed out polling for deployment status. For more information, see `Output Log`");
		static const auto kCreateContainerFleetDescription = NSLOCTEXT("DeployContainers", "CreateContainerFleetDescription", "We're deploying your container image to fleets of EC2 computing instances, based on the deployment information you provided. Initially, Amazon GameLift deploys one EC2 instance in each fleet, generating events and updating fleet status in real time.");
		static const auto kCreateContainerFleetDescriptionPart2 = NSLOCTEXT("DeployContainers", "CreateContainerFleetDescriptionPart2", "Use the Amazon GameLift console to monitor status and adjust the fleet's hosting capacity as needed.");

		// Fleet status for Container Fleet section
		static const auto kNoDeployment = NSLOCTEXT("DeployContainers", "ContainerFleetStatus", "Not created");
		static const auto kInProgressDeployment = NSLOCTEXT("DeployContainers", "ContainerFleetStatus", "Creating");
		static const auto kActiveDeployment = NSLOCTEXT("DeployContainers", "ContainerFleetStatus", "Active");
		static const auto kFailedDeployment = NSLOCTEXT("DeployContainers", "ContainerFleetStatus", "Failed");

		// Update fleet deployment
		static const auto kUpdateButtonText = NSLOCTEXT("DeployContainers", "UpdateButtonText", "Update");
		static const auto kUpdateFleetDeploymentTitle = NSLOCTEXT("DeployContainers", "UpdateFleetDeploymentTitle", "Update Fleet Deployment");
		static const auto kUpdateFleetDeploymentDescription = NSLOCTEXT("DeployContainers", "UpdateFleetDeploymentDescription", "You're about to modify the configuration of your fleet deployment. Adjust the following details as needed and click \"Update\" to apply the changes. \nThis action might create AWS resources and incur charges to your AWS account unless the account is eligible for AWS Free Tier benefits.");
		static const auto kUpdateWarningText = NSLOCTEXT("DeployContainers", "UpdateFleetDeploymentDescription", "Changing the configuration will restart the deployment, and any active game sessions will be terminated.");
		static const auto kDeploymentDetailsTitle = NSLOCTEXT("DeployContainers", "DeploymentDetailsTitle", "Deployment Details");

		// Reset fleet deployment
		static const auto kResetFleetDeploymentTitle = NSLOCTEXT("DeployContainers", "ResetFleetDeploymentTitle", "Reset Fleet Deployment");
		static const auto kResetButtonText = NSLOCTEXT("DeployContainers", "ResetButtonText", "Reset");
		static const auto kResetFleetDeploymentDescription = NSLOCTEXT("DeployContainers", "ResetFleetDeploymentDescription", "You're about to reset your configuration for a managed container fleet. Before you proceed, we recommend that you clean up the resources from your last deployment, if any were created.");
		static const auto kResetWarningText = NSLOCTEXT("DeployContainers", "ResetWarningText", "You can delete the deployed resource stack from the AWS CloudFormation console. Look for a stack name with the following pattern: gamelift-dev-{GameName}-Containers-main");
		static const auto kResetFleetDeploymentExplanantion = NSLOCTEXT("DeployContainers", "ResetFleetDeploymentExplanantion", "This action cannot be undone. You will lose all your progress, including any settings or configurations made so far.");

		//cfn
		static const auto kDeployCloudFormationButtonText = NSLOCTEXT("DeployContainers", "DeployCloudFormationButtonText", "Deployment cfn template");
		static const auto kDeploymentStartedAwsScenarioNotification = TEXT("Deploying Amazon GameLift scenario...");
		static const auto kDeploymentFailedMessage = NSLOCTEXT("DeployContainers", "DeploymentFailedMessage", "Deployment failed. For more information, view stack details.");
		static const auto kDeploymentSucceed = TEXT("Deployment succeed");
		static const auto kDeploymentFailed = TEXT("Deployment failed");
		static const auto kDeploymentCompletedNotification = TEXT("Amazon GameLift deployment is completed.");
		static const auto kDeploymentFailedNotification = TEXT("Amazon GameLift deployment is failed.");

		static const auto kRetryButtonLabel = NSLOCTEXT("DeployContainers", "RetryButtonLabel", "Try again");
		static const auto kViewLogButtonText = NSLOCTEXT("DeployContainers", "ViewLogButtonText", "View log");
	}  // namespace DeployContainers

	static const auto kErrorUserMessageWithLog = NSLOCTEXT("FGameLiftPluginModule", "ErrorUserMessageWithLog", "For more information, see `Output Log`");
	static const auto kErrorUserMessageNoLog = NSLOCTEXT("FGameLiftPluginModule", "ErrorUserMessageNoLog", "Unknown failure, for more information, see `Output Log`. \nError code is:");
} // namespace Menu

namespace Tabs
{
	static const auto kDeployAnywhereTabTitle = NSLOCTEXT("FGameLiftDeployAnywhereTab", "FGameLiftDeployAnywhereTab_Title", "Host with Anywhere");
	static const auto kDeployAnywhereTabTooltip = NSLOCTEXT("FGameLiftDeployAnywhereTab", "FGameLiftDeployAnywhereTab_Tooltip", "Deploy the game with Amazon GameLift Anywhere.");

	static const auto kDeployManagedEC2TabTitle = NSLOCTEXT("FGameLiftDeployManagedEC2Tab", "FGameLiftDeployManagedEC2Tab_Title", "Host with Managed EC2");
	static const auto kDeployManagedEC2TabTooltip = NSLOCTEXT("FGameLiftDeployManagedEC2Tab", "FGameLiftDeployManagedEC2Tab_Tooltip", "Deploy the game with Amazon GameLift on a managed EC2 instance.");

	static const auto kDeployContainersTabTitle = NSLOCTEXT("FGameLiftDeployContainersTab", "FGameLiftDeployContainersTab_Title", "Host with Managed Containers");
	static const auto kDeployContainersTabTooltip = NSLOCTEXT("FGameLiftDeployContainersTab", "FGameLiftDeployContainersTab_Tooltip", "Deploy the game with Amazon GameLift on a Container instance.");
} // namespace Tabs

namespace Defaults
{
	static const auto kDefaultS3Bucket = "";
} // namespace Defaults

namespace Style
{
	static const FName kGameLiftPluginStyleName = "GameLiftPluginStyle";

	constexpr auto KPrimaryWidthOverride = 220.0f;
	constexpr auto kSecondaryWidthOverride = 0.0f;  // Stretch to fill the whole column

	static const auto kLinkIconSize = 12;
	static const auto kStatusIconSize = 12;
	static const auto kTooltipIconSize = 12;
	static const auto kLoadingIconSize = 16;
	static const auto kBannerIconSize = 16;
	static const auto kCopyPasteIconSize = 14;
	static const auto kResetIconSize = 16;
	static const auto kStartIconSize = 12;

	static const auto kContainersContinueButtonHeight = 20;
	static const auto kContainersContinueButtonWidth = 65;

	static const auto kMessageTextBoxWidth = 900;
	static const auto kDocumentationIconWidth = 100;
	static const auto kWhitePaperIconWidth = 80;
	static const auto kHelpfulResourceIconHeight = 20;
	static const auto kHelpfulResourceCardWidth = 310;

	namespace Brush
	{
		static const auto kFolderOpenIconName = "FolderOpenIcon";
		static const auto kGameLiftTabIconName = "GameLiftTabIcon";
		static const auto kGameLiftContainersTabIconName = "GameLiftContainersTabIcon";
		static const auto kGameLiftToolbarIconName = "GameLiftToolbarIcon";
		static const auto kTooltipIconName = "TooltipIcon";
		static const auto kBorderImageName = "BorderImage";

		static const auto kExternalLinkIconName = "ExternalLinkIcon";
		static const auto kStatusSuccessIconName = "StatusSuccessIcon";
		static const auto kStatusWarningIconName = "StatusWarningIcon";
		static const auto kStatusErrorIconName = "StatusErrorIcon";
		static const auto kStatusInactiveIconName = "StatusInactiveIcon";
		static const auto kStatusInfoIconName = "StatusInfoIcon";
		static const auto kStatusHelpIconName = "StatusHelpIcon";
		static const auto kCopyPasteIconName = "CopyPasteIcon";
		static const auto kResetIconName = "ResetIcon";
		static const auto kStartIconName = "StartIcon";

		static const auto kAccountIconName = "AccountIcon";
		static const auto kAccountMissingIconName = "AccountMissingIcon";

		static const auto kGameLiftLogoIconName = "GameLiftLogoIcon";
		static const auto kGameLiftLogoIconSize = FVector2D(67, 51);

		static const auto kGameLiftAnywhereIconName = "GameLiftAnywhereIcon";
		static const auto kGameLiftManagedEC2IconName = "GameLiftManagedEC2Icon";

		static const auto kGameLiftDivider = "GameLiftDivider";
		static const auto kDocumentationIcon = "DocumentationIcon";
		static const auto kGuidanceIcon = "GuidanceIcon";
		static const auto kWhitepaperIcon = "WhitepaperIcon";

		// Progress bar
		static const auto kProgressCompleteIconName = "ProgressCompleteIcon";
		static const auto kProgressCompleteBarName = "ProgressCompleteBar";
		static const auto kProgressIncompleteIconName = "ProgressIncompleteIcon";
		static const auto kProgressIncompleteBarName = "ProgressIncompleteBar";
		static const auto kInProgressIconName = "InProgressIcon";
		static const auto kProgressWarningIconName = "ProgressWarningIcon";
		static const auto kProgressErrorIconName = "ProgressErrorIcon";
	}

	namespace Font
	{
		const int SectionHeaderFontSize = 9;
		const int ParagraphFontSize = 10;
		const int FieldFontSize = 10;
		const int ButtonFontSize = 10;

		const int NoteFontSize = 9;
		const int LinkFontSize = 8;

		static const auto kTitleFontSize = 16;
		static const auto kSubtitleFontSize = 13;
	}

	namespace Text
	{
		static const auto kMessageLog = "MessageLog";

		static const auto kParagraph = "ParagraphTextStyle";
		static const auto kField = "FieldTextStyle";
		static const auto kFieldMedium = "FieldMediumTextStyle";
		static const auto kFieldBold = "FieldBoldTextStyle";
		static const auto kFieldItalic = "FieldItalicTextStyle";
		static const auto kNote = "NoteTextStyle";
		static const auto kNoteLight = "NoteLightTextStyle";
		static const auto kNoteItalic = "NoteItalicTextStyle";
		static const auto kSelectionDescription = "SelectionDescriptionTextStyle";

		static const auto kTitleStyleName = "TitleTextStyle";
		static const auto kSubtitleStyleName = "SubtitleTextStyle";

		static const auto kButtonNormal = "ButtonNormalTextStyle";
		static const auto kButtonLight = "ButtonLightTextStyle";

		static const auto kSectionHeaderFontStyleName = "SectionHeaderTextStyle";
	}

	namespace EditableText
	{
		static const auto kParagraph = "ParagraphEditableTextStyle";
		static const auto kField = "FieldEditableTextStyle";
		static const auto kFieldMedium = "FieldMediumEditableTextStyle";
		static const auto kFieldBold = "FieldBoldEditableTextStyle";
	}

	namespace EditableTextBox
	{
		static const auto kField = "FieldEditableTextBoxStyle";
	}

	namespace Button
	{
		static const auto kFlatButtonStyleName = "FlatButton";

		static const auto kNormalButtonStyleName = "NormalButton";
		static const auto kSuccessButtonStyleName = "SuccessButton";
		static const auto kErrorButtonStyleName = "ErrorButton";
		static const auto kCloseButtonStyleName = "CloseButton";
		static const auto kLaunchBarNormalButtonStyle = "LaunchBarNormalButton";
		static const auto kLaunchBarSuccessButtonStyle = "LaunchBarSuccessButton";

		static const auto kVisibilityButtonWidth = 70;
	}

	namespace Color
	{
		static const auto kCredentialSeparator = "CredentialSeparator";
		static const auto kTableRowBackground = "TableRowBackground";
		static const auto kTableHeaderBackground = "TableHeaderBackground";
		static const auto kTableSeparator = "TableSeparator";

		static const auto kSectionHeaderBackground = "SectionHeaderBackground";
		static const auto kPanelBackground = "PanelBackgroundColor";

		static const auto kCardBackground = "CardBackground";

		static const auto kTogglerBackground = "TogglerBackground";
		static const auto kTogglerBorder = "TogglerBorder";

		static const auto kSuccessForeground = "SuccessForegroundColor";
		static const auto kSuccessBackground = "SuccessBackgrounColor";
		static const auto kWarningForeground = "WarningForegroundColor";
		static const auto kWarningBackground = "WarningBackgroundColor";
		static const auto kErrorForeground = "ErrorForegroundColor";
		static const auto kErrorBackground = "ErrorBackgroundColor";
		static const auto kInfoForeground = "InfoForeground";
		static const auto kInfoBackground = "InfoBackground";
		static const auto kHelpForeground = "HelpForeground";
		static const auto kHelpBackground = "HelpBackground";

		static const auto kInactive = "InactiveColor";
		static const auto kSuccess = "SuccessColor";
		static const auto kWarning = "WarningColor";
		static const auto kError = "ErrorColor";
		static const auto kLoading = "LoadingColor";

		static const auto kTooltip = "TooltipColor";

		// Button colors
		static const auto kSuccessButtonNormal = "SuccessButtonNormalColor";
		static const auto kSuccessButtonHovered = "SuccessButtonHoveredColor";
		static const auto kSuccessButtonPressed = "SuccessButtonPressedColor";

		static const auto kErrorButtonNormal = "ErrorButtonNormalColor";
		static const auto kErrorButtonHovered = "ErrorButtonHoveredColor";
		static const auto kErrorButtonPressed = "ErrorButtonPressedColor";

		static const auto kDisabledButton = "DisabledButtonColor";
	}

	static const auto kLinkStyleName = "LinkStyle";
} // namespace Style

namespace SPadding
{
	// Use these constants for padding widgets to avoid random numbers being used in the code base.

	static const auto Left = FMargin(5, 0, 0, 0);
	static const auto Right = FMargin(0, 0, 5, 0);
	static const auto Top = FMargin(0, 5, 0, 0);
	static const auto Bottom = FMargin(0, 0, 0, 5);

	static const auto Left2x = FMargin(10, 0, 0, 0);
	static const auto Right2x = FMargin(0, 0, 10, 0);
	static const auto Top2x = FMargin(0, 10, 0, 0);
	static const auto Bottom2x = FMargin(0, 0, 0, 10);

	static const auto Left3x = FMargin(15, 0, 0, 0);
	static const auto Right3x = FMargin(0, 0, 15, 0);
	static const auto Top3x = FMargin(0, 15, 0, 0);
	static const auto Bottom3x = FMargin(0, 0, 0, 15);

	static const auto Left4x = FMargin(20, 0, 0, 0);
	static const auto Right4x = FMargin(0, 0, 20, 0);
	static const auto Top4x = FMargin(0, 20, 0, 0);
	static const auto Bottom4x = FMargin(0, 0, 0, 20);

	static const auto Left5x = FMargin(25, 0, 0, 0);
	static const auto Top5x = FMargin(0, 25, 0, 0);
	static const auto Bottom5x = FMargin(0, 0, 0, 25);

	static const auto Top_Bottom = FMargin(0, 5, 0, 5);
	static const auto Top2x_Bottom = FMargin(0, 10, 0, 5);
	static const auto Top_Bottom2x = FMargin(0, 5, 0, 10);
	static const auto Top2x_Bottom2x = FMargin(0, 10, 0, 10);
	static const auto Top5x_Bottom5x = FMargin(0, 25, 0, 25);

	static const auto Left_Right = FMargin(5, 0, 5, 0);
	static const auto Left2x_Right = FMargin(10, 0, 5, 0);
	static const auto Left_Right2x = FMargin(5, 0, 10, 0);
	static const auto Left2x_Right2x = FMargin(10, 0, 10, 0);
	static const auto Left5x_Right5x = FMargin(25, 0, 25, 0);

	static const auto All = FMargin(5);
	static const auto All2x = FMargin(10);

	static const auto BannerContentAll = FMargin(8);
	static const auto Border = FMargin(2);
	static const auto Border_Bottom = FMargin(0, 0, 0, 1);
	static const auto Border_Top = FMargin(0, 2, 0, 0);

	static const auto ProgressBarPadding = FMargin(8, 0, 8, 0);
	static const auto SectionTitleTopPadding = FMargin(0, -4, 0, 0);
	static const auto SectionWithHeaderLeftPadding = FMargin(-20, 0, 0, 0);

	static const auto SetupMessageIconPadding = FMargin(8);
	static const auto SetupMessageContentPadding = FMargin(0, 8, 8, 8);

	static const auto CloseButtonTopPadding = FMargin(8);

	static const auto LaunchBarPadding = FMargin(18, 12, 18, 12);
	static const auto LaunchBarButtonPadding = FMargin(8, 4, 8, 4);
	static const auto LaunchBarCheckboxPadding = FMargin(8, 0, 0, 0);

	static const auto Extra_For_Page_End = FMargin(0);

	static const auto ContentAreaLeftIndent = SPadding::Left * 6;
} // namespace SPadding

namespace Url
{
	// AWS Authentication
	static const auto kUserCredentialsUrl = TEXT("https://docs.aws.amazon.com/cli/latest/userguide/cli-authentication-user.html");
	static const auto kIAMPermissionsUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/gamelift-iam-policy-examples.html");

	// Additional Resources
	static const auto kGameLiftDocumentationUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/gamelift-intro.html");
	static const auto kGameTechForumUrl = TEXT("https://repost.aws/tags/TAF8-XUqojTsadH5jSz3IfGQ/amazon-gamelift");

	static const auto kHelpSetupAWSAccountLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/setting-up-aws-login.html ");
	static const auto kHelpPluginGuideLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-profiles.html");
	static const auto kHelpGettingStartedLinkUrl = TEXT("https://docs.aws.amazon.com/accounts/latest/reference/getting-started.html");
	static const auto kHelpOrganizingEnvironmentLinkUrl = TEXT("https://docs.aws.amazon.com/whitepapers/latest/organizing-your-aws-environment/organizing-your-aws-environment.html");

	static const auto kHelpGetAccessKeyLinkUrl = TEXT("https://docs.aws.amazon.com/cli/latest/userguide/cli-authentication-user.html#cli-authentication-user-get");
	static const auto kHelpServiceLocationsLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/gamelift-regions.html#gamelift-regions-flex");
	static const auto kHelpManageAccessKeysLinkUrl = TEXT("https://docs.aws.amazon.com/IAM/latest/UserGuide/id_credentials_access-keys.html#Using_CreateAccessKey");

	static const auto kHelpGameLiftHostingSolutionsLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/gamelift-intro-flavors.html#gamelift-intro-flavors-hosting");
	static const auto kHelpSettingupHostingFleetLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/fleets-intro.html");

	static const auto kHelpDeployContainerLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-container.html");
	static const auto kHelpDeployEC2LinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-ec2.html");
	static const auto kHelpDeployAnywhereLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-anywhere.html");

	static const auto kWhatIsDockerLinkUrl = TEXT("https://www.docker.com/");
	static const auto kDockerInstallGuideLinkUrl = TEXT("https://docs.docker.com/engine/install/");
	static const auto kAmazonECRUserGuideLinkUrl = TEXT("https://docs.aws.amazon.com/AmazonECR/latest/userguide/what-is-ecr.html");

	// Unreal Github
	static const auto kUnrealSourceUrl = TEXT("https://www.unrealengine.com/en-US/ue-on-github");

	// Context Menu
	static const auto kOpenGameLiftDocumentationUrl = kGameLiftDocumentationUrl;
	static const auto kOpenAwsGameTechForumUrl = kGameTechForumUrl;
	static const auto kReportIssuesUrl = TEXT("https://github.com/amazon-gamelift/amazon-gamelift-plugin-unreal/issues");
	static const auto kOpenServiceAPIReferenceUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/apireference/Welcome.html");
	static const auto kOpenServerSDKReferenceUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/integration-server-sdk5-cpp.html");

	// Other
	static const auto kGameLiftHowToDeployYourFirstGameUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-ec2-scenarios.html");
	static const auto kGameLiftChooseLocationsUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/gamelift-regions.html");
	static const auto kGameLiftPricingPlanUrl = TEXT("https://aws.amazon.com/gamelift/pricing/");
	static const auto kAwsAccountCreateAwsAccountUrl = TEXT("https://repost.aws/knowledge-center/create-and-activate-aws-account");

	// Deployment scenarios
	static const auto kSingleRegionLearnMoreUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-ec2.html#unreal-plugin-ec2-scenarios");
	static const auto kFlexMatchLearnMoreUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-ec2.html#unreal-plugin-ec2-scenarios");

	// Integrate game help
	static const auto kHowToIntegrateYourGameForAnywhereUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-anywhere.html#unreal-plugin-anywhere-integrate");
	static const auto kHowToIntegrateYourGameForEC2Url = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-ec2.html#unreal-plugin-ec2-integrate");

	// ECR Help
	static const auto kECRRegistryAuthenticationUrl = TEXT("https://docs.aws.amazon.com/AmazonECR/latest/userguide/Registries.html#registry_auth");

	// ECS Help
	static const auto kECSCreateContainerImageUrl = TEXT("https://docs.aws.amazon.com/AmazonECS/latest/developerguide/create-container-image.html");

	// Control Bar Links
	static const auto kDocumentationLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-container.html");
	static const auto kAWSConsoleUrlContainerFleetFormat = TEXT("https://{0}.console.aws.amazon.com/gamelift/container-fleets");
	static const auto kInstallDockerUrl = TEXT("https://docs.docker.com/desktop/install/windows-install/");

	// Update Container fleet deployment
	static const auto kLearnMoreAboutContainerFleetUpdateLinkUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/containers-update-fleet.html");

	// View supported regions
	static const auto kContainerSupportedRegionsUrl = TEXT("https://docs.aws.amazon.com/gamelift/latest/developerguide/gamelift-regions.html#gamelift-regions-hosting-home");

} // namespace Url
