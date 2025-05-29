// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <unordered_map>

namespace Aws
{
	namespace Regions
	{
		static constexpr auto k_us_east_2 = TEXT("us-east-2");
		static constexpr auto k_us_east_1 = TEXT("us-east-1");
		static constexpr auto k_us_west_1 = TEXT("us-west-1");
		static constexpr auto k_us_west_2 = TEXT("us-west-2");
		static constexpr auto k_af_south_1 = TEXT("af-south-1");
		static constexpr auto k_ap_east_1 = TEXT("ap-east-1");
		static constexpr auto k_ap_southeast_3 = TEXT("ap-southeast-3");
		static constexpr auto k_ap_south_1 = TEXT("ap-south-1");
		static constexpr auto k_ap_northeast_3 = TEXT("ap-northeast-3");
		static constexpr auto k_ap_northeast_2 = TEXT("ap-northeast-2");
		static constexpr auto k_ap_southeast_1 = TEXT("ap-southeast-1");
		static constexpr auto k_ap_southeast_2 = TEXT("ap-southeast-2");
		static constexpr auto k_ap_northeast_1 = TEXT("ap-northeast-1");
		static constexpr auto k_ca_central_1 = TEXT("ca-central-1");
		static constexpr auto k_eu_central_1 = TEXT("eu-central-1");
		static constexpr auto k_eu_west_1 = TEXT("eu-west-1");
		static constexpr auto k_eu_west_2 = TEXT("eu-west-2");
		static constexpr auto k_eu_south_1 = TEXT("eu-south-1");
		static constexpr auto k_eu_west_3 = TEXT("eu-west-3");
		static constexpr auto k_eu_north_1 = TEXT("eu-north-1");
		static constexpr auto k_me_south_1 = TEXT("me-south-1");
		static constexpr auto k_sa_east_1 = TEXT("sa-east-1");
	}

	namespace RegionNames
	{
		static constexpr auto k_us_east_2 = TEXT("US East (Ohio)");
		static constexpr auto k_us_east_1 = TEXT("US East (N. Virginia)");
		static constexpr auto k_us_west_1 = TEXT("US West (N. California)");
		static constexpr auto k_us_west_2 = TEXT("US West (Oregon)");
		static constexpr auto k_af_south_1 = TEXT("Africa (Cape Town)");
		static constexpr auto k_ap_east_1 = TEXT("Asia Pacific (Hong Kong)");
		static constexpr auto k_ap_southeast_3 = TEXT("Asia Pacific (Jakarta)");
		static constexpr auto k_ap_south_1 = TEXT("Asia Pacific (Mumbai)");
		static constexpr auto k_ap_northeast_3 = TEXT("Asia Pacific (Osaka)");
		static constexpr auto k_ap_northeast_2 = TEXT("Asia Pacific (Seoul)");
		static constexpr auto k_ap_southeast_1 = TEXT("Asia Pacific (Singapore)");
		static constexpr auto k_ap_southeast_2 = TEXT("Asia Pacific (Sydney)");
		static constexpr auto k_ap_northeast_1 = TEXT("Asia Pacific (Tokyo)");
		static constexpr auto k_ca_central_1 = TEXT("Canada (Central)");
		static constexpr auto k_eu_central_1 = TEXT("Europe (Frankfurt)");
		static constexpr auto k_eu_west_1 = TEXT("Europe (Ireland)");
		static constexpr auto k_eu_west_2 = TEXT("Europe (London)");
		static constexpr auto k_eu_south_1 = TEXT("Europe (Milan)");
		static constexpr auto k_eu_west_3 = TEXT("Europe (Paris)");
		static constexpr auto k_eu_north_1 = TEXT("Europe (Stockholm)");
		static constexpr auto k_me_south_1 = TEXT("Middle East (Bahrain)");
		static constexpr auto k_sa_east_1 = TEXT("South America (São Paulo)");
	}

	enum class ERegions
	{
		us_east_2,
		us_east_1,
		us_west_1,
		us_west_2,
		af_south_1,
		ap_east_1,
		ap_southeast_3,
		ap_south_1,
		ap_northeast_3,
		ap_northeast_2,
		ap_southeast_1,
		ap_southeast_2,
		ap_northeast_1,
		ca_central_1,
		eu_central_1,
		eu_west_1,
		eu_west_2,
		eu_south_1,
		eu_west_3,
		eu_north_1,
		me_south_1,
		sa_east_1
	};

	static FString RegionToString(ERegions InRegion)
	{
#define CASE_REGION(__v__) case ERegions::__v__: return Regions::k_##__v__

		switch (InRegion)
		{
			CASE_REGION(us_east_2);
			CASE_REGION(us_east_1);
			CASE_REGION(us_west_1);
			CASE_REGION(us_west_2);
			CASE_REGION(af_south_1);
			CASE_REGION(ap_east_1);
			CASE_REGION(ap_southeast_3);
			CASE_REGION(ap_south_1);
			CASE_REGION(ap_northeast_3);
			CASE_REGION(ap_northeast_2);
			CASE_REGION(ap_southeast_1);
			CASE_REGION(ap_southeast_2);
			CASE_REGION(ap_northeast_1);
			CASE_REGION(ca_central_1);
			CASE_REGION(eu_central_1);
			CASE_REGION(eu_west_1);
			CASE_REGION(eu_west_2);
			CASE_REGION(eu_south_1);
			CASE_REGION(eu_west_3);
			CASE_REGION(eu_north_1);
			CASE_REGION(me_south_1);
			CASE_REGION(sa_east_1);
		default:
			break;
		}
		ensure(false);
		return "UnknownRegion";

#undef CASE_REGION
	}

	static FString RegionToNameString(ERegions InRegion)
	{
#define CASE_REGION(__v__) case ERegions::__v__: return RegionNames::k_##__v__

		switch (InRegion)
		{
			CASE_REGION(us_east_2);
			CASE_REGION(us_east_1);
			CASE_REGION(us_west_1);
			CASE_REGION(us_west_2);
			CASE_REGION(af_south_1);
			CASE_REGION(ap_east_1);
			CASE_REGION(ap_southeast_3);
			CASE_REGION(ap_south_1);
			CASE_REGION(ap_northeast_3);
			CASE_REGION(ap_northeast_2);
			CASE_REGION(ap_southeast_1);
			CASE_REGION(ap_southeast_2);
			CASE_REGION(ap_northeast_1);
			CASE_REGION(ca_central_1);
			CASE_REGION(eu_central_1);
			CASE_REGION(eu_west_1);
			CASE_REGION(eu_west_2);
			CASE_REGION(eu_south_1);
			CASE_REGION(eu_west_3);
			CASE_REGION(eu_north_1);
			CASE_REGION(me_south_1);
			CASE_REGION(sa_east_1);
		default:
			break;
		}
		ensure(false);
		return "UnknownRegionName";

#undef CASE_REGION
	}

	static ERegions RegionFromString(const FString& InRegionString)
	{
#define ADD_REGION(__v__) { Regions::k_##__v__, ERegions::__v__ }

		static std::unordered_map<std::wstring, ERegions> RegionMap
		{
			ADD_REGION(us_east_2),
			ADD_REGION(us_east_1),
			ADD_REGION(us_west_1),
			ADD_REGION(us_west_2),
			ADD_REGION(af_south_1),
			ADD_REGION(ap_east_1),
			ADD_REGION(ap_southeast_3),
			ADD_REGION(ap_south_1),
			ADD_REGION(ap_northeast_3),
			ADD_REGION(ap_northeast_2),
			ADD_REGION(ap_southeast_1),
			ADD_REGION(ap_southeast_2),
			ADD_REGION(ap_northeast_1),
			ADD_REGION(ca_central_1),
			ADD_REGION(eu_central_1),
			ADD_REGION(eu_west_1),
			ADD_REGION(eu_west_2),
			ADD_REGION(eu_south_1),
			ADD_REGION(eu_west_3),
			ADD_REGION(eu_north_1),
			ADD_REGION(me_south_1),
			ADD_REGION(sa_east_1)
		};
		auto It = RegionMap.find(*InRegionString);
		return It != RegionMap.end() ? It->second : ERegions::us_east_2;

#undef ADD_REGION
	}

} // namespace Aws
