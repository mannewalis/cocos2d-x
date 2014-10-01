
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

template <typename T>
class ObjectTraits
{
public:

	explicit ObjectTraits() 
	{}

	template <typename U>
	explicit ObjectTraits(ObjectTraits<U> const&) 
	{}

	virtual ~ObjectTraits() 
	{}
	
	void construct(T* address, const T& object)
	{
		new(address) T(object);
	}

	void destroy(T* address)
	{
		address->~T();
	}
};

template <typename T>
class AllocatorStrategy
{
public:

	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const pointer const_pointer;
	typedef const reference const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	explicit AllocatorStrategy() 
	{}

	explicit AllocatorStrategy(AllocatorStrategy const&) 
	{}
	
	template <typename U>
	explicit AllocatorStrategy(AllocatorStrategy<U> const&) 
	{}
	
	virtual ~AllocatorStrategy() 
	{}

	pointer allocate(size_type count = 1, typename std::allocator<void>::const_pointer = nullptr)
	{
		return reinterpret_cast<pointer>(::operator new(count * sizeof(T)));
	}

	void deallocate(pointer address, size_type)
	{
		::operator delete(address);
	}
};

template <typename T, typename Strategy = AllocatorStrategy<T>, typename Traits = ObjectTraits<T>>
class Allocator 
	: public Strategy
	, public Traits
{
public:

	typedef typename Strategy::value_type value_type;
	typedef typename Strategy::pointer pointer;
	typedef typename Strategy::reference reference;
	typedef typename Strategy::const_pointer const_pointer;
	typedef typename Strategy::const_reference const_reference;
	typedef typename Strategy::size_type size_type;

	explicit Allocator() 
	{}

	explicit Allocator(Allocator const& rhs)
		: Strategy(rhs)
		, Traits(rhs)
	{}

	template <typename U>
	explicit Allocator(Allocator<U> const&)
	{}

	template <typename U, typename S, typename Tr>
	explicit Allocator(Allocator<U, S, Tr> const& rhs)
		: Strategy(rhs)
		, Traits(rhs)
	{}

	virtual ~Allocator() 
	{}
};

NS_CC_END
