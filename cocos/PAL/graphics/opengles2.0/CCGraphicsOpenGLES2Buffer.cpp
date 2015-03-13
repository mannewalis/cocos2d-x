
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

#include "CCGraphicsOpenGLES2Buffer.h"

// remove cocos2d-x dependencies
#include "platform/CCGL.h"
#include "renderer/ccGLStateCache.h"
#include "base/ccMacros.h"
#include "base/CCConfiguration.h"

NS_PRIVATE_BEGIN

GraphicsOpenGLES2Buffer::GraphicsOpenGLES2Buffer()
    : _gltarget(-1)
    , _glusage(-1)
    , _boSize(0)
{}

GraphicsOpenGLES2Buffer::~GraphicsOpenGLES2Buffer()
{
    if (glIsBuffer(_bo))
    {
        glDeleteBuffers(1, &_bo);
        GL::bindVBO(_gltarget, 0);
    }    
}

bool GraphicsOpenGLES2Buffer::commitElements(const void* elements, ssize_t count, ssize_t begin)
{
    PAL_ASSERT(_bo, "native buffer object not initialized");
    PAL_ASSERT(elements != nullptr, "invalid elements array");
    PAL_ASSERT(count > 0, "invalid element count");
    
    GL::bindVBO(_gltarget, _bo);
    
    if (false == isDirty())
        return false;
    
//    // default to all elements
//    if (nullptr == elements)
//        elements = _elements;
//    
//    // default to all elements
//    if (0 == count)
//        count = _elementCount;
    
    GL::bindVBO(_gltarget, _bo);
    const auto size = getCapacityInBytes();
    CCASSERT(size, "size should not be 0");
    if (size >= _boSize)
    {
        _boSize = size;
        glBufferData(_gltarget, size, elements, _glusage);
        CHECK_GL_ERROR_DEBUG();
    }
    else
    {
        intptr_t p = (intptr_t)elements + begin * _elementSize;
        glBufferSubData(_gltarget, begin * _elementSize, count * _elementSize, (void*)p);
        CHECK_GL_ERROR_DEBUG();
    }

    return glGetError() == 0;
}

//
// Protected Methods
//

void GraphicsOpenGLES2Buffer::setupBO()
{
    PAL_ASSERT(!_bo, "native buffer object already created");
    _gltarget = arrayIntentToGLTarget(_arrayIntent);
    _glusage = arrayModeToGLUsage(_arrayMode);
    glGenBuffers(1, &_bo);
    CHECK_GL_ERROR_DEBUG();
}

unsigned GraphicsOpenGLES2Buffer::arrayIntentToGLTarget(ArrayIntent intent) const
{
    switch (intent)
    {
        case ArrayIntent::VertexData:
            return GL_ARRAY_BUFFER;
        case ArrayIntent::IndexData16:
        case ArrayIntent::IndexData32:
            return GL_ELEMENT_ARRAY_BUFFER;
        default:
            PAL_ASSERT(false, "Invalid array intent");
    }
}

unsigned GraphicsOpenGLES2Buffer::arrayModeToGLUsage(ArrayMode mode) const
{
    switch (mode)
    {
        case ArrayMode::Immutable:
            return GL_STATIC_DRAW;
        case ArrayMode::LongLived:
            return GL_DYNAMIC_DRAW;
        case ArrayMode::Dynamic:
            return GL_STREAM_DRAW;
        default:
            PAL_ASSERT(false, "invalid ArrayMode");
    }
}

NS_PRIVATE_END
