// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file command.h
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

#ifndef EDE_COMMAND_H_
#define EDE_COMMAND_H_

#include "utils.h"

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------

// Cursor Commands
// ---
void EDE_EditorMoveCursor(const int key);  // Move the cursor in the screen

// Edit Commands
// ---
void EDE_EditorDeleteChar();               // Delete the char at the position of cursor.
void EDE_EditorInsertChar(int c);          // Insert the require char to the current row.
void EDE_EditorInsertNewLine();            // Insert the new line at the cursor position.

// Rows Low Level API
// TODO(Nghia Lam): Consider to make this private/static
// ---
void EDE_EditorFreeRow(EDE_EditorRows* row);                                    // Cleanup the required row.
void EDE_EditorDeleteRow(int at);                                               // Delete the entire row.
void EDE_EditorUpdateRow(EDE_EditorRows* row);                                  // Update render status for the row
void EDE_EditorInsertRow(int at, const char* s, size_t len);                    // Insert line of text into specific position 
void EDE_EditorRowDeleteChar(EDE_EditorRows* row, int at);                      // Delete character at a specific position in a row
void EDE_EditorRowInsertChar(EDE_EditorRows* row, int at, int c);               // Insert character at a specific position in a row
void EDE_EditorRowAppendString(EDE_EditorRows* row, const char* s, size_t len); // Append a string to the end of a row.

#endif //  EDE_COMMAND_H_
