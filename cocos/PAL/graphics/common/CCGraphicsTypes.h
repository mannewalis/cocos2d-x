
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

enum class Primitive
{
    Invalid = -1,
    Points,
    Lines,
    LineLoop,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan
};

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
    Invalid = -1,
    Immutable,
    LongLived,
    Dynamic
};

enum class ArrayIntent
{
    Invalid = -1,
    VertexData,
    IndexData16,
    IndexData32
};

struct VertexAttribute
{
    VertexAttribute()
        : _index(0)
        , _offset(0)
        , _type(DataType::Invalid)
        , _size(0)
        , _normalized(false)
    {}
    
    VertexAttribute(int index, ssize_t offset, DataType type, ssize_t size, bool normalized)
        : _index(index)
        , _offset(offset)
        , _type(type)
        , _size(size)
        , _normalized(normalized)
    {}
    
    int      _index;
    ssize_t  _offset;
    DataType _type;
    ssize_t  _size;
    bool     _normalized;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_TYPES_H_
