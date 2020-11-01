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
#include "config.h"

#include <ctype.h>      // For iscntrl()
#include <string.h>     // For strlen(), memset()
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

void EDE_TermScreenScroll() {
  if (EDE().CursorY < EDE().RowOffset)
    EDE().RowOffset = EDE().CursorY;
  if (EDE().CursorY >= 
      EDE().RowOffset + EDE().ScreenRows)
    EDE().RowOffset = EDE().CursorY - EDE().ScreenRows + 1;
  if (EDE().CursorX < EDE().ColOffset)
    EDE().ColOffset = EDE().CursorX;
  if (EDE().CursorX >= 
      EDE().ColOffset + EDE().ScreenCols)
    EDE().ColOffset = EDE().CursorX - EDE().ScreenCols + 1;
}

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_TermRefreshScreen() {
  EDE_TermScreenScroll();
  
  // Init welcome message
  char welcome[80];
  int welcome_len = snprintf(welcome, 
                             sizeof(welcome), 
                             "Ethan Development Editor -- Version %s", 
                             EDE_VERSION);
  if (welcome_len > EDE().ScreenCols) {
    welcome_len = EDE().ScreenCols;
  }
  
  // Cursor Position Buffer
  char cursor_buf[32];
  snprintf(cursor_buf, 
           sizeof(cursor_buf), 
           "\x1b[%d;%dH", 
           EDE().CursorY - EDE().RowOffset + 1,  // Convert to 1-based Index
           EDE().CursorX - EDE().ColOffset + 1); // Convert to 1-based Index
  
  // Input Drawing Buffer
  int input_buf = 0;
  for(int i = 0; i < EDE().DisplayRows; ++i) {
    input_buf += (EDE().Rows[i].Size > EDE().ScreenCols) 
      ? EDE().ScreenCols : EDE().Rows[i].Size;
  }
  
  // NOTE(Nghia Lam): A fixed buffer to contain all the command lists for refresh
  // the terminal screen, which included:
  //  - 6: Escape sequence <ESC>[?25l - Hide cursor before re-drawing screen
  //  - 3: Escape sequence <ESC>[H    - Re-position cursor to top left corner
  //  - rows * 6: ~<ESC>[K\r\n at the begining of each row, then minus 2 byte of 
  //              \r\n for the last row.
  //  - Size for welcome message when open a blank EDE.
  //  - Size for cursor position drawing command.
  //  - Size for text input drawing command
  //  - 6: Escape sequence <ESC>[?25h - Re-enable cursor after re-drawing screen
  int buffer_size = 15                                        // Size of all Escape sequence command
    + (EDE().ScreenRows * 6 - 2)              // Size of each line drawing command
    + welcome_len                                             // Welcome message's length
    + (EDE().ScreenCols - welcome_len) / 2    // Welcome message's padding (for center)
    + strlen(cursor_buf)                                      // Cursor position drawing command
    + input_buf;                                              // Input drawing command
  
  // This fixed buffer will allocate all the memory require for all the command once.
  // Then we only need to add the character command to it later. This approach will
  // prevent from dynamic relocate memory again and again during run-time.
  FixedBuffer fb(buffer_size);
  
  // NOTE(Nghia Lam): 
  //   - \x1b: Escape squence (or 27 in decimal) start, follow by [ character
  EDE_FixedBufAppend(&fb, "\x1b[?25l", 6);                    // Hide cursor before re-drawing screen
  EDE_FixedBufAppend(&fb, "\x1b[H", 3);                       // Re-position cursor to top left corner
  
  EDE_TermDrawRows(&fb, welcome, welcome_len);
  
  EDE_FixedBufAppend(&fb, cursor_buf, strlen(cursor_buf));    // Positioning Cursor
  EDE_FixedBufAppend(&fb, "\x1b[?25h", 6);                    // Re-enable cursor after re-drawing screen
  
  // Write the batch buffer at once
  write(STDOUT_FILENO, fb.Buf, fb.Size);
}

void EDE_TermDrawRows(FixedBuffer *fb, const char* welcome_msg, int welcome_len) {
  for (int y = 0; y < EDE().ScreenRows; ++y) {
    int file_row = y + EDE().RowOffset;
    if (file_row >= EDE().DisplayRows) {
      if (EDE().DisplayRows == 0 && y == EDE().ScreenRows / 3) {
        // Center welcome message
        int padding = (EDE().ScreenCols - welcome_len) / 2;
        if (padding) {
          EDE_FixedBufAppend(fb, "~", 1);
          --padding;
        }
        while(--padding)
          EDE_FixedBufAppend(fb, " ", 1);
        
        EDE_FixedBufAppend(fb, welcome_msg, welcome_len);
      }
      else {
        EDE_FixedBufAppend(fb, "~", 1);
      }
      
    } else {
      int len = EDE().Rows[file_row].Size - EDE().ColOffset;
      if (len < 0) len = 0;
      if (len > EDE().ScreenCols)
        len = EDE().ScreenCols;
      EDE_FixedBufAppend(fb, 
                         &EDE().Rows[file_row].Chars[EDE().ColOffset], 
                         len);
    }
    
    EDE_FixedBufAppend(fb, "\x1b[K", 3);  // Clear the line
    if (y < EDE().ScreenRows -1)
      EDE_FixedBufAppend(fb, "\r\n", 2);
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
