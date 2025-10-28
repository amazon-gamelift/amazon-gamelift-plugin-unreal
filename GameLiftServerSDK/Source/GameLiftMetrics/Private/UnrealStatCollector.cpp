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

#include "UnrealStatCollector.h"

#include "Runtime/Launch/Resources/Version.h"

#include "TimeStats.h"

#include "Async/TaskGraphInterfaces.h"
#include "Stats/Stats.h"
#include "Stats/StatsData.h"
#include "EngineStats.h"


#if STATS
namespace
{
#if ENGINE_MAJOR_VERSION >= 5
    void IncrementStatsRefCount()
    {
        StatsPrimaryEnableAdd();
    }

    void DecrementStatsRefCount()
    {
        StatsPrimaryEnableSubtract();
    }
#else
    void IncrementStatsRefCount()
    {
        StatsMasterEnableAdd();
    }

    void DecrementStatsRefCount()
    {
        StatsMasterEnableSubtract();
    }
#endif
}

void FUnrealStatCollector::Subscribe()
{
    IncrementStatsRefCount();

/*
 * From 5.2.0 onwards, UE_STATS_THREAD_AS_PIPE is undefined and the only way to
 * access the stat system is via the new API.
 *
 * Between 5.0.0 and 5.2.0, UE_STATS_THREAD_AS_PIPE enables new API. If it is 0,
 * the old API can be used.
 *
 * See UE engine source commits 10b5e05d00df4f23d3b75f6af5ec08f8ae0e8618
 *                              327a94d1a4e021714f2c3ca820fe9ed5606465bf
 *                              d48f6660b807c7377926cffd12020645f91772f8
 *
 * All references to UE_STATS_THREAD_AS_PIPE are removed in
 * d48f6660b807c7377926cffd12020645f91772f8.
 */
#if ENGINE_MAJOR_VERSION >= 5 && (ENGINE_MINOR_VERSION >= 2 || UE_STATS_THREAD_AS_PIPE)
    {
        auto& StatsState = FStatsThreadState::GetLocalState();
        StatsState.NewFrameDelegate.AddSP(this, &FUnrealStatCollector::OnNewFrame);
    }
#else
    FSimpleDelegateGraphTask::CreateAndDispatchWhenReady
    (
        FSimpleDelegateGraphTask::FDelegate::CreateLambda(
            [WeakCollector = TWeakPtr<FUnrealStatCollector>(this->AsShared())]() {
                if (!WeakCollector.IsValid()) { return; }

                auto& StatsState = FStatsThreadState::GetLocalState();
                StatsState.NewFrameDelegate.AddSP(WeakCollector.Pin().Get(), &FUnrealStatCollector::OnNewFrame);
            }
        ),
        TStatId{},
        nullptr,
        /*
         * StatsThread is deprecated since 5.0
         *
         * Need to verify post-5.0 behaviour but:
         * - 4.27 to 5.0 should use the current method to subscribe to NewFrameDelegate
         * - 5.0 on can subscribe from game thread directly. (See FThreadStats::bUseThreadAsPipe)
         */
        ENamedThreads::StatsThread
    );
#endif // UE_STATS_THREAD_AS_PIPE
}

void FUnrealStatCollector::Unsubscribe()
{
    DecrementStatsRefCount();
}

void FUnrealStatCollector::OnNewFrame(int64 Frame)
{
    TArray<FStatMessage> OutStats;

    auto& Stats = FStatsThreadState::GetLocalState();
    Stats.GetInclusiveAggregateStackStats(Frame, OutStats);

    for (auto& Message : OutStats)
    {
        // GetShortName() is expensive and we're calling it for every stat.
        // In practice: we'll stick filter by GetRawName first and stick things in a hashmap
        //              might also be able to compare FName indices
        FName ShortName = Message.NameAndInfo.GetShortName();

        // Robustness: we can check the value type from message before blindly grabbing duration (or other types)

        // Consult EngineStats.h for stat names
        // (There's also many other stats in other subsystems.)
        if (ShortName == TEXT("STAT_GameEngineTick"))
        {
            GAMELIFT_METRICS_SET_MS(Aws::GameLift::Metrics::MetricGameTickTime, FPlatformTime::ToMilliseconds(Message.GetValue_Duration()));
        }
        else if (ShortName == TEXT("STAT_WorldTickTime"))
        {
            GAMELIFT_METRICS_SET_MS(Aws::GameLift::Metrics::MetricWorldTickTime, FPlatformTime::ToMilliseconds(Message.GetValue_Duration()));
        }
    }
}
#else
void FUnrealStatCollector::Subscribe() {}
void FUnrealStatCollector::Unsubscribe() {}
void FUnrealStatCollector::OnNewFrame(int64 Frame) {}
#endif // STATS
