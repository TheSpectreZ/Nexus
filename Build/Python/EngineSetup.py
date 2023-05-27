import os
import subprocess
import platform

from python import ConfigPython as ConfigPython
ConfigPython.Validate()

os.chdir('./../')

from premake import ConfigPremake as ConfigPremake
premakeInstalled = ConfigPremake.Validate()

from Vulkan import ConfigVulkan as ConfigVulkan
ConfigVulkan.Validate()

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if (premakeInstalled):
    if platform.system() == "Windows":
        print("\nRunning premake...")
        subprocess.call([os.path.abspath("./Build/GenerateProjects.bat"), "nopause"])
        os.chdir('./Source/NexusEditor/Sandbox')
        print("\n")
        subprocess.call([os.path.abspath("./GenerateProject.bat"), "nopause"])
        print("\n")
        subprocess.call([os.path.abspath('./Scripts/Build.bat')])

    print("\nSetup completed!")
else:
    print("Build System requires Premake to generate project files.")