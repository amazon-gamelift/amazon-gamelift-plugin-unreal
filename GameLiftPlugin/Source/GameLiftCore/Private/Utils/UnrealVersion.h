// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersion.h"

namespace UnrealVersion
{
    /**
     * Represents a semantic version with Major, Minor, and Patch components
     */
    struct Version {
        int32 Major;
        int32 Minor;
        int32 Patch;

        bool IsOlderThan(const Version& Other) const
        {
            if (Major != Other.Major) return Major < Other.Major;
            if (Minor != Other.Minor) return Minor < Other.Minor;
            return Patch < Other.Patch;
        }

        bool IsSameAs(const Version& Other) const
        {
            return Major == Other.Major &&
                Minor == Other.Minor &&
                Patch == Other.Patch;
        }

        bool IsNewerThan(const Version& Other) const
        {
            if (Major != Other.Major) return Major > Other.Major;
            if (Minor != Other.Minor) return Minor > Other.Minor;
            return Patch > Other.Patch;
        }

        // Operator overloads
        bool operator<(const Version& Other) const { return IsOlderThan(Other); }
        bool operator>(const Version& Other) const { return IsNewerThan(Other); }
        bool operator==(const Version& Other) const { return IsSameAs(Other); }
        bool operator!=(const Version& Other) const { return !IsSameAs(Other); }
        bool operator<=(const Version& Other) const { return IsOlderThan(Other) || IsSameAs(Other); }
        bool operator>=(const Version& Other) const { return IsNewerThan(Other) || IsSameAs(Other); }
    };

    // Predefine Unreal Versions
    inline const Version INVALID_VERSION{ 0, 0, 0 };
    inline const Version UE5_6_0{ 5, 6, 0 };

}; // namespace UnrealVersion

namespace UnrealVersionUtils
{
    /**
     * Gets the current Unreal Engine version
     * @return The current engine version
     */
    inline const FString GetCurrentEngineVersion()
    {
        FEngineVersion CurrentVersion = FEngineVersion::Current();
        FString VersionString = FString::Printf(TEXT("%d.%d.%d"),
            CurrentVersion.GetMajor(),
            CurrentVersion.GetMinor(),
            CurrentVersion.GetPatch());

        return VersionString;
    }

    inline UnrealVersion::Version ParseVersionString(FString& unrealStr)
    {
        TArray<FString> parts;
        unrealStr.ParseIntoArray(parts, TEXT("."), true);

        if (parts.Num() != 3)
        {
            return UnrealVersion::INVALID_VERSION;
        }

        // Convert strings to integers
        int32 major = FCString::Atoi(*parts[0]);
        int32 minor = FCString::Atoi(*parts[1]);
        int32 patch = FCString::Atoi(*parts[2]);

        return UnrealVersion::Version{ major, minor, patch };
    }

    /**
     * Converts a Version to a string representation
     * @param Version The version to convert
     * @return Version as a string in format "Major.Minor.Patch"
     */
    inline FString GetVersionString(UnrealVersion::Version& version) {

        FString VersionString = FString::Printf(TEXT("%d.%d.%d"),
            version.Major,
            version.Minor,
            version.Patch);

        return VersionString;
    }
} // namespace UnrealVersionUtils
