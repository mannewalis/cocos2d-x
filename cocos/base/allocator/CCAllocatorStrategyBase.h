
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
// AllocatorStrategyBase
// Provides a strategy base that contains a few methods and definitions
// that are helpful to all allocation strategies.
class AllocatorStrategyBase
{
public:
    
    typedef void* pointer;
    
    enum { kDefaultAlignment = sizeof(uint32_t) };
    
    virtual ~AllocatorStrategyBase()
    {}
    
    // @brief
    // Given an address and alignment in bytes, returns an address aligned to the number of bytes
    // For example, if the alignment is 4 which is standard, then the address is divisible evenly by 4.
    CC_ALLOCATOR_INLINE pointer aligned(const pointer address, const size_t alignment = kDefaultAlignment) const
    {
        return (pointer) (((intptr_t)address + (alignment - 1)) & ~(alignment - 1));
    }
    
    // @brief
    // Given an address and alignment in bytes, returns the number of additional bytes required
    // in order that the allocation can be aligned and still contain enough room.
    CC_ALLOCATOR_INLINE size_t alignedAdjustment(const pointer address, const size_t alignment = kDefaultAlignment) const
    {
        size_t adjustment = alignment - ((intptr_t)address & (alignment - 1));
        return adjustment == alignment ? 0 : adjustment;
    }
};

NS_CC_ALLOCATOR_END
NS_CC_END
