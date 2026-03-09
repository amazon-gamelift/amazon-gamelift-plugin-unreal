// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "SDeploymentFields.h"

#include <EditorStyleSet.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/Layout/SBorder.h>
#include <Widgets/Layout/SWidgetSwitcher.h>

#include "SWidgets/SPathInput.h"
#include "SWidgets/SNamedRow.h"
#include "SWidgets/SSelectionComboBox.h"

#include "GameLiftPluginConstants.h"
#include "GameLiftPluginStyle.h"

namespace Internal
{
	TSharedPtr<SPathInput> MakeBuildName()
	{
		return SNew(SPathInput)
			.Title(Menu::DeployManagedEC2::kBuildName)
			.PathHint(Menu::DeployManagedEC2::kBuildNameHint)
			.ButtonVisibility(EVisibility::Collapsed);
	}

	TSharedPtr<SPathInput> MakeBuildFolderPath()
	{
		return SNew(SPathInput)
			.Title(Menu::DeployManagedEC2::kBuildFolderPathTitle)
			.PathHint(Menu::DeployManagedEC2::kBuildFolderPathHint)
			.IsFileSelection(false)
			.ToolTipText(Menu::DeployManagedEC2::kBuildFolderPathTooltip);
	}

	TSharedPtr<SPathInput> MakeBuildFilePath()
	{
		return SNew(SPathInput)
			.Title(Menu::DeployManagedEC2::kBuildFilePathTitle)
			.PathHint(Menu::DeployManagedEC2::kBuildFilePathHint)
			.ToolTipText(Menu::DeployManagedEC2::kBuildFilePathTooltip)
			.FileTypes(Menu::DeployManagedEC2::kBuildFilePathFilter);
	}

	TSharedPtr<SPathInput> MakeExtraServerResourcesPath()
	{
		return SNew(SPathInput)
			.Title(Menu::DeployManagedEC2::kExtraServerResourcesPathTitle)
			.IsFileSelection(false)
			.ToolTipText(Menu::DeployManagedEC2::kExtraServerResourcesPathTooltip);
	}

	TSharedPtr<SPathInput> MakeOutConfigFilePath()
	{
		return SNew(SPathInput)
			.Title(Menu::DeployCommon::kOutConfigFilePathTitle)
			.PathHint(Menu::DeployCommon::kOutConfigFilePathHint)
			.IsFileSelection(false)
			.ToolTipText(Menu::DeployCommon::kOutConfigFilePathTooltip);
	}

} // namespace Internal

#define LOCTEXT_NAMESPACE "SDeploymentFields"

void SDeploymentFields::Construct(const FArguments& InArgs)
{
	SupportedOperatingSystems = MakeSupportedOperatingSystemList();

	BuildNameInput = Internal::MakeBuildName();
	BuildOperatingSystemInput = SDeploymentFields::MakeBuildOperatingSystem();
	BuildFolderPathInput = Internal::MakeBuildFolderPath();
	BuildFilePathInput = Internal::MakeBuildFilePath();
	ExtraServerResourcesPathInput = Internal::MakeExtraServerResourcesPath();
	OutConfigFilePathInput = Internal::MakeOutConfigFilePath();

	TSharedPtr<SWidget> BuildNameInputRow = SNew(SNamedRow)
		.NameText(Menu::DeployManagedEC2::kBuildName).RowWidget(BuildNameInput);

	TSharedPtr<SWidget> BuildNameInstructionsRow = SNew(SNamedRow)
		.SecondaryColumnLeftPadding(true)
		.RowWidget(
			SNew(STextBlock)
			.Text(Menu::DeployManagedEC2::kBuildNameInstructionText)
			.TextStyle(FGameLiftPluginStyle::Get(), Style::Text::kNote)
			.AutoWrapText(true)
		);

	TSharedPtr<SWidget> BuildOperatingSystemInputRow = SNew(SNamedRow)
		.NameText(Menu::DeployManagedEC2::kBuildOperatingSystemTitle).RowWidget(BuildOperatingSystemInput);

	TSharedPtr<SWidget> BuildFolderPathInputRow = SNew(SNamedRow)
		.NameText(Menu::DeployManagedEC2::kBuildFolderPathTitle).RowWidget(BuildFolderPathInput);

	TSharedPtr<SWidget> BuildFilePathInputRow = SNew(SNamedRow)
		.NameText(Menu::DeployManagedEC2::kBuildFilePathTitle).RowWidget(BuildFilePathInput);

	TSharedPtr<SWidget> ExtraServerResourcesPathInputRow = SNew(SNamedRow)
		.NameText(Menu::DeployManagedEC2::kExtraServerResourcesPathTitle).RowWidget(ExtraServerResourcesPathInput);

	// Currently, this field does not work so we hide it from users.
	ExtraServerResourcesPathInputRow->SetVisibility(EVisibility::Collapsed);

	TSharedPtr<SWidget> OutConfigFilePathInputRow = SNew(SNamedRow)
		.NameText(Menu::DeployCommon::kOutConfigFilePathTitle)
		.NameTooltipText(Menu::DeployCommon::kOutConfigFilePathTooltip)
		.RowWidget(OutConfigFilePathInput);

	TSharedPtr<SWidget> FullWidget =
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()
		[
			BuildNameInputRow.ToSharedRef()
		]
		+ SVerticalBox::Slot().AutoHeight()
		[
			BuildNameInstructionsRow.ToSharedRef()
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(SPadding::Top_Bottom)
		[
			BuildOperatingSystemInputRow.ToSharedRef()
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(SPadding::Top_Bottom)
		[
			BuildFolderPathInputRow.ToSharedRef()
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(SPadding::Top_Bottom)
		[
			BuildFilePathInputRow.ToSharedRef()
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(SPadding::Top_Bottom)
		[
			ExtraServerResourcesPathInputRow.ToSharedRef()
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(SPadding::Top_Bottom)
		[
			OutConfigFilePathInputRow.ToSharedRef()
		];

	ChildSlot
	[
		SNew(SBox)
		[
			FullWidget.ToSharedRef()
		]
	];
}

void SDeploymentFields::SetAllFieldsReadOnly(bool ReadOnly)
{
	BuildNameInput->SetReadonly(ReadOnly);
	BuildOperatingSystemInput->SetEnabled(!ReadOnly);
	BuildFolderPathInput->SetReadonly(ReadOnly);
	BuildFilePathInput->SetReadonly(ReadOnly);
	ExtraServerResourcesPathInput->SetReadonly(ReadOnly);
	OutConfigFilePathInput->SetReadonly(ReadOnly);
}

TSharedPtr<SSelectionComboBox> SDeploymentFields::MakeBuildOperatingSystem()
{
	return SNew(SSelectionComboBox)
		.OnListBuilding_Raw(this, &SDeploymentFields::OnBuildingSupportedOperatingSystemValues)
		.OnItemSelected_Raw(this, &SDeploymentFields::OnOperatingSystemSelected);
}

void SDeploymentFields::OnBuildingSupportedOperatingSystemValues(TArray<FTextIntPair>& Items)
{
	Items.Reset();

	for (const EFleetOperatingSystem& OS : SupportedOperatingSystems)
	{
		Items.Add(FTextIntPair(EFleetOperatingSystemToName(OS), (int)OS, EFleetOperatingSystemToValue(OS)));
	}
}

void SDeploymentFields::OnOperatingSystemSelected(int SelectionId, const FTextIntPair& Item)
{
	CurrentOperatingSystemSelected = SelectionId;
	OnOperatingSystemChanged.Broadcast();
}

void SDeploymentFields::SetGameOnlyMode()
{
	CachedState = Modes::GameNameOnly;
}

void SDeploymentFields::SetFullMode()
{
	CachedState = Modes::Full;
}

void SDeploymentFields::SetBuildName(const FText& Name)
{
	BuildNameInput->SetSelectedPath(Name);
}

void SDeploymentFields::SetBuildOperatingSystem(const FText& OSValue)
{
	CurrentOperatingSystemSelected = (int)EFleetOperatingSystemFromValueText(OSValue);

	// update selection combo box to show this item selection
	BuildOperatingSystemInput->SetSelectedId(CurrentOperatingSystemSelected);
}

void SDeploymentFields::SetBuildFolderPath(const FText& Path)
{
	BuildFolderPathInput->SetSelectedPath(Path);
}

void SDeploymentFields::SetBuildFilePath(const FText& Path)
{
	BuildFilePathInput->SetSelectedPath(Path);
}

void SDeploymentFields::SetExtraServerResourcesPath(const FText& Path)
{
	ExtraServerResourcesPathInput->SetSelectedPath(Path);
}

void SDeploymentFields::SetOutConfigFilePath(const FText& Path)
{
	OutConfigFilePathInput->SetSelectedPath(Path);
}

void SDeploymentFields::SetDeploymentScenario(const FText& Name)
{
	DeploymentScenario = Name;
}

const FText& SDeploymentFields::GetBuildName() const
{
	return BuildNameInput->GetSelectedPathRef();
}

FText SDeploymentFields::GetBuildOperatingSystem() const
{
	return EFleetOperatingSystemToValue(SupportedOperatingSystems[CurrentOperatingSystemSelected]);
}

const FText& SDeploymentFields::GetBuildFolderPath() const
{
	return BuildFolderPathInput->GetSelectedPathRef();
}

const FText& SDeploymentFields::GetBuildFilePath() const
{
	return BuildFilePathInput->GetSelectedPathRef();
}

const FText& SDeploymentFields::GetExtraServerResourcesPath() const
{
	return ExtraServerResourcesPathInput->GetSelectedPathRef();
}

const FText& SDeploymentFields::GetOutConfigFilePath() const
{
	return OutConfigFilePathInput->GetSelectedPathRef();
}

const FText& SDeploymentFields::GetDeploymentScenario() const
{
	return DeploymentScenario;
}

SDeploymentFields::Modes SDeploymentFields::GetCurrentState() const
{
	return CachedState;
}

int32 SDeploymentFields::GetSetupStateAsInt() const
{
	return (int32)CachedState;
}

#undef LOCTEXT_NAMESPACE
