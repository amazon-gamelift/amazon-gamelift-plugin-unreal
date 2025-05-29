// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SGameLiftDeployContainersMenu.h"

#include <SWidgets/SLaunchBar.h>
#include <SWidgets/SSectionsWithHeaders.h>
#include <SWidgets/SExpandableSection.h>
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "SMenu/SCommonMenuSections.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"

#include "SCreateContainerImageSection.h"
#include "SSetupECRRepositorySection.h"
#include "SBuildAndPushToECRSection.h"
#include "SCreateContainerGroupSection.h"
#include "SCreateContainerFleetSection.h"
#include "SHelpfulResourcesSection.h"
#include "SIntakeQuestionnaireSection.h"
#include "SUserInputSection.h"
#include "SControlBarSection.h"
#include "SResetDeploymentModal.h"
#include "SUpdateDeploymentModal.h"

#include "Settings/UGameLiftContainersStatus.h"
#include "Settings/UGameLiftSettings.h"
#include <GameLiftPluginConstants.h>
#include <GameLiftPluginStyle.h>
#include <GameLiftPlugin.h>
#include <Types/EContainersDeploymentScenario.h>

#include "Utils/Misc.h"
#include "Utils/Notifier.h"
#include "IGameLiftCoreModule.h"

#define LOCTEXT_NAMESPACE "SGameLiftDeployContainersMenu"

FModalOpened SGameLiftDeployContainersMenu::OnModalOpened;
FModalClosed SGameLiftDeployContainersMenu::OnModalClosed;

void SGameLiftDeployContainersMenu::Construct(const FArguments& InArgs)
{
    ContextWindow = InArgs._ContextWindow; 
    
    // Create the widgets
    CreateCreateContainerGroupSection();
    CreateBuildAndPushToECRSection();
    CreateSetupECRRepositorySection();
    CreateCreateContainerImageSection();
    CreateCreateContainerFleetSection();

    ContainerSteps = SNew(SVerticalBox);

    ContainerSteps->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
        [
            CreateDivider()
        ];

    ContainerSteps->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
        [
            CreateIntakeQuestionnaireSection()
        ];

    ContainerSteps->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
        [
            SNew(SBox)
                [
                    CreateDivider()
                ]
        ];

    ContainerSteps->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
        [
            SNew(SBox)
                [
                    CreateUserInputSection()
                ]
        ];

    ContainerSteps->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
        [
            SNew(SBox)
                [
                    CreateDivider()
                ]
        ];

    ContainerSteps->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
        [
            SNew(SBox)
                [
                    CreateControlBarSection()
                ]
        ];

    ContainerSteps->AddSlot()
        .AutoHeight()
        [
            SNew(SBox)
                .Visibility_Lambda([this]()
                    {
                        return IsContainerSupportedRegion() && HasDeploymentStarted() ? EVisibility::Visible : EVisibility::Collapsed;
                    })
                [
                    SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(SPadding::Left5x_Right5x + SPadding::Bottom5x)
                        [
                            CreateDivider()
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Fill)
                        .Padding(SPadding::Left5x_Right5x + SPadding::Extra_For_Page_End) // This adds more space at the bottom so users can scroll down further.
                        [
                            SAssignNew(DeployScenarioSwitcher, SWidgetSwitcher)
                                .WidgetIndex_Lambda([&]()
                                    {
                                        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                                        return ContainersStatus->DeploymentScenario;
                                    })
                                + SWidgetSwitcher::Slot()
                                [
                                    CreateScenarioOneWorkflow()
                                ]
                                + SWidgetSwitcher::Slot()
                                [
                                    CreateScenarioOneWorkflow()
                                ]
                                + SWidgetSwitcher::Slot()
                                [
                                    CreateScenarioTwoWorkflow()
                                ]
                                + SWidgetSwitcher::Slot()
                                [
                                    CreateScenarioThreeWorkflow()
                                ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            CreateLaunchBar()
                        ]
                ]
        ];

    ChildSlot
    [
        SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(SPadding::Left5x_Right5x + SPadding::Top5x_Bottom5x)
            [
                CreateAWSCredentialsProfileSection()
            ]
            + SVerticalBox::Slot()
            [
                ContainerSteps.ToSharedRef()
            ]
    ];

    SetExpandedSections();

    SUpdateDeploymentModal::OnUpdateDeploymentMultiDelegate.AddSP(this, &SGameLiftDeployContainersMenu::OnUpdateDeployment);
    SControlBarSection::OnNewDeploymentMultiDelegate.AddSP(this, &SGameLiftDeployContainersMenu::OnNewDeployment);
    SResetDeploymentModal::OnResetDeploymentMultiDelegate.AddSP(this, &SGameLiftDeployContainersMenu::OnResetDeployment);
    SGameLiftSettingsAwsAccountMenu::OnProfileSelectionChangedMultiDelegate.AddSP(this, &SGameLiftDeployContainersMenu::OnProfileSelectionChanged);

    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->IsUpdateContainerFleetModalOpen = false;
    ContainersStatus->IsResetContainerFleetModalOpen = false;

    UpdateAWSCredentialsProfileSectionUI();

    OnModalClosed.BindLambda([this]() {
        this->SetEnabled(true);
    });

    OnModalOpened.BindLambda([this]() {
        this->SetEnabled(false);
    });
    
}

void SGameLiftDeployContainersMenu::UpdateAWSCredentialsProfileSectionUI()
{
    bool IsSupportedRegion = IsContainerSupportedRegion();
    SetProfileSection->GetAwsAccountMenuRef()->SetContainersRegionSupported(IsSupportedRegion);
    ContainerSteps->SetEnabled(IsSupportedRegion);
}

void SGameLiftDeployContainersMenu::OnProfileSelectionChanged(const SGameLiftSettingsAwsAccountMenu* /* Sender */)
{
    UpdateAWSCredentialsProfileSectionUI();
}

TSharedRef<SLaunchBar> SGameLiftDeployContainersMenu::CreateLaunchBar()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    FString GameClientPath = ContainersStatus->GameClientExecutablePath;
    FString GameClientLauncherArgs = ContainersStatus->GameClientLauncherArguments;

    return SAssignNew(LaunchBar, SLaunchBar)
        .MenuType(EMenuType::Containers)
        .ParentWidget(this->AsWeak())
        .IsEnabled_Raw(this, &SGameLiftDeployContainersMenu::IsLaunchBarActive)
        .DefaultClientBuildExecutablePath(GameClientPath)
        .DefaultClientBuildLauncherArguments(GameClientLauncherArgs)
        .OnStartClientButtonClicked(FStartClient::CreateRaw(this, &SGameLiftDeployContainersMenu::OnLaunchClientButtonClicked));
}

void SGameLiftDeployContainersMenu::OnLaunchClientButtonClicked(FString GameClientPath, FString LauncherArgs)
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->GameClientExecutablePath = GameClientPath;
    ContainersStatus->GameClientLauncherArguments = LauncherArgs;
    ContainersStatus->SaveConfig();

    if (LaunchBar.IsValid())
    {
        LaunchBar->ChangeStartClientButtonUIState(SLaunchBar::EStartButtonUIState::Loading);
    }

    IsLaunchingGameClient = true;

    Async(EAsyncExecution::Thread, [this, GameClientPath, LauncherArgs]()
        {
            auto runner = IGameLiftCoreModule::Get().MakeRunner();
            bool bIsLaunched = runner->LaunchProcess(GameClientPath, { LauncherArgs });

            IsLaunchingGameClient = false;

            // Added delay for loading button visibility
            FPlatformProcess::Sleep(Menu::DeployCommon::kStartClientLoadingButtonDelay);

            Async(EAsyncExecution::TaskGraphMainThread, [this, bIsLaunched, GameClientPath]()
                {
                    if (bIsLaunched)
                    {
                        UE_LOG(GameLiftPluginLog, Log, TEXT("Launched game client: %s"), *GameClientPath);
                    }
                    else
                    {
                        UE_LOG(GameLiftPluginLog, Error, TEXT("Failed to launch game client: %s. As an alternative, try launching the client manually."), *GameClientPath);
                        Notifier::ShowFailedNotification(Menu::DeployCommon::kLaunchedGameClientFailureMessage);
                    }

                    if (LaunchBar.IsValid())
                    {
                        LaunchBar->ChangeStartClientButtonUIState(SLaunchBar::EStartButtonUIState::Start);
                    }
                });
        });
}

void SGameLiftDeployContainersMenu::OnUpdateDeployment() {
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    ContainersStatus->IsContainerFolderSetup = false;
    ContainersStatus->IsSetupECRRepoStepFinished = false;
    ContainersStatus->IsContainerImagePushed = false;
    ContainersStatus->IsCgdConfigurationFinished = false;
    ContainersStatus->IsLastStepComplete = false;
    ContainersStatus->ErrorCreatingContainerImage = false;
    ContainersStatus->ErrorCreatingECRRepo = false;
    ContainersStatus->ErrorRunningBuildAndPushScript = false;
    ContainersStatus->ErrorCreatingContainerGroupSection = false;
    ContainersStatus->IsCgdFailed = false;
    ContainersStatus->ErrorCreatingContainerFleet = false;
    ContainersStatus->SaveConfig();
    ResetAllSectionsToNotStarted();
    StartDeployment();
}

void SGameLiftDeployContainersMenu::ResetAllSectionsToNotStarted() {
    CreateContainerImageSection->ResetUIToNotStarted();
    SetupECRRepositorySection->ResetUIToNotStarted();
    BuildAndPushToECRSection->ResetUIToNotStarted();
    CreateContainerGroupSection->ResetUIToNotStarted();
    CreateContainerFleetSection->ResetUIToNotStarted();
}

void SGameLiftDeployContainersMenu::OnNewDeployment()
{    
    SetExpandedSections();
    StartDeployment();
}

void SGameLiftDeployContainersMenu::OnResetDeployment()
{
    ResetAllSectionsToNotStarted();
}

void SGameLiftDeployContainersMenu::StartDeployment()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    switch (ContainersStatus->DeploymentScenario)
    {
    case (int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo:
        CreateContainerImageSection->StartSectionWorkflow();
        return;
    case (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo:
        CreateContainerImageSection->StartSectionWorkflow();
        return;
    case (int)EContainersDeploymentScenario::HaveContainerImageInDocker:
        SetupECRRepositorySection->StartSectionWorkflow();
        return;
    case (int)EContainersDeploymentScenario::HaveContainerImageInEcr:
        CreateContainerGroupSection->StartSectionWorkflow();
        return;
    }
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateCreateContainerImageSection()
{
    return SAssignNew(CreateContainerImageSection, SCreateContainerImageSection)
        .OnContainersImageUpdateDelegate(this, &SGameLiftDeployContainersMenu::OnContainersImageUpdate);
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateSetupECRRepositorySection()
{
    return SAssignNew(SetupECRRepositorySection, SSetupECRRepositorySection)
        .OnECRUpdateDelegate(this, &SGameLiftDeployContainersMenu::OnECRUpdate);
}

TSharedRef<SBuildAndPushToECRSection> SGameLiftDeployContainersMenu::CreateBuildAndPushToECRSection()
{
    return SAssignNew(BuildAndPushToECRSection, SBuildAndPushToECRSection)
        .OnBuildAndPushFinishedDelegate(this, &SGameLiftDeployContainersMenu::OnBuildAndPushFinished);
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateCreateContainerGroupSection()
{
    return SAssignNew(CreateContainerGroupSection, SCreateContainerGroupSection);
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateCreateContainerFleetSection()
{
    return SAssignNew(CreateContainerFleetSection, SCreateContainerFleetSection)
        .OnRetryStepDelegate(this, &SGameLiftDeployContainersMenu::OnRetryContainerFleetStep);
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateHelpfulResourcesSection()
{
    return SAssignNew(HelpfulResourcesSection, SHelpfulResourcesSection);
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateAWSCredentialsProfileSection()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::ContentAreaLeftIndent)
        [
            SNew(STextBlock).Text(Menu::DeployContainers::kStepZero)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(SetProfileExpandableSection, SExpandableArea)
                .AllowAnimatedTransition(false)
                .InitiallyCollapsed(false)
                .HeaderPadding(SPadding::Left2x_Right2x)
                .Padding(SPadding::ContentAreaLeftIndent + SPadding::Right2x + SPadding::Top2x)
                .BorderImage(FStyleDefaults::GetNoBrush())
                .BodyBorderImage(FStyleDefaults::GetNoBrush())
                .BorderBackgroundColor(FColor::Transparent)
                .BodyBorderBackgroundColor(FColor::Transparent)
                .HeaderContent()
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployContainers::kAWSCredendentialsProfileTitle)
                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                ]
                .BodyContent()
                [
                    SAssignNew(SetProfileSection, SSetProfileSection)
                        .ReadDeveloperGuideLink(Url::kHelpDeployContainerLinkUrl)
                ]
        ];
}

bool SGameLiftDeployContainersMenu::IsContainerSupportedRegion() const
{
    UGameLiftSettings* GameLiftSettings = GetMutableDefault<UGameLiftSettings>();
    FString AwsRegion = GameLiftSettings->AwsRegion.ToString();

    Aws::ERegions Region = Aws::RegionFromString(AwsRegion);

    return ContainerSupportedRegions.Contains(Region);
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateIntakeQuestionnaireSection()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::ContentAreaLeftIndent)
        [
            SNew(STextBlock).Text(Menu::DeployContainers::kStepOne)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(IntakeQuestionnaireExpandableSection, SExpandableArea)
                .AllowAnimatedTransition(false)
                .InitiallyCollapsed(false)
                .HeaderPadding(SPadding::Left2x_Right2x)
                .Padding(SPadding::ContentAreaLeftIndent + SPadding::Right2x + SPadding::Top2x + SPadding::Bottom)
                .BorderImage(FStyleDefaults::GetNoBrush())
                .BodyBorderImage(FStyleDefaults::GetNoBrush())
                .BorderBackgroundColor(FColor::Transparent)
                .BodyBorderBackgroundColor(FColor::Transparent)
                .HeaderContent()
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployContainers::kIntakeQuestionnaireTitle)
                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                ]
                .BodyContent()
                [
                    SNew(SBox)
                        .IsEnabled_Lambda([&]
                            {
                                return !HasDeploymentStarted();
                            })
                        [
                            SAssignNew(IntakeQuestionnaireSection, SIntakeQuestionnaireSection)
                        ]
                ]
        ];
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateUserInputSection()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SPadding::ContentAreaLeftIndent)
        [
            SNew(STextBlock).Text(Menu::DeployContainers::kStepTwo)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(UserInputExpandableSection, SExpandableArea)
                .AllowAnimatedTransition(false)
                .InitiallyCollapsed(false)
                .HeaderPadding(SPadding::Left2x_Right2x)
                .Padding(SPadding::ContentAreaLeftIndent + SPadding::Right2x + SPadding::Top2x + SPadding::Bottom)
                .BorderImage(FStyleDefaults::GetNoBrush())
                .BodyBorderImage(FStyleDefaults::GetNoBrush())
                .BorderBackgroundColor(FColor::Transparent)
                .BodyBorderBackgroundColor(FColor::Transparent)
                .HeaderContent()
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployContainers::kUserInputSectionTitle)
                        .TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kTitleStyleName)
                ]
                .BodyContent()
                [
                    SNew(SBox)
                        .IsEnabled_Lambda([&]
                            {
                                return !HasDeploymentStarted();
                            })
                        [
                            SAssignNew(UserInputSection, SUserInputSection)
                        ]
                ]
        ];
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateControlBarSection()
{
    return SAssignNew(ControlBarSection, SControlBarSection);
}

void SGameLiftDeployContainersMenu::OnECRUpdate() {
    BuildAndPushToECRSection->StartSectionWorkflow();
}

void SGameLiftDeployContainersMenu::OnBuildAndPushFinished() {
    CreateContainerGroupSection->StartSectionWorkflow();
}

void SGameLiftDeployContainersMenu::OnContainersImageUpdate() {
    SetupECRRepositorySection->StartSectionWorkflow();
}

void SGameLiftDeployContainersMenu::OnRetryContainerFleetStep() {
    CreateContainerGroupSection->StartSectionWorkflow();
}

void SGameLiftDeployContainersMenu::SetExpandedSections()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    bool IsExpanded = ContainersStatus->ContainerFleetDeploymentStatus != (int)EContainerFleetDeploymentStatus::DeploymentInProgress;

    IntakeQuestionnaireExpandableSection->SetExpanded(IsExpanded);
    UserInputExpandableSection->SetExpanded(IsExpanded);
    SetProfileExpandableSection->SetExpanded(IsExpanded);
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateScenarioOneWorkflow()
{
    return SNew(SBox)
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    CreateContainerImageSection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SetupECRRepositorySection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    BuildAndPushToECRSection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    CreateContainerGroupSection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    CreateContainerFleetSection.ToSharedRef()
                ]
        ];
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateScenarioTwoWorkflow()
{
    return SNew(SBox)
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SetupECRRepositorySection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    BuildAndPushToECRSection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    CreateContainerGroupSection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    CreateContainerFleetSection.ToSharedRef()
                ]
        ];
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateScenarioThreeWorkflow()
{
    return SNew(SBox)
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    CreateContainerGroupSection.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    CreateContainerFleetSection.ToSharedRef()
                ]
        ];
}

TSharedRef<SWidget> SGameLiftDeployContainersMenu::CreateDivider()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        [
            SNew(SImage).Image(FGameLiftPluginStyle::Get().GetBrush(Style::Brush::kGameLiftDivider))
        ];
}

bool SGameLiftDeployContainersMenu::IsLaunchBarActive() const
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return ContainersStatus->IsLastStepComplete && !IsLaunchingGameClient;
}

bool SGameLiftDeployContainersMenu::HasDeploymentStarted()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return ContainersStatus->ContainerFleetDeploymentStatus != (int)EContainerFleetDeploymentStatus::DeploymentNotStart;
}

#undef LOCTEXT_NAMESPACE
