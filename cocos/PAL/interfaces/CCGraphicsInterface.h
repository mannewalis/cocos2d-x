
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

#ifndef _CC_GRAPHICS_INTERFACE_H_
#define _CC_GRAPHICS_INTERFACE_H_

#include "PAL/CCPALMacros.h"
#include "PAL/CCPALTypes.h"
#include "base/CCRef.h"

NS_CC_BEGIN
class Rect;
NS_CC_END

NS_PRIVATE_BEGIN

class GraphicsInterface
    : public Ref
{
public:
        
    virtual ~GraphicsInterface() {}
    
    // @brief initializes the graphics API
    // Some methods can be called before this to set specific features.
    virtual bool init() = 0;
    
    // @brief shuts down this interface, releasing all resources.
    // All weak references and cached interfaces are invalidated.
    virtual void shutdown() = 0;

    // @brief returns the name of the api implementation
    virtual const char* name() const = 0;
    
    // @brief destroy object
    // any handle returned from this interface can be destroyed here.
    virtual void destroy(handle object) = 0;
    
    //////////////////////////////////////////////////////////////////
    // MARK: view and window                                        //
    //////////////////////////////////////////////////////////////////

    // @brief create window with a view and make current.
    // optional size, if null then full screen window.
    virtual handle windowCreate() = 0;

    
    //////////////////////////////////////////////////////////////////
    // MARK: render context                                         //
    //////////////////////////////////////////////////////////////////

    // @brief create a new render context.
    virtual handle renderContextCreate() = 0;
    
    // @brief setup the gpu for drawing a frame
    virtual void frameBegin() = 0;
    
    // @brief finish the frame and present
    virtual void frameEnd() = 0;

    
    //////////////////////////////////////////////////////////////////
    // MARK: vertex array                                           //
    //////////////////////////////////////////////////////////////////
    
    // @brief creates a vertex array object.
    virtual handle vertexArrayCreate(Primitive drawPrimitive) = 0;
    
    // @brief destroy a vertex array object.
    virtual void vertexArrayDestroy(handle vao) = 0;
        
    // @brief specifies a vertex attribute.
    virtual bool vertexArraySpecifyVertexAttribute(handle vao, handle buffer, const VertexAttribute& attribute) = 0;
    
    // @brief removes a previously specified vertex attribute
    virtual void vertexArrayRemoveVertexAttribute(handle vao, int index) = 0;
    
    // @brief specifies an index buffer to use with a vertex array.
    virtual bool vertexArraySpecifyIndexBuffer(handle vao, handle buffer) = 0;
    
    // @brief draws the vertex array.
    virtual ssize_t vertexArrayDrawElements(handle object, ssize_t start, ssize_t count) = 0;
    
    // @brief returns true if all of the managed buffers are empty.
    virtual bool vertexArrayIsEmpty(handle object) const = 0;
    
    // @brief clears all buffers by setting element count to 0.
    virtual void vertexArrayClear(handle object) = 0;
    
    // @brief returns true if any managed buffer is dirty.
    virtual bool vertexArrayIsDirty(handle object) const = 0;
    
    // @brief sets all dirty flags to 'dirty'
    virtual void vertexArraySetDirty(handle object, bool dirty) = 0;
    
    
    //////////////////////////////////////////////////////////////////
    // MARK: Vertex and Index buffers                               //
    //////////////////////////////////////////////////////////////////
    
    // @brief create a native buffer object for vertices/attributes/indices.
    virtual handle bufferCreate(ssize_t size, ssize_t count, BufferMode bufferMode, BufferIntent bufferIntent, BufferType bufferType, bool zero) = 0;
    
    // @brief destroy a native buffer object.
    virtual bool bufferDestroy(handle object) = 0;
    
    // @brief returns the buffer element size.
    virtual ssize_t bufferGetElementSize(handle object) = 0;

    // @brief returns the buffer element count.
    virtual ssize_t bufferGetElementCount(handle object) = 0;

    // @brief set all elements of a native buffer object.
    // if the buffer type has client memory and defer is true then commit happens at draw.
    virtual void bufferSetElements(handle object, void* elements, ssize_t count, bool defer) = 0;

    virtual void* bufferGetElements(handle object) = 0;
    virtual void bufferSetElementCount(handle object, ssize_t count) = 0;
    virtual void bufferUpdateElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer) = 0;
    virtual ssize_t bufferGetCapacity(handle object) = 0;
    virtual void bufferSetDirty(handle object, bool dirty) = 0;
    virtual void bufferAppendElements(handle object, const void* elements, ssize_t count, bool defer) = 0;
    virtual void bufferInsertElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer) = 0;
    virtual void bufferRemoveElements(handle object, ssize_t start, ssize_t count, bool defer) = 0;
    virtual void bufferAddCapacity(handle object, ssize_t count, bool zero) = 0;
    virtual void bufferSwapElements(handle object, ssize_t source, ssize_t dest, ssize_t count) = 0;
    virtual void bufferMoveElements(handle object, ssize_t source, ssize_t dest, ssize_t count) = 0;
    
    // HACK for backwards compatibility with MeshCommand
    CC_DEPRECATED(v3) virtual unsigned bufferGetNativeBO(handle object) = 0;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_INTERFACE_H_
