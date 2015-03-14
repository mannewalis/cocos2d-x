
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

#ifndef _CC_GRAPHICS_VERTEX_ARRAY_H_
#define _CC_GRAPHICS_VERTEX_ARRAY_H_

#include "PAL/CCPALMacros.h"
#include "CCGraphicsTypes.h"
#include "CCGraphicsElementArrayBuffer.h"

// remove cocos2d-x dependencies
#include "base/CCRef.h"

NS_PRIVATE_BEGIN

template <class T, class B>
class GraphicsVertexArray
    : public Ref
{
public:
    
    typedef T traits;
    typedef GraphicsElementArrayBuffer<B> vertex_buffer_type;
    typedef GraphicsElementArrayBuffer<B> index_buffer_type;
    
    GraphicsVertexArray(Primitive drawPrimitive)
        : _indices(nullptr)
        , _dirty(true)
        , _vao(0)
        , _drawingPrimitive(drawPrimitive)
    {}
    
    virtual ~GraphicsVertexArray()
    {}
    
    // @brief Return the number of vertex streams
    ssize_t getVertexStreamCount() const
    {
        return _vertexStreams.size();
    }
    
    // @brief add a vertex attribute stream
    bool addStream(vertex_buffer_type* buffer, const VertexAttribute& stream)
    {
        PAL_ASSERT(buffer, "invalid buffer");
        
        setDirty(true);
        
        auto iter = _vertexStreams.find(stream._index);
        if (iter == _vertexStreams.end())
        {
            buffer->retain();
            auto& bufferAttribute = _vertexStreams[stream._index];
            bufferAttribute._buffer = buffer;
            bufferAttribute._stream = stream;
        }
        else
        {
            buffer->retain();
            iter->second._buffer->release();
            iter->second._stream = stream;
            iter->second._buffer = buffer;
        }
        
        _buffers.insert(buffer);
        
        return true;
    }
    
    void removeStream(int index)
    {
        auto iter = _vertexStreams.find(index);
        if (iter != _vertexStreams.end())
        {
            auto buffer = iter->second._buffer;
            auto bi = _buffers.find(buffer);
            if (bi != _buffers.end())
                _buffers.erase(bi);
            
            iter->second._buffer->release();
            _vertexStreams.erase(iter);
        }
        
        setDirty(true);
    }
    
    // @brief specify indexed drawing for vertex data
    void setIndexBuffer(index_buffer_type* indices)
    {
        CC_SAFE_RELEASE(_indices);
        _indices = indices;
        CC_SAFE_RETAIN(_indices);
    }
    
    void removeIndexBuffer()
    {
        CC_SAFE_RELEASE(_indices);
        _indices = nullptr;
    }
    
    const VertexAttribute* getStreamAttribute(int semantic) const
    {
        auto iter = _vertexStreams.find(semantic);
        if(iter == _vertexStreams.end()) return nullptr;
        else return &iter->second._stream;
    }
    
    VertexAttribute* getStreamAttribute(int semantic)
    {
        auto iter = _vertexStreams.find(semantic);
        if(iter == _vertexStreams.end()) return nullptr;
        else return &iter->second._stream;
    }
    
    // @brief update and draw the buffer.
    ssize_t draw(ssize_t start = 0, ssize_t count = 0)
    {
    }
    
    // @brief true/false if all vertex buffers are empty
    bool empty() const
    {
        for (auto b : _buffers)
        {
            if (0 != b->getElementCount())
                return false;
        }
        return true;
    }
    
    // @brief clears the vertex buffers associated with this vertex data
    void clear()
    {
        _dirty = true;
        for (auto& b : _buffers)
            b->clear();
    }
    
    // @brief returns the count of vertices added
    ssize_t getCount() const
    {
        ssize_t count = 0;
        for (auto b : _buffers)
        {
            auto c = b->getElementCount();
            CCASSERT(0 == count || c == count, "all buffers must have the same vertex count");
            count = c;
        }
        return count;
    }
    
    // @brief returns the capacity of the buffer in bytes
    ssize_t getCapacity() const
    {
        ssize_t capacity = 0;
        for (auto b : _buffers)
        {
            auto c = b->getCapacity();
            CCASSERT(0 == capacity || c == capacity, "all buffers must have the same capacity");
            capacity = c;
        }
        return capacity;
    }
    
    // @brief returns the dirty status of the data or vertex streams
    bool isDirty() const
    {
        if (_dirty)
            return true;
        for (auto b : _buffers)
            if (b->isDirty())
                return true;
        return false;
    }
    
    // @brief sets the dirty state of all vertex data
    void setDirty(bool dirty)
    {
        _dirty = dirty;
        for (auto b : _buffers)
            b->setDirty(dirty);
    }
    
protected:
    
    struct BufferAttribute
    {
        vertex_buffer_type* _buffer;
        VertexAttribute _stream;
    };
    std::map<int, BufferAttribute> _vertexStreams;
    
    typedef std::set<vertex_buffer_type*> tBuffers;
    tBuffers _buffers;
    
    index_buffer_type* _indices;
    
    bool _dirty;
    unsigned _vao;
    Primitive _drawingPrimitive;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_VERTEX_ARRAY_H_
