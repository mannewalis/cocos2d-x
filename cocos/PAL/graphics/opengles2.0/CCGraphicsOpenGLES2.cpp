
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

#include "PAL/CCPALMacros.h"
#include "PAL/CCPALManager.h"

#include "CCGraphicsOpenGLES2.h"
#include "CCGraphicsOpenGLES2VertexArray.h"
#include "CCGraphicsOpenGLES2Buffer.h"

#include "cocos2d.h"
#import <OpenGLES/EAGL.h>

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "platform/ios/CCEAGLView-ios.h"
#endif

USING_NS_CC;
NS_PRIVATE_BEGIN

PAL_DECLARE_FACTORY(GraphicsOpenGLES2);

GraphicsInterface* GraphicsOpenGLES2::create()
{
    auto obj = new GraphicsOpenGLES2;
    if (obj && obj->init())
    {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

void GraphicsOpenGLES2::shutdown()
{
    PAL_SAFE_RELEASE_NULL(_view);
}

bool GraphicsOpenGLES2::init()
{
    bool result = true;
    
//    // for now, to support easy backwards compatibility we just steal the default GL view.
//    _view = Director::getInstance()->getOpenGLView();
//    if (_view)
//        _view->retain();
//
//    result = _view ? result : false;
    
    return result;
}

const char* GraphicsOpenGLES2::name() const
{
    return GraphicsOpenGLES2::api_name;
}

bool GraphicsOpenGLES2::destroy(handle object)
{
    auto o = HANDLE_TOPTR(_handles, object, Ref);
    if (!o)
        return false;
    HANDLE_DESTROY(_handles, object);
    o->release();
    return true;
}

// MARK: windows and views

// @brief create window with a view and make current.
// optional size, if null then full screen window.
handle GraphicsOpenGLES2::windowCreate()
{
    return HANDLE_INVALID;
}

void GraphicsOpenGLES2::frameBegin()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [EAGLContext setCurrentContext: [(CCEAGLView*)Director::getInstance()->getOpenGLView()->getEAGLView() context]];
#endif
}

void GraphicsOpenGLES2::frameEnd()
{
    Director::getInstance()->getOpenGLView()->swapBuffers();
}

// MARK: vertex array

// @brief creates a vertex array object.
handle GraphicsOpenGLES2::vertexArrayCreate(Primitive drawPrimitive)
{
    auto vao = new (std::nothrow) GraphicsOpenGLES2VertexArray(drawPrimitive);
    return vao ? HANDLE_CREATE(_handles, vao) : HANDLE_INVALID;
}

// @brief delete a vertex array object.
void GraphicsOpenGLES2::vertexArrayDestroy(handle object)
{
    auto vao = HANDLE_TOPTR(_handles, object, Ref);
    PAL_ASSERT(vao != nullptr, "invalid handle");
    HANDLE_DESTROY(_handles, object);
    vao->release();
}

// @brief specifies a vertex attribute.
bool GraphicsOpenGLES2::vertexArraySpecifyVertexAttribute(handle object, handle buffer, const VertexAttribute& attribute)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    auto vbo = HANDLE_TOPTR(_handles, buffer, GraphicsOpenGLES2Buffer);
    PAL_ASSERT(vao && vbo, "invalid handle");
    return vao->specifyVertexAttribute(vbo, attribute);
}

// @brief removes a previously specified vertex attribute
void GraphicsOpenGLES2::vertexArrayRemoveVertexAttribute(handle object, int index)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    PAL_ASSERT(vao, "invalid handle");
    vao->removeVertexAttribute(index);
}

// @brief specifies an index buffer to use with a vertex array.
bool GraphicsOpenGLES2::vertexArraySpecifyIndexBuffer(handle object, handle buffer)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    auto ibo = HANDLE_TOPTR(_handles, buffer, GraphicsOpenGLES2Buffer);
    PAL_ASSERT(vao && ibo, "invalid handle");
    vao->specifyIndexBuffer(ibo);
    return true;
}

// @brief draws the vertex array.
ssize_t GraphicsOpenGLES2::vertexArrayDrawElements(handle object, ssize_t start, ssize_t count)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    PAL_ASSERT(vao, "invalid handle");
    return vao->draw(start, count);
}

bool GraphicsOpenGLES2::vertexArrayIsEmpty(handle object) const
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    return vao->empty();
}

void GraphicsOpenGLES2::vertexArrayClear(handle object)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    vao->clear();
}

bool GraphicsOpenGLES2::vertexArrayIsDirty(handle object) const
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    return vao->isDirty();
}

void GraphicsOpenGLES2::vertexArraySetDirty(handle object, bool dirty)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    vao->setDirty(dirty);
}


handle GraphicsOpenGLES2::bufferCreate(ssize_t size, ssize_t count, BufferMode mode, BufferIntent intent, BufferType type, bool zero)
{
    auto vbo = new (std::nothrow) GraphicsOpenGLES2Buffer;
    if (vbo && vbo->init(size, count, mode, intent, type, zero))
    {
        return HANDLE_CREATE(_handles, vbo);
    }
    return HANDLE_INVALID;
}

bool GraphicsOpenGLES2::bufferDestroy(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, Ref);
    HANDLE_DESTROY(_handles, object);
    bo->release();
    return true;
}

// @brief returns the buffer element size.
ssize_t GraphicsOpenGLES2::bufferGetElementSize(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    return bo->getElementSize();
}

// @brief returns the buffer element count.
ssize_t GraphicsOpenGLES2::bufferGetElementCount(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    return bo->getElementCount();
}

// @brief set all elements of a native buffer object.
// if the buffer type has client memory and defer is true then commit happens at draw.
void GraphicsOpenGLES2::bufferSetElements(handle object, void* elements, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->setElements(elements, count, defer);
}

void* GraphicsOpenGLES2::bufferGetElements(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    return bo->getElements();
}

void GraphicsOpenGLES2::bufferSetElementCount(handle object, ssize_t count)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->setElementCount(count);
}

void GraphicsOpenGLES2::bufferUpdateElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->updateElements(elements, start, count, defer);
}

ssize_t GraphicsOpenGLES2::bufferGetCapacity(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    return bo->getCapacity();
}

void GraphicsOpenGLES2::GraphicsOpenGLES2::bufferSetDirty(handle object, bool dirty)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->setDirty(dirty);
}

void GraphicsOpenGLES2::bufferAppendElements(handle object, const void* elements, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->appendElements(elements, count, defer);
}

void GraphicsOpenGLES2::bufferInsertElements(handle object, const void* elements, ssize_t start, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->insertElements(elements, start, count, defer);
}

void GraphicsOpenGLES2::bufferRemoveElements(handle object, ssize_t start, ssize_t count, bool defer)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->removeElements(start, count, defer);
}

void GraphicsOpenGLES2::bufferAddCapacity(handle object, ssize_t count, bool zero)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->addCapacity(count, zero);
}

void GraphicsOpenGLES2::bufferSwapElements(handle object, ssize_t source, ssize_t dest, ssize_t count)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->swapElements(source, dest, count);
}

void GraphicsOpenGLES2::bufferMoveElements(handle object, ssize_t source, ssize_t dest, ssize_t count)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    bo->moveElements(source, dest, count);
}

// HACK for backwards compatibility with MeshCommand
CC_DEPRECATED(v3) unsigned GraphicsOpenGLES2::bufferGetNativeBO(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    return bo->getBO();
}

NS_PRIVATE_END
