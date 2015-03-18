
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

#ifndef _CC_GRAPHICS_ATTRIBUTE_BUFFER_H_
#define _CC_GRAPHICS_ATTRIBUTE_BUFFER_H_

#include "PAL/CCPALMacros.h"
#include "PAL/CCPALTypes.h"

// remove cocos2d-x dependencies
#include "base/ccMacros.h"
#include "base/CCRef.h"
#include "base/allocator/CCAllocatorMacros.h"

NS_PRIVATE_BEGIN

template <class APITraits>
class GraphicsAttributeBuffer
    : public Ref
{
public:
    
    GraphicsAttributeBuffer()
        : _buffer(nullptr)
        , _elementCount(0)
        , _elementSize(0)
        , _capacity(0)
        , _bufferType(BufferType::None)
        , _bufferMode(BufferMode::Invalid)
        , _bufferIntent(BufferIntent::Invalid)
        , _dirty(true)
    {}
    
    virtual ~GraphicsAttributeBuffer()
    {}
    
    bool init(ssize_t elementSize, ssize_t maxElements, BufferMode mode, BufferIntent intent, BufferType type, bool zero)
    {
        CCASSERT(elementSize > 0 && elementSize < 512, "0 < maxElements size < 512");
        
        _bufferIntent = intent;
        _bufferMode   = mode;
        _bufferType   = type;
        _elementSize  = elementSize;
        _elementCount = 0;
        
        setCapacity(maxElements, zero);
        
        return true;
    }
    
    void* getElements() const
    {
        return _buffer;
    }
    
    void setElements(const void* elements, ssize_t count, bool defer)
    {
        setElementCount(count);
        updateElements(elements, 0, count, defer);
    }
    
    void updateElements(const void* elements, ssize_t start, ssize_t count, bool defer)
    {
        CCASSERT(start >= 0, "Invalid start value");
        CCASSERT(count >= 0, "Invalid count value");
        
        CCASSERT(hasClient() || hasNative(), "Can only update elements if there is an attached buffer");
        
        if (0 == count)
            return;
        
        setDirty(true);
        
        // if we have no client buffer, then commit to native immediately.
        if (false == hasClient())
            defer = false;
        
        auto needed = start + count;
        setCapacity(needed, false);
        
        if (hasClient())
        {
            intptr_t p = (intptr_t)_buffer + start * _elementSize;
            if (elements)
                memmove((void*)p, elements, count * _elementSize);
            else
                memset((void*)p, 0, count * _elementSize);
        }
        
        // empty elements do not count towards element count, only capacity
        if (elements)
            _elementCount = start + count > _elementCount ? start + count : _elementCount;
        
        if (false == defer && elements && hasNative())
            static_cast<APITraits*>(this)->bindAndCommit(elements, start, count);
    }
    
    void insertElements(const void* elements, ssize_t start, ssize_t count, bool defer)
    {
        CCASSERT(hasClient(), "can only insert into a client buffer");
        // first see if we need to move any elements
        if (start < getElementCount())
        {
            intptr_t dst = (intptr_t)_buffer + start + count;
            updateElements((void*)dst, start, count, defer);
        }
        updateElements(elements, start, count, defer);
    }
    
    void appendElements(const void* elements, ssize_t count, bool defer)
    {
        CCASSERT(hasClient(), "can only append into a client buffer");
        updateElements(elements, _elementCount, count, defer);
    }
    
    void removeElements(ssize_t start, ssize_t count, bool defer)
    {
        const auto ec = getElementCount();
        CCASSERT(hasClient(),         "can only remove from a client buffer");
        CCASSERT(start < ec,          "removeElements begin must be within range");
        CCASSERT(start + count <= ec, "removeElements count must be within range");
        _elementCount -= count;
        const intptr_t src = (intptr_t)_buffer + (start + count) * getElementSize();
        updateElements((void*)src, start, _elementCount - start, defer);
    }
    
    void addCapacity(ssize_t count, bool zero)
    {
        setCapacity(count + getCapacity(), zero);
    }
    
    void swapElements(ssize_t source, ssize_t dest, ssize_t count)
    {
        const auto ec = getElementCount();
        CCASSERT(hasClient(),          "can only swap elements in a client buffer");
        CCASSERT(source < ec,          "swapElements source must be within range");
        CCASSERT(source + count <= ec, "swapElements source + count must be within range");
        CCASSERT(dest < ec,            "swapElements dest must be within range");
        CCASSERT(dest + count <= ec,   "swapElements dest + count must be within range");
        const ssize_t s1 = source;
        const ssize_t s2 = s1 + count;
        const ssize_t d1 = dest;
        const ssize_t d2 = d1 + count;
        const ssize_t overlap = s1 < d1 ? s2 - d1 : d2 - s1;
        const ssize_t count2 = overlap > 0 ? count - overlap : count;
        const auto es = getElementSize();
        const auto tmpptr = CC_ALLOCA(count2 * es);
        const auto srcptr = (void*)((intptr_t)_buffer + s1 * es);
        const auto dstptr = (void*)((intptr_t)_buffer + d1 * es);
        memcpy (tmpptr, dstptr, count2 * es);
        memmove(dstptr, srcptr, count );
        memmove(srcptr, tmpptr, count2);
    }
    
    void moveElements(ssize_t source, ssize_t dest, ssize_t count)
    {
        const auto es = getElementSize();
        const auto srcptr = (void*)((intptr_t)_buffer + source * es);
        updateElements(srcptr, dest, count, false);
    }
        
    void clear()
    {
        _elementCount = 0;
        _dirty = true;
    }
        
    void setCapacity(ssize_t capacity, bool zero)
    {
        if (capacity > _capacity)
        {
            if (hasClient())
            {
                auto capacityInBytes = capacity * _elementSize;
                _buffer = realloc(_buffer, capacityInBytes);
                
                if (zero)
                {
                    intptr_t start = (intptr_t)_buffer + _capacity * _elementSize;
                    size_t count = _elementSize * (capacity - _capacity);
                    memset((void*)start, 0, count);
                }
                _dirty = true;
            }
            _capacity = capacity;
        }
    }
    
    ssize_t getSize() const
    {
        return getElementCount() * getElementSize();
    }
    
    ssize_t getCapacityInBytes() const
    {
        return getCapacity() * getElementSize();
    }

    void setElementCount(ssize_t count)
    {
        CCASSERT(count <= _capacity, "element count cannot exceed capacity");
        if (count != _elementCount)
        {
            _elementCount = count;
            setDirty(true);
        }
    }
    
    ssize_t getElementCount() const
    {
        return _elementCount;
    }
    
    ssize_t getElementSize() const
    {
        return _elementSize;
    }
    
    ssize_t getCapacity() const
    {
        return _capacity;
    }
    
    bool hasClient() const
    {
        return _bufferType & BufferType::Client ? true : false;
    }
    
    bool hasNative() const
    {
        return _bufferType & BufferType::Native ? true : false;
    }
    
    bool isDirty() const
    {
        return _dirty;
    }
    
    void setDirty(bool dirty)
    {
        _dirty = dirty;
    }

protected:
    
    void* _buffer;
    
    ssize_t _elementCount;
    ssize_t _elementSize;
    ssize_t _capacity;
    
    BufferType _bufferType;
    BufferMode _bufferMode;
    BufferIntent _bufferIntent;
    
    bool _dirty;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_ATTRIBUTE_BUFFER_H_
