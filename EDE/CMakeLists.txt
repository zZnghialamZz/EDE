# =====================================================
#               _______ _____  _______ 
#              |    ___|     \|    ___|
#              |    ___|  --  |    ___|
#              |_______|_____/|_______|
#                        ---
#              Ethan Development Editor
# =====================================================
# @file CMakeLists - EDE/EDE
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

set(EDE_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

set(HEADERS 
	"${EDE_SRC_DIR}/terminal.h"
	"${EDE_SRC_DIR}/command.h"
	"${EDE_SRC_DIR}/memory.h"
	"${EDE_SRC_DIR}/utils.h"
	"${EDE_SRC_DIR}/input.h"
)

set(SOURCES 
	"${EDE_SRC_DIR}/terminal.cpp"
	"${EDE_SRC_DIR}/command.cpp"
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
