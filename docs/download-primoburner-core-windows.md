# Download PrimoBurner Core on Windows

> Scripts are PowerShell

Change to the directory where you cloned this repository:

```powershell
cd primoburner-cpp
```

## PrimoBurner Core

In the script below, change the tag to the release that you need. For the available versions check the [PrimoBurner Core](https://github.com/primoburner/primoburner-core/releases) releases.   

```powershell
# select version and platform
$tag='v5.0.1-demo.1'
$platform='windows'

# download
new-item -Force -ItemType Directory ./sdk
curl.exe `
  --location `
  --output ./sdk/primoburner-$tag-$platform.zip `
  https://github.com/primoburner/primoburner-core/releases/download/$tag/primoburner-$tag-$platform.zip
  
# unzip
pushd sdk
expand-archive -Force -Path primoburner-$tag-$platform.zip -DestinationPath .
popd
```

