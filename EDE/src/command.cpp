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
  switch(key) {
    case KEY_UP: {
      if (EDE_GetEditorConfig().CursorY != 0)
        --EDE_GetEditorConfig().CursorY;
      break;
    }
    case KEY_DOWN: {
      if (EDE_GetEditorConfig().CursorY != EDE_GetEditorConfig().ScreenRows - 1)
        ++EDE_GetEditorConfig().CursorY;
      break;
    }
    case KEY_LEFT: {
      if (EDE_GetEditorConfig().CursorX != 0)
        --EDE_GetEditorConfig().CursorX;
      break;
    }
    case KEY_RIGHT: {
      if (EDE_GetEditorConfig().CursorX != EDE_GetEditorConfig().ScreenCols - 1)
        ++EDE_GetEditorConfig().CursorX;
      break;
    }
  }
}
