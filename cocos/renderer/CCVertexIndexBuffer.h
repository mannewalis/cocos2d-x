
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

#ifndef __CC_VERTEX_INDEX_BUFFER_H__
#define __CC_VERTEX_INDEX_BUFFER_H__

#include "base/ccMacros.h"
#include "base/CCRef.h"
#include "base/CCDirector.h"
#include "PAL/interfaces/CCGraphicsInterface.h"

NS_CC_BEGIN

using BufferIntent = NS_PRIVATE::BufferIntent;
using BufferMode   = NS_PRIVATE::BufferMode;
using BufferType   = NS_PRIVATE::BufferType;

class BufferBase
    : public Ref
{
public:

    #define CALL(proc, ...) Director::getInstance()->getGraphicsInterface()->proc(__VA_ARGS__)

    virtual ~BufferBase()
    {
        CALL(bufferDestroy, _bo);
    }
    
    CC_DEPRECATED(v3) unsigned getVBO() const
    {
        return CALL(bufferGetNativeBO, _bo);
    }
    
    ssize_t getElementSize() const
    {
        return CALL(bufferGetElementSize, _bo);
    }

    void setElementCount(ssize_t count)
    {
        CALL(bufferSetElementCount, _bo, count);
    }
    
    ssize_t getElementCount() const
    {
        return CALL(bufferGetElementCount, _bo);
    }
    
    void setElements(void* elements, ssize_t count, bool defer = true)
    {
        return CALL(bufferSetElements,_bo, elements, count, defer);
    }
    
    void updateElements(const void* elements, ssize_t start, ssize_t count = 0, bool defer = true)
    {
        CALL(bufferUpdateElements, _bo, elements, start, count, defer);
    }
    
    ssize_t getCapacity() const
    {
        return CALL(bufferGetCapacity, _bo);
    }
    
    void setDirty(bool dirty)
    {
        CALL(bufferSetDirty, _bo, dirty);
    }
    
    template <typename T>
    T* getElementsOfType()
    {
        return static_cast<T*>(CALL(bufferGetElements, _bo));
    }
    
    template <typename T>
    void setElementsOfType(const T* element, ssize_t count, bool defer = true)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferSetElements, _bo, element, mult * count, defer);
    }
    
    template <typename T>
    void appendElementsOfType(const T* element, ssize_t count = 1, bool defer = true)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferAppendElements, _bo, element, mult * count, defer);
    }
    
    template <typename T>
    void updateElementsOfType(const T* element, ssize_t start, ssize_t count, bool defer = true)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferUpdateElements, _bo, element, mult * start, mult * count, defer);
    }
    
    template <typename T>
    void insertElementsOfType(const T* element, ssize_t start, ssize_t count, bool defer = true)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferInsertElements, _bo, element, mult * start, mult * count, defer);
    }
    
    template <typename T>
    void removeElementsOfType(ssize_t start, ssize_t count, bool defer = true)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferRemoveElements, _bo, mult * start, mult * count, defer);
    }
    
    template <typename T>
    void addCapacityOfType(ssize_t count, bool zero = false)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferAddCapacity, _bo, mult * count, zero);
    }
    
    template <typename T>
    void swapElementsOfType(ssize_t source, ssize_t dest, ssize_t count)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferSwapElements, _bo, mult * source, mult * dest, mult * count);
    }
    
    template <typename T>
    void moveElementsOfType(ssize_t source, ssize_t dest, ssize_t count)
    {
        CCASSERT(0 == sizeof(T) % getElementSize(), "elements must divide evenly into elementSize");
        auto mult = sizeof(T) / getElementSize();
        CALL(bufferMoveElements, _bo, mult * source, mult * dest, mult * count);
    }
    
    template <typename T>
    void append(const T& element)
    {
        CCASSERT(sizeof(T) == getElementSize(), "can only append elements of the same size as the buffer");
        appendElementsOfType(&element);
    }
    
    NS_PRIVATE::handle handle() const
    {
        return _bo;
    }
    
    #undef CALL

protected:
    
    bool init(ssize_t size, ssize_t maxCount, BufferMode mode, BufferIntent intent, BufferType type, bool zero)
    {
        _bo = Director::getInstance()->getGraphicsInterface()->bufferCreate(size, maxCount, mode, intent, type, zero);
        return _bo != HANDLE_INVALID;
    }
    
protected:
    
    NS_PRIVATE::handle _bo;
};

class CC_DLL VertexBuffer
    : public BufferBase
{
public:
    
    template <class T = VertexBuffer>
    static T* create(ssize_t size, ssize_t count, BufferType type = BufferType::Default, BufferMode mode = NS_PRIVATE::BufferMode::LongLived, bool zero = false)
    {
        auto result = new (std::nothrow) T;
        if (result && result->init(size, count, mode, BufferIntent::VertexData, type, zero))
        {
            result->autorelease();
            return result;
        }
        CC_SAFE_DELETE(result);
        return nullptr;
    }
    
    CC_DEPRECATED(v3) int getSizePerVertex() const { return (int)getElementSize(); }
    CC_DEPRECATED(v3) int getVertexNumber() const { return (int)getElementCount(); }
    CC_DEPRECATED(v3) bool updateVertices(const void* vertices, int count, int begin) { updateElements(vertices, begin, count); return true; }
};

class CC_DLL IndexBuffer
    : public BufferBase
{
public:
    
    template <class T = IndexBuffer>
    static IndexBuffer* create(BufferIntent intent, ssize_t count, BufferType type = BufferType::Default, BufferMode mode = BufferMode::LongLived, bool zero = false)
    {
        PAL_ASSERT(intent == BufferIntent::IndexData16 || intent == BufferIntent::IndexData32);

        auto result = new (std::nothrow) T;
        if (result && result->init(intent == BufferIntent::IndexData16 ? 2 : 4, count, mode, intent, type, zero))
        {
            result->autorelease();
            return result;
        }
        CC_SAFE_DELETE(result);
        return nullptr;
    }
    
    CC_DEPRECATED(v3) int getSizePerIndex() const { return (int)getElementSize(); }
    CC_DEPRECATED(v3) int getIndexNumber() const { return (int)getElementCount(); }
    CC_DEPRECATED(v3) bool updateIndices(const void* indices, int count, int begin) { updateElements(indices, begin, count); return true; }

protected:

    NS_PRIVATE::handle _bo;
};

NS_CC_END

#endif /* __CC_VERTEX_INDEX_BUFFER_H__*/
