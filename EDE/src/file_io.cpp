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
#include "command.h"
#include "terminal.h"

#include <string.h>  // For memcpy(), strerror()
#include <fcntl.h>   // For open(), O_RDWR, O_CREAT
#include <unistd.h>  // For ftruncate(), close()
#include <errno.h>   // For errno

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------
char* EDE_EditorRowsToString(int *buffer_size) {
  int total_size = 0;
  for (int i = 0; i < EDE().DisplayRows; ++i) 
    total_size += EDE().Rows[i].Size + 1;   // Add 1 byte for the new line character
  *buffer_size = total_size;
  
  char* buf    = new char[total_size];
  char* travel = buf;
  for (int i = 0; i < EDE().DisplayRows; ++i) {
    memcpy(travel, EDE().Rows[i].Chars, EDE().Rows[i].Size);
    travel += EDE().Rows[i].Size;
    *travel = '\n';
    ++travel;
  }
  
  return buf; // We return the allocate chars here, expect the use to call delete[] afterward.
}

void EDE_EditorOpen(const char* file_name) {
  delete[] EDE().FileName; // Clean up current open file
  EDE().FileName = strdup(file_name);
  
  FILE *fp = fopen(file_name, "r");
  if (!fp) EDE_ErrorHandler("fopen");
  
  char *line      = nullptr;
  size_t linecap  = 0;
  ssize_t linelen = 0;
  
  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
      --linelen;
    EDE_EditorAppendRow(line, linelen);
  }
  free(line);
  fclose(fp);
  
  EDE().IsDirty = false;
}

void EDE_EditorSave() {
  if (EDE().FileName == nullptr) return;
  
  int bufsize = 0;
  char* buf = EDE_EditorRowsToString(&bufsize);
  
  // NOTE(Nghia Lam): 
  //   - O_RDWR  : Open for reading & writing.
  //   - O_CREAT : Create a new file if it doesnt exist.
  //   - 0644    : The code permission for reading and writing text file.
  //   - ftruncate(): Set the file size to the specific number. It will cut off
  //                  the data if the file is shorter or add 0 to the end if the 
  //                  file is longer.
  int fd = open(EDE().FileName, O_RDWR | O_CREAT, 0644);
  if (fd != -1) {
    if (ftruncate(fd, bufsize) != -1) {
      if (write(fd, buf, bufsize) == bufsize) {
        close(fd);
        delete[] buf; // Cleanup
        EDE().IsDirty = false;
        EDE_TermSetStatusMessage("%d bytes has been written to disk !", bufsize);
        return; 
      }
    }
    close(fd);
  }
  
  // Clean up
  delete[] buf;
  EDE_TermSetStatusMessage("Cannot save! I/O Error: %s", strerror(errno));
}