
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
#include <vector>

NS_CC_BEGIN
NS_CC_ALLOCATOR_BEGIN

// @brief ObjectTraits describes an allocatable object
// Templated class that represents a default allocatable object.
// Provide custom implementations to change the constructor/destructor behavior,
// or to change the default alignment of the object in memory.
template <typename T, size_t _alignment = sizeof(uint32_t)>
class ObjectTraits
{
public:
    
    static const size_t alignment = _alignment;
    
    virtual ~ObjectTraits()
    {}
    
    // @brief constructor implementation for type T
    void construct(T* address)
    {
        ::new(address) T();
    }
    
    // @brief destructor implementation for type T
    void destroy(T* address)
    {
        address->~T();
    }
};


// @brief
// Fixed sized pool allocator strategy for objects of type T
// Optionally takes a page size which determines how many objects
// are added when the allocator needs more storage.
// ObjectTraits allows you to control the alignment, construction
// and destruction of an object in the pool.
template <typename T, size_t _page_size = 100, typename Traits = ObjectTraits<T>>
class AllocatorStrategyPool
    : public Traits
{
public:
    
    typedef T value_type;
    typedef value_type* pointer;
    typedef std::size_t size_type;
    
    static const size_t page_size = _page_size;
    
    AllocatorStrategyPool()
        : _list(nullptr)
        , _pages()
#if DEBUG
        , _available(0)
#endif
    {}
    
    virtual ~AllocatorStrategyPool()
    {
        size_t count = _pages.size();
#if DEBUG
        CC_ASSERT(0 == _available); // assert if we didn't free all the blocks before destroying the allocator.
#endif
        for (int i = 0; i < count; ++i)
            free(_pages[i]);
        _pages.clear();
    }
    
    // @brief
    // allocate a block of memory sizeof(T) by returning the first item in the list or if empty
    // then allocate a new page of objects, and return the first element and store the rest.
    // if sizeof(T) does not match the requested size, then the standard allocation method malloc is used.
    CC_ALLOCATOR_INLINE pointer allocate(size_type size, typename std::allocator<void>::const_pointer = nullptr)
    {
        T* object;
        if (sizeof(T) == size)
        {
            object = pop_front();
        }
        else
        {
            object = static_cast<T*>(malloc(size));
        }
        return object;
    }
    
    // @brief
    // deallocate an object sizeof(T) by pushing it on the head of a linked list of free objects.
    // if size is not sizeof(T) then the default deallocation method free is used instead.
    CC_ALLOCATOR_INLINE void deallocate(pointer address, size_type size)
    {
        if (sizeof(T) == size)
        {
            push_front(address);
        }
        else
        {
            free(address);
        }
    }
    
    // @brief
    // allocate an object, call its constructor and return the object.
    CC_ALLOCATOR_INLINE T* allocateObject(size_t size)
    {
        auto object = allocate(size);
        Traits::construct(object);
        return object;
    }
    
    // @brief
    // call an objects destructor and deallocate the object
    CC_ALLOCATOR_INLINE void deleteObject(T* object, size_t size)
    {
        if (object)
        {
            Traits::destroy(object);
            deallocate(object, size);
        }
    }

protected:
    
    CC_ALLOCATOR_INLINE void push_front(T* object)
    {
        if (nullptr == _list)
        {
            _list = object;
            *(uintptr_t*)object = 0;
        }
        else
        {
            uintptr_t* p = (uintptr_t*)(object);
            *p = (uintptr_t)_list;
            _list = object;
        }
#if DEBUG
        ++_available;
#endif
    }
    
    CC_ALLOCATOR_INLINE T* pop_front()
    {
        if (nullptr == _list)
        {
            allocatePage();
        }
        auto next = (T*)*(uintptr_t*)_list;
        auto object = _list;
        _list = next;
#if DEBUG
        --_available;
#endif
        return object;
    }
    
protected:
    
    CC_ALLOCATOR_INLINE void allocatePage()
    {
        void* page = malloc(sizeof(T) * page_size);
        _pages.push_back(page);
        auto object = reinterpret_cast<T*>(page) + page_size - 1;
        for (int i = 0; i < page_size; ++i, --object)
        {
            push_front(object);
        }
    }
    
protected:
    
    T* _list; // linked list of free blocks.
    
    // This could be optimized by baking a next pointer into the page size
    // and linking the active pages together, especially in release builds.
    // This would avoid the vector container entirely.
    std::vector<void*> _pages; // array of allocated pages.

#if DEBUG
    size_t _available; // number of objects that are free in the list.
#endif
};

NS_CC_ALLOCATOR_END
NS_CC_END
