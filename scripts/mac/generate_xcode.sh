#!/bin/bash

# Author : Nghia Lam
# Copyright (c) Nghia Lam

rm -rf bin
rm -rf build
rm -rf compile_command.json

/usr/local/bin/cmake -G Xcode -H. -B build
