//
//  CCStencilCommand.cpp
//  cocos2d_libs
//
//  Created by Justin Graham on 11/12/14.
//
//

#include "CCStencilCommand.h"
#include "ccMacros.h"
#include "CCDirector.h"
#include "CCRenderer.h"
#include "CCGLProgram.h"
#include "CCGLProgramCache.h"
#include "ccGLStateCache.h"

NS_CC_BEGIN

GLint StencilCommand::_stencilBits = -1;
StencilCommand::tStencilStates StencilCommand::_stencilStates;
int StencilCommand::_stencilCount = 0;
GLuint StencilCommand::_layer = -1;

void BeginStencilCommand::init(float depth, bool inverted, float alphaThreshold, Node* stencil)
{
    _globalOrder = depth;
    _inverted = inverted;
    _alphaThreshold = alphaThreshold;
    _stencil = stencil;
    
    // get (only once) the number of bits of the stencil buffer
    static bool once = true;
    if (once)
    {
        glGetIntegerv(GL_STENCIL_BITS, &_stencilBits);
        if (_stencilBits <= 0)
        {
            CCLOG("Stencil buffer is not enabled.");
        }
        once = false;
    }
    
    if (_alphaThreshold < 1)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#else
        // since glAlphaTest do not exists in OES, use a shader that writes
        // pixel only if greater than an alpha threshold
        GLProgram *program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST_NO_MV);
        GLint alphaValueLocation = glGetUniformLocation(program->getProgram(), GLProgram::UNIFORM_NAME_ALPHA_TEST_VALUE);
        // set our alphaThreshold
        program->use();
        program->setUniformLocationWith1f(alphaValueLocation, _alphaThreshold);
        // we need to recursively apply this shader to all the nodes in the stencil node
        // FIXME: we should have a way to apply shader to all nodes without having to do this
        setProgram(_stencil, program);
#endif
    }
}
    
void BeginStencilCommand::execute()
{
    if (0 == _stencilCount)
        glEnable(GL_STENCIL_TEST);

    CHECK_GL_ERROR_DEBUG();

    ++_stencilCount;
    ++_layer;

    _stencilStates.emplace(getCurrentState());
    const auto& currentState = _stencilStates.top();
    
    // all bits on the stencil buffer are readonly, except the current layer bit,
    // this means that operation like glClear or glStencilOp will be masked with this value
    glStencilMask(currentState._mask_layer);
    
    // disable update to the depth buffer while drawing the stencil,
    // as the stencil is not meant to be rendered in the real scene,
    // it should never prevent something else to be drawn,
    // only disabling depth buffer update should do
    glDepthMask(GL_FALSE);
    
    // manually clear the stencil buffer by drawing a fullscreen rectangle on it
    // setup the stencil test func like this:
    // for each pixel in the fullscreen rectangle
    //     never draw it into the frame buffer
    //     if not in inverted mode: set the current layer value to 0 in the stencil buffer
    //     if in inverted mode: set the current layer value to 1 in the stencil buffer
    glStencilFunc(GL_NEVER, currentState._mask_layer, currentState._mask_layer);
    glStencilOp(!_inverted ? GL_ZERO : GL_REPLACE, GL_KEEP, GL_KEEP);
    
    // draw a fullscreen solid rectangle to clear the stencil buffer
    drawFullScreenQuadClearStencil();
    
    // setup the stencil test func like this:
    // for each pixel in the stencil node
    //     never draw it into the frame buffer
    //     if not in inverted mode: set the current layer value to 1 in the stencil buffer
    //     if in inverted mode: set the current layer value to 0 in the stencil buffer
    glStencilFunc(GL_NEVER, currentState._mask_layer, currentState._mask_layer);
    glStencilOp(!_inverted ? GL_REPLACE : GL_ZERO, GL_KEEP, GL_KEEP);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // enable alpha test only if the alpha threshold < 1,
    // indeed if alpha threshold == 1, every pixel will be drawn anyways
    if (_alphaThreshold < 1)
    {
        // manually save the alpha test state
        _currentStencilState._alphaTestEnabled = glIsEnabled(GL_ALPHA_TEST);
        glGetIntegerv(GL_ALPHA_TEST_FUNC, (GLint *)&_currentStencilState._alphaTestFunc);
        glGetFloatv(GL_ALPHA_TEST_REF, &_currentStencilState._currentAlphaTestRef);
        // enable alpha testing
        glEnable(GL_ALPHA_TEST);
        // check for OpenGL error while enabling alpha test
        CHECK_GL_ERROR_DEBUG();
        // pixel will be drawn only if greater than an alpha threshold
        glAlphaFunc(GL_GREATER, _alphaThreshold);
    }
#endif
}

StencilCommand::tStencilState BeginStencilCommand::getCurrentState() const
{
    tStencilState state;
    
    state._enabled = glIsEnabled(GL_STENCIL_TEST);
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint*)&state._writeMask);
    glGetIntegerv(GL_STENCIL_FUNC, (GLint *)&state._func);
    glGetIntegerv(GL_STENCIL_REF, &state._ref);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint *)&state._valueMask);
    glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&state._fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&state._passDepthFail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&state._passDepthPass);
    
    // mask of the current layer (ie: for layer 3: 00000100)
    state._mask_layer = 0x1 << _layer;
    
    // mask of all layers less than the current (ie: for layer 3: 00000011)
    state._mask_layer_l = state._mask_layer - 1;
    
    // mask of all layers less than or equal to the current (ie: for layer 3: 00000111)
    state._mask_layer_le = state._mask_layer | state._mask_layer_l;
    
    // manually save the depth test state
    glGetBooleanv(GL_DEPTH_WRITEMASK, &state._depthWriteMask);

    state._alphaTestEnabled = GL_FALSE;
    state._alphaTestFunc = GL_ALWAYS;
    state._alphaTestRef = 1;

    return state;
}

void BeginStencilCommand::drawFullScreenQuadClearStencil()
{
    Director* director = Director::getInstance();
    CCASSERT(nullptr != director, "Director is null when seting matrix stack");
    
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    
    Vec2 vertices[] = {
        Vec2(-1, -1),
        Vec2(1, -1),
        Vec2(1, 1),
        Vec2(-1, 1)
    };
    
    auto glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_U_COLOR);
    
    int colorLocation = glProgram->getUniformLocation("u_color");
    CHECK_GL_ERROR_DEBUG();
    
    Color4F color(1, 1, 1, 1);
    
    glProgram->use();
    glProgram->setUniformsForBuiltins();
    glProgram->setUniformLocationWith4fv(colorLocation, (GLfloat*) &color.r, 1);
    
    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);
    
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}


void ReadyStencilCommand::init(float depth)
{
    _globalOrder = depth;
}

void ReadyStencilCommand::execute()
{
    const auto& currentState = _stencilStates.top();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // restore alpha test state
    if (_alphaThreshold < 1)
    {
        // manually restore the alpha test state
        glAlphaFunc(_currentAlphaTestFunc, currentState._currentAlphaTestRef);
        if (!currentState._currentAlphaTestEnabled)
        {
            glDisable(GL_ALPHA_TEST);
        }
    }
#endif
    
    // restore the depth test state
    glDepthMask(currentState._depthWriteMask);
    
    // setup the stencil test func like this:
    // for each pixel of this node and its childs
    //     if all layers less than or equals to the current are set to 1 in the stencil buffer
    //         draw the pixel and keep the current layer in the stencil buffer
    //     else
    //         do not draw the pixel but keep the current layer in the stencil buffer
    glStencilFunc(GL_EQUAL, currentState._mask_layer_le, currentState._mask_layer_le);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}


void EndStencilCommand::init(float depth)
{
    _globalOrder = depth;
}

void EndStencilCommand::execute()
{
    --_stencilCount;
    --_layer;

    if (0 == _stencilCount)
        glDisable(GL_STENCIL_TEST);
    else
    {
        _stencilStates.pop();
        const auto& currentState = _stencilStates.top();
        
        glStencilFunc(currentState._func, currentState._ref, currentState._valueMask);
        glStencilOp(currentState._fail, currentState._passDepthFail, currentState._passDepthPass);
        glStencilMask(currentState._writeMask);
    }
}

NS_CC_END