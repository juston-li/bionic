/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KERNEL_ARGUMENT_BLOCK_H
#define KERNEL_ARGUMENT_BLOCK_H

#include <elf.h>
#include <stdint.h>
#include <sys/auxv.h>

// When the kernel starts the dynamic linker, it passes a pointer to a block
// of memory containing argc, the argv array, the environment variable array,
// and the array of ELF aux vectors. This class breaks that block up into its
// constituents for easy access.
class KernelArgumentBlock {
 public:
  KernelArgumentBlock(void* raw_args) {
    uint32_t* args = reinterpret_cast<uint32_t*>(raw_args);
    argc = static_cast<int>(*args);
    argv = reinterpret_cast<char**>(args + 1);
    envp = argv + argc + 1;

    // Skip over all environment variable definitions to find aux vector.
    // The end of the environment block is marked by two NULL pointers.
    char** p = envp;
    while (*p != NULL) {
      ++p;
    }
    ++p; // Skip second NULL;

    auxv = reinterpret_cast<Elf32_auxv_t*>(p);
  }

  // Similar to ::getauxval but doesn't require the libc global variables to be set up,
  // so it's safe to call this really early on. This function also lets you distinguish
  // between the inability to find the given type and its value just happening to be 0.
  unsigned long getauxval(unsigned long type, bool* found_match = NULL) {
    for (Elf32_auxv_t* v = auxv; v->a_type != AT_NULL; ++v) {
      if (v->a_type == type) {
        if (found_match != NULL) {
            *found_match = true;
        }
        return v->a_un.a_val;
      }
    }
    if (found_match != NULL) {
      *found_match = false;
    }
    return 0;
  }

  int argc;
  char** argv;
  char** envp;
  Elf32_auxv_t* auxv;

 private:
  // Disallow copy and assignment.
  KernelArgumentBlock(const KernelArgumentBlock&);
  void operator=(const KernelArgumentBlock&);
};

#endif // KERNEL_ARGUMENT_BLOCK_H
