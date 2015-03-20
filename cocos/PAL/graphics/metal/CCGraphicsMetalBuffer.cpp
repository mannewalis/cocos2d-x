
/****************************************************************************
 Copyright (c) 2013-2015 Chukong Technologies Inc.
 
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

#include "CCGraphicsMetalBuffer.h"

#ifdef CC_METAL_AVAILABLE

#include "CCGraphicsMetal.h"
#include "CCGraphicsMetalViewController.h"

#import "Metal/MTLBuffer.h"

NS_PRIVATE_BEGIN

GraphicsMetalBuffer::GraphicsMetalBuffer()
    : _bo(nullptr)
    , _boSize(0)
{}

GraphicsMetalBuffer::~GraphicsMetalBuffer()
{
    [(id)_bo release];
}
    
void GraphicsMetalBuffer::commit(const void* elements, ssize_t start, ssize_t count)
{
    const auto size = getCapacityInBytes();
    CCASSERT(size, "size should not be 0");

    if (!_bo || size > _boSize)
    {
        [(id)_bo release];
        auto const device = GraphicsMetal::getInstance()->viewController().device;
        auto bo = [device newBufferWithLength:size options:MTLResourceOptionCPUCacheModeDefault];
        PAL_ASSERT(bo, "invalid buffer object");
        _buffer = bo.contents;
        _bo = (void*)bo;
        _boSize = size;
    }
    
    void* p = (void*)((uintptr_t)_buffer + (start * getElementSize()));
    memcpy(p, elements, count * getElementSize());
}

void GraphicsMetalBuffer::recreate() const
{
    // unknown if Metal buffers need recreation
}

void* GraphicsMetalBuffer::getBO() const
{
    return _bo;
}

NS_PRIVATE_END

#endif//CC_METAL_AVAILABLE
