// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file utils.h
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

#ifndef EDE_UTILS_H_
#define EDE_UTILS_H_

// -----------------------------------------------------------------------
// Import Libraries
// TODO(Nghia Lam): Find a better way to organize this.
// -----------------------------------------------------------------------
#include <termios.h>    // For termios, tcgetattr(), tcgetattr(), ECHO, TCSAFLUSH
#include <unistd.h>     // For read(), STDIN_FILENO
#include <stdlib.h>     // For atexit();
#include <stdio.h>      // For printf(), vsnprint(), perror(), fopen(), getline(), FILE
#include <time.h>       // For time_t

// -----------------------------------------------------------------------
// Structure and type definition
// -----------------------------------------------------------------------
struct EDE_EditorRows {
  int Size;
  int RSize;
  char* Chars;
  char* Render;
};

struct EDE_EditorConfig {
  int CursorX, CursorY;
  int RowOffset;
  int ColOffset;
  int ScreenCols;
  int ScreenRows;
  int DisplayRows;
  char* FileName;
  char StatusMsg[80];
  time_t StatusTime;
  EDE_EditorRows* Rows;
  termios DefaultSettings;    // Default configuration of terminal
};

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_InitEditor();                         // Init Editor values when startup
void EDE_InitSettings();                       // Config the editor when startup
void EDE_ErrorHandler(const char* s);          // Out the error information

EDE_EditorConfig& EDE();                       // Get the global editor config

#endif // EDE_UTILS_H_
