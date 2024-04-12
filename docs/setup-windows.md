# Setup

## Environment

### PowerShell

In PowerShell as Administrator:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope LocalMachine
```

> This will allow you to execute PowerShell scripts locally on your machine.

### PSModulePath

Update `$env:PSModulePath`  so that you can use PowerShell modules. 

In PowerShell as Administrator:

```powershell
[Environment]::SetEnvironmentVariable("PSModulePath", "$HOME/Documents/WindowsPowerShell/Modules;" + $env:PSModulePath, "Machine")
```

## Visual Studio 2022

This is needed for the C++ compiler. Install [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/older-downloads/).  

During installation select C++ desktop development and latest Windows 10 SDK. 

After installation open Visual Studio and check for updates. Install the latest 2022 version that is available. 

## VSSetup PowerShell Module

Install the Visual Studio Setup PowerShell Module. In PowerShell as Administrator:

```powershell
Install-Module VSSetup -Scope CurrentUser
```

Close and reopen PowerShell

Test:

```powershell
Get-VSSetupInstance `
| Select-VSSetupInstance -Version '[17.0,18.0]' `
| Select-Object -ExpandProperty InstallationPath
```

>  This finds the installation path for Visual Studio 2022.


See [Selecting an Instance](https://github.com/Microsoft/vssetup.powershell/wiki/Selecting-an-Instance)

## vcpkg

```powershell
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
cd $HOME/vcpkg; ./bootstrap-vcpkg.bat
```

## cmake

Install via winget. In PowerShell:

```powershell
winget install Kitware.CMake --silent --override "ADD_CMAKE_TO_PATH=System"
```

## ninja

### x64

Install via winget. In PowerShell:

```powershell
winget install Ninja-build.Ninja --silent 
```

### ARM64

Build it from source:

```powershell
cd $HOME
git clone https://github.com/ninja-build/ninja

cd ninja
git checkout tags/v1.11.1 -b v1.11.1

cmake -Bbuild -A arm64
cmake --build build --parallel --config Release

mkdir C:/tools/ninja 
cp ./build/Release/ninja.exe C:/tools/ninja/ 
```

Add to PATH. In PowerShell as Administrator:

```powershell
[System.Environment]::SetEnvironmentVariable("Path", "C:\tools\ninja;" + $env:Path, "Machine")
```

Close and reopen PowerShell to refresh PATH.

Test:

```powershell
ninja --version
```

