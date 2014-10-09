
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

/****************************************************************************
                                   WARNING!
     Do not use Console::log or any other methods that use NEW inside of this
     allocator. Failure to do so will result in recursive memory allocation.
 ****************************************************************************/

#include "base/allocator/CCAllocatorMacros.h"
#include "base/allocator/CCAllocator.h"
#include "base/allocator/CCAllocatorGlobal.h"
#include "base/allocator/CCAllocatorStrategyFixedBlock.h"

NS_CC_BEGIN
NS_CC_ALLOCATOR_BEGIN

// @brief
class AllocatorStrategyGlobalSmallBlock
    : public Allocator<AllocatorStrategyGlobalSmallBlock>
{
public:
    
    static constexpr int kDefaultSmallBlockCount = 100;

    #define AType(size) Allocator<AllocatorStrategyFixedBlock<size, kDefaultSmallBlockCount>>
    
    void _lazy_init()
    {
        static bool first = true;
        if (first)
        {
            first = false;
            #define SBA(n, size) \
                _smallBlockAllocators[n] = nullptr; \
                if (size) \
                { \
                    auto v = ccAllocatorGlobal.allocate(2 << n); \
                    _smallBlockAllocators[n] = (void*)(new (v) AType(size)); \
                }

            SBA(2,  4);
            SBA(3,  8);
            SBA(4,  16);
            SBA(5,  32);
            SBA(6,  64);
            SBA(7,  128);
            SBA(8,  256);
            SBA(9,  512);
            SBA(10, 1024);
            SBA(11, 2048);
            SBA(12, 4096);
            SBA(13, 8192);

            #undef SBA
        }
    }
    
    virtual ~AllocatorStrategyGlobalSmallBlock()
    {}
    
    CC_ALLOCATOR_INLINE void* allocate(size_t size, typename std::allocator<void>::const_pointer = nullptr)
    {
        _lazy_init();
        
        if (size < sizeof(intptr_t)) // always allocate at least enough space to store a pointer. this is
            size = sizeof(intptr_t); // so we can link the empty blocks together in the block allocator.
        
        // if the size is greater than what we determine to be a small block size
        // then default to calling the
        if (size > kMaxSize)
            return ccAllocatorGlobal.allocate(size);
        
        // make sure the size fits into one of the
        // fixed sized block allocators we have above.
        size_t adjusted_size = nextPow2BlockSize(size);
       
        #define ALLOCATE(slot, size) \
            case size: \
            { \
                void* v = _smallBlockAllocators[slot]; \
                auto a = (AType(size)*)v; \
                address = a->allocate(size); \
            } \
            break;
        
        void* address;
        
        switch (adjusted_size)
        {
        ALLOCATE(2,  4);
        ALLOCATE(3,  8);
        ALLOCATE(4,  16);
        ALLOCATE(5,  32);
        ALLOCATE(6,  64);
        ALLOCATE(7,  128);
        ALLOCATE(8,  256);
        ALLOCATE(9,  512);
        ALLOCATE(10, 1024);
        ALLOCATE(11, 2048);
        ALLOCATE(12, 4096);
        ALLOCATE(13, 8192);
        default:
            CC_ASSERT(false);
            throw std::bad_alloc();
            break;
        }

        #undef ALLOCATE
        
        CC_ASSERT(nullptr != address);
        return address;
    }
    
    CC_ALLOCATOR_INLINE void deallocate(void* address, size_t size = 0)
    {
        if (size < sizeof(intptr_t)) // always allocate at least enough space to store a pointer. this is
            size = sizeof(intptr_t); // so we can link the empty blocks together in the block allocator.
        
        // if the size is greater than what we determine to be a small block size
        // then default to calling the
        if (size > kMaxSize)
            return ccAllocatorGlobal.deallocate(address, size);
        
        // make sure the size fits into one of the
        // fixed sized block allocators we have above.
        size_t adjusted_size = nextPow2BlockSize(size);
        
        #define DEALLOCATE(slot, size, address) \
            case size: \
            { \
                void* v = _smallBlockAllocators[slot]; \
                auto a = (AType(size)*)v; \
                return a->deallocate(address, size); \
            }
        
        switch (adjusted_size)
        {
        DEALLOCATE(2,  4,    address);
        DEALLOCATE(3,  8,    address);
        DEALLOCATE(4,  16,   address);
        DEALLOCATE(5,  32,   address);
        DEALLOCATE(6,  64,   address);
        DEALLOCATE(7,  128,  address);
        DEALLOCATE(8,  256,  address);
        DEALLOCATE(9,  512,  address);
        DEALLOCATE(10, 1024, address);
        DEALLOCATE(11, 2048, address);
        DEALLOCATE(12, 4096, address);
        DEALLOCATE(13, 8192, address);
        default:
            CC_ASSERT(false);
            throw std::bad_alloc();
        }
        
        #undef DEALLOCATE
        
        // should not reach here
        CC_ASSERT(false);
    }
    
protected:
    
    static constexpr size_t kMaxSmallBlockPower = 13; // 2^13 8192
    static constexpr size_t kMaxSize = 2 << (kMaxSmallBlockPower - 1); // 8192
    
    int nextPow2BlockSize(size_t size) const
    {
        --size;
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        return ++size;
    }
    
protected:
    
    void* _smallBlockAllocators[kMaxSmallBlockPower + 1];
};

NS_CC_ALLOCATOR_END
NS_CC_END
