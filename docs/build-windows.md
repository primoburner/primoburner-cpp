# Build on Windows

> Scripts are `PowerShell`

The project comes with a pre-configured Visual Studio Code workspace which uses `cmake` and `ninja` as a build system. 

## Setup

Make sure you have done the [setup for Windows](./setup-windows.md) 

## Build with Ninja

> This is the recommended way for building and working with the sample code.

### Configure

```powershell
. .\configure.ps1
```

### Build

#### x64

```powershell
New-Item -Force -Path ./build/debug_x64 -ItemType Directory 
pushd ./build/debug_x64

cmake --preset Debug_x64 -G 'Ninja' ../..
ninja

popd
```

#### arm64

```powershell
New-Item -Force -Path ./build/debug_arm64 -ItemType Directory 
pushd ./build/debug_arm64

cmake --preset Debug_arm64 -G 'Ninja' ../..
ninja

popd
```

### Clean

#### x64

```powershell
pushd ./build/debug_x64
ninja clean
popd
```

#### arm64

```powershell
pushd ./build/debug_arm64
ninja clean
popd
```

## Build with Visual Studio

### Configure

```powershell
. .\configure.ps1
```

### Generate Visual Studio 2022 project

```powershell

new-item -Force -ItemType Directory ./vs2022
pushd ./vs2022

# for AMD / Intel x64
cmake --preset Debug_x64 -G "Visual Studio 17 2022"  ..

# for ARM arm64
cmake --preset Debug_arm64 -G "Visual Studio 17 2022"  ..

popd
```

Open the project in Visual Studio:

```powershell
pushd ./vs2022
start .\primoburner-cpp.sln
popd
```

### Build

In the Visual Studio menu select `Build| Build Solution`

### Clean

In the Visual Studio menu select `Build| Clean Solution` 

