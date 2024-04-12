# get script dir
$script_dir=$PSScriptRoot

# create build dirs
New-Item -Force -ItemType Directory $script_dir/build/debug_x64
New-Item -Force -ItemType Directory $script_dir/build/release_x64

New-Item -Force -ItemType Directory $script_dir/build/debug_arm64
New-Item -Force -ItemType Directory $script_dir/build/release_arm64

# setup Visual C++ environment
# see: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2017
# see: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019

$vs_install_dir = $(Get-VSSetupInstance | Select-VSSetupInstance -Version '[17.0,18.0]' | Select-Object -ExpandProperty InstallationPath)
$vs_common_tools = "${vs_install_dir}/Common7/Tools/"

$arch = $env:PROCESSOR_ARCHITECTURE
if ($arch.Contains("ARM")) {
    ./scripts/Invoke-CmdScript.ps1 "$vs_common_tools/VsDevCmd.bat" "-arch=arm64 -host_arch=arm64"
} else {
    ./scripts/Invoke-CmdScript.ps1 "$vs_common_tools/VsDevCmd.bat" "-arch=amd64 -host_arch=amd64"
}

Write-Output "Configure vcpkg ..."

$env:VCPKG_DISABLE_METRICS=1
$env:VCPKG_ROOT = "$HOME/vcpkg"
Set-Alias -Name vcpkg -Value "$env:VCPKG_ROOT/vcpkg"
