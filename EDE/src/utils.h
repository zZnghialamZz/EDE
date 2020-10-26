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
// -----------------------------------------------------------------------
#include <termios.h>    // For termios, tcgetattr(), tcgetattr(), ECHO, TCSAFLUSH
#include <unistd.h>     // For read(), STDIN_FILENO

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_InitSettings();  // Config the editor when startup

#endif // EDE_UTILS_H_
