@ECHO off

REM  ==========================================================================
REM  @File   : build_msvc.bat
REM  @Brief  : All in one build file MSVC compiler.
REM  @Author : Nghia Lam <nghialam12795@gmail.com>
REM  @License: MIT License (C) Copyright 2020 - 2021 by Nghia Lam. 
REM            Take a look at LICENSE for more information.
REM  ==========================================================================

REM  Setup environment
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

REM  General settings
SET exe_name=ede
SET compile_flags= -FC -Zi
SET link_flags= user32.lib gdi32.lib

REM Move out to root folder and create build
PUSHD ../
IF NOT EXIST build MKDIR build
PUSHD build

cl %compile_flags% ..\source\main.cpp %link_flags% /Fe:%exe_name%

POPD
POPD