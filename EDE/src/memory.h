// =====================================================
//               _______ _____  _______ 
//              |    ___|     \|    ___|
//              |    ___|  --  |    ___|
//              |_______|_____/|_______|
//                        ---
//              Ethan Development Editor
// =====================================================
// @file memory.h
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

#ifndef EDE_MEMORY_H_
#define EDE_MEMORY_H_

// -----------------------------------------------------------------------
// Type Definition & Structure
// -----------------------------------------------------------------------
struct FixedBuffer {
  int Size;
  int Index;
  char *Buf;
  
  FixedBuffer(int size) : Size(size), Index(0) {
    Buf = &Buf[Size];
  }
};

// -----------------------------------------------------------------------
// Main APIs
// -----------------------------------------------------------------------

// Fixed Buffers
void EDE_FixedBufAppend(FixedBuffer *fb, const char *s, int len);  // Append new elements to the buffer
void EDE_FixedBufFree(FixedBuffer *fb);                            // Reset the memory in the buffer

#endif //  EDE_MEMORY_H_
