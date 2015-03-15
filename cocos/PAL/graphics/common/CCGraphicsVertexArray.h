
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

#include <vector>
#include <map>
#include "PAL/CCPALMacros.h"
#include "CCGraphicsTypes.h"
#include "CCGraphicsAttributeBuffer.h"

// remove cocos2d-x dependencies
#include "base/CCRef.h"

NS_PRIVATE_BEGIN

// @brief GraphicsVertexArray
// Template class that encapsulates a set of one for more attribute buffers.
// APITraits provides the graphics API specific implementations.
// BufferType specifies the graphics API specific implementation of an GraphicsAttributeBuffer.
template <class APITraits, class BufferType>
class GraphicsVertexArray
    : public Ref
{
public:
    
    typedef APITraits traits_type;
    typedef BufferType buffer_type;
    
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
        return _vertexAttributes.size();
    }
    
    // @brief add a vertex attribute
    bool specifyVertexAttribute(buffer_type* buffer, const VertexAttribute& attribute)
    {
        PAL_ASSERT(buffer, "invalid vertex attribute buffer");
        
        setDirty(true);
        
        auto iter = _vertexAttributes.find(attribute._index);
        auto& attributeEntry = (iter == _vertexAttributes.end()) ? _vertexAttributes[attribute._index] : iter->second;

        CC_SAFE_RELEASE(attributeEntry._buffer);

        buffer->retain();
        attributeEntry._buffer = buffer;
        attributeEntry._attribute = attribute;
        
        _buffers.insert(buffer);
        
        return true;
    }
    
    // @brief remove a vertex attribute
    void removeVertexAttribute(int index)
    {
        auto iter = _vertexAttributes.find(index);
        if (iter != _vertexAttributes.end())
        {
            auto buffer = iter->second._buffer;
            auto bi = _buffers.find(buffer);
            if (bi != _buffers.end())
                _buffers.erase(bi);
            
            iter->second._buffer->release();
            _vertexAttributes.erase(iter);
        }
        
        setDirty(true);
    }
    
    // @brief specify indexed drawing for vertex data
    void specifyIndexBuffer(buffer_type* indices)
    {
        CC_SAFE_RELEASE(_indices);
        _indices = indices;
        CC_SAFE_RETAIN(_indices);
    }
    
    // @brief stage elements for copying on draw.
    void stageElements(buffer_type* buffer, void* elements, ssize_t start, ssize_t count)
    {
        setDirty(true);
        _stagedElements.emplace_back(StagedElements{buffer, elements, start, count});
    }
    
    // @brief copy stage elements and draw
    void drawElements(ssize_t start, ssize_t count)
    {
        if (!_stagedElements.empty())
        {
            for (const auto& e : _stagedElements)
            {
                traits_cast<buffer_type>(e._buffer)->commitElements(e._elements, e._start, e._count);
            }
        }
        traits_cast<traits_type>(this)->_drawElements(start, count);
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
    void setDirty(bool dirty, bool cascade = false)
    {
        _dirty = dirty;
        if (cascade)
        {
            for (auto b : _buffers)
                b->setDirty(dirty);
        }
    }
    
protected:
    
    struct Attributes
    {
        buffer_type* _buffer;
        VertexAttribute _attribute;
    };
    std::map<int, Attributes> _vertexAttributes;
    
    struct StagedElements
    {
        buffer_type* _buffer;
        void* _elements;
        ssize_t _start;
        ssize_t _count;
    };
    std::vector<StagedElements> _stagedElements;
    
    typedef std::set<buffer_type*> tBufferSet;
    tBufferSet _buffers;
    
    buffer_type* _indices;
    
    bool _dirty;
    unsigned _vao;
    Primitive _drawingPrimitive;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_VERTEX_ARRAY_H_
