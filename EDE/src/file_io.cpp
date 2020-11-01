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
// Static Helpers
// -----------------------------------------------------------------------
void EDE_EditorAppendRow(const char* s, size_t len) {
  // TODO(Nghia Lam): This method reallocate each line of file, which may cause some
  // performance issue -> Need more tests and optimization ?
  EDE().Rows = (EDE_EditorRows*) realloc(EDE().Rows, 
                                         sizeof(EDE_EditorRows) 
                                         * (EDE().DisplayRows + 1));
  
  int at = EDE().DisplayRows;
  
  EDE().Rows[at].Size = len;
  EDE().Rows[at].Chars = new char[len + 1];
  memcpy(EDE().Rows[at].Chars, s, len);
  EDE().Rows[at].Chars[len] = '\0';
  ++EDE().DisplayRows;
}

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
void EDE_EditorOpen(const char* file_name) {
  FILE *fp = fopen(file_name, "r");
  if (!fp) EDE_ErrorHandler("fopen");
  
  char *line = nullptr;
  size_t linecap = 0;
  ssize_t linelen = 0;
  
  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
      --linelen;
    EDE_EditorAppendRow(line, linelen);
  }
  free(line);
  fclose(fp);
}
