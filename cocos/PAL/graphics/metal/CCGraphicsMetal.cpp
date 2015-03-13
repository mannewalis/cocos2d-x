
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

#include "CCGraphicsMetal.h"
#include "PAL/CCPALHandles.h"

#ifdef CC_METAL_AVAILABLE

#import "Metal/Metal.h"

NS_PRIVATE_BEGIN

GraphicsInterface* GraphicsMetal::create()
{
    auto obj = new GraphicsMetal;
    if (obj)
    {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

bool GraphicsMetal::init()
{
    return true;
}

void GraphicsMetal::shutdown()
{
}

// @brief creates a vertex array object.
handle GraphicsMetal::vertexArrayCreate()
{
    return HANDLE_INVALID;
}

// @brief delete a vertex array object.
bool GraphicsMetal::vertexArrayDelete(handle object)
{
    return false;
}

// @brief specifies a vertex attribute.
bool GraphicsMetal::vertexArraySpecifyAttribute(handle object, int index, ssize_t offset, DataType type, ssize_t count, bool normalized)
{
    return false;
}

// @brief draws the vertex array.
bool GraphicsMetal::vertexArrayDrawElements(handle object, ssize_t start, ssize_t count)
{
    return false;
}

NS_PRIVATE_END

#endif//CC_METAL_AVAILABLE
