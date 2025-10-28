/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

using System.IO;
using UnrealBuildTool;


public class GameLiftServerSDK : ModuleRules
{
    public GameLiftServerSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "Projects", "OpenSSL" });
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;
        bUseRTTI = true;

        // Disable windows min/max macros
        PublicDefinitions.Add("NOMINMAX");

        if (Target.Type == TargetRules.TargetType.Server)
        {
            PublicDefinitions.Add("WITH_GAMELIFT=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_GAMELIFT=0");
        }
	
        // Isolate asio namespace to improve packaging compatibility with other modules
        PrivateDefinitions.Add("asio=gamelift_asio");
        PrivateDefinitions.Add("asio_signal_handler=gamelift_asio_signal_handler");

        PrivateDefinitions.Add("AWS_GAMELIFT_EXPORTS");
        PrivateDefinitions.Add("ASIO_STANDALONE=1");
        PublicDefinitions.Add("SPDLOG_NO_EXCEPTIONS");

        // std::invoke_result replaces std::result_of for C++17 and later
        // Asio only auto-detects this for MSVC, so override for all compilers
        if (Target.CppStandard >= CppStandardVersion.Cpp17) {
            PrivateDefinitions.Add("ASIO_HAS_STD_INVOKE_RESULT");
        }

        PrivateDefinitions.Add("USE_IMPORT_EXPORT=1");
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateDefinitions.Add("_WEBSOCKETPP_CPP11_STRICT_=1");
            PrivateDefinitions.Add("SPDLOG_WCHAR_TO_UTF8_SUPPORT=1");
            PrivateDefinitions.AddRange(new string[] {
                "_WIN32_WINNT_WIN10_TH2=0x0A00",
                "_WIN32_WINNT_WIN10_RS1=0x0A00",
                "_WIN32_WINNT_WIN10_RS2=0x0A00",
                "_WIN32_WINNT_WIN10_RS3=0x0A00",
                "_WIN32_WINNT_WIN10_RS4=0x0A00",
                "_WIN32_WINNT_WIN10_RS5=0x0A00",
            });
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux || Target.Platform == UnrealTargetPlatform.LinuxArm64)
        {
            PrivateDefinitions.Add("ASIO_DISABLE_CO_AWAIT");
            PrivateDefinitions.Add("RAPIDJSON_NOMEMBERITERATORCLASS");
        }

        string SpdlogPath = Path.Combine(ModuleDirectory, "../../ThirdParty/spdlog/include");
        string SpdlogSrcPath = Path.Combine(ModuleDirectory, "../../ThirdParty/spdlog/src");
        string RapidJSONPath = Path.Combine(ModuleDirectory, "../../ThirdParty/rapidjson/include");
        string AsioPath = Path.Combine(ModuleDirectory, "../../ThirdParty/asio/include");
        string WebSocketPPPath = Path.Combine(ModuleDirectory, "../../ThirdParty/websocketpp");
        string ConcurrentQueuePath = Path.Combine(ModuleDirectory, "../../ThirdParty/concurrentqueue");

        PublicIncludePaths.Add(SpdlogPath);
        PublicIncludePaths.Add(SpdlogSrcPath);
        PrivateIncludePaths.Add(RapidJSONPath);
        PrivateIncludePaths.Add(AsioPath);
        PrivateIncludePaths.Add(WebSocketPPPath);
        PrivateIncludePaths.Add(ConcurrentQueuePath);
    }
}
