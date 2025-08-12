CHANGELOG

# 3.0.1 (8/12/2025)
- Support the Unreal Plugin and Server SDK on Unreal Engine 5.6
- Fix compilation errors when packaging Android client targets.

# 3.0.0 (5/29/2025)
- In the Editor's plugin window for Managed EC2:
  - The game client path field now allows spaces.
  - The server build path field now allows spaces.
  - A new text field lets you add command-line arguments for use when launching the client from the Editor.
- Supports ARM game server builds for UE5.
- Fixes MSVC 5130 Macro Warnings.
- Merges the two Unreal plugins (lightweight and standalone) into one repository.
- Removes the need to manually download and build the server SDK. The new plugin version now includes the server SDK source code, which is automatically built along with the Unreal project using the Unreal Build Tool.
  - CMake is no longer a prerequisite.
- Removes the need to install OpenSSL. The new plugin uses OpenSSL source in the Unreal Engine with static linking.
  - When packaging a build for Amazon GameLift Servers, you no longer need to include the libcrypto and libssl files into your game server build.
- Integrates the Unreal cross-compiling toolkit that enables cross-compiled builds on native Windows and Linux. Removes the need for special steps, including the Amazon GameLift Servers toolkit to generate Linux libraries.
- Adds the missing Destroy() action.
- Adds or improves client-side validation in all server SDK actions.
- Adds more specific and improved error responses to API errors.
- Fixes an edge case in GetFleetRoleCredentials() that causes game server crash.
- Adds default logic to the onProcessTerminate() callback that terminates the game server process when this callback is invoked.
- Adds idempotency token support to allow InitSDK() retries to succeed.
- Includes autoBackfillMode in the data passed in the OnUpdateGameSession() callback.

# 2.0.1 (1/9/2025)

- Fixed an issue with initializing containers status.

# 2.0.0 (12/19/2024)

This version supports Unreal Engine 5.1.1 - 5.5.
- Updated the server SDK to 5.2.0.
- Added support for Unreal Engine 5.4 and 5.5.
- Added support for creating container images and Amazon GameLift managed container fleets.
- Added a new AWS user profile workflow.
- Refreshed Amazon GameLift Anywhere and managed EC2 flows with new progress indicators.

# 1.1.2 (9/19/2024)

- Updated the plugin to support the latest version of the GameLift Server SDK.
- Added `All Files` option when browsing for `Server Build Executable`.

# 1.1.1 (3/8/2024)

- Fixed an issue with importing GameLiftServerSDK module

# 1.1.0 (2/13/2024)

- Consolidated spot fleet scenario into the existing FlexMatch scenario
- Removed the C++ Server SDK Plugin for Unreal source code from the GitHub repository
- Added release scripts and updated the release package structure to include C++ Server SDK 5.1.2
- Fixed an issue where large .pdb files can cause uploading game server builds to fail

# 1.0.0 (9/28/2023)

## Added:

- Plugin installation package and the plugin file structure
- The GameLift Plugin item in the Unreal menu with the following sub-menu items:
  - Set AWS User Profiles
  - Host with Anywhere
  - Host with Managed EC2
  - Help
- 3 predefined deployment scenario templates:
  - Single-Region Fleet
  - SPOT Fleets with Queue and Custom Matchmaker
  - FlexMatch
- The Host with Managed EC2 window providing the following functionality:
  - Ability to choose a predefined scenario for deployment
    - Displaying the scenario template description
    - Ability to open the AWS instructions
  - Ability to specify a game server build
  - Ability to set game server parameters
  - Ability to start deployment
  - Ability to cancel current deployment
  - Displaying the stack deployment status and details
  - Displaying the deployment outcomes (Cognito Client ID, API Gateway Endpoint)
  - Ability to open the AWS CloudFormation console
- The Host with Anywhere window providing the following functionality:
  - Ability to create Anywhere fleet with custom name
  - Ability to register a local workstation in the Anywhere fleet, with unique compute name and IP address
