
#pragma once

/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
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
#include "base/CCAllocator.h"
#include <vector>

NS_CC_BEGIN

template <typename T, size_t _page_size = 100>
class AllocatorStrategyFixed
{
public:
    
    typedef T value_type;
    typedef value_type* pointer;
    typedef std::size_t size_type;
    
    static const size_t page_size = _page_size;
    
    AllocatorStrategyFixed()
        : _list(nullptr)
        , _pages()
        , _available(0)
    {}
    
    virtual ~AllocatorStrategyFixed()
    {
        size_t count = _pages.size();
        for (int i = 0; i < count; ++i)
            free(_pages[i]);
        _pages.clear();
    }
    
#define PUSH_FRONT(x) \
    if (nullptr == _list) \
    { \
        _list = x; \
        *(uintptr_t*)x = 0; \
    } \
    else \
    { \
        uintptr_t* p = (uintptr_t*)(x); \
        *p = (uintptr_t)_list; \
        _list = x; \
    } \
    ++_available
    
#define POP_FRONT(object) \
    if (nullptr == _list) \
    { \
        allocatePage(); \
    } \
    auto next = (T*)*(uintptr_t*)_list; \
    object = _list; \
    _list = next; \
    --_available

    inline pointer allocate(size_type size, typename std::allocator<void>::const_pointer = nullptr)
    {
        T* object;
        if (sizeof(T) == size)
        {
            POP_FRONT(object);
        }
        else
        {
            object = static_cast<T*>(malloc(size));
        }
        return object;
    }
    
    inline void deallocate(pointer address, size_type size)
    {
        if (sizeof(T) == size)
        {
            PUSH_FRONT(address);
        }
        else
        {
            free(address);
        }
    }
    
    inline size_t available() const
    {
        return _available;
    }
    
protected:
    
    inline void allocatePage()
    {
        void* page = malloc(sizeof(T) * page_size);
        _pages.push_back(page);
        auto object = reinterpret_cast<T*>(page) + page_size - 1;
        for (int i = 0; i < page_size; ++i, --object)
        {
            PUSH_FRONT(object);
        }
    }
    
protected:
    
    // linked list of free blocks
    T* _list;
    std::vector<void*> _pages;
    size_t _available;
};
        
NS_CC_END