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
static const char* HL_C_Extensions[] = { ".h", ".c", ".cpp", NULL };
static const char* HL_C_KeyWords[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
  "struct", "union", "typedef", "static", "enum", "class", "case",
  
  "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
  "void|", NULL
};
static EDE_EditorSyntax HLDB[] = {
  {
    "c",
    HL_C_Extensions,
    HL_C_KeyWords,
    "//",
    HLFLAGS_NUMBER | HLFLAGS_STRING
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
        
        for (int filerow = 0; filerow < EDE().DisplayRows; ++filerow)
          EDE_EditorUpdateSyntax(&EDE().Rows[filerow]);
        
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
  
  char* scs   = (char*) EDE().Syntax->SingleLineComment;
  int scs_len = scs ? strlen(scs) : 0;
  
  char** keywords = (char**) EDE().Syntax->KeyWords;
  
  bool prev_separator = true;
  int inside_string   = 0;
  int i = 0;
  
  while (i < row->RSize) {
    char c = row->Render[i];
    unsigned char prev_hl = (i > 0) ? row->HighLight[i - 1] : HL_NORMAL;
    
    // Highlight comment
    if (scs_len && !inside_string) {
      if (!strncmp(&row->Render[i], scs, scs_len)) {
        memset(&row->HighLight[i], HL_COMMENT, row->RSize - i);
        break;
      }
    }
    
    // Highlight strings
    if (EDE().Syntax->Flags &HLFLAGS_STRING) {
      if (inside_string) {
        row->HighLight[i] = HL_STRING;
        if (c == '\\' && i + 1 < row->Size) {
          row->HighLight[i + 1] = HL_STRING;
          i += 2;
          continue;
        }
        
        if (c == inside_string) inside_string = 0;
        ++i;
        prev_separator = true;
        continue;
        
      } else {
        if (c == '"' || c == '\'') {
          inside_string = c;
          row->HighLight[i] = HL_STRING;
          ++i;
          continue;
        }
      }
    }
    
    // Hightlight numbers
    if (EDE().Syntax->Flags & HLFLAGS_NUMBER) {
      if ((isdigit(c) && (prev_separator || prev_hl == HL_NUMBER)) ||
          (c == '.' && prev_hl == HL_NUMBER)) {
        row->HighLight[i] = HL_NUMBER;
        ++i;
        prev_separator = false;
        continue;
      }
    }
    
    // Highlight key words
    if (prev_separator) {
      int j;
      for (j = 0; keywords[j]; ++j) {
        int klen = strlen(keywords[j]);
        bool ktype = keywords[j][klen - 1] == '|';
        if (ktype) --klen;
        
        if (!strncmp(&row->Render[i], keywords[j], klen) &&
            IsSeparator(row->Render[i + klen])) {
          memset(&row->HighLight[i], ktype ? HL_KEYTYPE : HL_KEYWORD, klen);
          i += klen;
          break;
        }
      }
      if (keywords[j] != NULL) {
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
    case HL_COMMENT: return 36;
    case HL_KEYWORD: return 33;
    case HL_KEYTYPE: return 32;
    case HL_NUMBER : return 31;
    case HL_STRING : return 35;
    case HL_MATCH  : return 34;
    default: return 37;
  }
}

// HLDB API
EDE_EditorSyntax* EDE_HLDB() { return HLDB; }
int EDE_HLDB_Length() { return sizeof(HLDB) / sizeof(HLDB[0]); }
