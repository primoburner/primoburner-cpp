# Setup for Linux

## Ubuntu 22.04

> Scripts are `bash`

### Compilers

```bash
sudo apt install build-essential
```

### cmake

```bash
sudo apt install cmake
```

### ninja

```bash
sudo apt install ninja-build
```

### vcpkg

```bash
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
pushd $HOME/vcpkg && ./bootstrap-vcpkg.sh && popd
```

### Visual Studio Code

Install via Snap Store:

```bash
sudo snap install --classic code
```

