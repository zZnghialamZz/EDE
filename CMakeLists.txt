# =====================================================
#               _______ _____  _______ 
#              |    ___|     \|    ___|
#              |    ___|  --  |    ___|
#              |_______|_____/|_______|
#                        ---
#              Ethan Development Editor
# =====================================================
# @file CMakeLists - EDE
# @author Nghia Lam <nghialam12795@gmail.com>
#
# @brief
#
# @license Copyright 2020 Nghia Lam
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# -----------------------------------------------------
# Default Setup
# -----------------------------------------------------
cmake_minimum_required(VERSION 3.2)

project(EDE)

# ------------------------------------------------------
# C++17 Support
# ------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------
# Compiler Flag
# ------------------------------------------------------
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  # using Clang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
  add_definitions( -c -Wall -msse2 )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  add_definitions( -c -Wall -msse2 )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  add_definitions( -c -Wall -msse2 )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  add_definitions( -c -W4 )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "Debug"
      CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) \"Debug\" \"Release\" \"RelWithDebInfo\" \"MinSizeRel\"."
      FORCE)
  add_definitions(-DEDE_DEBUG)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  add_definitions(-DEDE_RELEASE)
endif()

# ------------------------------------------------------
# Build Folders
# ------------------------------------------------------
set(EDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EDE_BIN "bin")
set(EDE_LIB "lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EDE_PATH}/${EDE_BIN})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EDE_PATH}/${EDE_BIN})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EDE_PATH}/${EDE_BIN})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EDE_PATH}/${EDE_BIN})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EDE_PATH}/${EDE_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${EDE_PATH}/${EDE_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${EDE_PATH}/${EDE_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EDE_PATH}/${EDE_BIN})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EDE_PATH}/${EDE_BIN}/${EDE_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${EDE_PATH}/${EDE_BIN}/${EDE_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${EDE_PATH}/${EDE_BIN}/${EDE_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EDE_PATH}/${EDE_BIN}/${EDE_LIB})


# ------------------------------------------------------
# Build Sources
# ------------------------------------------------------
set(EDE_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

set(HEADERS 
	"${EDE_SRC_DIR}/terminal.h"
	"${EDE_SRC_DIR}/command.h"
	"${EDE_SRC_DIR}/file_io.h"
	"${EDE_SRC_DIR}/memory.h"
	"${EDE_SRC_DIR}/utils.h"
	"${EDE_SRC_DIR}/input.h"
)

set(SOURCES 
	"${EDE_SRC_DIR}/terminal.cpp"
	"${EDE_SRC_DIR}/command.cpp"
	"${EDE_SRC_DIR}/file_io.cpp"
	"${EDE_SRC_DIR}/memory.cpp"
	"${EDE_SRC_DIR}/utils.cpp"
	"${EDE_SRC_DIR}/input.cpp"
	"${EDE_SRC_DIR}/main.cpp"
)

if (WIN32)
    add_executable(EDE WIN32 ${SOURCES} ${HEADERS})
else()
    add_executable(EDE ${SOURCES} ${HEADERS})
endif()
