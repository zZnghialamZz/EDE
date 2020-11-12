// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file syntax.cpp
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

#include "syntax.h"

#include <ctype.h>   // For isdigit()
#include <string.h>  // For memset(), strchr(), strrchr()

// -----------------------------------------------------------------------
// File type support definition
// -----------------------------------------------------------------------
static char* HL_C_Extensions[] = { 
  (char*) ".c", 
  (char*) ".h", 
  (char*) ".cpp", 
  NULL
};
static EDE_EditorSyntax HLDB[] = {
  {
    (char*) "c",
    HL_C_Extensions,
    HLFLAGS_NUMBERS
  }
};

// -----------------------------------------------------------------------
// Static Helpers
// -----------------------------------------------------------------------
static bool IsSeparator(int c) {
  return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_EditorSelectSyntax() {
  EDE().Syntax = nullptr; // Cleanup
  if (!EDE().FileName) return;
  
  char* ext = strrchr(EDE().FileName, '.');
  for (int hl = 0; hl < EDE_HLDB_Length(); ++hl) {
    EDE_EditorSyntax* s = &HLDB[hl];
    unsigned int i = 0;
    while(s->FileMatch[i]) {
      bool is_ext = (s->FileMatch[i][0] == '.');
      if ((is_ext && ext && !strcmp(ext, s->FileMatch[i])) ||
          (!is_ext && strstr(EDE().FileName, s->FileMatch[i]))) {
        EDE().Syntax = s;
        return;
      }
      ++i;
    }
  }
}

void EDE_EditorUpdateSyntax(EDE_EditorRows* row) {
  // TODO(Nghia Lam): Again this function realloc everytime it is called 
  // to update the syntax highlighting, which is not good in my opinion.
  row->HighLight = (unsigned char*) realloc(row->HighLight, row->RSize);
  memset(row->HighLight, HL_NORMAL, row->RSize);
  
  if (!EDE().Syntax) return;
  
  bool prev_separator = true;
  int i = 0;
  
  while (i < row->RSize) {
    char c = row->Render[i];
    unsigned char prev_hl = (i > 0) ? row->HighLight[i - 1] : HL_NORMAL;
    
    // Hightlight numbers
    if (EDE().Syntax->Flags & HLFLAGS_NUMBERS) {
      if ((isdigit(c) && (prev_separator || prev_hl == HL_NUMBER)) ||
          (c == '.' && prev_hl == HL_NUMBER)) {
        row->HighLight[i] = HL_NUMBER;
        ++i;
        prev_separator = false;
        continue;
      }
    }
    
    prev_separator = IsSeparator(c);
    ++i;
  }
}

// TODO(Nghia Lam): Abstract this for easy theme maker
int  EDE_EditorSyntaxToColor(int highlight) {
  switch(highlight) {
    case HL_NUMBER: return 31;
    case HL_MATCH : return 34;
    default: return 37;
  }
}

// HLDB API
EDE_EditorSyntax* EDE_HLDB() { return HLDB; }
int EDE_HLDB_Length() { return sizeof(HLDB) / sizeof(HLDB[0]); }
