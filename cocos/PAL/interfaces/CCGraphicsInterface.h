
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

    // MARK: vertex array
    
    // @brief creates a vertex array object.
    virtual handle vertexArrayCreate() = 0;
    
    // @brief destroy a vertex array object.
    virtual bool vertexArrayDestroy(handle object) = 0;
        
    // @brief specifies a vertex attribute.
    virtual bool vertexArraySpecifyAttribute(handle object, handle buffer, ssize_t index, ssize_t offset, DataType type, ssize_t count, bool normalized) = 0;
    
    // @brief draws the vertex array.
    virtual bool vertexArrayDrawElements(handle object, ssize_t start, ssize_t count) = 0;

    // MARK: Vertex and Index buffers
    
    virtual handle bufferCreate(ssize_t size, ssize_t count, ArrayMode arrayMode, bool zero) = 0;
    virtual bool bufferDestroy(handle object) = 0;
    virtual bool bufferCommitElements(handle object, const void* elements, ssize_t count, ssize_t begin) = 0;
    // HACK for backwards compatibility with MeshCommand
    CC_DEPRECATED(v3) virtual unsigned bufferGetBO(handle object) = 0;
};

NS_PRIVATE_END

#endif//_CC_GRAPHICS_INTERFACE_H_
