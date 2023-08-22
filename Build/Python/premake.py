import os
import shutil
from pathlib import Path

import Utils 

class ConfigPremake:
    premakeVersion = "5.0.0-beta2"
    premakeZipUrls = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-windows.zip"
    premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeDirectory = "./Build/premake"
    premakeSandboxDirectory = "./Source/Tools/NxEditor/Sandbox/Binaries/premake"

    @classmethod
    def Validate(cls):
        if (not cls.CheckIfPremakeInstalled()):
            print("Premake is not installed.")
            return False
        print(f"Correct Premake located at {os.path.abspath(cls.premakeDirectory)}")

        if(not cls.CheckIsPremakeCopied()):
            print("Premake is not Copied.")
            return False
        print(f"Correct Premake located at {os.path.abspath(cls.premakeSandboxDirectory)}")

        return True

    @classmethod
    def CheckIfPremakeInstalled(cls):
        premakeExe = Path(f"{cls.premakeDirectory}/premake5.exe")
        if (not premakeExe.exists()):
            return cls.InstallPremake()

        return True

    @classmethod
    def InstallPremake(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Premake not found. Would you like to download Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        premakePath = f"{cls.premakeDirectory}/premake-{cls.premakeVersion}-windows.zip"
        print("Downloading {0:s} to {1:s}".format(cls.premakeZipUrls, premakePath))
        Utils.DownloadFile(cls.premakeZipUrls, premakePath)
        print("Extracting", premakePath)
        Utils.UnzipFile(premakePath, deleteZipFile=True)
        print(f"Premake {cls.premakeVersion} has been downloaded to '{cls.premakeDirectory}'")

        premakeLicensePath = f"{cls.premakeDirectory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premakeLicenseUrl, premakeLicensePath))
        Utils.DownloadFile(cls.premakeLicenseUrl, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{cls.premakeDirectory}'")

        return True

    @classmethod
    def CheckIsPremakeCopied(cls):
        premakeExe = Path(f"{cls.premakeSandboxDirectory}/premake5.exe")
        if (not premakeExe.exists()):
            return cls.CopyPremake()

        return True
    
    @classmethod
    def CopyPremake(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Premake not found for Sandbox Project. Would you like to Copy Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        premakePath = os.path.curdir + "\\Build\\premake\\premake5.exe"
        dstPath =  os.path.curdir + "\\Source\\Tools\\NxEditor\\Sandbox\\Binaries\\premake\\premake5.exe"
        os.makedirs(os.path.dirname(dstPath))
        shutil.copyfile(premakePath,dstPath)

        return True

if __name__ == "__main__":
    ConfigPremake.Validate()