## bluray-data

A command line tool for Blu-ray Disc (e.g. BD-R, BD-RE) formatting and data recording. 

### Set PATH to compiled programs 

```powershell
# AMD / Intel
$env:Path = "$( (pwd).Path )/bin/x64/;" + $env:Path 

# ARM
$env:Path = "$( (pwd).Path )/bin/arm64/;" + $env:Path 
```

### List options

```sh
bluray-data --help
```

### List available devices

```sh
bluray-data list-devices
```

### Eject

```sh
bluray-data eject --device-index 0
```

### Close tray

```sh
bluray-data close-tray --device-index 0
```


### Disc Info

```sh
bluray-data disc-info --device-index 0
```

### Write Speed Info

```sh
bluray-data speed-info --device-index 0
```


### Quick format

```sh
bluray-data format --device-index 0

# force
bluray-data format --force --device-index 0

```

### Burn

#### Download test files

You need `wget`. Install with `winget`:

```sh
winget install wget
```

```sh
mkdir -p $HOME/DataDisc/Hippo

pushd $HOME/DataDisc/Hippo
wget https://archive.org/download/WildlifeVideoMedia/Hippo.ogv 
popd

mkdir -p $HOME/DataDisc/Elephant

pushd $HOME/DataDisc/Elephant
wget https://archive.org/download/WildlifeVideoMedia/Elephant.ogv
popd
```
#### Start a new disc

> NOTE: We use the `--overwrite` flag  to start a new disc. 

```sh
# Windows (PowerShell)
bluray-data burn `
    --overwrite `
    --source-folder $HOME/DataDisc/Elephant `
    --volume-label Animals `
    --eject `
    --device-index 0
```

#### Append to existing data

> NOTE: No `--overwrite` flag

```sh
# Windows (PowerShell)
bluray-data burn `
    --source-folder $HOME/DataDisc/Hippo `
    --volume-label Animals `
    --eject `
    --device-index 0
```
