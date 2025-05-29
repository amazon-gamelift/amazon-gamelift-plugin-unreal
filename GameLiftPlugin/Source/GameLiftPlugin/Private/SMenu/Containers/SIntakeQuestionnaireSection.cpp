// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SIntakeQuestionnaireSection.h"

#include <Widgets/Layout/SWidgetSwitcher.h>
#include <Settings/UGameLiftContainersStatus.h>
#include <Types/EContainersDeploymentScenario.h>
#include <GameLiftPluginConstants.h>

#define LOCTEXT_NAMESPACE "SIntakeQuestionnaireSection"

FDeploymentScenarioState SIntakeQuestionnaireSection::OnDeploymentScenarioStateChangedMultiDelegate;

void SIntakeQuestionnaireSection::Construct(const FArguments& InArgs)
{
    TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

    // Question 1
    VerticalBox->AddSlot()
        .AutoHeight()
        .Padding(SPadding::Top_Bottom2x)
        [
            CreateFirstQuestion()
        ];

    // Question 2
    VerticalBox->AddSlot()
        .AutoHeight()
        [
            CreateSecondQuestion()
        ];

    ChildSlot
        .Padding(SPadding::Top_Bottom)
        [
            VerticalBox
        ];
}

TSharedRef<SWidget> SIntakeQuestionnaireSection::CreateFirstQuestion()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(SPadding::Bottom)
        .AutoHeight()
        [
            SNew(STextBlock).Text(Menu::DeployContainers::kExistingContainerImageQuestion)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(SPadding::Right2x)
                [
                    SNew(SCheckBox)
                        .Style(FCoreStyle::Get(), "RadioButton")
                        .IsChecked(this, &SIntakeQuestionnaireSection::IsRadioChecked, EHasExistingContainerImageChoice::HasExistingContainerImage)
                        .OnCheckStateChanged(this, &SIntakeQuestionnaireSection::OnRadioChanged, EHasExistingContainerImageChoice::HasExistingContainerImage)
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text(Menu::DeployContainers::kAnswerYes)
                        ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SCheckBox)
                        .Style(FCoreStyle::Get(), "RadioButton")
                        .IsChecked(this, &SIntakeQuestionnaireSection::IsRadioChecked, EHasExistingContainerImageChoice::NoExistingContainerImage)
                        .OnCheckStateChanged(this, &SIntakeQuestionnaireSection::OnRadioChanged, EHasExistingContainerImageChoice::NoExistingContainerImage)
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text(Menu::DeployContainers::kAnswerNo)
                        ]
                ]
        ];
}

TSharedRef<SWidget> SIntakeQuestionnaireSection::CreateSecondQuestion()
{
    return SNew(SWidgetSwitcher)
        .WidgetIndex_Lambda([&]()
            {
                UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
                return ContainersStatus->HasExistingContainerImage;
            })
        + SWidgetSwitcher::Slot()
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(SPadding::Bottom)
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployContainers::kECRRepositoryQuestion)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(SPadding::Right2x)
                        [
                            SNew(SCheckBox)
                                .Style(FCoreStyle::Get(), "RadioButton")
                                .IsChecked(this, &SIntakeQuestionnaireSection::IsRadioChecked, EUseExistingECRRepoChoice::UseExistingECRRepo)
                                .OnCheckStateChanged(this, &SIntakeQuestionnaireSection::OnRadioChanged, EUseExistingECRRepoChoice::UseExistingECRRepo)
                                .Content()
                                [
                                    SNew(STextBlock)
                                        .Text(Menu::DeployContainers::kAnswerYes)
                                ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SCheckBox)
                                .Style(FCoreStyle::Get(), "RadioButton")
                                .IsChecked(this, &SIntakeQuestionnaireSection::IsRadioChecked, EUseExistingECRRepoChoice::NoExistingECRRepo)
                                .OnCheckStateChanged(this, &SIntakeQuestionnaireSection::OnRadioChanged, EUseExistingECRRepoChoice::NoExistingECRRepo)
                                .Content()
                                [
                                    SNew(STextBlock)
                                        .Text(Menu::DeployContainers::kAnswerNo)
                                ]
                        ]
                ]

        ]
        + SWidgetSwitcher::Slot()
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(SPadding::Bottom)
                [
                    SNew(STextBlock)
                        .Text(Menu::DeployContainers::kWhereIsContainerImageQuestion)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(SPadding::Right2x)
                        [
                            SNew(SCheckBox)
                                .Style(FCoreStyle::Get(), "RadioButton")
                                .IsChecked(this, &SIntakeQuestionnaireSection::IsRadioChecked, EContainerImageChoice::Docker)
                                .OnCheckStateChanged(this, &SIntakeQuestionnaireSection::OnRadioChanged, EContainerImageChoice::Docker)
                                .Content()
                                [
                                    SNew(STextBlock)
                                        .Text(Menu::DeployContainers::kAnswerDocker)
                                ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SCheckBox)
                                .Style(FCoreStyle::Get(), "RadioButton")
                                .IsChecked(this, &SIntakeQuestionnaireSection::IsRadioChecked, EContainerImageChoice::AmazonECR)
                                .OnCheckStateChanged(this, &SIntakeQuestionnaireSection::OnRadioChanged, EContainerImageChoice::AmazonECR)
                                .Content()
                                [
                                    SNew(STextBlock)
                                        .Text(Menu::DeployContainers::kAnswerAmazonECR)
                                ]
                        ]
                ]
        ];
}

ECheckBoxState SIntakeQuestionnaireSection::IsRadioChecked(EHasExistingContainerImageChoice RadioButtonChoice) const
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return (ContainersStatus->HasExistingContainerImage == (int)RadioButtonChoice) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SIntakeQuestionnaireSection::OnRadioChanged(ECheckBoxState NewRadioState, EHasExistingContainerImageChoice RadioButtonChoice)
{
    if (NewRadioState == ECheckBoxState::Checked)
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->HasExistingContainerImage = (int)RadioButtonChoice;
        ContainersStatus->SaveConfig();
    }
    UpdateDeploymentScenarioState();
}

ECheckBoxState SIntakeQuestionnaireSection::IsRadioChecked(EUseExistingECRRepoChoice RadioButtonChoice) const
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return (ContainersStatus->UseExistingECRRepo == (int)RadioButtonChoice) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SIntakeQuestionnaireSection::OnRadioChanged(ECheckBoxState NewRadioState, EUseExistingECRRepoChoice RadioButtonChoice)
{
    if (NewRadioState == ECheckBoxState::Checked)
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->UseExistingECRRepo = (int)RadioButtonChoice;
        ContainersStatus->SaveConfig();
    }
    UpdateDeploymentScenarioState();
}

ECheckBoxState SIntakeQuestionnaireSection::IsRadioChecked(EContainerImageChoice RadioButtonChoice) const
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    return (ContainersStatus->ContainerImageChoice == (int)RadioButtonChoice) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SIntakeQuestionnaireSection::OnRadioChanged(ECheckBoxState NewRadioState, EContainerImageChoice RadioButtonChoice)
{
    if (NewRadioState == ECheckBoxState::Checked)
    {
        UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
        ContainersStatus->ContainerImageChoice = (int)RadioButtonChoice;
        ContainersStatus->SaveConfig();
    }
    UpdateDeploymentScenarioState();
}

void SIntakeQuestionnaireSection::UpdateDeploymentScenarioState()
{
    UGameLiftContainersStatus* ContainersStatus = GetMutableDefault<UGameLiftContainersStatus>();
    if (ContainersStatus->HasExistingContainerImage == (int)EHasExistingContainerImageChoice::NoExistingContainerImage &&
        ContainersStatus->UseExistingECRRepo == (int)EUseExistingECRRepoChoice::NoExistingECRRepo) 
    {
        ContainersStatus->DeploymentScenario = (int)EContainersDeploymentScenario::NoContainerImageNoExistingEcrRepo;
    }
    else if (ContainersStatus->HasExistingContainerImage == (int)EHasExistingContainerImageChoice::NoExistingContainerImage &&
        ContainersStatus->UseExistingECRRepo == (int)EUseExistingECRRepoChoice::UseExistingECRRepo)
    {
        ContainersStatus->DeploymentScenario = (int)EContainersDeploymentScenario::NoContainerImageUseExistingEcrRepo;
    }
    else if (ContainersStatus->HasExistingContainerImage == (int)EHasExistingContainerImageChoice::HasExistingContainerImage &&
        ContainersStatus->ContainerImageChoice == (int)EContainerImageChoice::Docker)
    {
        ContainersStatus->DeploymentScenario = (int)EContainersDeploymentScenario::HaveContainerImageInDocker;
    }
    else if (ContainersStatus->HasExistingContainerImage == (int)EHasExistingContainerImageChoice::HasExistingContainerImage &&
        ContainersStatus->ContainerImageChoice == (int)EContainerImageChoice::AmazonECR)
    {
        ContainersStatus->DeploymentScenario = (int)EContainersDeploymentScenario::HaveContainerImageInEcr;
    }

    ContainersStatus->SaveConfig();
    OnDeploymentScenarioStateChangedMultiDelegate.Broadcast(this);
}

#undef LOCTEXT_NAMESPACE
