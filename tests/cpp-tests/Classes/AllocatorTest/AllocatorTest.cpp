
/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 
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

#include "AllocatorTest.h"

static int sceneIdx = -1;

static std::function<Layer*()> createFunctions[] =
{
    CL(AllocatorTest)
};

#define MAX_LAYER (sizeof(createFunctions) / sizeof(createFunctions[0]))

static Layer* nextAllocatorTestAction()
{
    sceneIdx++;
    sceneIdx = sceneIdx % MAX_LAYER;
    
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* backAllocatorTestAction()
{
    sceneIdx--;
    int total = MAX_LAYER;
    if( sceneIdx < 0 )
        sceneIdx += total;
    
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* restartAllocatorTestAction()
{
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

AllocatorTest::AllocatorTest()
{
}

AllocatorTest::~AllocatorTest()
{
}

std::string AllocatorTest::title() const
{
    return "Allocator Test";
}

std::string AllocatorTest::subtitle() const
{
    return "";
}

void AllocatorTest::restartCallback( Ref* sender )
{
    auto s = new AllocatorTestScene();
    s->addChild(restartAllocatorTestAction());
    Director::getInstance()->replaceScene(s);
    s->release();
}

void AllocatorTest::nextCallback( Ref* sender )
{
    auto s = new AllocatorTestScene();
    s->addChild( nextAllocatorTestAction() );
    Director::getInstance()->replaceScene(s);
    s->release();
}

void AllocatorTest::backCallback( Ref* sender )
{
    auto s = new AllocatorTestScene();
    s->addChild( backAllocatorTestAction() );
    Director::getInstance()->replaceScene(s);
    s->release();
}

void AllocatorTest::onEnter()
{
    BaseTest::onEnter();
}

void AllocatorTest::onExit()
{
    BaseTest::onExit();
}
