#!/bin/sh
export STB_INCLUDE_DIR=/home/adrian/stb
export VULKAN_SDK=/home/adrian/vulkan_sdk/1.1.85.0/x86_64
export LD_LIBRARY_PATH="$VULKAN_SDK/lib:${LD_LIBRARY_PATH:-}"
export LD_LIBRARY_PATH="`pwd`/build/lib:${LD_LIBRARY_PATH:-}"
export VK_LAYER_PATH="$VULKAN_SDK/etc/explicit_layer.d"