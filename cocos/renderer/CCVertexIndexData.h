
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

#ifndef __CC_VERTEX_INDEX_DATA_H__
#define __CC_VERTEX_INDEX_DATA_H__

#include "base/CCRef.h"
#include "base/CCDirector.h"
#include "renderer/CCVertexIndexBuffer.h"
#include "PAL/interfaces/CCGraphicsInterface.h"

NS_CC_BEGIN

using Primitive       = NS_PRIVATE::Primitive;
using DataType        = NS_PRIVATE::AttributeDataType;
using VertexAttribute = NS_PRIVATE::VertexAttribute;

class CC_DLL VertexData
    : public Ref
{
public:
    
    #define CALL(proc, ...) Director::getInstance()->getGraphicsInterface()->proc(__VA_ARGS__)
    
    template <typename T = VertexData>
    static T* create(Primitive primitive = Primitive::Triangles)
    {
        auto result = new (std::nothrow) T(primitive);
        if (result)
        {
            result->autorelease();
            return result;
        }
        return nullptr;
    }

    virtual ~VertexData()
    {
        CALL(vertexArrayDestroy, _vao);
    }
    
    CC_DEPRECATED(v3) bool setStream(VertexBuffer* buffer, const VertexAttribute& attribute) { return specifyVertexAttribute(buffer, attribute); }
    CC_DEPRECATED(v3) bool addStream(VertexBuffer* buffer, const VertexAttribute& attribute) { return specifyVertexAttribute(buffer, attribute); }

    bool specifyVertexAttribute(VertexBuffer* buffer, const VertexAttribute& attribute)
    {
        return CALL(vertexArraySpecifyVertexAttribute, _vao, buffer->handle(), attribute);
    }
    
    // @brief specify indexed drawing for vertex data
    void setIndexBuffer(IndexBuffer* indices)
    {
        CALL(vertexArraySpecifyIndexBuffer, _vao, indices->handle());
    }
    
    void removeIndexBuffer()
    {
        CALL(vertexArraySpecifyIndexBuffer, _vao, HANDLE_INVALID);
    }
    
    // @brief update and draw the buffer.
    ssize_t draw(ssize_t start = 0, ssize_t count = 0)
    {
        return CALL(vertexArrayDrawElements, _vao, start, count);
    }

    // @brief true/false if all vertex buffers are empty
    bool empty() const
    {
        return CALL(vertexArrayIsEmpty, _vao);
    }
    
    // @brief clears the vertex buffers associated with this vertex data
    void clear()
    {
        CALL(vertexArrayClear, _vao);
    }

    // @brief returns the dirty status of the data or vertex streams
    bool isDirty() const
    {
        return CALL(vertexArrayIsDirty, _vao);
    }
    
    // @brief sets the dirty state of all vertex data
    void setDirty(bool dirty)
    {
        CALL(vertexArraySetDirty, _vao, dirty);
    }
    
protected:

    VertexData(Primitive primitive)
        : _vao(CALL(vertexArrayCreate, primitive))
    {}

    #undef CALL
    
protected:
    
    NS_PRIVATE::handle _vao;
};

NS_CC_END

#endif //__CC_VERTEX_INDEX_DATA_H__
