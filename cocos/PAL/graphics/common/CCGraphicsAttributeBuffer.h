
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
#include "base/CCRef.h"

// remove cocos2d-x dependencies
#include "base/ccMacros.h"

NS_PRIVATE_BEGIN

template <class APITraits>
class GraphicsAttributeBuffer
    : public Ref
{
public:
    
    typedef APITraits traits_type;

    GraphicsAttributeBuffer()
        : _elementCount(0)
        , _elementSize(0)
        , _capacity(0)
        , _bufferMode(BufferMode::Invalid)
        , _usage(0)
        , _dirty(true)
    {}
    
    virtual ~GraphicsAttributeBuffer()
    {}
    
    // @brief initialize the element array buffer.
    bool init(ssize_t elementSize, ssize_t maxElements, BufferMode bufferMode, BufferIntent bufferIntent, bool zero)
    {
        CCASSERT(elementSize > 0 && elementSize < 512, "0 < maxElements size < 512");
        
        _bufferMode = bufferMode;
        _bufferIntent = bufferIntent;
        
        _elementSize  = elementSize;
        _elementCount = 0;
        
        _setCapacity(maxElements, zero);
        
        return true;
    }
    
    // @brief increases the capacity of the buffer by count elements
    //        optionally zeroes out the elements.
    void addCapacity(ssize_t count, bool zero = false)
    {
        setCapacity(count + getCapacity(), zero);
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
        PAL_ASSERT(count <= _capacity, "element count cannot exceed capacity");
        if (count != _elementCount)
        {
            _elementCount = count;
            setDirty(true);
        }
    }
    
    void* getElements()
    {
        if (!_buffer || getSize() != _bufferSize)
        {
            _bufferSize = getSize();
            _buffer = realloc(_buffer, _bufferSize);
        }
        return _buffer;
    }
    
    ssize_t getElementCount() const
    {
        return _elementCount;
    }
    
    ssize_t getElementSize() const
    {
        return _elementSize;
    }
    
    // @brief sets the capacity for the buffer.
    void setCapacity(ssize_t capacity, bool zero)
    {
        if (capacity > _capacity)
        {
            if (_buffer)
            {
                auto capacityInBytes = capacity * _elementSize;
                _buffer = realloc(_buffer, capacityInBytes);
                
                if (zero)
                {
                    intptr_t start = (intptr_t)_buffer + _capacity * _elementSize;
                    size_t count = _elementSize * (capacity - _capacity);
                    memset((void*)start, 0, count);
                }
                setDirty(true);
            }
            _capacity = capacity;
        }
    }

    ssize_t getCapacity() const
    {
        return _capacity;
    }
    
    bool isDirty() const
    {
        return _dirty;
    }
    
    void setDirty(bool dirty)
    {
        _dirty = dirty;
    }
    
    void clear()
    {
        _elementCount = 0;
        setDirty(true);
    }

protected:
    
    void* _buffer;
    ssize_t _bufferSize;
    
    ssize_t _elementCount;
    ssize_t _elementSize;
    ssize_t _capacity;
    
    BufferMode _bufferMode;
    BufferIntent _bufferIntent;
    
    unsigned _usage;
    bool _dirty;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_ATTRIBUTE_BUFFER_H_
