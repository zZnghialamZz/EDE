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
#include "config.h"
#include "command.h"
#include "file_io.h"
#include "terminal.h"

#include <errno.h> // for errno, EAGAIN

// -----------------------------------------------------------------------
// Main APIs
// TODO(Nghia Lam): Change to Command system and mapping command.
// -----------------------------------------------------------------------
int EDE_ReadKey() {
  char c    = '\0';
  int nread = 0;
  while(!(nread = read(STDIN_FILENO, &c, 1))) {
    if (nread == -1 && errno != EAGAIN) 
      EDE_ErrorHandler("read");
  }
  
  // Handle special command key 
  // ---
  // NOTE(Nghia Lam): When ever we meet an escape sequence we will try
  // to read two more byte to find which key is pressed.  
  if (c == '\x1b') {
    char seq[3];
    
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    
    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '1') {
        // NOTE(Nghia Lam): 
        //   - Page-Up is represent as <Esc>[5~
        //   - Page-Down is represent as <Esc>[6~
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1': return KEY_HOME;
            case '3': return KEY_DEL;
            case '4': return KEY_END;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return KEY_HOME;
            case '8': return KEY_END;
          }
        }
        
      } else {
        // NOTE(Nghia Lam): Since arrow key terminal will return as an escape sequence 
        // with '\x1b[' follow with A, B, C, D (depend on which arrow is pressed up, 
        // down, right or left). 
        switch(seq[1]) {
          case 'A': return KEY_UP;    // Up arrow key
          case 'B': return KEY_DOWN;  // Down arrow key
          case 'C': return KEY_RIGHT; // Right arrow key
          case 'D': return KEY_LEFT;  // Left arrow key
          case 'H': return KEY_HOME;  // Home key
          case 'F': return KEY_END;   // End key
        }
      }
    } else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H': return KEY_HOME;
        case 'F': return KEY_END;
      }
    }
    
    return '\x1b';
    
  } else {
    return c;
  }
}

// NOTE(Nghia Lam): Currently using vi-binding for this editor
void EDE_ProcessKeyPressed() {
  static int quit_times = EDE_QUIT_TIME;
  
  int c = EDE_ReadKey();
  switch (c) {
    // Quit: Ctrl-Q
    // ---
    case CTRL_KEY('q'): {
      if (EDE().IsDirty && quit_times > 0) {
        EDE_TermSetStatusMessage("Warning!! File has unsaved changes. "
                                 "Press Ctrl-Q %d more times to quit.",
                                 quit_times);
        quit_times--;
        return;
      }
      
      // Clear screen when exit
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
    }
    
    // Primitives
    // ---
    case '\r': {
      // TODO(Nghia Lam): Handle this key
      break;
    }
    
    // NOTE(Nghia Lam): Originally, the Ctrl-H combo would return the control code 8,
    // which is similar to the backspace control code in some old terminal.
    // -> We might dont want it to be like this.
    case CTRL_KEY('h'):
    case BACKSPACE:
    case KEY_DEL: {
      // TODO(Nghia Lam): Handle this key
      break;
    }
    
    case CTRL_KEY('l'):
    case '\x1b': {
      // TODO(Nghia Lam): Handle this key
      break;
    }
    
    // File IO
    // ---
    case CTRL_KEY('s'): {
      EDE_EditorSave();
      break;
    }
    
    // Cursor Movement
    // ---
    case KEY_UP:
    case KEY_DOWN:
    case KEY_RIGHT:
    case KEY_LEFT: {
      EDE_EditorMoveCursor(c);
      break;
    }
    
    case PAGE_UP:
    case PAGE_DOWN: {
      // Make PAGE_UP, PAGE_DOWN scrolling
      if (c == PAGE_UP) {
        EDE().CursorY = EDE().RowOffset;
      } else if (c == PAGE_DOWN) {
        EDE().CursorY = EDE().RowOffset + EDE().ScreenRows - 1;
        if (EDE().CursorY > EDE().DisplayRows) 
          EDE().CursorY = EDE().DisplayRows;
      }
      
      int times = EDE().ScreenRows;
      while (--times)
        EDE_EditorMoveCursor(c == PAGE_UP ? KEY_UP : KEY_DOWN);
      break;
    }
    
    case KEY_HOME: {
      EDE().CursorX = 0;
      break;
    }
    case KEY_END: {
      if (EDE().CursorY < EDE().DisplayRows)
        EDE().CursorX = EDE().Rows[EDE().CursorY].Size;
      break;
    }
    
    // Input char
    // ---
    default: {
      EDE_EditorInsertChar(c);
      break;
    }
  }
  
  quit_times = EDE_QUIT_TIME;
}