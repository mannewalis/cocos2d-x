
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

    
    //////////////////////////////////////////////////////////////////
    // MARK: vertex array
    
    // @brief creates a vertex array object.
    virtual handle vertexArrayCreate(Primitive drawPrimitive) = 0;
    
    // @brief destroy a vertex array object.
    virtual void vertexArrayDestroy(handle vao) = 0;
        
    // @brief specifies a vertex attribute.
    virtual bool vertexArraySpecifyVertexAttribute(handle vao, handle buffer, int index, ssize_t offset, AttributeDataType type, ssize_t count, bool normalized) = 0;
    
    // @brief removes a previously specified vertex attribute
    virtual void vertexArrayRemoveVertexAttribute(handle vao, int index) = 0;
    
    // @brief specifies an index buffer to use with a vertex array.
    virtual bool vertexArraySpecifyIndexBuffer(handle vao, handle buffer) = 0;
    
    // @brief stage client side geometry that is to be copied to native buffers when drawn.
    // this is better than bufferCommitElements because it doesn't have to bind the buffer twice,
    // but the elements need to live unmodified until vertexArrayDrawElements is called.
    // works with vertex and index buffers. Overlapping ranges will be copied multiple times.
    virtual void vertexArrayStageElements(handle vao, handle buffer, int index, void* elements, ssize_t start, ssize_t count) = 0;
    
    // @brief draws the vertex array.
    virtual void vertexArrayDrawElements(handle object, ssize_t start, ssize_t count) = 0;

    
    //////////////////////////////////////////////////////////////////
    // MARK: Vertex and Index buffers
    
    // @brief create a native buffer object for vertices/attributes/indices.
    virtual handle bufferCreate(ssize_t size, ssize_t count, BufferMode bufferMode, BufferIntent bufferIntent, bool zero) = 0;
    
    // @brief destroy a native buffer object.
    virtual bool bufferDestroy(handle object) = 0;
    
    // @brief commit elements from a client array to a native buffer object.
    virtual bool bufferCommitElements(handle object, const void* elements, ssize_t start, ssize_t count) = 0;
    
    // HACK for backwards compatibility with MeshCommand
    CC_DEPRECATED(v3) virtual unsigned bufferGetBO(handle object) = 0;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_INTERFACE_H_
