// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ScenarioWithParameters.h"

#include <regex>

#include "aws/gamelift/core/exports.h"

namespace AwsScenarios
{
	class ScenarioWithGameServer : public ScenarioWithParameters
	{
	public:
		virtual int UploadGameServer(
			IAWSAccountInstance* AwsAccountInstance,
			const std::string& ServerFolderPath,
			const std::string& ExtraServerResourcesPath
		) override
		{
			auto Callback = [](DISPATCH_RECEIVER_HANDLE DispatchReceiver, const char* CharPtr)
				{
					static_cast<ScenarioWithGameServer*>(DispatchReceiver)->SetBuildS3Path(CharPtr);
				};

			return GameLiftAccountUploadGameServerEx(AwsAccountInstance->GetInstance(), ServerFolderPath.c_str(), ExtraServerResourcesPath.c_str(), this, Callback);
		}

		int CreateLaunchPathParameter(
			const FString& InBuildOperatingSystem,
			const FString& InBuildFolderPath,
			const FString& InBuildFilePath,
			std::string& InStdLaunchPathParameter
		)
		{
			FString RelativePath = InBuildFilePath;

			if (!FPaths::IsUnderDirectory(InBuildFilePath, InBuildFolderPath) || !FPaths::MakePathRelativeTo(RelativePath, *InBuildFolderPath))
			{
				UE_LOG(GameLiftCoreLog, Error, TEXT("%s."), Deploy::Logs::kInvalidServerFilePath);
				return GameLift::GAMELIFT_ERROR_GENERAL;
			}

			auto StdServerFileRelPath = Convertors::FSToStdS(RelativePath);
			auto StdServerFileRelPathPosition = StdServerFileRelPath.find("/");

			if (StdServerFileRelPathPosition == std::string::npos)
			{
				UE_LOG(GameLiftCoreLog, Error, TEXT("%s."), Deploy::Logs::kInvalidServerFilePath);
				return GameLift::GAMELIFT_ERROR_GENERAL;
			}

			if (InBuildOperatingSystem.StartsWith(TEXT("WINDOWS")))
			{
				// Windows
				InStdLaunchPathParameter = "C:/game/" + StdServerFileRelPath.substr(StdServerFileRelPathPosition + 1);
				// Convert forward slashes to back slashes ( / -> \ )
				InStdLaunchPathParameter = std::regex_replace(InStdLaunchPathParameter, std::regex("/"), "\\\\");
			}
			else
			{
				// Linux
				InStdLaunchPathParameter = "/local/game/" + StdServerFileRelPath.substr(StdServerFileRelPathPosition + 1);
			}

			return GameLift::GAMELIFT_SUCCESS;
		}
	};
} // namespace AwsScenarios