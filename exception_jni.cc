//#include <stdarg.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string>
//#include <iostream>
#include "exception_jni.h"

using namespace std;

// const char kIllegalArgumentException[] = "java/lang/IllegalArgumentException";
// const char kIllegalStateException[] = "java/lang/IllegalStateException";
// const char kNullPointerException[] = "java/lang/NullPointerException";
// const char kIndexOutOfBoundsException[] = "java/lang/IndexOutOfBoundsException";
// const char kUnsupportedOperationException[] =
//     "java/lang/UnsupportedOperationException";

// void throwException(JNIEnv* env, const char* clazz, const char* fmt, ...) {
//   va_list args;
//   va_start(args, fmt);
//   const size_t max_msg_len = 512;
//   auto* message = static_cast<char*>(malloc(max_msg_len));
//   if (vsnprintf(message, max_msg_len, fmt, args) >= 0) {
//     env->ThrowNew(env->FindClass(clazz), message);
//   } else {
//     env->ThrowNew(env->FindClass(clazz), "");
//   }
//   free(message);
//   va_end(args);
// }
/**LiYu*/

// BufferErrorReporter::BufferErrorReporter(JNIEnv* env, int limit) {
//   buffer_ = new char[limit];
//   if (!buffer_) {
//     throwException(env, kNullPointerException,
//                    "Internal error: Malloc of BufferErrorReporter to hold %d "
//                    "char failed.",
//                    limit);
//     return;
//   }
//   start_idx_ = 0;
//   end_idx_ = limit - 1;
// }
/**LiYu*/


// void BufferErrorReporter::outInfo(){

// 	cout << "outInfo" << endl;
// }


// BufferErrorReporter::~BufferErrorReporter() { delete[] buffer_; }

// int BufferErrorReporter::Report(const char* format, va_list args) {
//   int size = 0;
//   if (start_idx_ < end_idx_) {
//     size = vsnprintf(buffer_ + start_idx_, end_idx_ - start_idx_, format, args);
//   }
//   start_idx_ += size;
//   return size;
// }

// const char* BufferErrorReporter::CachedErrorMessage() { return buffer_; }
