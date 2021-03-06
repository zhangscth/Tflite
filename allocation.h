/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
// Main abstraction controlling the tflite interpreter.
// See context.h for the API for defining operations (TfLiteRegistration).
#ifndef TENSORFLOW_CONTRIB_LITE_ALLOCATION_H_
#define TENSORFLOW_CONTRIB_LITE_ALLOCATION_H_

#include <iostream>
#include <cstdio>
#include <memory>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include "error_reporter.h"
#include "dhfs.h"

namespace tflite {

// A memory allocation handle. This could be a mmap or shared memory.
class Allocation {
 public:
  Allocation(ErrorReporter* error_reporter) : error_reporter_(error_reporter) {}
  virtual ~Allocation() {}

  // Base pointer of this allocation
  virtual const void* base() const = 0;
  // Size in bytes of the allocation
  virtual size_t bytes() const = 0;
  // Whether the allocation is valid
  virtual bool valid() const = 0;

 protected:
  ErrorReporter* error_reporter_;
};

class MMAPAllocation : public Allocation {
 public:
  // test : MAP_FAILED
  MMAPAllocation(const char* filename, ErrorReporter* error_reporter)
  : Allocation(error_reporter), mmapped_buffer_(MAP_FAILED){
    mmap_fd_ = open(filename, O_RDONLY);
    if (mmap_fd_ == -1) {
      error_reporter_->Report("Could not open '%s'.", filename);
      return; 
    }
    struct stat sb;
    fstat(mmap_fd_, &sb);
    buffer_size_bytes_ = sb.st_size;
    mmapped_buffer_ =
        mmap(nullptr, buffer_size_bytes_, PROT_READ, MAP_SHARED, mmap_fd_, 0);
    if (mmapped_buffer_ == MAP_FAILED) {
      error_reporter_->Report("Mmap of '%s' failed.", filename);
      return;
    }
  }

  // virtual ~MMAPAllocation();
  const void* base() const override{ return mmapped_buffer_; }
  size_t bytes() const override{ return buffer_size_bytes_; }
  bool valid() const override{ return mmapped_buffer_ != MAP_FAILED; }
  static bool IsSupported(){ return true; }

 protected:
  // Data required for mmap.
  int mmap_fd_ = -1;  // mmap file descriptor
  const void* mmapped_buffer_;
  size_t buffer_size_bytes_ = 0;
};



class FileCopyAllocation : public Allocation {
 public:
  FileCopyAllocation(const char* filename, ErrorReporter* error_reporter): Allocation(error_reporter) {
  // Obtain the file size, using an alternative method that is does not
  // require fstat for more compatibility.
  std::unique_ptr<FILE, decltype(&fclose)> file(fopen(filename, "rb"), fclose);
  if (!file) {
    error_reporter_->Report("Could not open '%s'.", filename);
    return;
  }
  // TODO(ahentz): Why did you think using fseek here was better for finding
  // the size?
  struct stat sb;
  if (fstat(fileno(file.get()), &sb) != 0) {
    error_reporter_->Report("Failed to get file size of '%s'.", filename);
    return;
  }
  buffer_size_bytes_ = sb.st_size;
  std::unique_ptr<char[]> buffer(new char[buffer_size_bytes_]);
  if (!buffer) {
    error_reporter_->Report("Malloc of buffer to hold copy of '%s' failed.",
                            filename);
    return;
  }
  size_t bytes_read =
      fread(buffer.get(), sizeof(char), buffer_size_bytes_, file.get());
  if (bytes_read != buffer_size_bytes_) {
    error_reporter_->Report("Read of '%s' failed (too few bytes read).",
                            filename);
    return;
  }
  // Versions of GCC before 6.2.0 don't support std::move from non-const
  // char[] to const char[] unique_ptrs.
  copied_buffer_.reset(const_cast<char const*>(buffer.release()));
}
  // virtual ~FileCopyAllocation();
  ~FileCopyAllocation(){}
  const void* base() const { return copied_buffer_.get(); }
  size_t bytes() const { return buffer_size_bytes_; }
  bool valid() const { return copied_buffer_ != nullptr; }

 private:
    // Data required for mmap.
    std::unique_ptr<const char[]> copied_buffer_;
    size_t buffer_size_bytes_ = 0;
};

// class MemoryAllocation : public Allocation {
//  public:
//   // Allocates memory with the pointer and the number of bytes of the memory.
//   // The pointer has to remain alive and unchanged until the destructor is
//   // called.
//   MemoryAllocation(const void* ptr, size_t num_bytes,
//                    ErrorReporter* error_reporter);
//   virtual ~MemoryAllocation();
//   const void* base() const override;
//   size_t bytes() const override;
//   bool valid() const override;

//  private:
//   const void* buffer_;
//   size_t buffer_size_bytes_ = 0;
// };

}  // namespace tflite

#endif  // TENSORFLOW_CONTRIB_LITE_ALLOCATION_H_
