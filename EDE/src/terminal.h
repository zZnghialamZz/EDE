// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file terminal.h
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

#ifndef EDE_TERMINAL_H_
#define EDE_TERMINAL_H_

#include "memory.h"

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_TermRefreshScreen();                         // Refresh the terminal screen.
int  EDE_TermGetSize(int *cols, int * rows);          // Get the size of the terminal.

void EDE_TermSetStatusMessage(const char* msg, ...);  // Set the message to the status line

// Render
// ---
void EDE_TermDrawMessageBar(FixedBuffer *fb, int message_len);                     // Drawing the message bar below the status bar.
void EDE_TermDrawStatusBar(FixedBuffer *fb, const char* status, int status_len);   // Drawing the status bar at the end line of terminal screen.
void EDE_TermDrawRows(FixedBuffer *fb, const char* welcome_msg, int welcome_len);  // Drawing the rows primitives.

#endif // EDE_TERMINAL_H_
