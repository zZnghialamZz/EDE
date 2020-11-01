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

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
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
