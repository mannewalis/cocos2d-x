
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

#include "base/allocator/CCAllocator.h"
#include "base/allocator/CCAllocatorStrategyGlobalSmallBlock.h"
#include <new>
#include <exception>

USING_NS_CC_ALLOCATOR;

#if CC_ENABLE_ALLOCATOR

namespace
{
    static CC_ALLOCATOR_GLOBAL_NEW_DELETE global;
}

void* operator new[] (std::size_t size)
{
    void* ptr = global.allocate(size);
    if (nullptr == ptr)
        throw std::bad_alloc();
    return ptr;    
}

void* operator new(std::size_t size)
{
    void* ptr = global.allocate(size);
    if (nullptr == ptr)
        throw std::bad_alloc();
    return ptr;
}

void operator delete(void* p) throw()
{
    global.deallocate(p);
}

#endif // CC_ENABLE_ALLOCATOR
