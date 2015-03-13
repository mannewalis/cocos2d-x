
/****************************************************************************
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
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

#ifndef _CC_GRAPHICS_OPEN_GL_ES_20_H_
#define _CC_GRAPHICS_OPEN_GL_ES_20_H_

#include "base/CCRef.h"
#include "PAL/CCPALMacros.h"
#include "PAL/CCPALTypes.h"
#include "PAL/interfaces/CCGraphicsInterface.h"
#include "PAL/CCPALHandles.h"

NS_CC_BEGIN
class GLView;
NS_CC_END

NS_PRIVATE_BEGIN

class GraphicsOpenGLES20
    : public GraphicsInterface
{
public:
    
    GraphicsOpenGLES20()
        : _handles(1000)
        , _view(nullptr)
    {}
    
    static GraphicsInterface* create();
    
    // @brief initialize the API
    bool init();

    // @brief shuts down this interface.
    void shutdown();
    
    // @brief creates a vertex array object.
    handle vertexArrayCreate();
    
    // @brief destroy a vertex array object.
    bool vertexArrayDestroy(handle object);
    
    // @brief specifies a vertex attribute.
    bool vertexArraySpecifyAttribute(handle object, handle buffer, ssize_t index, ssize_t offset, DataType type, ssize_t count, bool normalized);
    
    // @brief draws the vertex array.
    bool vertexArrayDrawElements(handle object, ssize_t start, ssize_t count);

    handle bufferCreate(ssize_t size, ssize_t count, ArrayMode arrayMode, bool zero);
    bool bufferDestroy(handle object);
    bool bufferCommitElements(handle object, const void* elements, ssize_t count, ssize_t begin);
    // HACK for backwards compatibility with MeshCommand
    CC_DEPRECATED(v3) unsigned bufferGetBO(handle object);

protected:
    
    Handles _handles;
    NS_CC::GLView* _view;
};

NS_PRIVATE_END

#endif // _CC_GRAPHICS_OPEN_GL_ES_20_H_
