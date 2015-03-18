
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
    : _glbo(0)
    , _glboSize(0)
{}

GraphicsOpenGLES2Buffer::~GraphicsOpenGLES2Buffer()
{
    if (glIsBuffer(_glbo))
    {
        glDeleteBuffers(1, &_glbo);
        GL::bindVBO(_bufferIntentToGLTarget(_bufferIntent), 0);
    }    
}

void GraphicsOpenGLES2Buffer::bindAndCommit(const void* elements, ssize_t start, ssize_t count)
{
    CCASSERT(true == hasNative(), "bindAndCommit : array has no native buffer");
    
    if (_glbo)
        GL::bindVBO(_bufferIntentToGLTarget(_bufferIntent), _glbo);
    
    if (false == isDirty())
        return;
    
    // default to all elements
    if (nullptr == elements)
        elements = _buffer;
    
    // default to all elements
    if (0 == count)
        count = _elementCount;
    
    const auto target = _bufferIntentToGLTarget(_bufferIntent);
    const auto usage  = _bufferModeToGLUsage(_bufferMode);
    
    if (0 == _glbo)
    {
        glGenBuffers(1, &_glbo);
        _glboSize = getCapacityInBytes();
        CCASSERT(_glboSize, "_vboSize should not be 0");
        GL::bindVBO(target, _glbo);
        glBufferData(target, _glboSize, nullptr, usage);
        glBufferData(target, count * getElementSize(), elements, usage);
        CHECK_GL_ERROR_DEBUG();
    }
    else
    {
        GL::bindVBO(target, _glbo);
        const auto size = getCapacityInBytes();
        CCASSERT(size, "size should not be 0");
        if (size >= _glboSize)
        {
            _glboSize = size;
            glBufferData(target, size, elements, usage);
            CHECK_GL_ERROR_DEBUG();
        }
        else
        {
            intptr_t p = (intptr_t)elements + start * _elementSize;
            glBufferSubData(target, start * _elementSize, count * _elementSize, (void*)p);
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    setDirty(false);
}

void GraphicsOpenGLES2Buffer::recreate() const
{
    const auto target = _bufferIntentToGLTarget(_bufferIntent);

    if (glIsBuffer(_glbo))
    {
        glDeleteBuffers(1, &_glbo);
        GL::bindVBO(target, 0);
    }
    glGenBuffers(1, (GLuint*)&_glbo);
    GL::bindVBO(target, _glbo);
    if (_buffer)
    {
        const auto usage  = _bufferModeToGLUsage(_bufferMode);
        glBufferData(target, _elementSize * _elementCount, _buffer, usage);
        GL::bindVBO(target, _glbo);
        if(!glIsBuffer(_glbo))
        {
            CCLOGERROR("Renderer::recreate() : recreate VertexBuffer Error");
        }
    }
}

unsigned GraphicsOpenGLES2Buffer::getBO() const
{
    return _glbo;
}

//
// Protected Methods
//

unsigned GraphicsOpenGLES2Buffer::_bufferIntentToGLTarget(BufferIntent intent) const
{
    switch (intent)
    {
        case BufferIntent::VertexData:
            return GL_ARRAY_BUFFER;
        case BufferIntent::IndexData16:
        case BufferIntent::IndexData32:
            return GL_ELEMENT_ARRAY_BUFFER;
        default:
            PAL_ASSERT(false, "Invalid array intent");
    }
    return -1;
}

unsigned GraphicsOpenGLES2Buffer::_bufferModeToGLUsage(BufferMode mode) const
{
    switch (mode)
    {
        case BufferMode::Immutable:
            return GL_STATIC_DRAW;
        case BufferMode::LongLived:
            return GL_DYNAMIC_DRAW;
        case BufferMode::Dynamic:
            return GL_STREAM_DRAW;
        default:
            PAL_ASSERT(false, "invalid BufferMode");
    }
    return -1;
}

NS_PRIVATE_END
