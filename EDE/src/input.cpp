// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file input.cpp
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

#include "input.h"
#include "utils.h"

#include <errno.h> // for errno, EAGAIN

// -----------------------------------------------------------------------
// Main APIs
// TODO(Nghia Lam): Change to Command system and mapping command.
// -----------------------------------------------------------------------
const char EDE_ReadKey() {
  char c    = '\0';
  int nread = 0;
  while(!(nread = read(STDIN_FILENO, &c, 1))) {
    if (nread == -1 && errno != EAGAIN) 
      EDE_ErrorHandler("read");
  }
  return c;
}

void EDE_ProcessKeyPressed() {
  char c = EDE_ReadKey();
  switch (c) {
    // Quit: Ctrl-Q
    case CTRL_KEY('q'): {
      // Clear screen when exit
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      
      exit(0);
      break;
    }
  }
}
