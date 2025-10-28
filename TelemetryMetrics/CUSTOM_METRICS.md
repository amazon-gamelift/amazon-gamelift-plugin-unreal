# Amazon GameLift Servers SDK for Unreal Engine Metrics API

The Amazon GameLift Servers SDK for Unreal Engine provides a comprehensive metrics system for collecting and sending custom metrics from your game servers to Amazon GameLift. These metrics can be integrated with various visualization and aggregation tools including Amazon Managed Grafana, Prometheus, Amazon CloudWatch, and other monitoring platforms.

## Quick Start

### Initialize the GameLift Telemetry Metrics

**Prerequisites:** Complete [Step 1.2 from METRICS.md](METRICS.md#step-12-include-the-telemetry-metrics-files-in-your-game-server-build) as a prerequisite to get started with the integration of custom telemetry metrics in your game code.

### Game Session Related Custom Metrics

1. Record players joining the game. This will increment the `server_player_count` metric. Override the `PreLogin` function of your GameMode. Change `AGameLiftUE5SampleGameMode` to your game mode class. Add a corresponding function to the header.

```c
void AGameLiftUE5SampleGameMode::PreLogin(
    const FString& Options,
    const FString& Address,
    const FUniqueNetIdRepl& UniqueId,
    FString& ErrorMessage)
{
#if WITH_GAMELIFT
    FGameLiftMetricsModule::Get().OnAcceptPlayerSession();
#endif // WITH_GAMELIFT
}
```

2. Record players leaving the game. This will decrement the `server_player_count` metric. Override the `Logout` function of your GameMode. Change `AGameLiftUE5SampleGameMode` to your game mode class. Add a corresponding function to the header.

```c
void AGameLiftUE5SampleGameMode::Logout(AController* Player)
{
#if WITH_GAMELIFT
    FGameLiftMetricsModule::Get().OnRemovePlayerSession();

    check(GameLiftModule);
#endif // WITH_GAMELIFT
}
```

## Custom metrics via the C++ server SDK API

This SDK uses the same C++ API as defined in the C++ server SDK for Amazon GameLift Servers. See the `METRICS.md` file in the C++ SDK for documenation on the API.

### Adding Custom Metrics

To add custom metrics to your game code, follow these steps:

1. **Define a platform** in your header file:
```cpp
#include "GameLiftMetrics.h"

// Define platform for your custom metrics
GAMELIFT_METRICS_DEFINE_PLATFORM(MyGameMetrics, 1);
```

2. **Declare the metrics** in your header file:
```cpp
GAMELIFT_METRICS_DECLARE_COUNTER(
    MetricAPICalls,
    "api_calls",
    MyGameMetrics,
    Aws::GameLift::Metrics::SampleAll()
);

GAMELIFT_METRICS_DECLARE_COUNTER(
    MetricFailedAPICalls,
    "failed_api_calls", 
    MyGameMetrics,
    Aws::GameLift::Metrics::SampleAll()
);
```

3. **Define the metrics** in your source file:
```cpp
GAMELIFT_METRICS_DEFINE_COUNTER(MetricAPICalls);
GAMELIFT_METRICS_DEFINE_COUNTER(MetricFailedAPICalls);
```

4. **Use the metrics** in your game logic:
```cpp
// Increment API calls counter
GAMELIFT_METRICS_INCREMENT(MetricAPICalls);

// Increment failed API calls counter
GAMELIFT_METRICS_INCREMENT(MetricFailedAPICalls);
```

Available metric types:
- `GAMELIFT_METRICS_DECLARE_COUNTER` - For cumulative values (API calls, failed requests)
- `GAMELIFT_METRICS_DECLARE_GAUGE` - For current state values (active players, memory usage)
- `GAMELIFT_METRICS_DECLARE_TIMER` - For timing measurements (match duration, response time)

These counters, gauges, and timers can be used as per your usecase.

## Unreal Engine API

Once the Unreal Plugin is enabled, any module that wants to use GameLift Metrics must add the `GameLiftMetrics` module as a dependency in the `.Build.cs` file. Then, all the metrics features can be accessed by including the `GameLiftMetrics.h` header.

Outside of custom metrics, there are a few functions that must be called to set up or tear-down the library, and capture session ID or player count. They are all exposed by the `FGameLiftMetricsModule` class in the aforementioned header.


### Access Helpers

Static member helpers exist to ease access to the module.

#### FGameLiftMetricsModule::Load()

Static member function returning a reference to the module and loading it first if needed.

#### FGameLiftMetricsModule::Get()

Static member function returning a reference to the module if loaded. (Assertion triggered otherwise.)

#### FGameLiftMetricsModule::GetPtr()

Static member function returning a pointer to the module if loaded or null if otherwise.

### Initialization and Termination

#### FGameLiftMetricsModule::Initialize()

Initializes the library using the plugin configuration in `Game.ini` or the default values.

#### FGameLiftMetricsModule::Initialize(const FMetricsParameters& metricsParameters)

Initializes the library using user specified configuration.

#### FGameLiftMetricsModule::Terminate()

Tears down the library.

#### FGameLiftMetricsModule::SetMetricsEnabled(bool bEnable)

Enable or disable metrics at runtime. If metrics were previously disabled, this will initialize the metrics system. If metrics were previously enabled, this will terminate the metrics system.

Returns `true` if the operation was successful.

#### FGameLiftMetricsModule::IsMetricsEnabled()

Check if metrics are currently enabled and running.

Returns `true` if metrics are currently enabled and running.

### Sessions

#### FGameLiftMetricsModule::OnStartGameSession(const GameSession&)

Must be called from the start session callback in the Server SDK with the received session data.

Updates the maximum player count for the session.

### Player Events

#### FGameLiftMetricsModule::OnAcceptPlayerSession()

Must be called whenever a player session is accepted.

Increments the player count metric.

#### FGameLiftMetricsModule::OnRemovePlayerSession()

Must be called whenever an existing player session ends.

Decrements the player count metric.

### Configuration

For programmatic initialization, use `const FMetricsParameters& metricsParameters` with the Initialize method:

```cpp
FMetricsParameters metricsParameters;
metricsParameters.m_statsDHost = TEXT("127.0.0.1");
metricsParameters.m_statsDPort = 8125;
metricsParameters.m_crashReporterHost = TEXT("127.0.0.1");
metricsParameters.m_crashReporterPort = 8126;
metricsParameters.m_flushIntervalMs = 10000;
metricsParameters.m_maxPacketSize = 1472;

FGameLiftMetricsModule::Get().Initialize(metricsParameters);
```

Note: If `FGameLiftMetricsModule::Get().Initialize()` is called without `FMetricsParameters metricsParameters`, the metrics module is initialized with evironment variables (if available) or defaults. 

### Pre-defined Platforms

A number of platforms are pre-defined for use in custom metric declarations. See `GameLiftMetricsTypes.h`. See C++ API for usage details.

#### Server

Logs metric for Shipping, Development, and Debug server builds.

#### ServerDevelopment

Logs metric for Development and Debug server builds.

#### ServerDebug

Logs metric for Debug server builds only.

## Out of the Box Metrics

The following metrics are automatically gathered by the Unreal SDK.
Corresponding header names are recorded. Stats can be modified by the user by
customizing the declarations. For instance, they can be disabled by changing
`SampleAll` to `SampleNone`.

### Memory

Header: `Private/MemoryStats.h`.

- `server_mem_physical_total`
  Total physical memory on the server machine.
- `server_mem_physical_available`
  Total physical memory available to the server process.
- `server_mem_physical_used`
  Total physical memory used by the server process.

- `server_mem_virtual_total`
  Total virtual memory on the server machine.
- `server_mem_virtual_available`
  Total virtual memory available to the server process.
- `server_mem_virtual_used`
  Total virtual memory used by the server process.

### Network

Header: `Private/NetworkStats.h`.

- `server_connections`
  The number of clients connected to the server. The source for this metric is Unreal's `UNetDriver` class. Thus it is available regardless of whether the SDK user has implemented `OnAcceptPlayerSession` and `OnRemovePlayerSession`.

- `server_bytes_in`
  Counts the total bytes received by the server.
- `server_bytes_out`
  Counts the total bytes sent by the server.

- `server_packets_in`
  Counts the number of packets received by the server.
- `server_packets_in_lost`
  Counts the number of packets sent by the clients that never reached the server. (They were dropped.)

- `server_packets_out`
  Counts the number of packets sent by the server.
- `server_packets_out_lost`
  Counts the number of packets sent by the server that never reached the clients. (They were dropped.)

### Timing

Header: `Private/TimeStats.h`.

> [!NOTE]
> Each of these timing metrics has 3 companion metrics to indicate the 50th, 90th and 95th percentiles suffixed with `_p50`, `_p90` and `_p95` respectively

- `server_delta_time`
  Time between server ticks in milliseconds. When the server is performing well, this will hover around the configured timestep. (`30hz -> 33.33333ms` by default.) If the server is severely underperforming, this time will exceed the desired timestep and reduce the tick rate. The instantaneous tick rate can be computed as the reciprocal of this value: `1000 / server_delta_time`.

- `server_tick_time`
  Time a server tick takes to process in milliseconds. This is usually below or the same as `server_delta_time` unless the server is severely underperforming. (`server_tick_time <= server_delta_time`.) Once a tick is processed, the remainder of `server_delta_time` is spent waiting for the next tick to begin. On Shipping builds this value is accurately estimated using delta time and `IdleTime` which refers to the time spent waiting. On builds with `USE_FORCE_STATS` or built with `Development` or `Debug` configurations, this is precisely recorded using Unreal's STAT counters.

- `server_world_time`
  Time a server tick takes to process the currently loaded level. This includes the time spent on Actor or Blueprint ticks. Subset of `server_tick_time`. (`server_world_time <= server_tick_time`.) The remainder of `server_tick_time` is spent processing network replications, physics, and other engine subsystems. **Important**: this metric is only available in `Debug` or `Development` builds. On `Shipping` builds it is only recorded when `USE_FORCE_STATS=1` as it depends on Engine STAT instrumentation.

### Default tags

By default all metrics are tagged with:

- `process_id`
  Server process ID set via `GAMELIFT_SDK_PROCESS_ID` environment variable.
- `fleet_id`
  Server host ID set via `GAMELIFT_SDK_PROCESS_ID` environment variable.

If the corresponding environment variable is missing, the tag is not set for metrics.

See `GameLiftMetrics.cpp` for implementation details.

### Player Count

The SDK records the following player count metrics:

- `server_max_players`
  The maximum players for the server instance. The SDK User **must** call `FGameLiftMetricsModule::OnStartGameSession` from the `GameLiftServerSDK`'s `OnStartGameSession` callback. If not called, the metric will not be recorded.
- `server_players`
  The currently _active_ player sessions. The SDK User **must** call `FGameLiftMetricsModule::OnAcceptPlayerSession` and `FGameLiftMetricsModule::OnRemovePlayerSession` whenever they would accept or remove the GameLift player session. (Or third-party matchmaking if applicable.) If these functions are not called - the metric is not recorded. This metric is distinct from `server_connections`. `server_connections` is _always_ recorded as it uses the Engine network stats as the ground truth. For example, it is updated whether the player connects using the `GameLift` player session APIs or a direct IP connection.

## Unreal Stat System

A couple features rely on [Unreal's STAT system](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-stats-system-overview) to operate:

- `server_tick_time` uses an _exact_ timing when available and falls back to an accurate estimate when not.
- `server_world_time` is only recorded when the stat system is enabled as it relies on Engine instrumentation.

By default the system is enabled for `Debug` and `Development` builds. It can be enabled for `Shipping` builds by setting the `USE_FORCE_STATS` preprocessor definition to `1` in a `.Build.cs` file. (Best: the `.Build.cs` of the game module.)
