# Amazon GameLift Servers Plugin for Unreal Engine
Amazon GameLift Servers Plugin for Unreal Engine is compatible with Unreal Engine 5 (versions 5.0, 5.1, 5.2, 5.3, 5.4, and 5.5).  Use the plugin to develop and package game servers for Windows or Linux, and game clients for any Unreal-supported platform.

## Overview
[Amazon GameLift Servers](https://aws.amazon.com/gamelift/servers/) is a fully managed service that gives game developers the ability to manage and scale dedicated game servers for session-based multiplayer games. The **Amazon GameLift Servers Plugin for Unreal Engine** streamlines the process of setting up an Unreal game project for hosting on Amazon GameLift Servers.
With the plugin added to your Unreal game project, you can work with Amazon GameLift Servers from inside the Unreal Editor. Use the plugin to complete these steps:

* Integrate your game server code with the server SDK for Amazon GameLift Servers (included with the plugin), so that the game server can communicate with the service and respond to prompts to start and run game sessions.
* Deploy your integrated game server to an Amazon GameLift Servers fleet. Use the plugin’s guided workflows to configure and deploy a Windows or Linux game server using any of these deployment options. [Learn more about each option.](https://docs.aws.amazon.com/gamelift/latest/developerguide/gamelift-intro-flavors.html#gamelift-intro-flavors-hosting)
    * **Anywhere fleet.** Deploy an Anywhere fleet to run game servers on hosting resources that you control. Anywhere fleets are commonly used to test on a local device, or to act as test environments for iterative development. You can also use Anywhere fleets to set up game hosting on-premises hardware, other resources, or as part of a hybrid solution.
    * **Managed EC2 fleet.** Deploy a managed EC2 fleet to run game servers on AWS Cloud resources that are managed by Amazon GameLift Servers. With this option you get resources and management tools (such as automatic scaling) that are optimized for multiplayer game hosting.
    * **Managed container fleet (for Linux servers).** Deploy a managed container fleet to run game servers on AWS Cloud resources in a containerized environment. Container fleets are managed by Amazon GameLift Servers and provide EC2 resources and management tools (such as automatic scaling) that are optimized for multiplayer game hosting.
* Build a backend service with the AWS SDK for Amazon GameLift Servers (included with the plugin) and add functionality to your game client to start a game session and connect to it. When you deploy to a managed fleet, the plugin also deploys a simple backend service for your game. You can use the default backend service or customize it as needed.
* Perform tests using plugin functionality to start a game client and join a running game session.
* Continue to use the plugin’s SDKs and tools when you want to customize your game hosting beyond the plugin’s guided workflows.

When deploying a game server to a fleet for hosting, Amazon GameLift Servers uses an AWS CloudFormation template to create a resource stack for your solution. You can view and manage your resource stacks in the AWS Management Console for AWS CloudFormation.


## Install the plugin

Follow these steps to set up the Amazon GameLift Servers plugin and add it to your Unreal Engine editor.
After you've installed the plugin, see the [plugin documentation](https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin.html)
for help with preparing your game server for hosting and deploying to Amazon GameLift Servers.

### Step 1: Install prerequisites

You’ll need the following tools to install and run the plugin with your Unreal projects.

* An AWS account to use with Amazon GameLift Servers. To use the plugin guided workflows, you need a user profile with your AWS account and access credentials. See [Set up an AWS account](https://docs.aws.amazon.com/gamelift/latest/developerguide/setting-up-aws-login.html) for help with these steps:
    * Sign up for an AWS account.
    * Create a user with permissions to use Amazon GameLift Servers.
    * Set up programmatic access with long-term IAM credentials.
* A source-built version of the Unreal Engine editor. You need a source-built editor to package a multiplayer game server build. See these Unreal Engine documentation topics:
    * [Accessing Unreal Engine source code on GitHub](https://www.unrealengine.com/ue-on-github) (requires GitHub and Epic Games accounts).
    * [Building Unreal Engine from Source](https://docs.unrealengine.com/5.3/en-US/building-unreal-engine-from-source/)
* A multiplayer game project with C++ game code. (Blueprint-only projects aren’t compatible with the plugin.) If you don’t have a project in progress, use one of the Unreal Engine sample games such as the [Third Person Template](https://dev.epicgames.com/documentation/en-us/unreal-engine/third-person-template-in-unreal-engine).
  If you're starting a new Unreal project, create a game using the Third Person template and use the following settings:
  * C++
  * With starter content
  * Desktop
  * Custom project name (examples in this README use the name `GameLiftUnrealApp`)
* [Microsoft Visual Studio](https://visualstudio.microsoft.com/vs/) 2019 or newer.
* [Unreal Engine cross-compiling toolchain](https://dev.epicgames.com/documentation/en-us/unreal-engine/linux-development-requirements-for-unreal-engine#cross-compiletoolchain). This tool is required only if you’re building a Linux game server.

### Step 2: Get the plugin

1. Download the plugin `amazon-gamelift-plugin-unreal-release-<version>.zip` from the repository’s [**Releases**](https://github.com/amazon-gamelift/amazon-gamelift-plugin-unreal/releases) page or clone the repository if you plan to customize it.
2. If you downloaded the plugin from the [**Releases**](https://github.com/amazon-gamelift/amazon-gamelift-plugin-unreal/releases) page, unzip the downloaded file `amazon-gamelift-plugin-unreal-release-<version>.zip`.
3. If you cloned the repository, run the following command in the root directory of the repository:

   For Linux or Max:
    ```sh
    chmod +x setup.sh
    sh setup.sh
    ```

   For Windows:
    ```
    powershell -file setup.ps1
    ```

   Once completed, the plugin is ready to be added to an Unreal game project.

### Step 3: Add the plugin to your Unreal game project
1. Locate the root folder of your Unreal game project. Look for a subfolder named `Plugins`. If it doesn’t exist, create it.
2. Copy the entire contents of `GameLiftPlugin/` folder, either from this repository (after running the setup script) or from the unzipped release bundle, into the `Plugins` folder in your game project.
3. Open the `.uproject` file for your game project. Add the `GameLiftPlugin` to the `Plugins` section:
    ```
    "Plugins": [
        ......
        {
            "Name": "GameLiftPlugin",
            "Enabled": true
        }
    ]
    ```
4. In a file browser, select the game project `.uproject` file and choose the option **Switch Unreal Engine Version**. Set the game project to use the source-built Unreal Editor.
5. (Windows) In the game project root folder, right-click the `.uproject` file and choose the option to generate project files. Open the solution ( `*.sln`) file and build or rebuild the project.
6. Open the game project in your Unreal Editor. If you already have the editor open, you might need to restart it before it recognizes the new plugin.
7. Verify that the plugin is installed. Check the main editor toolbar for the new Amazon GameLift Servers menu button.
Look in the Content Drawer/Content Browser (usually at the bottom of Unreal Editor) for the Amazon GameLift Servers plugin assets.
(Make sure that **Settings** has the **Show Plugin Content** option selected.)


## Next steps: Get started with the Unreal Plugin

See these Amazon GameLift Servers documentation topics for help with plugin features.

* [Set up an AWS account and user profile](https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-profiles.html)
* [Integrate server SDK functionality into your game server code](https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-integrate.html)
* [Deploy your game with an Anywhere fleet](https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-anywhere.html)
* [Deploy your game with a managed EC2 fleet](https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-ec2.html)
* [Deploy your game with a managed container fleet](https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin-container.html)


## Troubleshoot plugin installation

#### Issue: When rebuilding the game project’s solution file after adding the plugin, I get some compile errors.

**Resolution:** Add the following line to your `<ProjectName>Editor.Target.cs` file to disable adaptive unity build, which may cause conflicts:
  ```
  bUseAdaptiveUnityBuild = false;
  ```

#### Issue: When extracting the plugin files or server SDK files from the downloaded zip file, I get a `path too long` error.

**Resolution:** Some of the plugin files have very long paths and filenames. To resolve this issue, retry extracting the files and choose a destination folder that’s closer to the root directory. Alternatively, you might be able to shorten the destination folder name.

#### Issue: When rebuilding the game project’s solution file after adding the plugin (Windows), I get a  `No such file or directory` error for files in the `aws/gamelift/server/model/` path.

**Resolution:** This issue is likely caused by very long file paths to the server SDK header files. Sometimes zip tools silently skip files if the file path lenth exceeds a limit. To resolve this issue, retry extracting the files and choose a destination folder that’s closer to the root directory. Alternatively, you might be able to shorten the destination folder name.

#### Issue: I need to remove a successfully installed plugin or recover from a failed installation.

**Resolution:** To remove plugin components from your Unreal Editor, undo the tasks in [Step 2: Add the plugin to your Unreal game project](#step-2-add-the-plugin-to-your-unreal-game-project).
