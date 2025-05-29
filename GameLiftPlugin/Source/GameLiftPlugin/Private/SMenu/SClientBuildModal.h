// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "GameLiftPluginConstants.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"

class SWindow;
class SPathInput;

DECLARE_DELEGATE_TwoParams(FStartClient, FString, FString);

class SClientBuildModal : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SClientBuildModal) {}

    SLATE_ARGUMENT(FString, DefaultClientBuildExecutablePath)

    SLATE_ARGUMENT(FString, DefaultClientBuildLauncherArguments)

    SLATE_ARGUMENT(TWeakPtr<SWidget>, ParentWidget)

    SLATE_EVENT(FStartClient, OnStartClientClickedDelegate)

    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs);
    void ShowModal();
    void CloseModal();

private:
    FReply OnStartClientClicked();
    FReply OnCloseButtonClicked();
    void SetParentWindow();
    void OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu*);
    void OnGameClientExecutablePathInputUpdated(const FString& NewPath);
    TSharedRef<SWidget> CreateGameClientPathInput();
    TSharedRef<SWidget> CreateGameClientLauncherArgumentsInput();

private:
    TSharedPtr<SPathInput> GameClientExecutablePathInput;
    TSharedPtr<SEditableTextBox> GameClientLauncherArgumentsInput;
    TSharedPtr<SWindow> OwnerWindow;
    TWeakPtr<SWidget> ParentWidget;
    TSharedPtr<SButton> StartClient;
    FString ClientBuildExecutablePathToUpdate;
    FString ClientBuildExecutablePath;
    FString ClientBuildLauncherArguments;
    FStartClient OnStartClientClickedDelegate;
    const int ModalWindowWidth = 600;
};