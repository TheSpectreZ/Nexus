import sys
import os
from pathlib import Path

import Utils 

class ConfigPremake:
    premakeVersion = "5.0.0-beta2"
    premakeZipUrls = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-windows.zip"
    premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeDirectory = "./Build/premake"
    premakeLauncherDirectory = "./Source/NexusLauncher/Resources/Projects/Binaries/premake"

    @classmethod
    def Validate(cls):
        if (not cls.CheckIfPremakeInstalled(f"{cls.premakeDirectory}/premake5.exe")):
            print("Premake is not installed.")
            return False
        print(f"Correct Premake located at {os.path.abspath(cls.premakeDirectory)}")

        if(not cls.CheckIfPremakeCopied(f"{cls.premakeLauncherDirectory}/premake5.exe")):
            print("Premake is not Found.")
            return False;   
        print(f"Correct Premake located at {os.path.abspath(cls.premakeLauncherDirectory)}")

        return True

    @classmethod
    def CheckIfPremakeInstalled(cls,directory):
        premakeExe = Path(directory);
        if (not premakeExe.exists()):
            return cls.InstallPremake(directory)

        return True

    @classmethod
    def InstallPremake(cls,directory):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Premake not found. Would you like to download Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        premakePath = f"{directory}/premake-{cls.premakeVersion}-windows.zip"
        print("Downloading {0:s} to {1:s}".format(cls.premakeZipUrls, premakePath))
        Utils.DownloadFile(cls.premakeZipUrls, premakePath)
        print("Extracting", premakePath)
        Utils.UnzipFile(premakePath, deleteZipFile=True)
        print(f"Premake {cls.premakeVersion} has been downloaded to '{directory}'")

        premakeLicensePath = f"{directory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premakeLicenseUrl, premakeLicensePath))
        Utils.DownloadFile(cls.premakeLicenseUrl, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{directory}'")

        return True
    
    @classmethod
    def CheckIfPremakeCopied(cls,directory):
        premakeExe = Path(directory);
        if(not premakeExe.exists()):
            return cls.CopyPremake(directory)
        
        return True

    @classmethod
    def CopyPremake(cls,directory):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Premake not found for Launcher. Would you like to Copy Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        premakePath = Path(directory)
        
        if(not premakePath.exists()):
            os.makedirs(os.path.dirname(premakePath), exist_ok=True)

        Utils.CopyFile(f"{cls.premakeDirectory}/premake5.exe",premakePath)
        print(f"Premake {cls.premakeVersion} has been Copied to {premakePath}")
        
        return True
        
if __name__ == "__main__":
    ConfigPremake.Validate()