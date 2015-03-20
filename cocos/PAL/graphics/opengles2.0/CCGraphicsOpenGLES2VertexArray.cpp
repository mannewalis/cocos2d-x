
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

#include "PAL/CCPALTypes.h"
#include "CCGraphicsOpenGLES2VertexArray.h"
#include "CCGraphicsOpenGLES2Buffer.h"

#include "platform/CCGL.h"
#include "renderer/ccGLStateCache.h"
#include "base/ccMacros.h"
#include "base/CCConfiguration.h"

NS_PRIVATE_BEGIN

GraphicsOpenGLES2VertexArray::GraphicsOpenGLES2VertexArray(Primitive drawPrimitive)
    : super(drawPrimitive)
    , _vao(0)
{}

GraphicsOpenGLES2VertexArray::~GraphicsOpenGLES2VertexArray()
{
    if (_vao && glIsBuffer(_vao))
    {
        glDeleteVertexArrays(1, (GLuint*)&_vao);
        GL::bindVAO(0);
        _vao = 0;
    }
}

ssize_t GraphicsOpenGLES2VertexArray::draw(ssize_t start, ssize_t count)
{
    CCASSERT(start >= 0, "Invalid start value");
    CCASSERT(count >= 0, "Invalid count value");
    
    if (0 == count)
    {
        // if we are drawing indexed, then use the count of indices to draw
        count = _indices ? _indices->getElementCount() : this->getCount();
    }
    
    if (_vao)
    {
        GL::bindVAO(_vao);
        CHECK_GL_ERROR_DEBUG();
    }
    
    if (0 == _vao || isDirty())
    {
        if (0 == _vao && Configuration::getInstance()->supportsShareableVAO())
        {
            glGenVertexArrays(1, (GLuint*)&_vao);
            GL::bindVAO(_vao);
        }
        
        CHECK_GL_ERROR_DEBUG();
        
        for (auto& it : _vertexAttributes)
        {
            const auto ba   = it.second;
            const auto attr = ba._attribute;
            const auto vb   = ba._buffer;
            
            // commit any outstanding client side geometry to the native buffers.
            // for interleaved data this will happen only the first time through.
            vb->commit(vb->getElements(), 0, vb->getElementCount());
            
            auto offset = attr._offset;
            auto stride = vb->getElementSize();
            
            glEnableVertexAttribArray(GLint(attr._index));
            glVertexAttribPointer(GLint(attr._index), (GLint)attr._size, _attributeDataTypeToGL(attr._type), attr._normalized, (GLsizei)stride, (GLvoid*)(size_t)offset);
            
            CHECK_GL_ERROR_DEBUG();
        }
        
        if (_indices != nullptr)
            _indices->commit(_indices->getElements(), 0, _indices->getElementCount());
        
        setDirty(false);
    }
    
    if (_indices != nullptr)
    {
        intptr_t offset = start * _indices->getElementSize();
        glDrawElements((GLenum)_drawingPrimitive, (GLsizei)count, _glIndexType(), (GLvoid*)offset);
    }
    else
    {
        glDrawArrays((GLenum)_drawingPrimitive, (GLsizei)start, (GLsizei)count);
    }
    
    CHECK_GL_ERROR_DEBUG();
    
    if (_vao)
        GL::bindVAO(0);
    GL::bindVBO(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return count;
}

//
// Protected Methods
//

inline
unsigned GraphicsOpenGLES2VertexArray::_attributeDataTypeToGL(AttributeDataType type)
{
    const static int gltypes[] = {GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, /*GL_FIXED*/};
    auto t = (unsigned)type;
    PAL_ASSERT(t < sizeof(gltypes) / sizeof(gltypes[0]), "Invalid GL AttributeDataType index");
    return gltypes[t];
}

inline
unsigned GraphicsOpenGLES2VertexArray::_glIndexType()
{
    CCASSERT(_indices, "no index buffer specified");
    return _indices->getElementSize() == sizeof(uint16_t) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
}

NS_PRIVATE_END
