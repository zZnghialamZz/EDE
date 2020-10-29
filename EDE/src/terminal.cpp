// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file terminal.cpp
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

#include "terminal.h"
#include "utils.h"

#include <ctype.h>      // For iscntrl()
#include <sys/ioctl.h>  // For ioctl(), winsize, TIOCGWINSZ

// -----------------------------------------------------------------------
// Static Helpers
// -----------------------------------------------------------------------
int EDE_TermGetCursorPosition(int *cols, int *rows) {
  // NOTE(Nghia Lam): <ESC>[6n query the cursor information data.
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) 
    return -1;
  
  char buf[32];
  unsigned int i = 0;
  
  while (i < sizeof(buf) - 1) {
    if (read(STDOUT_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    ++i;
  }
  buf[i] = '\0';
  
  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
  
  return 0;
}

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_TermRefreshScreen() {
  // NOTE(Nghia Lam): Escape sequence <ESC>[2J - Clear the entire screen
  //   - \x1b: Escape squence (or 27 in decimal) start, follow by [ character
  write(STDOUT_FILENO, "\x1b[2J", 4);
  // NOTE(Nghia Lam): Escape sequence <ESC>[H - Re-position cursor to top
  // left corner.
  write(STDOUT_FILENO, "\x1b[H", 3);
  
  EDE_TermDrawRows();
  
  write(STDOUT_FILENO, "\x1b[H", 3); // Re-position cursor back after drawing
}

void EDE_TermDrawRows() {
  for (int y = 0; y < EDE_GetEditorConfig().ScreenRows; ++y) {
    write(STDOUT_FILENO, "~", 1);
    
    if (y < EDE_GetEditorConfig().ScreenRows -1)
      write(STDOUT_FILENO, "\r\n", 2);
  }
}

int EDE_TermGetSize(int *cols, int * rows) {
  winsize ws;
  // NOTE(Nghia Lam): Since ioctl() wont always give us the right number from 
  // every terminal, so we will provide some fallback here.
  // The idea is to position the cursor at the bottom right of the terminal 
  // screen, then query the columns and rows based on the the cursor's pos.
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) 
      return -1;
    return EDE_TermGetCursorPosition(cols, rows);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}
