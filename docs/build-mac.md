# Build on macOS

> Scripts are `bash`

## Setup

Make sure you have done the [setup for macOS](./setup-mac.md) 

## Build with Ninja

> This is the recommended way for building and working with the sample code.

Start `bash` (if your default shell is `zsh` or something else):

```bash
bash
```

#### Configure

```bash
source ./configure.sh
```

#### Build

##### Intel x64

```bash
mkdir -p ./build/debug_x64
pushd ./build/debug_x64

cmake --preset Debug_x64 -G 'Ninja' ../..
ninja

popd
```

##### Apple Silicon arm64

```bash
mkdir -p ./build/debug_arm64
pushd ./build/debug_arm64

cmake --preset Debug_arm64 -G 'Ninja' ../..
ninja

popd
```

#### Clean

##### Intel x64

```bash
pushd ./build/debug_x64
ninja clean
popd
```

##### Apple Silicon arm64

```bash
pushd ./build/debug_arm64
ninja clean
popd
```

## Build with Xcode

#### Configure

```bash
source ./configure.sh
```

### Generate Xcode project

```bash
mkdir -p ./xcode
pushd ./xcode

# Intel x64
cmake --preset Debug_x64 -G 'Xcode' ..

# Apple Silicon arm64
cmake --preset Debug_arm64 -G 'Xcode' ..

popd  
```

Open the project in Xcode:

```bash
pushd ./xcode 
open primoburner-cpp.xcodeproj
popd
```

### Build

In the Xcode menu select `Product | Scheme | ALL_BUILD`

In the Xcode menu select `Product | Build For | Running`

### Clean

In the Xcode menu select `Product | Clean Build Folder` 