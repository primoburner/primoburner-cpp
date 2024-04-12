#!/usr/bin/env bash

# get script dir
script_dir=$( cd `dirname ${BASH_SOURCE[0]}` >/dev/null 2>&1 ; pwd -P )

# create build dirs
mkdir -p $script_dir/build/debug_x64
mkdir -p $script_dir/build/release_x64

mkdir -p $script_dir/build/debug_arm64
mkdir -p $script_dir/build/release_arm64

echo "Configure vcpkg ..."
arch=$(arch)

export VCPKG_ROOT="$HOME/vcpkg"
alias vcpkg="$VCPKG_ROOT/vcpkg"

export VCPKG_DISABLE_METRICS=1

# Environment variable VCPKG_FORCE_SYSTEM_BINARIES must be set on arm, s390x, ppc64le and riscv platforms.
if [[ "$arch" == "aarch64" || "$arch" == "arm64" ]]; then 
    export VCPKG_FORCE_SYSTEM_BINARIES=1
fi
