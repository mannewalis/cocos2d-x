
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

// remove cocos2d-x dependencies
#include "platform/CCGL.h"
#include "renderer/ccGLStateCache.h"
#include "base/ccMacros.h"
#include "base/CCConfiguration.h"

NS_PRIVATE_BEGIN

GraphicsOpenGLES2VertexArray::GraphicsOpenGLES2VertexArray(Primitive drawPrimitive)
    : super(drawPrimitive)
{
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, (GLuint*)&_vao);
        GL::bindVAO(_vao);
    }
}

GraphicsOpenGLES2VertexArray::~GraphicsOpenGLES2VertexArray()
{
    if (_vao && glIsBuffer(_vao))
    {
        glDeleteVertexArrays(1, (GLuint*)&_vao);
        GL::bindVAO(0);
        _vao = 0;
    }
}

void GraphicsOpenGLES2VertexArray::drawElements(ssize_t start, ssize_t count)
{
    PAL_ASSERT(start >= 0, "Invalid start value");
    PAL_ASSERT(count >= 0, "Invalid count value");
    
    // if we are drawing indexed, then use the count of indices to draw
    if (!count)
        count = _indices ? _indices->getElementCount() : this->getCount();
    
    if (_vao)
    {
        GL::bindVAO(_vao);
        CHECK_GL_ERROR_DEBUG();
    }
    
    if (isDirty())
    {
        CHECK_GL_ERROR_DEBUG();
        
        for (auto& element : _vertexAttributes)
        {
            auto& b = element.second;
            
            const auto& attributeBuffer = b._buffer;
            const auto& attribute = b._attribute;
            
            auto& regions = b._regions;
            for (auto r : regions)
                attributeBuffer->commitElements(r._elements, r._start, r._count);
            regions.clear();
            
            auto offset = attribute._offset;
            auto stride = attributeBuffer->getElementSize();
            
            glEnableVertexAttribArray(GLint(attribute._index));
            glVertexAttribPointer(GLint(attribute._index), (GLint)attribute._size, _attributeDataTypeToGL(attribute._type), attribute._normalized, (GLsizei)stride, (GLvoid*)(size_t)offset);
            
            CHECK_GL_ERROR_DEBUG();
        }
        
        if (_indices != nullptr)
            GL::bindVBO(GL_ELEMENT_ARRAY_BUFFER, (GLuint)_indices->getBO());
        
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
