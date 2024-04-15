# Download PrimoBurner Core on macOS

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
platform="darwin"

# download
mkdir -p ./sdk
curl \
  --location \
  --output ./sdk/primoburner-$tag-$platform.zip \
  https://github.com/primoburner/primoburner-core/releases/download/$tag/primoburner-$tag-$platform.zip
  
# unzip
pushd sdk
unzip primoburner-$tag-$platform.zip
popd
```

### arm64

```bash
# select version and platform
tag="v5.0.1-demo.1"
platform="darwin-arm64"

# download
mkdir -p ./sdk
curl \
  --location \
  --output ./sdk/primoburner-$tag-$platform.zip \
  https://github.com/primoburner/primoburner-core/releases/download/$tag/primoburner-$tag-$platform.zip
  
# unzip
pushd sdk
unzip primoburner-$tag-$platform.zip
popd
```

