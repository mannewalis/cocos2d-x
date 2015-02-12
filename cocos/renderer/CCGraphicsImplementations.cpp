
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

#include "CCGraphicsImplementations.h"
#include "renderer/CCGraphicsInterface.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "renderer/implementations/metal/CCGraphicsMetal.h"
#endif
#include "renderer/implementations/opengles2/CCGraphicsOpenGLES2.h"

NS_CC_BEGIN

bool GraphicsImplementations::_lazyInit = true;
GraphicsImplementations::tAvailableImplementations GraphicsImplementations::_availableImplementations;

void GraphicsImplementations::lazyInit()
{
    if (_lazyInit)
    {
        _lazyInit = false;
        
#define ADD_IMPLEMENTATION(x) \
{ \
    auto imp = x::create(); \
    imp->retain(); \
    _availableImplementations.push_back(imp); \
}

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        ADD_IMPLEMENTATION(GraphicsMetal);
#endif
        ADD_IMPLEMENTATION(GraphicsOpenGLES2);
    }
}

GraphicsInterface* GraphicsImplementations::getAvailableGraphicsInterfaces()
{
    lazyInit();
    return _availableImplementations.empty() ? nullptr : _availableImplementations.front();
}

NS_CC_END
