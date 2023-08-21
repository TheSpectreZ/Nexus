# Welcome to Nexus 
Nexus Engine is an exciting project that serves as a Game Engine and Rendering Framework primarily developed for Windows. Please keep in mind that it is currently in the development phase, with continuous improvements being made. You can find the Roadmap [Here](https://trello.com/b/U7P1rx0g/nexus-engine).
This project is inspired from TheCherno's [Game Engine Series](https://hazelengine.com/).

## Getting Started

To begin working with Nexus, It is recommended to use **Visual Studio 2022** as the development environment, as it is the officially supported platform for this project (Make Sure to have **Visual Studio Build** Tools Installed as well). Follow the steps below to get started:

<ins>**1. Downloading the Repository**</ins>

Clone the repository using the following command: `git clone --recursive https://github.com/TheSpectreZ/Nexus`

If you had previously cloned the repository non-recursively, you can clone the necessary submodules by running: `git submodule update --init`

<ins>**2. Configuring Dependencies**</ins>

Before building the engine, you need to set up some dependencies. Follow these instructions carefully:

- Add an environment variable called `NEXUS_ROOT_PATH`. Set its value to the file path of the root Nexus Engine folder (e.g. F:/Development/Nexus-Engine). Take a look at [this](https://docs.oracle.com/en/database/oracle/machine-learning/oml4r/1.5.1/oread/creating-and-modifying-environment-variables-on-windows.html) if you aren't sure how to do it.
- Locate and run the `EngineBuild.bat` file found in the `Build` folder. This script will automatically download any required prerequisites for the project, if they are not already present on your system.
- One of the essential prerequisites is the Vulkan SDK. If the SDK is not found, the script will download the `VulkanSDK.exe` file and guide you through the installation process.
- Once the Vulkan SDK is installed, run the `EngineBuild.bat` file again.
- The `EngineBuild.bat` script will generate a Visual Studio solution file for your convenience. This solution file will allow you to work with the Nexus Engine.
- If you make any changes to the Engine or if you want to regenerate the Engine project files, simply rerun the `GenerateProjects.bat` script found in the `Build` folder.
- Similarly, if you make changes to the Sandbox Project, rerun the `GenerateProject.bat` script located in the `Source\NexusEditor\Sandbox` folder.

Feel free to explore the Nexus Game Engine and contribute to its development. If you encounter any issues or have suggestions, please open a new [issue](https://github.com/TheSpectreZ/Nexus/issues) on the GitHub repository.

---
Here's a sneak peek of the current state of the engine's editor:


https://github.com/TheSpectreZ/Nexus/assets/122666662/cd899f85-8e21-4464-b862-571f28e9c3c2


