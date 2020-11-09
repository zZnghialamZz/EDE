// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file main.cpp
// @author Nghia Lam <nghialam12795@gmail.com>
//
// @brief
//
// @license Copyright 2020 Nghia Lam
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// -----------------------------------------------------------------------
// NOTE(Nghia Lam): This program is currently follow the instruction of:
//          
//            https://viewsourcecode.org/snaptoken/kilo/
//
// The tutorial is implemented in C, but the goal is a fast, robust modern
// text editor which is written in C++17
// Here is a todo list:
// ---
// TODO(Nghia Lam): Walk through the tutorial of kilo.
// TODO(Nghia Lam): Add support for Windows
// TODO(Nghia Lam): Re-create it in C++17 with data oriented mindset.
// TODO(Nghia Lam): Support both terminal mode and graphical mode.
// TODO(Nghia Lam): Multiple file support
// -----------------------------------------------------------------------

// Feature tests macros for getline()
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

// -----------------------------------------------------------------------
// Import Libraries
// -----------------------------------------------------------------------
#include "utils.h"
#include "input.h"
#include "terminal.h"
#include "file_io.h"

// -----------------------------------------------------------------------
// Entry point
// -----------------------------------------------------------------------
int main(int argc, char* argv[]) {
  EDE_InitSettings();
  EDE_InitEditor();
  
  if (argc >= 2)
    EDE_EditorOpen(argv[1]);
  
  EDE_TermSetStatusMessage("HELP: Ctrl-S to save | Ctrl-Q to quit | Ctrl-F to find !");
  
  while (1) {
    // TODO(Nghia Lam): Check whether we are using GUI mode or terminal mode.
    EDE_TermRefreshScreen();
    EDE_ProcessKeyPressed();
  }
  
  return 0;
}