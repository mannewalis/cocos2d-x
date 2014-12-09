
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

#include "platform/CCPlatformMacros.h"
#include "base/allocator/CCAllocatorMacros.h"

NS_CC_BEGIN
NS_CC_ALLOCATOR_BEGIN

// @brief
// AllocatorBase
// Provides a base that contains a few methods and definitions
// that are helpful to all allocation strategies.
class AllocatorBase
{
public:
    
    typedef void* pointer;
    
    enum { kDefaultAlignment = 16 };
    
    // this must be true for SSE instructions to be 16 byte aligned
    // we can now use kDefault alignment as our smallest alloc size
    static_assert(sizeof(uintptr_t) <= kDefaultAlignment, "pointer size must be smaller than default alignment");
    
    virtual ~AllocatorBase()
    {}
    
    // @brief
    // Given an address and alignment in bytes, returns an address aligned to the number of bytes
    // For example, if the alignment is 4 which is standard, then the address is divisible evenly by 4.
    CC_ALLOCATOR_INLINE pointer aligned(const pointer address, const size_t alignment = kDefaultAlignment) const
    {
        return (pointer) (((intptr_t)address + (alignment - 1)) & ~(alignment - 1));
    }
    
    // @brief Calculate the next power of two for a given size.
    // Most blocks requested are already a power of two. For small block alloc
    // this means we cannot add overhead, hence the slightly less performant
    // searching of fixed block pages to determine size if none is specified.
    size_t nextPow2BlockSize(size_t size) const
    {
        --size;
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        return ++size;
    }
};

NS_CC_ALLOCATOR_END
NS_CC_END
