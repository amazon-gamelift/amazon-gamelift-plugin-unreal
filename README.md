# Amazon GameLift Servers Plugin for Unreal Engine


![GitHub](https://img.shields.io/github/license/amazon-gamelift/amazon-gamelift-plugin-unreal)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/amazon-gamelift/amazon-gamelift-plugin-unreal)
![GitHub all releases](https://img.shields.io/github/downloads/amazon-gamelift/amazon-gamelift-plugin-unreal/total)
![GitHub release (latest by date)](https://img.shields.io/github/downloads/amazon-gamelift/amazon-gamelift-plugin-unreal/latest/total)


[Overview](#overview)

[Additional Resources](#additional-resources)

[Instructions for Contributors](#instructions-for-contributors)

## Overview
This repository contains the C++ server SDK plugin for Unreal Engine integration. It includes two variants of the plugin:

- `Amazon GameLift Servers SDK for Unreal Engine` includes the server SDK only. Use this plugin to add the server SDK to your game projects
and customize your integration. For more details, [see the README file](./GameLiftServerSDK) in the `GameLiftServerSDK` folder of this repository.
- `Amazon GameLift Servers Plugin for Unreal Engine` includes the server SDK and additional UI components for the Unreal Editor. The UI components
give you guided workflows so you can deploy your game for hosting with Amazon GameLift Servers directly from the Editor.
For more details, [see the README file](./GameLiftPlugin) in the `GameLiftPlugin` folder of this repository.

Download the plugin variant that best fits your project requirements from the Releases page of this repository.

## Additional Resources

* [About Amazon GameLift Servers](http://aws.amazon.com/gamelift/servers)
* [Amazon GameLift Servers plugin guide](https://docs.aws.amazon.com/gamelift/latest/developerguide/unreal-plugin.html)
* [AWS Game Tech forum](https://repost.aws/topics/TAo6ggvxz6QQizjo9YIMD35A/game-tech/c/amazon-gamelift)
* [AWS for Games blog](https://aws.amazon.com/blogs/gametech/)
* [Amazon GameLift Servers toolkit](https://github.com/amazon-gamelift/amazon-gamelift-toolkit)
* [Contributing guidelines](https://github.com/amazon-gamelift/amazon-gamelift-plugin-unreal/blob/main/CONTRIBUTING.md)

## Instructions for Contributors

If you’re interested in contributing to the Amazon GameLift Servers Plugins, clone the source code from the GitHub repository and follow these instructions.
1. Implement your desired changes or additions to the codebase within the cloned repository.
2. Once your changes are ready for testing, navigate to the root directory of the repository and run the following command:
    
    For Linux or Max:
    ```sh
    chmod +x setup.sh
    sh setup.sh
    ```
   
    For Windows:
    ```
    powershell -file setup.ps1
    ```
    Once the setup is completed, the Plugin and the SDK are ready for use.
3. Follow the instructions provided in the README of either the GameLiftPlugin or GameLiftServerSDK to install and use the plugins. Ensure that the plugin functions as expected and your modifications work as intended.
4. Submit a pull request to the repository's **develop** branch. Be sure to provide a clear and detailed description of your modifications, as well as any relevant information about your testing process and results.
