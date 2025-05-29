// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SGameLiftDeployAnywhereMenu.h"

#include <Widgets/SBoxPanel.h>
#include <Widgets/Layout/SSeparator.h>
#include <Framework/MetaData/DriverMetaData.h>
#include <Async/Async.h>

#include "SAnywhereGenerateAuthTokenMenu.h"
#include "SAnywhereRegisterComputeMenu.h"
#include "SMenu/SCommonMenuSections.h"
#include "SConnectAnywhereFleetMenu.h"
#include "SWidgets/SPathInput.h"
#include "SWidgets/STextStatus.h"
#include "SWidgets/SOnlineHyperlink.h"
#include "SWidgets/SSetupMessage.h"
#include "SWidgets/SBootstrapStatus.h"
#include "SWidgets/SLaunchBar.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SSectionsWithProgressBars.h"

#include "IGameLiftAnywhereHandler.h"
#include "IGameLiftCoreModule.h"
#include "GameLiftPlugin.h"
#include "GameLiftPluginStyle.h"
#include <Developer/Settings/Public/ISettingsModule.h>

#include "Settings/UGameLiftAnywhereStatus.h"
#include "Settings/UGameLiftSettings.h"
#include "Utils/Misc.h"
#include "Utils/Notifier.h"

#define LOCTEXT_NAMESPACE "SGameLiftDeploymentAnywhereMenu"

void SGameLiftDeployAnywhereMenu::Construct(const FArguments& InArgs)
{
	ContextWindow = InArgs._ContextWindow;
	
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);
	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Left5x_Right5x + SPadding::Top5x_Bottom5x)
		[
			SNew(SSetProfileSection).ReadDeveloperGuideLink(Url::kHelpDeployAnywhereLinkUrl)
		];

	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
		[
			CreateDivider()
		];

	// Create the widgets
	TSharedPtr<SSectionsWithProgressBars> SectionsWithProgressBars = SNew(SSectionsWithProgressBars);
	SectionsWithProgressBars->AddSection(CreateIntegrateGameSection());
	SectionsWithProgressBars->AddSection(CreateConnectToFleetSection());
	SectionsWithProgressBars->AddSection(CreateRegisterComputeSection());

	// Remove optional section from progress bar but keep it for internal auth token generation
	CreateAuthTokenSection();

	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(SPadding::Left5x_Right5x + SPadding::Extra_For_Page_End) // This adds more space at the bottom so users can scroll down further.
		[
			SectionsWithProgressBars.ToSharedRef()
		];

	VerticalBox->AddSlot()
		.AutoHeight()
		[
			CreateLaunchBar()
		];

	ChildSlot
		[
			VerticalBox
		];
}

TSharedRef<SSectionStep> SGameLiftDeployAnywhereMenu::CreateIntegrateGameSection()
{
	return SAssignNew(IntegrateGameSection, SIntegrateGameSection)
		.HowToIntegrateYourGameLinkUrl(Url::kHowToIntegrateYourGameForAnywhereUrl);
}

TSharedRef<SSectionStep> SGameLiftDeployAnywhereMenu::CreateConnectToFleetSection()
{
	return SAssignNew(AnywhereConnectFleetSection, SConnectAnywhereFleetMenu)
		.OnFleetChangedDelegate(this, &SGameLiftDeployAnywhereMenu::OnFleetChanged);
}

TSharedRef<SSectionStep> SGameLiftDeployAnywhereMenu::CreateRegisterComputeSection()
{
	return SAssignNew(AnywhereRegisterComputeSection, SAnywhereRegisterComputeMenu)
		.OnComputeChangedDelegate(this, &SGameLiftDeployAnywhereMenu::OnComputeChanged);
}

TSharedRef<SSectionStep> SGameLiftDeployAnywhereMenu::CreateAuthTokenSection()
{
	return SAssignNew(AnywhereGenerateAuthTokenSection, SAnywhereGenerateAuthTokenMenu);
}

TSharedRef<SLaunchBar> SGameLiftDeployAnywhereMenu::CreateLaunchBar()
{
	UGameLiftAnywhereStatus* AnywhereStatus = GetMutableDefault<UGameLiftAnywhereStatus>();
	FString GameClientPath = AnywhereStatus->PathToClientBuild;
	FString GameClientLauncherArguments = AnywhereStatus->ClientBuildLauncherArguments;

	return SAssignNew(LaunchBar, SLaunchBar)
		.MenuType(EMenuType::Anywhere)
		.ParentWidget(this->AsWeak())
		.IsEnabled_Raw(this, &SGameLiftDeployAnywhereMenu::CanLaunchServer)
		.DefaultClientBuildExecutablePath(GameClientPath)
		.DefaultClientBuildLauncherArguments(GameClientLauncherArguments)
		.IsStartServerButtonEnabled_Raw(this, &SGameLiftDeployAnywhereMenu::CanLaunchServer)
		.IsStartClientButtonEnabled_Raw(this, &SGameLiftDeployAnywhereMenu::CanLaunchClient)
		.OnStartClientButtonClicked(FStartClient::CreateRaw(this, &SGameLiftDeployAnywhereMenu::OnLaunchClientButtonClicked))
		.OnStartServerButtonClicked_Raw(this, &SGameLiftDeployAnywhereMenu::OnLaunchServerButtonClicked);
}

bool SGameLiftDeployAnywhereMenu::CanLaunchServer() const
{
	UGameLiftAnywhereStatus* AnywhereStatus = GetMutableDefault<UGameLiftAnywhereStatus>();
	return AnywhereStatus->IsAnywhereReady() && !IsLaunchingGameServer && !IntegrateGameSection->GetGameServerPath().IsEmpty();
}

bool SGameLiftDeployAnywhereMenu::CanLaunchClient() const
{
	UGameLiftAnywhereStatus* AnywhereStatus = GetMutableDefault<UGameLiftAnywhereStatus>();
	return AnywhereStatus->IsAnywhereReady() && !IsLaunchingGameClient && HasLaunchedGameServer;
}

void SGameLiftDeployAnywhereMenu::UpdateLaunchServerButtonState()
{
	if (IsLaunchingGameServer)
	{
		LaunchBar->ChangeStartServerButtonUIState(SLaunchBar::EStartButtonUIState::Loading);
	}
	else
	{
		LaunchBar->ChangeStartServerButtonUIState(SLaunchBar::EStartButtonUIState::Start);
	}
}

void SGameLiftDeployAnywhereMenu::UpdateLaunchClientButtonState()
{
	if (IsLaunchingGameClient)
	{
		LaunchBar->ChangeStartClientButtonUIState(SLaunchBar::EStartButtonUIState::Loading);
	}
	else
	{
		LaunchBar->ChangeStartClientButtonUIState(SLaunchBar::EStartButtonUIState::Start);
	}
}

void SGameLiftDeployAnywhereMenu::OnFleetChanged()
{
	AnywhereRegisterComputeSection->InvalidateCompute();
	AnywhereGenerateAuthTokenSection->InvalidateAuthToken();
}

void SGameLiftDeployAnywhereMenu::OnComputeChanged()
{
	AnywhereGenerateAuthTokenSection->InvalidateAuthToken();
}

void SGameLiftDeployAnywhereMenu::OnLaunchServerButtonClicked()
{
	FString GameServerPath = IntegrateGameSection->GetGameServerPath();

	IsLaunchingGameServer = true;
	UpdateLaunchServerButtonState();

	Async(EAsyncExecution::Thread,
		[this, GameServerPath = MoveTemp(GameServerPath)]
		{
			bool bGenerateResult = AnywhereGenerateAuthTokenSection->GenerateAuthTokenSync();
			HasLaunchedGameServer = false;

			if (!bGenerateResult)
			{
				IsLaunchingGameServer = false;

				UE_LOG(GameLiftPluginLog, Error, TEXT("Launch aborted. Failed to generate auth token when launching server."));
				return;
			}

			UGameLiftAnywhereStatus* AnywhereStatus = GetMutableDefault<UGameLiftAnywhereStatus>();

			TArray<FString> LaunchParameters;
			LaunchParameters.Add(Menu::DeployAnywhere::kGameServerLaunchParameterLog);
			LaunchParameters.Add(Menu::DeployAnywhere::kGameServerLaunchParameterEnableAnywhere);
			LaunchParameters.Add(FString::Format(Menu::DeployAnywhere::kGameServerLaunchParameterFleetId, { *AnywhereStatus->FleetId }));
			LaunchParameters.Add(FString::Format(Menu::DeployAnywhere::kGameServerLaunchParameterHostId, { *AnywhereStatus->ComputeName }));
			LaunchParameters.Add(FString::Format(Menu::DeployAnywhere::kGameServerLaunchParameterAuthToken, { *AnywhereStatus->AuthToken }));

			FString WebSocketUrl = FString::Format(Menu::DeployAnywhere::kGameServerWebSocketUrlFormat, { *AnywhereStatus->DeployedRegion });
			LaunchParameters.Add(FString::Format(Menu::DeployAnywhere::kGameServerLaunchParameterWebSocketUrl, { *WebSocketUrl }));

			auto Runner = IGameLiftCoreModule::Get().MakeRunner();
			bool bIsLaunched = Runner->LaunchProcess(GameServerPath, LaunchParameters);

			if (bIsLaunched)
			{
				UE_LOG(GameLiftPluginLog, Log, TEXT("Launched game server: %s"), *GameServerPath);
				HasLaunchedGameServer = true;
			}
			else
			{
				UE_LOG(GameLiftPluginLog, Error, TEXT("Failed to launch game server: %s"), *GameServerPath);
			}

			IsLaunchingGameServer = false;

			Async(EAsyncExecution::TaskGraphMainThread, [this] {
				UpdateLaunchServerButtonState(); 
			});
		});
}

void SGameLiftDeployAnywhereMenu::OnLaunchClientButtonClicked(FString GameClientPath, FString LauncherArgs)
{
	UGameLiftAnywhereStatus* AnywhereStatus = GetMutableDefault<UGameLiftAnywhereStatus>();
	AnywhereStatus->PathToClientBuild = GameClientPath;
	AnywhereStatus->ClientBuildLauncherArguments = LauncherArgs;
	AnywhereStatus->SaveConfig();

	IsLaunchingGameClient = true;
	UpdateLaunchClientButtonState();

	Async(EAsyncExecution::Thread, [this, GameClientPath, LauncherArgs]
		{
			UGameLiftAnywhereStatus* AnywhereStatus = GetMutableDefault<UGameLiftAnywhereStatus>();
			UGameLiftSettings* Settings = GetMutableDefault<UGameLiftSettings>();

			TArray<FString> Args{ LauncherArgs };
			Args.Add(Menu::DeployAnywhere::kGameClientLaunchParameterWindowMode);

			// Pass credential profile name to the client build so the client can set it to call Amazon GameLift APIs.
			Args.Add(FString::Format(Menu::DeployAnywhere::kGameClientLaunchParameterCredentialsName, { *Settings->CurrentProfileName }));
			Args.Add(FString::Format(Menu::DeployAnywhere::kGameClientLaunchParameterFleetId, { *AnywhereStatus->FleetId }));
			Args.Add(FString::Format(Menu::DeployAnywhere::kGameClientLaunchParameterCustomLocation, { *AnywhereStatus->CustomLocation }));

			auto Runner = IGameLiftCoreModule::Get().MakeRunner();
			bool bIsLaunched = Runner->LaunchProcess(AnywhereStatus->PathToClientBuild, Args);

			IsLaunchingGameClient = false;

			// Added delay for loading button visibility
			FPlatformProcess::Sleep(Menu::DeployCommon::kStartClientLoadingButtonDelay);

			Async(EAsyncExecution::TaskGraphMainThread, [this, bIsLaunched, GameClientPath] {

				if (bIsLaunched)
				{
					UE_LOG(GameLiftPluginLog, Log, TEXT("Launched game client: %s"), *GameClientPath);
				}
				else
				{
					UE_LOG(GameLiftPluginLog, Error, TEXT("Failed to launch game client: %s. As an alternative, try launching the client manually."), *GameClientPath);
					Notifier::ShowFailedNotification(Menu::DeployCommon::kLaunchedGameClientFailureMessage);
				}

				UpdateLaunchClientButtonState();
			});
		});
}

TSharedRef<SWidget> SGameLiftDeployAnywhereMenu::CreateDivider()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNew(SImage).Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kGameLiftDivider))
		];
}

#undef LOCTEXT_NAMESPACE
