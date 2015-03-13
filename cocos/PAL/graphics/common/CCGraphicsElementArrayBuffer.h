
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

#ifndef _CC_GRAPHICS_VERTEX_ARRAY_BUFFER_H_
#define _CC_GRAPHICS_VERTEX_ARRAY_BUFFER_H_

#include "PAL/CCPALMacros.h"
#include "PAL/CCPALTypes.h"
#include "base/CCRef.h"

// remove cocos2d-x dependencies
#include "base/ccMacros.h"

NS_PRIVATE_BEGIN

template <class T>
class GraphicsElementArrayBuffer
    : public Ref
{
public:
    
    typedef T traits;

    GraphicsElementArrayBuffer()
        : _bo(0)
        , _elementCount(0)
        , _elementSize(0)
        , _capacity(0)
        , _arrayMode(ArrayMode::Invalid)
        , _usage(0)
        , _dirty(true)
    {}
    
    virtual ~GraphicsElementArrayBuffer()
    {}
    
    // @brief initialize the element array buffer.
    bool init(ssize_t elementSize, ssize_t maxElements, ArrayMode arrayMode, ArrayIntent arrayIntent, bool zero);
    
    // @brief updates a region of the client and native buffer
    bool commitElements(const void* elements, ssize_t count, ssize_t begin);
    
    // @brief increases the capacity of the buffer by count elements
    //        optionally zeroes out the elements.
    void addCapacity(ssize_t count, bool zero = false);
        
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
        _dirty = true;
    }
    
    void recreate()
    {
         traits_cast<traits>()->recreate();
    }
    
    unsigned getBO() const
    {
        return _bo;
    }
    
protected:
    
    // @brief sets the capacity for the buffer.
    void setCapacity(ssize_t capacity, bool zero)
    {
        _capacity = capacity;
    }

protected:
    
    // native only
    uint32_t _bo;
    
    // client buffer only
    ssize_t _elementCount;
    void* _elements;
    
    ssize_t _elementSize;
    ssize_t _capacity;
    
    ArrayMode _arrayMode;
    ArrayIntent _arrayIntent;
    
    unsigned _usage;
    bool _dirty;
};

template <class T>
bool GraphicsElementArrayBuffer<T>::init(ssize_t elementSize, ssize_t maxElements, ArrayMode arrayMode, ArrayIntent arrayIntent, bool zero)
{
    CCASSERT(elementSize > 0 && elementSize < 512, "0 < maxElements size < 512");
    
    _arrayMode = arrayMode;
    _arrayIntent = arrayIntent;
    
    _elementSize  = elementSize;
    _elementCount = 0;
    
    setCapacity(maxElements, zero);
    
    traits_cast<T>(this)->setupBO();

    return true;
}

template <class T>
bool GraphicsElementArrayBuffer<T>::commitElements(const void* elements, ssize_t count, ssize_t begin)
{
    CCASSERT(begin >= 0, "Invalid being value");
    CCASSERT(count >= 0, "Invalid count value");
    CCASSERT(elements != nullptr, "Invalid elements value");
    
    if (0 == count)
        return false;
    
    auto needed = count + begin;
    setCapacity(needed, false);
    
    // empty elements do not count towards element count, only capacity
    _elementCount = begin + count > _elementCount ? begin + count : _elementCount;
    
    return traits_cast<T>(this)->commitElements(elements, count, begin);
}

template <class T>
void GraphicsElementArrayBuffer<T>::addCapacity(ssize_t count, bool zero)
{
    setCapacity(count + getCapacity(), zero);
}

NS_PRIVATE_END

#endif//_CC_GRAPHICS_VERTEX_ARRAY_BUFFER_H_
