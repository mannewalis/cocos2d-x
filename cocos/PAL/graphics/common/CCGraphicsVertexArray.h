
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

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#include "base/CCEventType.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "base/CCDirector.h"
#define SUPPORT_EVENT_RENDERER_RECREATED
#endif

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
    
    typedef BufferType buffer_type;
    
    GraphicsVertexArray(Primitive drawPrimitive)
        : _indices(nullptr)
        , _interleaved(false)
        , _dirty(true)
        , _drawingPrimitive(drawPrimitive)
    {
#ifdef SUPPORT_EVENT_RENDERER_RECREATED
        _recreateEventListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_RENDERER_RECREATED, [this](EventCustom* event){this->recreate();});
#endif
    }
    
    ~GraphicsVertexArray()
    {
        for (auto& element : _vertexAttributes)
            element.second._buffer->release();
        _vertexAttributes.clear();
        CC_SAFE_RELEASE(_indices);
#ifdef SUPPORT_EVENT_RENDERER_RECREATED
        Director::getInstance()->getEventDispatcher()->removeEventListener(_recreateEventListener);
#endif
    }
    
    bool specifyVertexAttribute(buffer_type* buffer, const VertexAttribute& attribute)
    {
        PAL_ASSERT(buffer, "invalid buffer");
        
        setDirty(true);
        
        auto iter = _vertexAttributes.find(attribute._index);
        auto& attributeEntry = (iter == _vertexAttributes.end()) ? _vertexAttributes[attribute._index] : iter->second;

        CC_SAFE_RELEASE(attributeEntry._buffer);

        buffer->retain();
        attributeEntry._buffer = buffer;
        attributeEntry._attribute = attribute;
        
        _buffers.insert(buffer);
        
        // flag whether or not this vertex data is interleaved or not.
        _interleaved = determineInterleave();
        
        return true;
    }
    
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
        
        _interleaved = determineInterleave();
        
        setDirty(true);
    }
    
    // @brief specify indexed drawing for vertex data with optional precision
    void specifyIndexBuffer(buffer_type* indices)
    {
        if (indices != _indices)
            setDirty(true);
        CC_SAFE_RELEASE(_indices);
        _indices = indices;
        CC_SAFE_RETAIN(_indices);
    }
    
    void removeIndexBuffer()
    {
        CC_SAFE_RELEASE(_indices);
        _indices = nullptr;
    }
        
    bool empty() const
    {
        for (auto b : _buffers)
        {
            if (0 != b->getElementCount())
                return false;
        }
        return true;
    }
    
    void clear()
    {
        _dirty = true;
        for (auto& b : _buffers)
            b->clear();
    }
    
    bool isDirty() const
    {
        if (_dirty || (_indices && _indices->isDirty()))
            return true;
        for (auto b : _buffers)
            if (b->isDirty())
                return true;
        return false;
    }
    
    void setDirty(bool dirty)
    {
        _dirty = dirty;
        for (auto b : _buffers)
            b->setDirty(dirty);
    }
    
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
    
    void append(buffer_type* buffer, void* source, ssize_t count)
    {
        _dirty = true;
        buffer->appendElements(source, count);
    }
    
protected:
    
    // @brief If all streams use the same buffer, then the data is interleaved.
    bool determineInterleave() const
    {
        return _buffers.size() == 1;
    }

protected:
    
    struct Attributes
    {
        buffer_type* _buffer;
        VertexAttribute _attribute;
    };
    std::map<int, Attributes> _vertexAttributes;
    
    typedef std::set<buffer_type*> tBufferSet;
    tBufferSet _buffers;
    
    buffer_type* _indices;
    
    bool _interleaved;
    bool _dirty;
    Primitive _drawingPrimitive;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_VERTEX_ARRAY_H_
