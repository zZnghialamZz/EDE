// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file utils.cpp
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

#include "utils.h"

#include <stdlib.h> // For atexit();

// -----------------------------------------------------------------------
// Static Helpers
// -----------------------------------------------------------------------
static termios origin_settings; // Default configuration of terminal

static void DisableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &origin_settings);
}

static void EnableRawMode() {
  tcgetattr(STDIN_FILENO, &origin_settings);
  atexit(DisableRawMode);
  
  termios raw = origin_settings;
  // NOTE(Nghia Lam): 
  //   - ECHO is a bitflag which allow us to print what we are typing to the 
  // terminal, which maybe useful for some cases, but not in a text editor,
  // so we disable it here in the local flags.
  //   - ICANON is a flag that allow us to disable the canonical mode, this 
  // means that we will finally read input byte-by-byte instead of line-by-line
  // (We can get the input immediately without pressing Enter).
  raw.c_lflag &= ~(ECHO | ICANON); 
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_InitSettings() {
  EnableRawMode();
}
