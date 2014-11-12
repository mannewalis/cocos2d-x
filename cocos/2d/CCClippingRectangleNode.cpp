
#include "CCClippingRectangleNode.h"
#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"
#include "math/Vec2.h"
#include "CCGLView.h"

NS_CC_BEGIN

ClippingRectangleNode* ClippingRectangleNode::create(const Rect& clippingRegion)
{
    ClippingRectangleNode* node = new ClippingRectangleNode();
    if (node && node->init()) {
        node->setClippingRegion(clippingRegion);
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }
    
    return node;
}

ClippingRectangleNode* ClippingRectangleNode::create()
{
    ClippingRectangleNode* node = new ClippingRectangleNode();
    if (node && node->init()) {
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }
    
    return node;
}

void ClippingRectangleNode::setClippingRegion(const Rect &clippingRegion)
{
    _clippingRegion = clippingRegion;
}

void ClippingRectangleNode::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if (_clippingEnabled)
    {
        float scaleX = _scaleX;
        float scaleY = _scaleY;
        Node *parent = this->getParent();
        while (parent) {
            scaleX *= parent->getScaleX();
            scaleY *= parent->getScaleY();
            parent = parent->getParent();
        }
        
        const Point pos = convertToWorldSpace(Point(_clippingRegion.origin.x, _clippingRegion.origin.y));
        const Rect clippingRect = {pos.x * scaleX,
                                   pos.y * scaleY,
                                   _clippingRegion.size.width * scaleX,
                                    _clippingRegion.size.height * scaleY};
    
        _beforeVisitCmdScissor.init(_globalZOrder, clippingRect);
        renderer->addCommand(&_beforeVisitCmdScissor);
    }
    
    Node::visit(renderer, parentTransform, parentFlags);
    
    if (_clippingEnabled)
    {
        _afterVisitCmdScissor.init(_globalZOrder);
        renderer->addCommand(&_afterVisitCmdScissor);
    }
}

NS_CC_END
