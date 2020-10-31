// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file file_io.cpp
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

#include "file_io.h"
#include "utils.h"

#include <string.h>  // For memcpy()

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_EditorOpen(const char* file_name) {
  FILE *fp = fopen(file_name, "r");
  if (!fp) EDE_ErrorHandler("fopen");
  
  char *line = nullptr;
  size_t linecap = 0;
  ssize_t linelen = getline(&line, &linecap, fp);
  
  if (linelen != -1) {
    while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
      --linelen;
    
    EDE_GetEditorConfig().Row.Size = linelen;
    EDE_GetEditorConfig().Row.Chars = new char[linelen + 1];
    memcpy(EDE_GetEditorConfig().Row.Chars, line, linelen);
    EDE_GetEditorConfig().Row.Chars[linelen] = '\0';
    EDE_GetEditorConfig().DisplayRows = 1;
  }
  
  free(line);
  fclose(fp);
}
