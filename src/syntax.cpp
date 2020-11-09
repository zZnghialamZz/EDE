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
#include <string.h>  // For memset()

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_EditorUpdateSyntax(EDE_EditorRows* row) {
  // TODO(Nghia Lam): Again this function realloc everytime it is called 
  // to update the syntax highlighting, which is not good in my opinion.
  row->HighLight = (unsigned char*) realloc(row->HighLight, row->RSize);
  memset(row->HighLight, HL_NORMAL, row->RSize);
  
  for (int i = 0; i < row->RSize; ++i) {
    if (isdigit(row->Render[i]))
      row->HighLight[i] = HL_NUMBER;
  }
}

// TODO(Nghia Lam): Abstract this for easy theme maker
int  EDE_EditorSyntaxToColor(int highlight) {
  switch(highlight) {
    case HL_NUMBER: return 31;
    default: return 37;
  }
}
