
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

#include "CCGraphicsOpenGLES2VertexBuffer.h"

NS_PRIVATE_BEGIN

NS_PRIVATE_END


/*
 
 if (glIsBuffer(_vbo))
 {
 glDeleteBuffers(1, &_vbo);
 GL::bindVBO(_target, 0);
 _vbo = 0;
 }
 
 if (glIsBuffer(_vbo))
 {
 glDeleteBuffers(1, &_vbo);
 GL::bindVBO(_target, 0);
 }
 glGenBuffers(1, (GLuint*)&_vbo);
 GL::bindVBO(_target, _vbo);
 if (_elements)
 {
 glBufferData(_target, _elementSize * _elementCount, _elements, GL_STATIC_DRAW);
 GL::bindVBO(_target, _vbo);
 if(!glIsBuffer(_vbo))
 {
 CCLOGERROR("Renderer::recreate() : recreate VertexBuffer Error");
 }
 }
 
 
 
 
 
 if (_vbo)
 GL::bindVBO(_target, _vbo);
 
 if (false == isDirty())
 return;
 
 // default to all elements
 if (nullptr == elements)
 elements = _elements;
 
 // default to all elements
 if (0 == count)
 count = _elementCount;
 
 if (0 == _vbo)
 {
 glGenBuffers(1, &_vbo);
 _vboSize = getCapacityInBytes();
 CCASSERT(_vboSize, "_vboSize should not be 0");
 GL::bindVBO(_target, _vbo);
 glBufferData(_target, _vboSize, nullptr, _usage);
 glBufferData(_target, count * getElementSize(), elements, _usage);
 CHECK_GL_ERROR_DEBUG();
 }
 else
 {
 GL::bindVBO(_target, _vbo);
 const auto size = getCapacityInBytes();
 CCASSERT(size, "size should not be 0");
 if (size >= _vboSize)
 {
 _vboSize = size;
 glBufferData(_target, size, elements, _usage);
 CHECK_GL_ERROR_DEBUG();
 }
 else
 {
 intptr_t p = (intptr_t)elements + begin * _elementSize;
 glBufferSubData(_target, begin * _elementSize, count * _elementSize, (void*)p);
 CHECK_GL_ERROR_DEBUG();
 }
 }
 
 setDirty(false);
 
 
 
 
 
 
 switch (_arrayMode)
 {
 case ArrayMode::Immutable:
 _usage = GL_STATIC_DRAW;
 break;
 case ArrayMode::LongLived:
 _usage = GL_DYNAMIC_DRAW;
 break;
 case ArrayMode::Dynamic:
 _usage = GL_STREAM_DRAW;
 break;
 default:
 CCASSERT(false, "invalid ArrayMode");
 return false;
 }
 
 */
