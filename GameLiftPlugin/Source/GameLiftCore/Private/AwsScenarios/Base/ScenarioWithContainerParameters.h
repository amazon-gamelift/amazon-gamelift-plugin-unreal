// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Base/ScenarioWithGameServer.h"
#include "GameLiftCoreConstants.h"
#include "ScenarioWithParameters.h"
#include "GameLiftCore/Private/AwsScenarios/IAwsScenario.h"
#include "GameLiftCore/Public/IAWSAccountInstance.h"
#include "aws/gamelift/core/errors.h"

namespace AwsScenarios
{
	class ScenarioWithContainerParameters : public ScenarioWithParameters
	{
	public:
		virtual const char* const* GetParamNames() const override
		{
			static const char* paramNames[] =
			{
				"AccountId",
				"ApiGatewayStageNameParameter",
				"ContainerGroupDefinitionNameParameter",
				"ContainerImageNameParameter",
				"ContainerImageUriParameter",
				"GameNameParameter",
				"LambdaZipS3BucketParameter",
				"LambdaZipS3KeyParameter",
				"LaunchPathParameter",
				"FleetUdpFromPortParameter",
				"FleetUdpToPortParameter",
				"TotalVcpuLimitParameter",
				"TotalMemoryLimitParameter",
				"UnrealEngineVersionParameter",
				"EnableMetricsParameter"
			};
			return paramNames;
		}

		virtual int SaveFeatureInstanceTemplate(IAWSAccountInstance* AwsAccountInstance,
			const TMap<FString, FString>& InParams) override
		{
			ContainerInstanceTemplateParams TemplateParams;
			TemplateParams.FromMap(InParams);

			const char* ParamValues[] =
			{
				TemplateParams.AccountId.c_str(),
				TemplateParams.ApiGatewayStageNameParameter.c_str(),
				TemplateParams.ContainerGroupDefinitionNameParameter.c_str(),
				TemplateParams.ContainerImageNameParameter.c_str(),
				TemplateParams.ContainerImageUriParameter.c_str(),
				TemplateParams.GameNameParameter.c_str(),
				TemplateParams.LambdaZipS3BucketParameter.c_str(),
				TemplateParams.LambdaZipS3KeyParameter.c_str(),
				TemplateParams.LaunchPathParameter.c_str(),
				TemplateParams.FleetUdpFromPortParameter.c_str(),
				TemplateParams.FleetUdpToPortParameter.c_str(),
				TemplateParams.TotalVcpuLimitParameter.c_str(),
				TemplateParams.TotalMemoryLimitParameter.c_str(),
				TemplateParams.UnrealEngineVersionParameter.c_str(),
				TemplateParams.EnableMetricsParameter.c_str()
			};

			auto* ParamNames = GetParamNames();
			size_t Size = sizeof(ParamValues) / sizeof(*ParamValues);

			return GameLiftAccountSaveFeatureInstanceTemplates(AwsAccountInstance->GetInstance(), ParamNames, ParamValues, Size);
		}

		virtual int UploadGameServer(IAWSAccountInstance* AwsAccountInstance, const std::string& ServerFolderPath, const std::string& ExtraServerResourcesPath) override
		{
			return GameLift::GAMELIFT_SUCCESS;
		};
	};
} // namespace AwsScenarios