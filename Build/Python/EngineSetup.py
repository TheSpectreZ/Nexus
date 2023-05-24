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
        print("\n")
        os.chdir('./Source/NexusEditor/Sandbox')
        subprocess.call([os.path.abspath("./GenerateProject.bat"),"nopause"])

    print("\nSetup completed!")
else:
    print("Build System requires Premake to generate project files.")