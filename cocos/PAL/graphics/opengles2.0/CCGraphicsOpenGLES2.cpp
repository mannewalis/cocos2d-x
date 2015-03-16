
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
#include "cocos2d.h"

#include "CCGraphicsOpenGLES2.h"
#include "CCGraphicsOpenGLES2VertexArray.h"
#include "CCGraphicsOpenGLES2Buffer.h"

USING_NS_CC;
NS_PRIVATE_BEGIN

GraphicsInterface* GraphicsOpenGLES20::create()
{
    auto obj = new GraphicsOpenGLES20;
    if (obj)
    {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

void GraphicsOpenGLES20::shutdown()
{
    PAL_SAFE_RELEASE_NULL(_view);
}

bool GraphicsOpenGLES20::init()
{
    bool result = true;
    
    // for now, to support easy backwards compatibility we just steal the default GL view.
    _view = Director::getInstance()->getOpenGLView();
    if (_view)
        _view->retain();

    result = _view ? result : false;
    
    return result;
}

// MARK: vertex array

// @brief creates a vertex array object.
handle GraphicsOpenGLES20::vertexArrayCreate(Primitive drawPrimitive)
{
    auto vao = new (std::nothrow) GraphicsOpenGLES2VertexArray(drawPrimitive);
    return vao ? HANDLE_CREATE(_handles, vao) : HANDLE_INVALID;
}

// @brief delete a vertex array object.
void GraphicsOpenGLES20::vertexArrayDestroy(handle object)
{
    auto vao = HANDLE_TOPTR(_handles, object, Ref);
    PAL_ASSERT(vao != nullptr, "invalid handle");
    HANDLE_DESTROY(_handles, object);
    vao->release();
}

// @brief specifies a vertex attribute.
bool GraphicsOpenGLES20::vertexArraySpecifyVertexAttribute(handle object, handle buffer, int index, ssize_t offset, AttributeDataType type, ssize_t count, bool normalized)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    auto vbo = HANDLE_TOPTR(_handles, buffer, GraphicsOpenGLES2Buffer);
    PAL_ASSERT(vao && vbo, "invalid handle");
    return vao->specifyVertexAttribute(vbo, {index, offset, type, count, normalized});
}

// @brief removes a previously specified vertex attribute
void GraphicsOpenGLES20::vertexArrayRemoveVertexAttribute(handle object, int index)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    PAL_ASSERT(vao, "invalid handle");
    vao->removeVertexAttribute(index);
}

// @brief specifies an index buffer to use with a vertex array.
bool GraphicsOpenGLES20::vertexArraySpecifyIndexBuffer(handle object, handle buffer)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    auto ibo = HANDLE_TOPTR(_handles, buffer, GraphicsOpenGLES2Buffer);
    PAL_ASSERT(vao && ibo, "invalid handle");
    vao->specifyIndexBuffer(ibo);
    return true;
}

void GraphicsOpenGLES20::vertexArrayStageElements(handle object, handle buffer, int index, void* elements, ssize_t start, ssize_t count)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    auto bo  = HANDLE_TOPTR(_handles, buffer, GraphicsOpenGLES2Buffer);
    PAL_ASSERT(vao && bo, "invalid handle");
    vao->stageElements(bo, index, elements, start, count);
}

// @brief draws the vertex array.
void GraphicsOpenGLES20::vertexArrayDrawElements(handle object, ssize_t start, ssize_t count)
{
    auto vao = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2VertexArray);
    PAL_ASSERT(vao, "invalid handle");
    vao->drawElements(start, count);
}

handle GraphicsOpenGLES20::bufferCreate(ssize_t size, ssize_t count, BufferMode bufferMode, BufferIntent bufferIntent, bool zero)
{
    auto vbo = new (std::nothrow) GraphicsOpenGLES2Buffer;
    if (vbo && vbo->init(size, count, bufferMode, bufferIntent, zero))
    {
        return HANDLE_CREATE(_handles, vbo);
    }
    return HANDLE_INVALID;
}

bool GraphicsOpenGLES20::bufferDestroy(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, Ref);
    HANDLE_DESTROY(_handles, object);
    bo->release();
    return true;
}

bool GraphicsOpenGLES20::bufferCommitElements(handle object, const void* elements, ssize_t start, ssize_t count)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    return bo->commitElements(elements, start, count);
}

// HACK for backwards compatibility with MeshCommand
CC_DEPRECATED(v3) unsigned GraphicsOpenGLES20::bufferGetBO(handle object)
{
    auto bo = HANDLE_TOPTR(_handles, object, GraphicsOpenGLES2Buffer);
    return bo->getBO();
}

NS_PRIVATE_END
