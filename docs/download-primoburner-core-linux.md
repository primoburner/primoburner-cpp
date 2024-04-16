# Download PrimoBurner Core on Linux

Change to the directory where you cloned this repository:

```bash
cd primoburner-cpp
```

## PrimoBurner Core

In the script below, change the tag to the release that you need. For the available versions check the [AVBlocks Core](https://github.com/avblocks/avblocks-core/releases) releases.   

### x64

```bash
# select version and platform
tag="v5.0.1-demo.1"
platform="linux"

# download
mkdir -p ./sdk
curl \
  --location \
  --output ./sdk/primoburner-$tag-$platform.tar.gz \
  https://github.com/primoburner/primoburner-core/releases/download/$tag/primoburner-$tag-$platform.tar.gz
  
# unzip
pushd sdk
tar -xvf primoburner-$tag-$platform.tar.gz
popd
```

### arm64

```bash
# select version and platform
tag="v5.0.1-demo.1"
platform="linux-arm64"

# download
mkdir -p ./sdk
curl \
  --location \
  --output ./sdk/primoburner-$tag-$platform.tar.gz \
  https://github.com/primoburner/primoburner-core/releases/download/$tag/primoburner-$tag-$platform.tar.gz
  
# unzip
pushd sdk
tar -xvf primoburner-$tag-$platform.tar.gz
popd
```

