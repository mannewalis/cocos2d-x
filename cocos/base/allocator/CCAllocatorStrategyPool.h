
#ifndef CC_ALLOCATOR_STRATEGY_POOL_H
#define CC_ALLOCATOR_STRATEGY_POOL_H

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
#include "base/allocator/CCAllocatorGlobal.h"
#include "base/allocator/CCAllocatorStrategyFixedBlock.h"
#include "base/allocator/CCAllocatorDiagnostics.h"
#include <vector>
#include <typeinfo>
#include <sstream>

NS_CC_BEGIN
NS_CC_ALLOCATOR_BEGIN

// @brief ObjectTraits describes an allocatable object
// Templated class that represents a default allocatable object.
// Provide custom implementations to change the constructor/destructor behavior,
// or to change the default alignment of the object in memory.
// @param T Type of object
// @param _alignment Alignment of object T
template <typename T, size_t _alignment = sizeof(uint32_t)>
class ObjectTraits
{
public:
    
    typedef T value_type;
    
    static constexpr size_t alignment = _alignment;
    
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
    
    // @brief returns the name of this object type T
    const char* name() const
    {
        return typeid(T).name();
    }
};


// @brief
// Fixed sized pool allocator strategy for objects of type T
// Optionally takes a page size which determines how many objects
// are added when the allocator needs more storage.
// ObjectTraits allows you to control the alignment, construction
// and destruction of an object in the pool.
// @param T Type of object.
// @param _page_size Number of objects of T in each page.
// @param O ObjectTraits for type T
// @see CC_USE_ALLOCATOR_POOL
template <typename T, size_t _page_size = 100, typename O = ObjectTraits<T>>
class AllocatorStrategyPool
    : public AllocatorStrategyFixedBlock<sizeof(T), _page_size>
    , public O
{
public:
    
    typedef T value_type;
    typedef value_type* pointer;
    
    // ugh wish I knew a way that I could declare this just once
    typedef AllocatorStrategyFixedBlock<sizeof(T), _page_size> tParentStrategy;
        
    // @brief Allocate block of size T
    // if size does not match sizeof(T) then the global allocator is called instead.
    // @see CC_USE_ALLOCATOR_POOL
    CC_ALLOCATOR_INLINE void* allocate(size_t size)
    {
        T* object;
        if (sizeof(T) == size)
        {
            object = (pointer)tParentStrategy::allocate(sizeof(T));
        }
        else
        {
            object = (T*)ccAllocatorGlobal.allocate(size);
        }
        O::construct(object);
        return object;
    }
    
    // @brief Deallocate block of size T
    // if size does not match sizeof(T) then the global allocator is called instead.
    // @see CC_USE_ALLOCATOR_POOL
    CC_ALLOCATOR_INLINE void deallocate(void* address, size_t size = 0)
    {
        if (address)
        {
            O::destroy((T*)address);
            if (sizeof(T) == size)
            {
                tParentStrategy::deallocate(address, sizeof(T));
            }
            else
            {
                ccAllocatorGlobal.deallocate(address, size);
            }
        }
    }
    
#if CC_ENABLE_ALLOCATOR_DIAGNOSTICS
    std::string diagnostics() const
    {
        std::stringstream s;
        s << typeid(AllocatorStrategyPool).name() << "count:" << tParentStrategy::_available << "\n";
        return s.str();
    }
#endif
};

NS_CC_ALLOCATOR_END
NS_CC_END

#endif//CC_ALLOCATOR_STRATEGY_POOL_H
