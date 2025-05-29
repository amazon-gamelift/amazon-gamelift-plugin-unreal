// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Types/EAwsRegions.h"

class SGameLiftSettingsAwsAccountMenu;
class SSetProfileSection;
class SCreateContainerImageSection;
class SSetupECRRepositorySection;
class SBuildAndPushToECRSection;
class SCreateContainerGroupSection;
class SCreateContainerFleetSection;
class SHelpfulResourcesSection;
class SIntakeQuestionnaireSection;
class SUserInputSection;
class SControlBarSection;
class SLaunchBar;
class SWindow;

DECLARE_DELEGATE(FModalOpened);
DECLARE_DELEGATE(FModalClosed);

class SGameLiftDeployContainersMenu : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SGameLiftDeployContainersMenu) { }

    SLATE_ARGUMENT(TWeakPtr<SWindow>, ContextWindow)

    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs);

    static FSimpleDelegate OnModalOpened;
    static FSimpleDelegate OnModalClosed;
    
private:
    void OnECRUpdate();
    void OnBuildAndPushFinished();
    void OnContainersImageUpdate();
    void OnRetryContainerFleetStep();
    void SetExpandedSections();
    void OnLaunchClientButtonClicked(FString GameClientPath, FString LauncherArgs);
    void OnProfileSelectionChanged(const SGameLiftSettingsAwsAccountMenu*);
    void UpdateAWSCredentialsProfileSectionUI();
    TSharedRef<SLaunchBar> CreateLaunchBar();
    TSharedRef<SWidget> CreateCreateContainerImageSection();
    TSharedRef<SWidget> CreateSetupECRRepositorySection();
    TSharedRef<SBuildAndPushToECRSection> CreateBuildAndPushToECRSection();
    TSharedRef<SWidget> CreateCreateContainerGroupSection();
    TSharedRef<SWidget> CreateCreateContainerFleetSection();
    TSharedRef<SWidget> CreateHelpfulResourcesSection();
    TSharedRef<SWidget> CreateAWSCredentialsProfileSection();
    TSharedRef<SWidget> CreateIntakeQuestionnaireSection();
    TSharedRef<SWidget> CreateUserInputSection();
    TSharedRef<SWidget> CreateControlBarSection();
    void StartDeployment();
    TSharedRef<SWidget> CreateScenarioOneWorkflow();
    TSharedRef<SWidget> CreateScenarioTwoWorkflow();
    TSharedRef<SWidget> CreateScenarioThreeWorkflow();
    void OnUpdateDeployment();
    TSharedRef<SWidget> CreateDivider();
    bool IsLaunchBarActive() const;
    bool HasDeploymentStarted();
    void ResetAllSectionsToNotStarted();
    void OnNewDeployment();
    void OnResetDeployment();
    bool IsLaunchingGameClient;
    bool IsContainerSupportedRegion() const;

    TArray<Aws::ERegions> ContainerSupportedRegions =
    {
        Aws::ERegions::us_east_1,
        Aws::ERegions::us_west_2,
        Aws::ERegions::ap_northeast_1,
        Aws::ERegions::ap_northeast_2,
        Aws::ERegions::ap_southeast_2,
        Aws::ERegions::ca_central_1,
        Aws::ERegions::eu_central_1,
        Aws::ERegions::eu_west_1
    };

private:
    TSharedPtr<SVerticalBox> ContainerSteps;
    TSharedPtr<SSetProfileSection> SetProfileSection;
    TSharedPtr<SCreateContainerImageSection> CreateContainerImageSection;
    TSharedPtr<SSetupECRRepositorySection> SetupECRRepositorySection;
    TSharedPtr<SBuildAndPushToECRSection> BuildAndPushToECRSection;
    TSharedPtr<SCreateContainerGroupSection> CreateContainerGroupSection;
    TSharedPtr<SCreateContainerFleetSection> CreateContainerFleetSection;
    TSharedPtr<SHelpfulResourcesSection> HelpfulResourcesSection;
    TSharedPtr<SIntakeQuestionnaireSection> IntakeQuestionnaireSection;
    TSharedPtr<SUserInputSection> UserInputSection;
    TSharedPtr<SControlBarSection> ControlBarSection;
    TSharedPtr<SWidgetSwitcher> DeployScenarioSwitcher;
    TSharedPtr<SExpandableArea> SetProfileExpandableSection;
    TSharedPtr<SExpandableArea> IntakeQuestionnaireExpandableSection;
    TSharedPtr<SExpandableArea> UserInputExpandableSection;
    TSharedPtr<SLaunchBar> LaunchBar;
    TWeakPtr<SWindow> ContextWindow;

};
