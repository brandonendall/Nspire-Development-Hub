#!/usr/bin/env bash
# Build pinned Ndless SDK including bundled nSDL for the isolated restart.
# Debian/Ubuntu packages: build-essential git php-cli gcc-arm-none-eabi
# binutils-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-dev
# libboost-program-options-dev zlib1g-dev
set -euo pipefail
project_dir="$(cd "$(dirname "$0")/.." && pwd)"
checkout_dir="${1:-$project_dir/.toolchain/Ndless}"
revision=c659d1691295dd5db79ab171fff680638ca6a4bf
if [[ ! -d "$checkout_dir" ]]; then
  git clone https://github.com/ndless-nspire/Ndless.git "$checkout_dir"
  git -C "$checkout_dir" checkout --detach "$revision"
fi
if [[ "$(git -C "$checkout_dir" rev-parse HEAD)" != "$revision" ]]; then
  echo "Expected Ndless $revision. Use a separate checkout at that revision." >&2
  exit 1
fi
git -C "$checkout_dir" submodule update --init ndless-sdk/thirdparty/nspire-io
sdk_dir="$checkout_dir/ndless-sdk"
export _NDLESS_TOOLCHAIN_PATH="$(dirname "$(command -v arm-none-eabi-gcc)")"
export PATH="$sdk_dir/bin:$PATH"
jobs="${NX_BUILD_JOBS:-4}"
mkdir -p "$sdk_dir/lib"
make -C "$sdk_dir/libsyscalls" -j"$jobs" \
  CXXFLAGS='-mcpu=arm926ej-s -std=c++11 -nostdlib -O3 -fPIE -fno-exceptions -fno-rtti -mlong-calls -Wall -Werror -I ../include/ -I ../thirdparty/nspire-io/include/ -D_TINSPIRE -DPATH_MAX=512 -ffunction-sections -fdata-sections'
make -C "$sdk_dir/libndls" -j"$jobs"
make -C "$sdk_dir/system" -j"$jobs"
make -C "$sdk_dir/tools/zehn_loader" zehn_loader.tns
# Overrides allow a locally extracted compiler/dependency installation.
read -r -a host_flags <<< "${NX_GENZEHN_CXXFLAGS:-}"
read -r -a host_libs <<< "${NX_GENZEHN_LIBS:--lboost_program_options -lz}"
g++ -std=c++11 -O2 "${host_flags[@]}" \
  -I "$sdk_dir/tools/genzehn/elfio-3.2" \
  "$sdk_dir/tools/genzehn/genzehn.cpp" "${host_libs[@]}" \
  -o "$sdk_dir/bin/genzehn"
make -C "$project_dir" -j"$jobs" NDLESS_SDK="$sdk_dir"
