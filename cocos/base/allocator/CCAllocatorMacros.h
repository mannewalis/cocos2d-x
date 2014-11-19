
#pragma once

/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 Author: Justin Graham (https://github.com/mannewalis)
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "base/ccConfig.h"

// namespace allocator {}
#ifdef __cplusplus
    #define NS_CC_ALLOCATOR_BEGIN   namespace allocator {
    #define NS_CC_ALLOCATOR_END     }
    #define USING_NS_CC_ALLOCATOR   using namespace allocator
    #define NS_CC_ALLOCATOR         ::allocator
#else
    #define NS_CC_ALLOCATOR_BEGIN
    #define NS_CC_ALLOCATOR_END
    #define USING_NS_CC_ALLOCATOR
    #define NS_CC_ALLOCATOR
#endif

// CC_ALLOCATOR_INLINE
// Turn off inlining of methods when debugging to make stack traces readable and stepping through code sane.
// By default inlined methods are hidden symbols since symbols are unique and inlines are not.
#if DEBUG
    #define CC_ALLOCATOR_INLINE
#else
    #define CC_ALLOCATOR_INLINE _LIBCPP_INLINE_VISIBILITY
#endif

// allocator macros
#if CC_ENABLE_ALLOCATOR

    // macros for new/delete
    // these will use a general thread safe allocator
    #define CC_NEW(klass, ...) new klass(__VAR_ARGS__)
    #define CC_DELETE(object) delete object

    // macros for malloc/free
    // these will use a general thread safe allocator
    #define CC_MALLOC(size) malloc(size)
    #define CC_FREE(address) free(address)

    // alloc on the stack
    #define CC_ALOCA(size) alloca(size)

    // helper macro for overriding new/delete operators for a class.
    #define CC_USE_ALLOCATOR_POOL(T, A) \
        CC_ALLOCATOR_INLINE void* operator new (size_t size) \
        { \
            return (void*)A.allocateObject(size); \
        } \
        CC_ALLOCATOR_INLINE void operator delete (void* object, size_t size) \
        { \
            A.deleteObject((T*)object, size); \
        }

    // Macro to override global new and delete with a general allocator
    #define CC_OVERRIDE_GLOBAL_NEWDELETE_WITH_ALLOCATOR(A) \
        void* operator new (size_t size) \
        { \
            void* ptr = A.allocate(size); \
            if (nullptr == ptr) \
                throw std::bad_alloc(); \
            return ptr; \
        } \
        void operator delete (void *p) \
        { \
            A.deallocate(p); \
        }

#else

    // macros for new/delete
    // these will use a general thread safe allocator
    #define CC_NEW(klass, ...) new klass(__VAR_ARGS__)
    #define CC_DELETE(object) delete object

    // macros for malloc/free
    // these will use a general thread safe allocator
    #define CC_MALLOC(size) malloc(size)
    #define CC_FREE(address) free(address)

    // alloc on the stack
    #define CC_ALOCA(size) alloca(size)

    // throw these away if not enabled
    #define CC_USE_ALLOCATOR_POOL(...)
    #define CC_OVERRIDE_GLOBAL_NEWDELETE_WITH_ALLOCATOR(...)

#endif

