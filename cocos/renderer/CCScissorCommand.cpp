//
//  CCScissorCommand.cpp
//  cocos2d_libs
//
//  Created by Justin Graham on 11/11/14.
//
//

#include "CCScissorCommand.h"
#include "CCDirector.h"

NS_CC_BEGIN

int ScissorCommand::_scissorCount = 0;
Rect ScissorCommand::_currentClippingRegion;

BeginScissorCommand::BeginScissorCommand()
{
    _type = RenderCommand::Type::BEGIN_SCISSOR_COMMAND;
}

void BeginScissorCommand::init(float depth, const Rect& clippingRegion, bool respectPreviousScissor)
{
    _globalOrder = depth;
    _clippingRegion = clippingRegion;
    
    if (0 < _scissorCount)
        _previousClippingRegion = _currentClippingRegion;
    
    if (respectPreviousScissor)
    {
        //set the intersection of _parentScissorRect and frame as the new scissor rect
        if (_clippingRegion.intersectsRect(_previousClippingRegion))
        {
            float x = MAX(_clippingRegion.origin.x, _previousClippingRegion.origin.x);
            float y = MAX(_clippingRegion.origin.y, _previousClippingRegion.origin.y);
            float xx = MIN(_clippingRegion.origin.x+_clippingRegion.size.width, _previousClippingRegion.origin.x+_previousClippingRegion.size.width);
            float yy = MIN(_clippingRegion.origin.y+_clippingRegion.size.height, _previousClippingRegion.origin.y+_previousClippingRegion.size.height);
            _clippingRegion = Rect(x, y, xx-x, yy-y);
        }
    }
    
    _currentClippingRegion = _clippingRegion;
}

void BeginScissorCommand::execute()
{
    if (0 == _scissorCount)
        glEnable(GL_SCISSOR_TEST);
    GLView* glView = Director::getInstance()->getOpenGLView();
    glView->setScissorInPoints(_clippingRegion.origin.x, _clippingRegion.origin.y, _clippingRegion.size.width, _clippingRegion.size.height);
    ++_scissorCount;
}


EndScissorCommand::EndScissorCommand()
{
    _type = RenderCommand::Type::END_SCISSOR_COMMAND;
}

void EndScissorCommand::init(float depth)
{
    _globalOrder = depth;
}

void EndScissorCommand::execute()
{
    --_scissorCount;
    if (0 == _scissorCount)
        glDisable(GL_SCISSOR_TEST);
    else
    {
        GLView* glView = Director::getInstance()->getOpenGLView();
        glView->setScissorInPoints(_previousClippingRegion.origin.x, _previousClippingRegion.origin.y, _previousClippingRegion.size.width, _previousClippingRegion.size.height);
        _currentClippingRegion = _previousClippingRegion;
    }
}

NS_CC_END