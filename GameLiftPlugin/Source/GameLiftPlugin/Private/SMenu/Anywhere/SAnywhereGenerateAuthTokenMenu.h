// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SWidgets/SSectionStep.h"
#include "SMenu/SGameLiftSettingsAwsAccountMenu.h"

class SBootstrapStatus;
class SNamedRow;
class STextBlock;

class SAnywhereGenerateAuthTokenMenu : public SSectionStep
{
	SLATE_BEGIN_ARGS(SAnywhereGenerateAuthTokenMenu) {}

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

	void UpdateUI();

	void InvalidateAuthToken();

	// Generate auth token synchronously. Similar to OnGenerateAuthTokenButtonClicked but it does not return immediately.
	// This can be used by caller to continue on some work after auth token is ready.
	bool GenerateAuthTokenSync();

private:
	void OnBootstrapStatusChanged(const SGameLiftSettingsAwsAccountMenu* /* Sender */);

	bool CanGenerateAuthToken() const;

	FReply OnGenerateAuthTokenButtonClicked();

	TTuple<bool, FString> GenerateAuthToken();

	void UpdateErrorMessage(const FText& ErrorMessage);

	void StartSection();

private:
	TSharedPtr<SEditableText> AuthTokenTextView;
	TSharedPtr<STextBlock> AuthTokenNoteView;

	TSharedPtr<SNamedRow> GenerateAuthTokenErrorRow;
	TSharedPtr<STextBlock> GenerateAuthTokenErrorTextBlock;

	bool IsLoading = false;
};
