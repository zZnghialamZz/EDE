#!/bin/bash

# ==========================================================================
# @File   : build_clang.sh
# @Brief  : All in one build file using clang compiler.
# @Author : Nghia Lam <nghialam12795@gmail.com>
# @License: MIT License (C) Copyright 2020 - 2021 by Nghia Lam. 
#           Take a look at LICENSE for more information.
# ==========================================================================

# General Settings
APP_NAME="ede"
COMPILE_FLAGS=""

# Create the build dir and build
if [ ! -d "./build" ]; then
    mkdir build
fi

pushd build
gcc $COMPILE_FLAGS -o $APP_NAME ../source/main.cpp
popd
