
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
#include "CCGraphicsMetalVertexArray.h"
#include "CCGraphicsMetalBuffer.h"
#include "PAL/CCPALHandles.h"
#include "PAL/CCPALManager.h"

NS_PRIVATE_BEGIN
void* __GraphicsMetal_factory()
{
#ifdef CC_METAL_AVAILABLE
    return cls::create();
#else
    return nullptr;
#endif
}
NS_PRIVATE_END

#ifdef CC_METAL_AVAILABLE

NS_PRIVATE_BEGIN

PAL_REGISTER_FACTORY(GraphicsInterface, metal, GraphicsMetal);

GraphicsInterface* GraphicsMetal::create()
{
    auto obj = new GraphicsMetal;
    if (obj && obj->init())
    {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

void GraphicsMetal::shutdown()
{
    [_mtlCommandQueue release];
    _mtlCommandQueue = nil;
    
    [_mtlDevice release];
    _mtlDevice = nil;
}

bool GraphicsMetal::init()
{
    _mtlDevice = MTLCreateSystemDefaultDevice();
    _mtlCommandQueue = [_mtlDevice newCommandQueue];
    return _mtlDevice != nil && _mtlCommandQueue != nil;
}

// MARK: windows and views

// @brief create window with a view and make current.
// optional size, if null then full screen window.
handle GraphicsMetal::windowCreate(Rect* size)
{
    return 0;
}

// MARK: vertex array

// @brief creates a vertex array object.
handle GraphicsMetal::vertexArrayCreate(Primitive drawPrimitive)
{
    auto vao = new (std::nothrow) GraphicsMetalVertexArray(drawPrimitive);
    return vao ? HANDLE_CREATE(_handles, vao) : HANDLE_INVALID;
}

// @brief delete a vertex array object.
void GraphicsMetal::vertexArrayDestroy(handle object)
{
    auto vao = HANDLE_TOPTR(_handles, object, Ref);
    PAL_ASSERT(vao != nullptr, "invalid handle");
    HANDLE_DESTROY(_handles, object);
    vao->release();
}

// @brief specifies a vertex attribute.
bool GraphicsMetal::vertexArraySpecifyVertexAttribute(handle object, handle buffer, const VertexAttribute& attribute)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    auto vbo = HANDLE_TOPTR(_handles, buffer, GraphicsMetalBuffer);
    PAL_ASSERT(vao && vbo, "invalid handle");
    return vao->specifyVertexAttribute(vbo, attribute);
}

// @brief removes a previously specified vertex attribute
void GraphicsMetal::vertexArrayRemoveVertexAttribute(handle object, int index)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    PAL_ASSERT(vao, "invalid handle");
    vao->removeVertexAttribute(index);
}

// @brief specifies an index buffer to use with a vertex array.
bool GraphicsMetal::vertexArraySpecifyIndexBuffer(handle object, handle buffer)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    auto ibo = HANDLE_TOPTR(_handles, buffer, GraphicsMetalBuffer);
    PAL_ASSERT(vao && ibo, "invalid handle");
    vao->specifyIndexBuffer(ibo);
    return true;
}

// @brief draws the vertex array.
ssize_t GraphicsMetal::vertexArrayDrawElements(handle object, ssize_t start, ssize_t count)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    PAL_ASSERT(vao, "invalid handle");
    return vao->draw(start, count);
}

bool GraphicsMetal::vertexArrayIsEmpty(handle object) const
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    return vao->empty();
}

void GraphicsMetal::vertexArrayClear(handle object)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    vao->clear();
}

bool GraphicsMetal::vertexArrayIsDirty(handle object) const
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    return vao->isDirty();
}

void GraphicsMetal::vertexArraySetDirty(handle object, bool dirty)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsMetalVertexArray);
    vao->setDirty(dirty);
}


handle GraphicsMetal::bufferCreate(ssize_t size, ssize_t count, BufferMode mode, BufferIntent intent, BufferType type, bool zero)
{
    auto vbo = new (std::nothrow) GraphicsMetalBuffer;
    if (vbo && vbo->init(size, count, mode, intent, type, zero))
    {
        return HANDLE_CREATE(_handles, vbo);
    }
    return HANDLE_INVALID;
}

bool GraphicsMetal::bufferDestroy(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, Ref);
    HANDLE_DESTROY(_handles, object);
    bo->release();
    return true;
}

// @brief returns the buffer element size.
ssize_t GraphicsMetal::bufferGetElementSize(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    return bo->getElementSize();
}

// @brief returns the buffer element count.
ssize_t GraphicsMetal::bufferGetElementCount(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    return bo->getElementCount();
}

// @brief set all elements of a native buffer object.
// if the buffer type has client memory and defer is true then commit happens at draw.
void GraphicsMetal::bufferSetElements(handle object, void* elements, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->setElements(elements, count, defer);
}

void* GraphicsMetal::bufferGetElements(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    return bo->getElements();
}

void GraphicsMetal::bufferSetElementCount(handle object, ssize_t count)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->setElementCount(count);
}

void GraphicsMetal::bufferUpdateElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->updateElements(elements, start, count, defer);
}

ssize_t GraphicsMetal::bufferGetCapacity(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    return bo->getCapacity();
}

void GraphicsMetal::GraphicsMetal::bufferSetDirty(handle object, bool dirty)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->setDirty(dirty);
}

void GraphicsMetal::bufferAppendElements(handle object, const void* elements, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->appendElements(elements, count, defer);
}

void GraphicsMetal::bufferInsertElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->insertElements(elements, start, count, defer);
}

void GraphicsMetal::bufferRemoveElements(handle object, ssize_t start, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->removeElements(start, count, defer);
}

void GraphicsMetal::bufferAddCapacity(handle object, ssize_t count, bool zero)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->addCapacity(count, zero);
}

void GraphicsMetal::bufferSwapElements(handle object, ssize_t source, ssize_t dest, ssize_t count)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->swapElements(source, dest, count);
}

void GraphicsMetal::bufferMoveElements(handle object, ssize_t source, ssize_t dest, ssize_t count)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsMetalBuffer);
    bo->moveElements(source, dest, count);
}

NS_PRIVATE_END

#endif//CC_METAL_AVAILABLE
