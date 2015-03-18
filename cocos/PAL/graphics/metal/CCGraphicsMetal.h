
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
#include "PAL/CCPALHandles.h"
#include "PAL/interfaces/CCGraphicsInterface.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#    import "TargetConditionals.h"
#    if !TARGET_IPHONE_SIMULATOR
#        define CC_METAL_AVAILABLE
#    endif // TARGET_IPHONE_SIMULATOR
#endif // CC_PLATFORM_IOS

#ifdef CC_METAL_AVAILABLE

#import "Metal/Metal.h"

@class MTLDevice;
@class MTLCommandQueue;

NS_PRIVATE_BEGIN

class GraphicsMetal
    : public GraphicsInterface
{
public:
    
    GraphicsMetal()
        : _handles(1000)
    {}
    
    static GraphicsInterface* create();
    
    // @brief initialize the API
    bool init();
    
    // @brief shuts down this interface.
    void shutdown();
    
    //////////////////////////////////////////////////////////////////
    // MARK: view and window                                        //
    //////////////////////////////////////////////////////////////////
    
    // @brief create window with a view and make current.
    // optional size, if null then full screen window.
    handle windowCreate(Rect* size);
    
    
    //////////////////////////////////////////////////////////////////
    // MARK: vertex array                                           //
    //////////////////////////////////////////////////////////////////
    
    // @brief creates a vertex array object.
    handle vertexArrayCreate(Primitive drawPrimitive);
    
    // @brief destroy a vertex array object.
    void vertexArrayDestroy(handle object);
    
    // @brief specifies a vertex attribute.
    bool vertexArraySpecifyVertexAttribute(handle object, handle buffer, const VertexAttribute& attribute);
    
    // @brief removes a previously specified vertex attribute
    void vertexArrayRemoveVertexAttribute(handle object, int index);
    
    // @brief specifies an index buffer to use with a vertex array.
    bool vertexArraySpecifyIndexBuffer(handle object, handle buffer);
    
    // @brief draws the vertex array.
    ssize_t vertexArrayDrawElements(handle object, ssize_t start, ssize_t count);
    
    bool vertexArrayIsEmpty(handle object) const;
    void vertexArrayClear(handle object);
    bool vertexArrayIsDirty(handle object) const;
    void vertexArraySetDirty(handle object, bool dirty);
    
    
    //////////////////////////////////////////////////////////////////
    // MARK: Vertex and Index buffers                               //
    //////////////////////////////////////////////////////////////////
    
    handle bufferCreate(ssize_t size, ssize_t count, BufferMode mode, BufferIntent intent, BufferType type, bool zero);
    
    bool bufferDestroy(handle object);
    
    // @brief returns the buffer element size.
    ssize_t bufferGetElementSize(handle object);
    
    // @brief returns the buffer element count.
    ssize_t bufferGetElementCount(handle object);
    
    // @brief set all elements of a native buffer object.
    // if the buffer type has client memory and defer is true then commit happens at draw.
    void bufferSetElements(handle object, void* elements, ssize_t count, bool defer = true);
    
    void* bufferGetElements(handle object);
    void bufferSetElementCount(handle object, ssize_t count);
    void bufferUpdateElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer);
    ssize_t bufferGetCapacity(handle object);
    void bufferSetDirty(handle object, bool dirty);
    void bufferAppendElements(handle object, const void* elements, ssize_t count, bool defer);
    void bufferInsertElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer);
    void bufferRemoveElements(handle object, ssize_t start, ssize_t count, bool defer);
    void bufferAddCapacity(handle object, ssize_t count, bool zero);
    void bufferSwapElements(handle object, ssize_t source, ssize_t dest, ssize_t count);
    void bufferMoveElements(handle object, ssize_t source, ssize_t dest, ssize_t count);

    // HACK for backwards compatibility with MeshCommand
    CC_DEPRECATED(v3) unsigned bufferGetNativeBO(handle object) {return 0;}
    
protected:
    
    Handles _handles;
    id <MTLDevice> _mtlDevice;
    id <MTLCommandQueue> _mtlCommandQueue;
};

NS_PRIVATE_END

#endif//CC_METAL_AVAILABLE

#endif//_CC_GRAPHICS_METAL_H_
