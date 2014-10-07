
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
#include "base/allocator/CCAllocatorStrategyDefault.h"

NS_CC_BEGIN
NS_CC_ALLOCATOR_BEGIN

/** @brief Allocator takes an allocation strategy which provides the methods
    allocate and deallocate.
 
    The default allocator stragey AllocatorStrategyDefault uses malloc and
    free directly. Different strategies will allow for allocating all memory
    up front, and allow for different diagnostic features.
 
    Allocators can be globally enabled/disabled using the preprocessor define
    CC_ENABLE_ALLOCATOR in ccConfig.h. Setting this define to 0 disables all
    custom allocator functionality for Cocos2d-X
 */
template <typename Strategy = AllocatorStrategyDefault>
class Allocator
	: public Strategy
{
public:

	virtual ~Allocator()
	{}
};

NS_CC_ALLOCATOR_END
NS_CC_END
