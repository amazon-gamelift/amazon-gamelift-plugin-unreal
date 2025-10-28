// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "AwsScenarios/IAWSScenario.h"

#include "aws/gamelift/core/exports.h"

namespace AwsScenarios
{
	class ScenarioWithParameters : public IAWSScenario
	{
	public:
		virtual ~ScenarioWithParameters() = default;

		virtual const char* const* GetParamNames() const override
		{
			static const char* paramNames[] =
			{
				"GameNameParameter",
				"BuildOperatingSystemParameter",
				"BuildS3BucketParameter",
				"LambdaZipS3BucketParameter",
				"LambdaZipS3KeyParameter",
				"ApiGatewayStageNameParameter",
				"AccountId",
				"LaunchPathParameter",
				"BuildS3KeyParameter",
				"UnrealEngineVersionParameter",
				"EnableMetricsParameter"
			};
			return paramNames;
		}

		virtual int SaveFeatureInstanceTemplate(IAWSAccountInstance* AwsAccountInstance, const TMap<FString, FString>& InParams) override
		{
			ManagedEC2InstanceTemplateParams TemplateParams;
			TemplateParams.FromMap(InParams);

			const char* ParamValues[] =
			{
				TemplateParams.GameNameParameter.c_str(),
				TemplateParams.BuildOperatingSystemParameter.c_str(),
				TemplateParams.BuildS3BucketParameter.c_str(),
				TemplateParams.LambdaZipS3BucketParameter.c_str(),
				TemplateParams.LambdaZipS3KeyParameter.c_str(),
				TemplateParams.ApiGatewayStageNameParameter.c_str(),
				TemplateParams.AccountId.c_str(),
				TemplateParams.LaunchPathParameter.c_str(),
				BuildS3Path.c_str(),
				TemplateParams.UnrealEngineVersionParameter.c_str(),
				TemplateParams.EnableMetricsParameter.c_str()
			};

			auto* ParamNames = GetParamNames();
			size_t Size = sizeof(ParamValues) / sizeof(*ParamValues);

			return GameLiftAccountSaveFeatureInstanceTemplates(AwsAccountInstance->GetInstance(), ParamNames, ParamValues, Size);
		}

		virtual FString GetScenarioPath() const override
		{
			return Paths::ScenarioPath(GetFolderName());
		}

		virtual FString GetScenarioInstancePath() const override
		{
			return Paths::ScenarioInstancePath(GetFolderName());
		}

		void SetBuildS3Path(const std::string& NewBuildS3Path)
		{
			BuildS3Path = NewBuildS3Path;
		}

	private:
		std::string BuildS3Path;
	};
} // namespace AwsScenarios