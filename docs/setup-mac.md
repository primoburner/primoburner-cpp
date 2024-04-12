# Setup for macOS

> Scripts are `bash`

## Xcode

Install Command Line Tools (CLT) for Xcode:

```bash
xcode-select --install
```

## Homebrew

Install Homebrew:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

## Development Tools

### vcpkg

```bash
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
cd $HOME/vcpkg && ./bootstrap-vcpkg.sh
```

### cmake

Install via Homebrew:

```bash
brew install cmake 
```

Verify:

```bash
cmake --version
```

### ninja

```bash
brew install ninja
```

Verify:

```bash
ninja --version
```

### Visual Studio Code

Download and install from [Visual Studio Code](https://code.visualstudio.com/download) site.

Open Visual Studio Code and press `Cmd + Shift + p`. Select `Shell Command: Install 'code' command in PATH`. 

