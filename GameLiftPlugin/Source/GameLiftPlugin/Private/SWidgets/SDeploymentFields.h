// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Styling/SlateColor.h"
#include "Types/FTextIntPair.h"
#include "Types/EFleetOperatingSystem.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SPathInput;
class SSelectionComboBox;

DECLARE_MULTICAST_DELEGATE(FOnOperatingSystemChanged);

class SDeploymentFields : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SDeploymentFields) {}

	SLATE_END_ARGS()

public:
	enum class Modes
	{
		GameNameOnly,
		Full
	};

	void Construct(const FArguments& InArgs);

	void SetGameOnlyMode();
	void SetFullMode();
	void SetAllFieldsReadOnly(bool ReadOnly);

	void SetBuildName(const FText& Name);
	void SetBuildOperatingSystem(const FText& OSValue);
	void SetBuildFolderPath(const FText& Path);
	void SetBuildFilePath(const FText& Path);
	void SetExtraServerResourcesPath(const FText& Path);
	void SetOutConfigFilePath(const FText& Path);
	void SetDeploymentScenario(const FText& Name);

	const FText& GetBuildName() const;
	FText GetBuildOperatingSystem() const;
	const FText& GetBuildFolderPath() const;
	const FText& GetBuildFilePath() const;
	const FText& GetExtraServerResourcesPath() const;
	const FText& GetOutConfigFilePath() const;
	const FText& GetDeploymentScenario() const;
	Modes GetCurrentState() const;

	FOnOperatingSystemChanged OnOperatingSystemChanged;

private:
	int32 GetSetupStateAsInt() const;

	void OnBuildingSupportedOperatingSystemValues(TArray<FTextIntPair>& Items);
	void OnOperatingSystemSelected(int SelectionId, const FTextIntPair& Item);

private:
	TSharedPtr<SSelectionComboBox> MakeBuildOperatingSystem();

	Modes CachedState = Modes::Full;

	TArray<EFleetOperatingSystem> SupportedOperatingSystems;
	int CurrentOperatingSystemSelected = 0;

	TSharedPtr<SPathInput> BuildNameInput;
	TSharedPtr<SSelectionComboBox> BuildOperatingSystemInput;
	TSharedPtr<SPathInput> BuildFolderPathInput;
	TSharedPtr<SPathInput> BuildFilePathInput;
	TSharedPtr<SPathInput> ExtraServerResourcesPathInput;
	TSharedPtr<SPathInput> OutConfigFilePathInput;
	TSharedPtr<SPathInput> GameClientFilePathInput;
	FText DeploymentScenario;
};

inline TSharedRef<SDeploymentFields> AsSDeploymentFieldsRef(TSharedPtr<SWidget> WidgetToCast)
{
	return StaticCastSharedRef<SDeploymentFields>(WidgetToCast.ToSharedRef());
}

inline TSharedRef<SDeploymentFields> AsSDeploymentFieldsRef(TSharedRef<SWidget> WidgetToCast)
{
	return StaticCastSharedRef<SDeploymentFields>(WidgetToCast);
}
