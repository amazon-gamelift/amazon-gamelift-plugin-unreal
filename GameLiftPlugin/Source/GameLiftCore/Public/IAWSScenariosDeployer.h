// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once 

#include "IAWSAccountInstance.h"

enum class IAWSScenariosCategory : unsigned int
{
	ManagedEC2 = 0,
	Containers = 1
};

class IAWSScenariosDeployer
{
public:
	virtual ~IAWSScenariosDeployer() = default;

	virtual bool DeployManagedEC2Scenario(
		const FText& Scenario,
		IAWSAccountInstance* AccountInstance,
		const FString& GameName,
		const FString& BuildOperatingSystem,
		const FString& BuildFolderPath,
		const FString& BuildFilePath,
		const FString& OutConfigFilePath,
		const FString& ExtraServerResourcesPath,
		bool EnableMetrics
	) = 0;
	virtual bool DeployCustomScenario(
		const FString& CustomScenarioPath,
		IAWSAccountInstance* AccountInstance,
		const FString& GameName,
		const FString& BuildOperatingSystem,
		const FString& BuildFolderPath,
		const FString& BuildFilePath,
		const FString& OutConfigFilePath,
		const FString& ExtraServerResourcesPath
	) = 0;
	virtual bool DeployContainerScenario(
		const FText& Scenario,
		IAWSAccountInstance* AccountInstance,
		const FString& ContainerGroupDefinitionName,
		const FString& ContainerImageName,
		const FString& ContainerImageUri,
		const FString& IntraContainerLaunchPath,
		const FString& GameName, const FString& OutConfigFilePath,
		const FText& ConnectionPortRange,
		const FString& TotalVcpuLimit,
		const FString& TotalMemoryLimit,
		bool EnableMetrics
	) = 0;
	virtual bool StopDeployment(IAWSAccountInstance* AwsAccountInstance) = 0;

	virtual FString GetLastCognitoClientId() const = 0;
	virtual FString GetLastApiGatewayEndpoint() const = 0;
	virtual FString GetLastError() const = 0;
	virtual FString GetLastErrorMessage() const = 0;

	virtual TArray<FText> GetScenarios(const IAWSScenariosCategory Category) const = 0;
	virtual FText GetToolTip(const FText& ScenarioName, const IAWSScenariosCategory Category) const = 0;
};