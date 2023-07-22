@echo off
"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\MSBuild.exe" /p:Configuration=Debug Scripts/Sandbox.csproj
"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\MSBuild.exe" /p:Configuration=Release Scripts/Sandbox.csproj
"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\MSBuild.exe" /p:Configuration=Dist Scripts/Sandbox.csproj
PAUSE