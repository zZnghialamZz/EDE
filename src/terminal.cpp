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
#include "syntax.h"

#include <ctype.h>      // For iscntrl()
#include <stdarg.h>     // For va_list(), va_start(), va_end()
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
  
  // Calculate buffer size of all drawing command
  // TODO(Nghia Lam): Maybe we can abstract this part to another function
  // ---
  // Init welcome message
  char welcome[80];
  int welcome_len = snprintf(welcome, 
                             sizeof(welcome), 
                             "Ethan Development Editor -- Version %s", 
                             EDE_VERSION);
  if (welcome_len > EDE().ScreenCols) 
    welcome_len = EDE().ScreenCols;
  // Init status bar
  char status[80];
  int status_len = snprintf(status, 
                            sizeof(status), 
                            " %.20s [%s] - %d/%d lines %s", 
                            EDE().FileName ? EDE().FileName : "[No name]", 
                            EDE().Syntax ? EDE().Syntax->FileType : "No file type",
                            EDE().CursorY + 1,
                            EDE().DisplayRows,
                            EDE().IsDirty ? "- (modified)" : "");
  if (status_len > EDE().ScreenCols) 
    status_len = EDE().ScreenCols;
  // Init message bar
  int message_len = (strlen(EDE().StatusMsg) > EDE().ScreenCols) 
    ? EDE().ScreenCols : strlen(EDE().StatusMsg);
  // Cursor Position Buffer
  char cursor_buf[32];
  snprintf(cursor_buf, 
           sizeof(cursor_buf), 
           "\x1b[%d;%dH", 
           EDE().CursorY - EDE().RowOffset + 1,  // Convert to 1-based Index
           EDE().CursorX - EDE().ColOffset + 1); // Convert to 1-based Index
  // Input Drawing Buffer
  int input_buf     = 0;
  for(int i = 0; i < EDE().DisplayRows; ++i) {
    int row_size = (EDE().Rows[i].Size > EDE().ScreenCols) ? EDE().ScreenCols : EDE().Rows[i].Size;
    int current_color = -1;
    input_buf += row_size;
    // Syntax coloring add to buffer
    // TODO(Nghia Lam): Doing this way is optimal with memory but its speed may not be good.
    // Since we loop through all the file 2 two just to store the memory and then drawing...
    for (int j = 0; j < row_size; ++j) {
      if (EDE().Rows[i].HighLight[j] == HL_NORMAL) {
        if (current_color != -1) {
          input_buf += 5; // Change to text default color
          current_color = -1;
        }
      } else {
        int color = EDE_EditorSyntaxToColor(EDE().Rows[i].HighLight[j]);
        if (color != current_color) {
          current_color = color;
          char buf_color[16];
          int clen = snprintf(buf_color, sizeof(buf_color), "\x1b[%dm", color);
          input_buf += clen;
        }
      }
    }
    input_buf += 5; // Reset to text default color
  }
  
  // NOTE(Nghia Lam): A fixed buffer to contain all the command lists for refresh
  // the terminal screen, which included:
  //  - 6: Escape sequence <ESC>[?25l - Hide cursor before re-drawing screen     -
  //  - 3: Escape sequence <ESC>[H    - Re-position cursor to top left corner     |-> 15 bytes
  //  - 6: Escape sequence <ESC>[?25h - Re-enable cursor after re-drawing screen -
  //  - rows * 6: ~<ESC>[K\r\n at the begining of each row.
  //  - welcome_len: Size for welcome message when open a blank EDE.
  //  - cursor_buf:  Size for cursor position drawing command.
  //  - input_buf:   Size for text input drawing command
  //  - status_len:  Size for drawing status bar. Please see EDE_TermDrawStatusBar() for more details.
  //  - message_len: Size for drawing message bar. Please see EDE_TermDrawMessageBar() for more details.
  int buffer_size = 15                        // Size of all Escape sequence command
    + (EDE().ScreenRows * 6)                  // Size of each line drawing command
    + welcome_len                             // Welcome message's length
    + (EDE().ScreenCols - welcome_len) / 2    // Welcome message's padding (for center)
    + strlen(cursor_buf)                      // Cursor position drawing command
    + input_buf                               // Input drawing command
    + status_len + EDE().ScreenCols + 9       // Status Bar drawing command
    + message_len + 3;                        // Message Bar drawing command
  
  // This fixed buffer will allocate all the memory require for all the command once.
  // Then we only need to add the character command to it later. This approach will
  // prevent from dynamic relocate memory again and again during run-time.
  FixedBuffer fb(buffer_size);
  
  // NOTE(Nghia Lam): 
  //   - \x1b: Escape squence (or 27 in decimal) start, follow by [ character
  EDE_FixedBufAppend(&fb, "\x1b[?25l", 6);                    // Hide cursor before re-drawing screen
  EDE_FixedBufAppend(&fb, "\x1b[H", 3);                       // Re-position cursor to top left corner
  
  EDE_TermDrawRows(&fb, welcome, welcome_len);
  EDE_TermDrawStatusBar(&fb, status, status_len);
  EDE_TermDrawMessageBar(&fb, message_len);
  
  EDE_FixedBufAppend(&fb, cursor_buf, strlen(cursor_buf));    // Positioning Cursor
  EDE_FixedBufAppend(&fb, "\x1b[?25h", 6);                    // Re-enable cursor after re-drawing screen
  
  // Write the batch buffer at once
  write(STDOUT_FILENO, fb.Buf, fb.Size);
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

void EDE_TermSetStatusMessage(const char* msg, ...) {
  va_list ap;
  va_start(ap, msg);
  vsnprintf(EDE().StatusMsg, sizeof(EDE().StatusMsg), msg, ap);
  va_end(ap);
  EDE().StatusTime = time(NULL); // Got the current time
}

// Render
// ---

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
      int len = EDE().Rows[file_row].RSize - EDE().ColOffset;
      if (len < 0) len = 0;
      if (len > EDE().ScreenCols) len = EDE().ScreenCols;
      
      // Render each text with different color based on syntax highlighting
      char* c           = &EDE().Rows[file_row].Render[EDE().ColOffset];
      unsigned char* hl = &EDE().Rows[file_row].HighLight[EDE().ColOffset];
      int current_color = -1;
      
      for (int i = 0; i < len; ++i) {
        if (hl[i] == HL_NORMAL) {
          if (current_color != -1) {
            EDE_FixedBufAppend(fb, "\x1b[39m", 5);
            current_color = -1;
          }
        } else {
          int color = EDE_EditorSyntaxToColor(hl[i]);
          if (color != current_color) {
            char buf_color[16];
            int clen = snprintf(buf_color, sizeof(buf_color), "\x1b[%dm", color);
            EDE_FixedBufAppend(fb, buf_color, clen); 
            current_color = color;
          }
        }
        EDE_FixedBufAppend(fb, &c[i], 1);      // Append the character
      }
      EDE_FixedBufAppend(fb, "\x1b[39m", 5); // Reset to text color default
    }
    
    EDE_FixedBufAppend(fb, "\x1b[K", 3);  // Clear the line
    EDE_FixedBufAppend(fb, "\r\n", 2);    // Go to the new line
  }
}

// NOTE(Nghia Lam): 
//   - Escape sequence <Esc>[7m: Invert drawing color to use black as text color,
// white as background color.
//   - Escape sequence <Esc>[m: Invert back to normal drawing color.
void EDE_TermDrawStatusBar(FixedBuffer *fb, const char* status, int status_len) {
  EDE_FixedBufAppend(fb, "\x1b[7m", 4); // Invert drawing color to use black as text color, white as background color.
  EDE_FixedBufAppend(fb, status, status_len);
  
  while (status_len < EDE().ScreenCols) {
    EDE_FixedBufAppend(fb, " ", 1); 
    ++status_len;
  }
  
  EDE_FixedBufAppend(fb, "\x1b[m", 3);  // Revert back to normal drawing color.
  EDE_FixedBufAppend(fb, "\r\n", 2);    // Status line.
}

// NOTE(Nghia Lam): 
//   - Escape sequence <Esc>[K: Clear the message bar
void EDE_TermDrawMessageBar(FixedBuffer *fb, int message_len) {
  EDE_FixedBufAppend(fb, "\x1b[K", 3);  // Clear the message bar.
  if (message_len && (time(NULL) - EDE().StatusTime) < 1) // Draw the message only 1 seconds
    EDE_FixedBufAppend(fb, EDE().StatusMsg, message_len);
}
