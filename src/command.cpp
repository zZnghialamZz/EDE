// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file command.cpp
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

#include "command.h"
#include "utils.h"
#include "input.h"
#include "config.h"
#include "syntax.h"

#include <string.h> // For memmove()

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------

// Cursor Commands
// ---
void EDE_EditorMoveCursor(const int key) {
  // Query the current row
  EDE_EditorRows* row = (EDE().CursorY >= EDE().DisplayRows) ? nullptr : &EDE().Rows[EDE().CursorY];
  
  switch(key) {
    case KEY_UP: {
      if (EDE().CursorY != 0)
        --EDE().CursorY;
      break;
    }
    case KEY_DOWN: {
      if (EDE().CursorY < EDE().DisplayRows)
        ++EDE().CursorY;
      break;
    }
    case KEY_LEFT: {
      if (EDE().CursorX != 0) {
        --EDE().CursorX;
      } else if (EDE().CursorY > 0) {
        // Move to the end of the previous line
        --EDE().CursorY;
        EDE().CursorX = EDE().Rows[EDE().CursorY].Size;
      }
      break;
    }
    case KEY_RIGHT: {
      if (row && EDE().CursorX < row->Size) {
        ++EDE().CursorX;
      } else if (row && EDE().CursorX == row->Size) {
        // Move to the begining of the next line
        ++EDE().CursorY;
        EDE().CursorX = 0;
      }
      break;
    }
  }
  
  // Re-query the row for snapping cursor position
  //   - Only allow the cursor the position at the very end of texts 
  //     in current line
  row = (EDE().CursorY >= EDE().DisplayRows) ? nullptr : &EDE().Rows[EDE().CursorY];
  int row_len = row ? row->Size : 0;
  if (EDE().CursorX > row_len)
    EDE().CursorX = row_len;
  
}

// Find Commands
// ---
static void EDE_EditorFindCallback(char* query, int key) {
  static int last_match = -1;
  static int direction  =  1;
  
  // NOTE(Nghia Lam): Support search forward and backward
  switch(key) {
    case '\r':
    case '\x1b': {
      last_match = -1;
      direction  =  1;
      return;
    }
    
    case KEY_RIGHT:
    case KEY_DOWN: {
      direction = 1;
      break;
    }
    
    case KEY_LEFT:
    case KEY_UP: {
      direction = -1;
      break;
    }
    
    default: {
      last_match = -1;
      direction  =  1;
      break;
    }
  }
  
  if (last_match == -1) direction = 1;
  int current = last_match;
  // TODO(Nghia Lam): This is not quite optimal for searching, 
  // maybe I should think of another idea to make this better.
  for (int i = 0; i < EDE().DisplayRows; ++i) {
    current += direction;
    if (current == -1) current = EDE().DisplayRows - 1;
    else if (current == EDE().DisplayRows) current = 0;
    
    EDE_EditorRows* row = &EDE().Rows[current];
    char* match = strstr(row->Render, query);
    if (match) {
      last_match      = current;
      EDE().CursorY   = current;
      EDE().CursorX   = match - row->Render;
      EDE().RowOffset = EDE().DisplayRows;
      break;
    }
  }
}

void EDE_EditorFind() {
  int saved_cx     = EDE().CursorX;
  int saved_cy     = EDE().CursorY;
  int saved_coloff = EDE().ColOffset;
  int saved_rowoff = EDE().RowOffset;
  
  char* query = EDE_MessagePrompt("Search: %s (Use ESC | Arrows | Enter)",
                                  EDE_EditorFindCallback);
  if (query) {
    delete[] query;
  } else {
    EDE().CursorX   = saved_cx;
    EDE().CursorY   = saved_cy;
    EDE().ColOffset = saved_coloff;
    EDE().RowOffset = saved_rowoff;
  }
}

// Edit Commands
// ---
void EDE_EditorDeleteChar() {
  if (EDE().CursorY == EDE().DisplayRows) return;
  if (EDE().CursorX == 0 && EDE().CursorY == 0) return;
  
  EDE_EditorRows* row = &EDE().Rows[EDE().CursorY];
  if (EDE().CursorX > 0) {
    EDE_EditorRowDeleteChar(row, EDE().CursorX - 1);
    --EDE().CursorX;
  } else {
    EDE().CursorX = EDE().Rows[EDE().CursorY - 1].Size;
    EDE_EditorRowAppendString(&EDE().Rows[EDE().CursorY - 1], row->Chars, row->Size);
    EDE_EditorDeleteRow(EDE().CursorY);
    --EDE().CursorY;
  }
}

void EDE_EditorInsertChar(int c) {
  if (EDE().CursorY == EDE().DisplayRows)
    EDE_EditorInsertRow(EDE().DisplayRows, "", 0);
  EDE_EditorRowInsertChar(&EDE().Rows[EDE().CursorY], EDE().CursorX, c);
  ++EDE().CursorX;
}

void EDE_EditorInsertNewLine() {
  if (EDE().CursorX == 0) {
    EDE_EditorInsertRow(EDE().CursorY, "", 0);
  } else {
    EDE_EditorRows* row = &EDE().Rows[EDE().CursorY];
    EDE_EditorInsertRow(EDE().CursorY + 1, 
                        &row->Chars[EDE().CursorX], 
                        row->Size - EDE().CursorX);
    row = &EDE().Rows[EDE().CursorY];
    row->Size = EDE().CursorX;
    row->Chars[row->Size] = '\0';
    EDE_EditorUpdateRow(row);
  }
  ++EDE().CursorY;
  EDE().CursorX = 0;
}

// Row Low Level API
// ---
void EDE_EditorRowAppendString(EDE_EditorRows* row, const char* s, size_t len) {
  row->Chars = (char*) realloc(row->Chars, row->Size + len + 1);
  memcpy(&row->Chars[row->Size], s, len);
  row->Size += len;
  row->Chars[row->Size] = '\0';
  EDE_EditorUpdateRow(row);
  
  EDE().IsDirty = true;
}

void EDE_EditorRowInsertChar(EDE_EditorRows* row, int at, int c) {
  // TODO(Nghia Lam): Again, this method reallocate the memory every input...
  // -> Is there any better way to solve this?
  if (at < 0 || at > row->Size) at = row->Size;
  row->Chars = (char*) realloc(row->Chars, row->Size + 2);
  memmove(&row->Chars[at + 1], &row->Chars[at], row->Size - at + 1);
  ++row->Size;
  row->Chars[at] = c;
  EDE_EditorUpdateRow(row);
  
  EDE().IsDirty = true;
}

void EDE_EditorRowDeleteChar(EDE_EditorRows* row, int at) {
  if (at < 0 || at >= row->Size) return;
  memmove(&row->Chars[at], &row->Chars[at + 1], row->Size - at);
  --row->Size;
  EDE_EditorUpdateRow(row);
  
  EDE().IsDirty = true;
}

void EDE_EditorUpdateRow(EDE_EditorRows* row) {
  int tabs = 0;
  for (int i = 0; i < row->Size; ++i)
    if (row->Chars[i] == '\t') ++tabs;
  
  // Cleanup previouse row
  delete[] row->Render;
  row->Render = new char[row->Size + tabs * EDE_TAB_WIDTH + 1];
  
  int idx = 0;
  for (int i = 0; i < row->Size; ++i) {
    if (row->Chars[i] == '\t') {
      // NOTE(Nghia Lam): Currently we only support render space for tabs
      row->Render[idx++] = ' ';
      while (idx % EDE_TAB_WIDTH != 0)
        row->Render[idx++] = ' ';
    } else {
      row->Render[idx++] = row->Chars[i];
    }
  }
  row->Render[idx] = '\0';
  row->RSize = idx;
  
  EDE_EditorUpdateSyntax(row);
}

void EDE_EditorInsertRow(int at, const char* s, size_t len) {
  if (at < 0 || at > EDE().DisplayRows) return;
  
  // TODO(Nghia Lam): This method reallocate each line of file, which may cause some
  // performance issue -> Need more tests and optimization ?
  EDE().Rows = (EDE_EditorRows*) realloc(EDE().Rows, 
                                         sizeof(EDE_EditorRows) * (EDE().DisplayRows + 1));
  memmove(&EDE().Rows[at + 1], 
          &EDE().Rows[at], 
          sizeof(EDE_EditorRows) * (EDE().DisplayRows - at));
  
  EDE().Rows[at].Size  = len;
  EDE().Rows[at].Chars = new char[len + 1];
  memcpy(EDE().Rows[at].Chars, s, len);
  EDE().Rows[at].Chars[len] = '\0';
  
  EDE().Rows[at].RSize     = 0;
  EDE().Rows[at].Render    = nullptr;
  EDE().Rows[at].HighLight = nullptr;
  EDE_EditorUpdateRow(&EDE().Rows[at]);
  
  ++EDE().DisplayRows;
  
  EDE().IsDirty = true;
}

void EDE_EditorDeleteRow(int at) {
  if (EDE().CursorY == EDE().DisplayRows)
    return;
  EDE_EditorFreeRow(&EDE().Rows[at]);
  memmove(&EDE().Rows[at], 
          &EDE().Rows[at + 1], 
          sizeof(EDE_EditorRows) * (EDE().DisplayRows - at - 1));
  --EDE().DisplayRows;
  
  EDE().IsDirty = true;
}

void EDE_EditorFreeRow(EDE_EditorRows* row) {
  delete[] row->HighLight;
  delete[] row->Render;
  delete[] row->Chars;
}
