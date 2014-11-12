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


BeginScissorCommand::BeginScissorCommand()
{
    _type = RenderCommand::Type::BEGIN_SCISSOR_COMMAND;
}

void BeginScissorCommand::init(float depth, const Rect& clippingRegion)
{
    _globalOrder = depth;
    _clippingRegion = clippingRegion;
}

void BeginScissorCommand::execute()
{
    ++_scissorCount;
    if (1 == _scissorCount)
        glEnable(GL_SCISSOR_TEST);
    GLView* glView = Director::getInstance()->getOpenGLView();
    glView->setScissorInPoints(_clippingRegion.origin.x, _clippingRegion.origin.y, _clippingRegion.size.width, _clippingRegion.size.height);
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
}

NS_CC_END