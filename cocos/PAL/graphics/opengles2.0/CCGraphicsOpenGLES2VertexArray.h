
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

#ifndef _CC_GRAPHICS_OPENGLES2_VERTEX_ARRAY_
#define _CC_GRAPHICS_OPENGLES2_VERTEX_ARRAY_

#include <set>

#include "PAL/CCPALMacros.h"
#include "PAL/CCPALTypes.h"
#include "PAL/graphics/common/CCGraphicsVertexArray.h"

// remove cocos2d-x dependencies
#include "base/CCRef.h"

NS_PRIVATE_BEGIN

class GraphicsOpenGLES2ArrayBuffer;
class GraphicsOpenGLES2IndexBuffer;

class GraphicsOpenGLES2VertexArray
    : public GraphicsVertexArray<GraphicsOpenGLES2VertexArray>
{
public:
    
    GraphicsOpenGLES2VertexArray();
    virtual ~GraphicsOpenGLES2VertexArray();
    
    bool destroy();
    bool specifyAttribute(ssize_t index, ssize_t offset, DataType type, ssize_t count, bool normalized);
    void drawElements(ssize_t start, ssize_t count);
    
protected:
    
    unsigned DataTypeToGL(DataType type);
    unsigned GLIndexType();

protected:
    
    unsigned _vao;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_OPENGLES2_VERTEX_ARRAY_
