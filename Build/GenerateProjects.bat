@echo off
pushd %~dp0\..\
call Build\premake\premake5.exe vs2022
popd
PAUSE