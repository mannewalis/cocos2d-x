
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

#ifndef _CC_GRAPHICS_TYPES_H_
#define _CC_GRAPHICS_TYPES_H_

#include "PAL/CCPALMacros.h"

NS_PRIVATE_BEGIN

enum class DataType
{
    Invalid = -1,
    Byte,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    Float,
    Fixed
};

enum class ArrayMode
{
    Invalid,
    Immutable, // GL_STATIC_DRAW
    LongLived, // GL_DYNAMIC_DRAW
    Dynamic    // GL_STREAMED_DRAW
};

struct VertexAttribute
{
    VertexAttribute(int index, int offset, DataType type, int size, bool normalized)
    : _index(index)
    , _offset(offset)
    , _type(type)
    , _size(size)
    , _normalized(normalized)
    {}
    
    int  _index;
    int  _offset;
    DataType _type;
    int  _size;
    bool _normalized;
};

enum class IndexType
{
    INDEX_TYPE_NONE = -1,
    INDEX_TYPE_SHORT_16,
    INDEX_TYPE_UINT_32
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_TYPES_H_
