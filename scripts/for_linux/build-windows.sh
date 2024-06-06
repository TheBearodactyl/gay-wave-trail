#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

export SPLAT_DIR="$HOME/.local/share/splat/"
export TOOLCHAIN="$HOME/.local/share/toolchain/clang-cl-msvc.cmake"
export HOST_ARCH="x86_64"

cmake \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -B build

cmake --build build --config RelWithDebInfo
