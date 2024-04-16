# Build on Linux

> Scripts are `bash`

## Setup

Make sure you have done the [setup for Linux](./setup-linux.md) 

## Build with Ninja

> This is the recommended way for building and working with the sample code.

#### Configure

```bash
source ./configure.sh
```

#### Build

##### x64

```bash
mkdir -p ./build/debug_x64
pushd ./build/debug_x64

cmake --preset Debug_x64 -G 'Ninja' ../..
ninja

popd
```

##### arm64

```bash
mkdir -p ./build/debug_arm64
pushd ./build/debug_arm64

cmake --preset Debug_arm64 -G 'Ninja' ../..
ninja

popd
```

#### Clean

##### x64

```bash
pushd ./build/debug_x64
ninja clean
popd
```

##### arm64

```bash
pushd ./build/debug_arm64
ninja clean
popd
```

