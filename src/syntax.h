// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file syntax.h
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

#ifndef EDE_SYNTAX_H_
#define EDE_SYNTAX_H_

#include "utils.h"

// -----------------------------------------------------------------------
// Structure and type definition
// -----------------------------------------------------------------------
enum EditorHighlight {
  HL_NORMAL  = 0,
  HL_COMMENT    ,
  HL_KEYWORD    ,
  HL_KEYTYPE    ,
  HL_NUMBER     ,
  HL_STRING     ,
  HL_MATCH
};

enum EditorHighlightFlags {
  HLFLAGS_NUMBER = 1 << 0,
  HLFLAGS_STRING = 1 << 1,
};

struct EDE_EditorSyntax {
  const char* FileType;
  const char** FileMatch;
  const char** KeyWords;
  const char* SingleLineComment;
  int Flags;
};

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_EditorSelectSyntax();                       // Tries to match the current filename to one of the filematch in HLDB
void EDE_EditorUpdateSyntax(EDE_EditorRows* row);    // Update the syntax highlighting
int  EDE_EditorSyntaxToColor(int highlight);         // Return the color based on syntax

EDE_EditorSyntax* EDE_HLDB();                        // Return the Hightlight Database
int               EDE_HLDB_Length();                 // Return the length of the Highlight Database

#endif // EDE_SYNTAX_H_
