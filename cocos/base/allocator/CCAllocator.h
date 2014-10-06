
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

NS_CC_BEGIN

/** @brief ObjectTraits describes an allocatable object
    
 Templated class that represents a default allocatable object.
    Provide custom implementations to change the constructor/destructor behavior,
    or to change the default alignment of the object in memory.
 
*/
template <typename T, size_t _alignment = sizeof(uint32_t)>
class ObjectTraits
{
public:
    
    static const size_t alignment = _alignment;
    
    static constexpr size_t sizeForAlignment(size_t size)
    {
        // default alignment is 4 bytes, but we need to make
        // sure that the size is divisible by the alignment.
        return size + (alignment >> 1) - (size + (alignment >> 1)) % alignment;
    }

	virtual ~ObjectTraits()
	{}
	
	void construct(T* address)
	{
        ::new(address) T();
	}

	void destroy(T* address)
	{
		address->~T();
	}
};

/*
 Allocator Strategy
 allocates and deallocates memory
 */
template <typename T>
class AllocatorStrategy
{
public:

	typedef T value_type;
	typedef value_type* pointer;
	typedef std::size_t size_type;
    
    virtual ~AllocatorStrategy()
	{}

	inline pointer allocate(size_type size, typename std::allocator<void>::const_pointer = nullptr)
	{
		return (pointer)::operator new(size);
	}

	inline void deallocate(pointer address, size_type)
	{
        if (nullptr != address)
            ::operator delete(address);
	}
};

/*
 Allocator brings it all together
 - provides an STL compliant allocator interface for a given type T
 - allows customization of allocation strategy through template AllocatorStrategy
 - allows customization of object type and hints through ObjectTraits
 */
template <typename T, typename Strategy = AllocatorStrategy<T>, typename Traits = ObjectTraits<T>>
class ObjectAllocator
	: public Strategy
    , public Traits
{
public:

    ObjectAllocator()
        : _count(0)
    {}
    
	virtual ~ObjectAllocator()
	{}
    
    inline T* allocateObject(size_t size)
    {
        auto object = Strategy::allocate(Traits::sizeForAlignment(size));
        Traits::construct(object);
        ++_count;
        return object;
    }
    
    inline void deleteObject(T* object, size_t size)
    {
        if (object)
        {
            Traits::destroy(object);
            Strategy::deallocate(object, Traits::sizeForAlignment(size));
            --_count;
        }
    }
    
    inline size_t count() const
    {
        return _count;
    }
    
protected:
    
    size_t _count;
};

#define CC_USE_ALLOCATOR(T, A) \
    inline void* operator new (size_t size) \
    { \
        return (void*)A.allocateObject(size); \
    } \
    inline void operator delete (void* object, size_t size) noexcept \
    { \
        A.deleteObject((T*)object, size); \
    }

NS_CC_END
