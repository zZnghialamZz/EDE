// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file utils.cpp
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

#include "utils.h"
#include "terminal.h"

// -----------------------------------------------------------------------
// Static Helpers
// -----------------------------------------------------------------------
static EDE_EditorConfig E; // Is this safe?

static void DisableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.DefaultSettings) == -1)
    EDE_ErrorHandler("tcsetattr");
}

static void EnableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.DefaultSettings) == -1)
    EDE_ErrorHandler("tcgetattr");
  atexit(DisableRawMode);
  
  termios raw = E.DefaultSettings;
  
  // NOTE(Nghia Lam): Input Flags
  //   - BRKINT turn off the break condition, which will send signal to the
  // program, just like how Ctrl-C did.
  //   - IXON disable software flow control like Ctrl-S & Ctrl-Q in terminal
  // mode. (Ctrl-S stop data being transmitted util you pressed Ctrl-Q).
  //   - ICRNL fix the Ctrl-M and Enter key to be overlap output 10 byte as
  // Ctrl-J.
  //   - INPCK disable parity checking, which doesnt seem to support in
  // modern terminal emulators.
  //   - ISTRIP make the 8th bits of each input to be stripped. This may
  // be already turn off in some systems.
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP); 
  // NOTE(Nghia Lam): Output Flags
  //   - OPOST turn off the post processing output.
  raw.c_oflag &= ~(OPOST); 
  // NOTE(Nghia Lam): Set the input character size to be 8 bits per bytes.
  // Some systems may already set this.
  raw.c_cflag |= (CS8); 
  // NOTE(Nghia Lam): Local Flags
  //   - ECHO is a bitflag which allow us to print what we are typing to the 
  // terminal, which maybe useful for some cases, but not in a text editor,
  // so we disable it here in the local flags.
  //   - ICANON is a flag that allow us to disable the canonical mode, this 
  // means that we will finally read input byte-by-byte instead of line-by-line
  // (We can get the input immediately without pressing Enter).
  //   - ISIG is a flag which allow us to disable Ctrl-C and Ctrl-Z in 
  // terminal mode.
  //   - IEXTEN disable the Ctrl-V function in terminal where it waits for us
  // to type another character and then send it literally.
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
  raw.c_cc[VMIN]  = 0; // read() return as soon as possible if there is any input.
  raw.c_cc[VTIME] = 1; // maximum of time to wait before read(), it is 100ms here.
  
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
    EDE_ErrorHandler("tcsetattr");
}

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_InitEditor() {
  // TODO(Nghia Lam): Checking whether we are using GUI or Terminal mode here
  E.CursorX = 0;
  E.CursorY = 0;
  
  if (EDE_TermGetSize(&E.ScreenCols, &E.ScreenRows) == -1)
    EDE_ErrorHandler("EDE_TermGetSize");
}

void EDE_InitSettings() {
  EnableRawMode();
}

void EDE_ErrorHandler(const char* s) {
  // Clear the screen when exit
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  // Output error and exit
  perror(s);
  exit(1);
}

EDE_EditorConfig& EDE_GetEditorConfig() { return E; }
