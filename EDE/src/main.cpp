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

// NOTE(Nghia Lam): This program is currently follow the instruction of:
//          
//            https://viewsourcecode.org/snaptoken/kilo/
//
// The tutorial is implemented in C, but the goal is a fast, robust modern
// text editor which is written in C++17
// Here is a long todo list:
// ---
// TODO(Nghia Lam): Walk through the tutorial of kilo.
// TODO(Nghia Lam): Re-create it in C++17 with data oriented mindset.
// TODO(Nghia Lam): Support both terminal mode and graphical mode.

// -----------------------------------------------------------------------
// Import Libraries
// -----------------------------------------------------------------------
#include "utils.h"

#include <ctype.h> // for iscntrl()
#include <errno.h> // for errno, EAGAIN

// -----------------------------------------------------------------------
// Entry point
// -----------------------------------------------------------------------
int main(int argc, char* argv[]) {
  EDE_InitSettings();
  
  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) 
      EDE_ErrorHandler("read");
    
    // NOTE(Nghia Lam): iscntrl test whether a character is a control character.
    // Control characters are characters which are nonprintable that we dont
    // want to print to the screen. ASCII codes 0-31 are control characters, and
    // 127 is also a control character. ASCII codes 32-126 are all printable.
    if (iscntrl(c))
      printf("%d\r\n", c);
    else
      printf("%d ('%c')\r\n", c, c);
    
    if (c == 'q') break;
  }
  
  return 0;
}