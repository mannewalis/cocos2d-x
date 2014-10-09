
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

#include "base/allocator/CCAllocatorMacros.h"
#include "base/allocator/CCAllocator.h"
#include "base/allocator/CCAllocatorGlobal.h"
#include <vector>

NS_CC_BEGIN
NS_CC_ALLOCATOR_BEGIN

// @brief
// Fixed sized block allocator strategy for allocating blocks
// of memory that are the same size.
// Optionally takes a page size which determines how many blocks
// are added when the allocator needs more storage.
template <size_t _block_size, size_t _page_size = 100, size_t _alignment = sizeof(uint32_t)>
class AllocatorStrategyFixedBlock
    : public Allocator<AllocatorStrategyFixedBlock<_block_size, _page_size, _alignment>>
{
public:
    
    static constexpr size_t block_size = _block_size;
    static constexpr size_t page_size  = _page_size;
    static constexpr size_t alignment  = _alignment;
    
    AllocatorStrategyFixedBlock()
    : _list(nullptr)
    , _pages()
#if DEBUG
    , _available(0)
#endif
    {}
    
    virtual ~AllocatorStrategyFixedBlock()
    {
        size_t count = _pages.size();
#if DEBUG
        CC_ASSERT(0 == _available); // assert if we didn't free all the blocks before destroying the allocator.
#endif
        for (int i = 0; i < count; ++i)
            ccAllocatorGlobal.deallocate(_pages[i], page_size);
        _pages.clear();
    }
    
    // @brief
    // allocate a block of memory sizeof(T) by returning the first item in the list or if empty
    // then allocate a new page of blocks, and return the first element and store the rest.
    // if sizeof(T) does not match the requested size, then the global allocator is used.
    CC_ALLOCATOR_INLINE void* allocate(size_t size)
    {
        CC_ASSERT(block_size == size);
        return pop_front();
    }
    
    // @brief
    // deallocate a block sizeof(T) by pushing it on the head of a linked list of free blocks.
    // if size is not sizeof(T) then the global allocator is used instead.
    CC_ALLOCATOR_INLINE void deallocate(void* address, size_t size = 0)
    {
        CC_ASSERT(0 == size || block_size == size);
        push_front(address);
    }
    
protected:
    
    CC_ALLOCATOR_INLINE void push_front(void* block)
    {
        if (nullptr == _list)
        {
            _list = block;
            *(uintptr_t*)block = 0;
        }
        else
        {
            uintptr_t* p = (uintptr_t*)(block);
            *p = (uintptr_t)_list;
            _list = block;
        }
#if DEBUG
        ++_available;
#endif
    }
    
    CC_ALLOCATOR_INLINE void* pop_front()
    {
        if (nullptr == _list)
        {
            allocatePage();
        }
        auto next = (void*)*(uintptr_t*)_list;
        auto block = _list;
        _list = next;
#if DEBUG
        --_available;
#endif
        return block;
    }
    
protected:
    
    CC_ALLOCATOR_INLINE void allocatePage()
    {
        void* page = ccAllocatorGlobal.allocate(block_size * page_size);
        _pages.push_back(page);
        uint8_t* block = (uint8_t*)page;
        for (int i = 0; i < page_size; ++i, block += block_size)
        {
            push_front(block);
        }
    }
    
protected:
    
    void* _list; // linked list of free blocks.
    
    // This could be optimized by baking a next pointer into the page size
    // and linking the active pages together, especially in release builds.
    // This would avoid the vector container entirely.
    std::vector<void*> _pages; // array of allocated pages.
    
#if DEBUG
    size_t _available; // number of blocks that are free in the list.
#endif
};

NS_CC_ALLOCATOR_END
NS_CC_END
