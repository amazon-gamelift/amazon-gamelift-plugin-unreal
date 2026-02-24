// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GameLiftPluginConstants.h"

enum class EFleetOperatingSystem : int
{
	Windows2016,
	Windows2022,
	AmazonLinux2,
	AmazonLinux2023
};

inline TArray<EFleetOperatingSystem> MakeSupportedOperatingSystemList()
{
	TArray<EFleetOperatingSystem> OSList;
	OSList.Add(EFleetOperatingSystem::Windows2016);
	OSList.Add(EFleetOperatingSystem::Windows2022);
	OSList.Add(EFleetOperatingSystem::AmazonLinux2);
	OSList.Add(EFleetOperatingSystem::AmazonLinux2023);
	return OSList;
}

inline FText EFleetOperatingSystemToName(EFleetOperatingSystem OSType)
{
	switch (OSType)
	{
		case EFleetOperatingSystem::Windows2016:
			return Menu::DeployManagedEC2::kWindowsServer2016Name;
		case EFleetOperatingSystem::Windows2022:
			return Menu::DeployManagedEC2::kWindowsServer2022Name;
		case EFleetOperatingSystem::AmazonLinux2:
			return Menu::DeployManagedEC2::kAmazonLinux2Name;
		case EFleetOperatingSystem::AmazonLinux2023:
			return Menu::DeployManagedEC2::kAmazonLinux2023Name;
		default:
			return Menu::DeployManagedEC2::kWindowsServer2016Name;
	}
}

inline FText EFleetOperatingSystemToValue(EFleetOperatingSystem OSType)
{
	switch (OSType)
	{
	case EFleetOperatingSystem::Windows2016:
		return Menu::DeployManagedEC2::kWindowsServer2016Value;
	case EFleetOperatingSystem::Windows2022:
		return Menu::DeployManagedEC2::kWindowsServer2022Value;
	case EFleetOperatingSystem::AmazonLinux2:
		return Menu::DeployManagedEC2::kAmazonLinux2Value;
	case EFleetOperatingSystem::AmazonLinux2023:
		return Menu::DeployManagedEC2::kAmazonLinux2023Value;
	default:
		return Menu::DeployManagedEC2::kWindowsServer2016Value;
	}
}

inline EFleetOperatingSystem EFleetOperatingSystemFromValueText(const FText& OSValue)
{
	if (OSValue.EqualTo(Menu::DeployManagedEC2::kWindowsServer2016Value))
	{
		return EFleetOperatingSystem::Windows2016;
	}
	else if (OSValue.EqualTo(Menu::DeployManagedEC2::kWindowsServer2022Value))
	{
		return EFleetOperatingSystem::Windows2022;
	}
	else if (OSValue.EqualTo(Menu::DeployManagedEC2::kAmazonLinux2Value))
	{
		return EFleetOperatingSystem::AmazonLinux2;
	}
	else if (OSValue.EqualTo(Menu::DeployManagedEC2::kAmazonLinux2023Value))
	{
		return EFleetOperatingSystem::AmazonLinux2023;
	}
	else
	{
		return EFleetOperatingSystem::Windows2016;
	}
}
