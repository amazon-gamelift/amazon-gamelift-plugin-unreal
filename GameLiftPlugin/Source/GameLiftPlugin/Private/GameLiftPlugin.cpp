// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "GameLiftPlugin.h"
#include "GameLiftPluginStyle.h"
#include "GameLiftPluginCommands.h"
#include "GameLiftPluginContext.h"

#include <Misc/MessageDialog.h>
#include <ToolMenus.h>
#include <LevelEditor.h>
#include <Developer/Settings/Public/ISettingsModule.h>

#include "Tabs/FGameLiftDeployAnywhereTab.h"
#include "Tabs/FGameLiftDeployManagedEC2Tab.h"
#include "Tabs/FGameLiftDeployContainersTab.h"

#include "Settings/UGameLiftSettings.h"
#include "Settings/UGameLiftDeploymentStatus.h"
#include "Settings/UGameLiftAnywhereStatus.h"
#include "Settings/UGameLiftContainersStatus.h"
#include "Settings/FGameLiftSettingsCustomization.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"

#include "Types/EBootstrapMessageState.h"
#include "Types/EDeploymentMessageState.h"

#include "Utils/Misc.h"

#include "GameLiftPluginConstants.h"

#include "IGameLiftCoreModule.h"

DEFINE_LOG_CATEGORY(GameLiftPluginLog);

#define LOCTEXT_NAMESPACE "FGameLiftPluginModule"

FGameLiftPluginModule::FGameLiftPluginModule():
	DeployAnywhereTab(MakeShared<FGameLiftDeployAnywhereTab>()),
	DeployManagedEC2Tab(MakeShared<FGameLiftDeployManagedEC2Tab>()),
	DeployContainersTab(MakeShared<FGameLiftDeployContainersTab>())
{
}

void FGameLiftPluginModule::StartupModule()
{
	FGameLiftPluginStyle::Initialize();
	FGameLiftPluginStyle::ReloadTextures();

	FGameLiftPluginCommands::Register();

	DeployAnywhereTab->Register();
	DeployManagedEC2Tab->Register();
	DeployContainersTab->Register();
	
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGameLiftPluginModule::RegisterMenus));
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGameLiftPluginModule::OnPostEngineInit);

	SGameLiftSettingsAwsAccountMenu::RestoreAccount();
	ResetDeploymentStatus();
}

void FGameLiftPluginModule::OnPostEngineInit()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		UGameLiftSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FGameLiftSettingsCustomization::MakeInstance)
	);
	PropertyModule.NotifyCustomizationModuleChanged();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", 
			Settings::kSettingsSectionName,
			Plugin::kGameLiftProductName, // display name
			Settings::kSettingsSectionTooltip,
			GetMutableDefault<UGameLiftSettings>());
	}
}

void FGameLiftPluginModule::ResetDeploymentStatus()
{
	UGameLiftDeploymentStatus* DeploySettings = GetMutableDefault<UGameLiftDeploymentStatus>();
	UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();

	auto EC2State = EDeploymentMessageStateFromString(DeploySettings->Status.ToString());
	auto ContainersState = EDeploymentMessageStateFromString(ContainersStatus->Status.ToString());

	if (IsInProgressState(EC2State) || IsInProgressState(ContainersState))
	{
		auto& Deployer = IGameLiftCoreModule::Get().GetScenarioDeployer();
		Deployer.StopDeployment(IGameLiftCoreModule::Get().GetProfileBootstrap().GetAccountInstance());
		DeploySettings->Status = FText::FromString(EDeploymentMessageStateToString(EDeploymentMessageState::FailureMessage));
		DeploySettings->LatestError = Utils::BuildLastestErrorMessage(Deployer);
		DeploySettings->SaveConfig();
	}
}

void FGameLiftPluginModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGameLiftPluginStyle::Shutdown();

	FGameLiftPluginCommands::Unregister();

	DeployAnywhereTab->UnRegister();
	DeployManagedEC2Tab->UnRegister();
	DeployContainersTab->UnRegister();

	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", Settings::kSettingsSectionName);
	}

}

void FGameLiftPluginModule::DeployManagedEC2ButtonClicked()
{
	DeployManagedEC2Tab->Invoke();
}

void FGameLiftPluginModule::DeployAnywhereButtonClicked()
{
	DeployAnywhereTab->Invoke();
}

void FGameLiftPluginModule::DeployContainersButtonClicked()
{
	DeployContainersTab->Invoke();
}

void FGameLiftPluginModule::OpenSettingsButtonClicked()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	SettingsModule->ShowViewer("Project", "Plugins", "GameLiftPlugin");
}

void FGameLiftPluginModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	RegisterMenuExtensions();
	RegisterLevelEditorExtensions();
	RegisterToolBarMenu();
}

void FGameLiftPluginModule::RegisterMenuExtensions()
{
	PluginCommands = MakeShareable(new FUICommandList);
#define MAP_ACTION(__action__) \
	PluginCommands->MapAction( \
		FGameLiftPluginCommands::Get().__action__, \
		FExecuteAction::CreateRaw(this, &FGameLiftPluginModule::__action__##ButtonClicked), \
		FCanExecuteAction())

#define MAP_HELP_URL_ACTION(__action__) \
	PluginCommands->MapAction( \
		FGameLiftPluginCommands::Get().__action__, \
		FExecuteAction::CreateLambda([](){ \
			FPlatformProcess::LaunchURL(Url::k##__action__##Url, nullptr, nullptr); \
		}), \
		FCanExecuteAction())

	MAP_ACTION(OpenSettings);
	MAP_ACTION(DeployAnywhere);
	MAP_ACTION(DeployManagedEC2);
	MAP_ACTION(DeployContainers);

	MAP_HELP_URL_ACTION(OpenGameLiftDocumentation);
	MAP_HELP_URL_ACTION(OpenAwsGameTechForum);
	MAP_HELP_URL_ACTION(ReportIssues);
	MAP_HELP_URL_ACTION(OpenServiceAPIReference);
	MAP_HELP_URL_ACTION(OpenServerSDKReference);

#undef MAP_ACTION
#undef MAP_HELP_ACTION
}

/* Registers level editor extensions. */
void FGameLiftPluginModule::RegisterLevelEditorExtensions()
{
#if(ENGINE_MAJOR_VERSION == 5)
	constexpr const char* kToolsMenu = "LevelEditor.MainMenu.Tools";
	constexpr const char* kLevelToolBar = "LevelEditor.LevelEditorToolBar.AssetsToolBar";
	constexpr const char* kLevelToolBarSection = "Content";
#else
	constexpr const char* kToolsMenu = "LevelEditor.MainMenu.Window";
	constexpr const char* kLevelToolBar = "LevelEditor.LevelEditorToolBar";
	constexpr const char* kLevelToolBarSection = "Settings";
#endif
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(kToolsMenu);
		{
			FToolMenuSection& Section = Menu->AddSection("AmazonSection", ContextMenu::kAmazonSectionText, FToolMenuInsert(NAME_None, EToolMenuInsertType::First));
			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitSubMenu(
				"GameLiftPlugin",
				ContextMenu::kPluginMainMenuText,
				ContextMenu::kPluginMainMenuTooltip,
				FNewToolMenuDelegate::CreateStatic(&FGameLiftPluginModule::GenerateWindowMenu, PluginCommands.ToSharedRef()),
				false,
				FSlateIcon(Style::kGameLiftPluginStyleName, Style::Brush::kGameLiftToolbarIconName)
			));
			FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
			LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(kLevelToolBar);
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection(kLevelToolBarSection);
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitComboButton(
					"GameLiftPluginAction",
					FUIAction(),
					FOnGetContent::CreateStatic(&FGameLiftPluginModule::GenerateToolBarMenu, PluginCommands.ToSharedRef()),
					ContextMenu::kPluginMainMenuText,
					ContextMenu::kPluginMainMenuTooltip,
					FSlateIcon(Style::kGameLiftPluginStyleName, Style::Brush::kGameLiftToolbarIconName),
					false,
					"GameLiftQuickSettings"
				));
			}
		}
	}
}

void FGameLiftPluginModule::GenerateWindowMenu(UToolMenu* InToolMenu, TSharedRef<FUICommandList> InCommandList)
{
	GenerateMainMenu(InToolMenu);
}

TSharedRef<SWidget> FGameLiftPluginModule::GenerateToolBarMenu(TSharedRef<FUICommandList> InCommandList)
{
	// Get all menu extenders for this context menu from the level editor module
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<FExtender> MenuExtender = LevelEditorModule.AssembleExtenders(InCommandList, LevelEditorModule.GetAllLevelEditorToolbarViewMenuExtenders());

	FToolMenuContext MenuContext(InCommandList, MenuExtender);
	UObject* obj = NewObject<UGameLiftToolbarMenuContext>();
	MenuContext.AddObject(obj);

	return UToolMenus::Get()->GenerateWidget("LevelEditor.LevelEditorToolBar.GameLiftPluginAction", MenuContext);
}

void FGameLiftPluginModule::RegisterToolBarMenu()
{
	static const FName BaseMenuName = "LevelEditor.LevelEditorToolBar.GameLiftPluginAction";
	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu(BaseMenuName);
	ensure(Menu != nullptr);
	GenerateMainMenu(Menu);
}

void FGameLiftPluginModule::GenerateMainMenu(UToolMenu* InMenu)
{
	{
		FToolMenuSection& Section = InMenu->AddSection("GameLiftPlugin_Profile_Setup", ContextMenu::kProfileSetupSectionText);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().OpenSettings);
	}

	{
		FToolMenuSection& Section = InMenu->AddSection("GameLiftPlugin_Hosting_Solutions", ContextMenu::kHostingSolutionsSectionText);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().DeployAnywhere);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().DeployManagedEC2);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().DeployContainers);
	}

	{
		FToolMenuSection& Section = InMenu->AddSection("GameLiftPlugin_Help", ContextMenu::kHelpSectionText);
		Section.AddSubMenu("Help", ContextMenu::kHelpSubMenuText, ContextMenu::kHelpSubMenuTooltip,
			FNewToolMenuDelegate::CreateStatic(&FGameLiftPluginModule::GenerateHelpMenu),
			FUIAction(FExecuteAction(), FCanExecuteAction(), FIsActionChecked()),
			EUserInterfaceActionType::Button);
	}
}

void FGameLiftPluginModule::GenerateHelpMenu(UToolMenu* InMenu)
{
	{
		FToolMenuSection& Section = InMenu->AddSection("GameLiftPlugin_HelpDocumentation", ContextMenu::kHelpDocumentationSectionText);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().OpenGameLiftDocumentation);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().OpenAwsGameTechForum);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().ReportIssues);
	}

	{
		FToolMenuSection& Section = InMenu->AddSection("GameLiftPlugin_HelpSDK", ContextMenu::kHelpSdkSectionText);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().OpenServiceAPIReference);
		Section.AddMenuEntry(FGameLiftPluginCommands::Get().OpenServerSDKReference);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameLiftPluginModule, GameLiftPlugin)
