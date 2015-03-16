
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

#ifndef _CC_GRAPHICS_METAL_H_
#define _CC_GRAPHICS_METAL_H_

#include "cocos2d.h"
#include "PAL/CCPALMacros.h"
#include "PAL/interfaces/CCGraphicsInterface.h"

NS_PRIVATE_BEGIN

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#    import "TargetConditionals.h"
#    if !TARGET_IPHONE_SIMULATOR
#        define CC_METAL_AVAILABLE
#    endif // TARGET_IPHONE_SIMULATOR
#endif // CC_PLATFORM_IOS

#ifdef CC_METAL_AVAILABLE

class GraphicsMetalViewController;

class GraphicsMetal
    : public GraphicsInterface
{
public:
    
    static GraphicsInterface* create();
    
    // @brief initialize the API
    bool init();

    // @brief shuts down this interface.
    void shutdown();
    
    // @brief creates a vertex array object.
    handle vertexArrayCreate(Primitive drawPrimitive);
    
    // @brief delete a vertex array object.
    bool vertexArrayDelete(handle object);
    
    // @brief specifies a vertex attribute.
    bool vertexArraySpecifyVertexAttribute(handle object, handle buffer, int index, ssize_t offset, AttributeDataType type, ssize_t count, bool normalized);
    
    // @brief removes a previously specified vertex attribute
    void vertexArrayRemoveVertexAttribute(handle vao, int index);

    // @brief specifies an index buffer to use with a vertex array.
    bool vertexArraySpecifyIndexBuffer(handle object, handle buffer);

    // @brief stage client side geometry that is to be copied to native buffers when drawn.
    // this is better than bufferCommitElements because it doesn't have to bind the buffer twice,
    // but the elements need to live unmodified until vertexArrayDrawElements is called.
    // works with vertex and index buffers. Overlapping ranges will be copied multiple times.
    void vertexArrayStageElements(handle vao, handle buffer, int index, void* elements, ssize_t start, ssize_t count);

    // @brief draws the vertex array.
    void vertexArrayDrawElements(handle object, ssize_t start, ssize_t count);
    
    handle bufferCreate(ssize_t size, ssize_t count, BufferMode bufferMode, BufferIntent bufferIntent, bool zero);
    bool bufferDestroy(handle object);
    bool bufferCommitElements(handle object, void*, ssize_t start, ssize_t count);
    // HACK for backwards compatibility with MeshCommand
    CC_DEPRECATED(v3) unsigned bufferGetBO(handle object);

protected:
    
    GraphicsMetalViewController* _controller;
};

#endif//CC_METAL_AVAILABLE

NS_PRIVATE_END

#endif//_CC_GRAPHICS_METAL_H_
