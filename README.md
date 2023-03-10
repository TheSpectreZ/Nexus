# Nexus 

Nexus is primarily a hobby project, a Game-Engine / Rendering-Framework for Windows. Note that Currently it's still in developement.

***

## Getting Started
Visual Studio 2022 is recommended, Nexus is officially untested on other development environments other than Windows.

<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/TheSpectreZ/Nexus`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

1. Run the `EngineBuild.bat` file found in `Build` folder. This will download the required prerequisites for the project if they are not present yet.
2. One prerequisite is the Vulkan SDK. If it is not installed, the script will execute the `VulkanSDK.exe` file, and will prompt the user to install the SDK.
3. After installation, run the `EngineBuild.bat` file again. If the Vulkan SDK is installed properly, it will then download the Vulkan SDK Debug libraries. (This may take a longer amount of time)
4. After downloading and unzipping the files, the `GenerateProjects.bat` script file will get executed automatically, which will then generate a Visual Studio solution file for user's usage.

If changes are made, or if you want to regenerate project files, rerun the `GenerateProjects.bat` script file found in `Build` folder.

***
