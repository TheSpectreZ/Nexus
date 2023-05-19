# Nexus 

Nexus is primarily a hobby project, a Game-Engine / Rendering-Framework for Windows. Note that Currently it's still in developement.

![Screenshot 2023-05-19 125434](https://github.com/TheSpectreZ/Nexus/assets/122666662/c05b0fcc-6feb-4736-9897-effdb5ff1b4e)


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
## Goals
- Renderer
  - [x] Vulkan Backend
  - [x] Shader and Descriptor Management
  - [x] Scene System
  - [x] glTF Mesh Loading with Materials
  - [x] PBR with Point and Directional Lights
  - [ ] Image based Lighting
  - [ ] Shadows  
  - [ ] Bloom
  - [ ] Frustrum Culling
  - [ ] Rendering Billboards
  - [ ] Text Rendering
  - [ ] Instanced Rendering
  - [ ] External Materials
  - [ ] Animation
  - [ ] Terrain Generation
  - [ ] Water Rendering
- Physics
  - [x] Jolt Physics Backend
  - [x] Simple Collider and RigidBody
  - [ ] More Colliders Types
  - [ ] Debug Renderer to Visualize Colliders
  - [ ] Physics Materials
  - [ ] Height-Fields
- Scripting
  - [x] Mono Backend
  - [x] Scriptclass and Instances
  - [x] Calling C# from C++ and Vice-Versa
  - [x] Fields and Properties
  - [ ] Serialzing Field Data
  - [ ] Seperating App and Core DLLs
  - [ ] Reloading Assembly With Filewatcher
  - [ ] Debugging Scripts
- Editor
  - [x] ImGui Backend
  - [x] Scene Heirarchy and Properties
  - [x] Content Browser
  - [x] Editor Viewport
  - [ ] Editor Console/Log
  - [ ] Material Asset Creator
  - [ ] Launcher
- Misc
  - [x] Profile Timer
  - [ ] Custom Asset Formats
  - [ ] Multi-Threading Support
  - [ ] Instrumentation
  - [ ] Runtime
