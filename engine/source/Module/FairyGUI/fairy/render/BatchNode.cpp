#include "BatchNode.h"
#include "RenderDataProcess.h"
#include "FairyGUI/fairy/display/DisplayObject.h"

NS_FGUI_BEGIN

RenderQueueObject::RenderQueueObject(FairyRenderType t, RenderContext rc) :type(t), fairyContext(rc) {}

RenderQueueObject::RenderQueueObject(FairyRenderType t, FontContext fc) : type(t), fontContext(fc) {}

RenderQueueObject::RenderQueueObject(FairyRenderType t, GObject* go) : type(t), extensionObject(go) {}

RenderQueueObject::RenderQueueObject(FairyRenderType t, MaskContext mask) : type(t), maskContext(mask) {}

RenderQueueObject::RenderQueueObject(FairyRenderType t, ShapeContext shape) : type(t), shapeContext(shape) {}

RenderQueueObject::~RenderQueueObject() {}

RenderQueueObject::RenderQueueObject(const RenderQueueObject& rhv)
{
    this->type = rhv.type;
    if(this->type == FairyRenderType::Fairy)
    {
        this->fairyContext = rhv.fairyContext;
    }
    else if (this->type == FairyRenderType::Font)
    {
        this->fontContext = rhv.fontContext;
    }
    else if (this->type == FairyRenderType::Mask)
    {
        this->maskContext = rhv.maskContext;
    }
    else if(this->type == FairyRenderType::Shape)
    {
        this->shapeContext = rhv.shapeContext;
    }
    else
    {
        this->extensionObject = rhv.extensionObject;
    }
}

RenderQueueObject& RenderQueueObject::operator =(const RenderQueueObject& rhv)
{
    this->type = rhv.type;
    if (this->type == FairyRenderType::Fairy)
    {
        this->fairyContext = rhv.fairyContext;
    }
    else if (this->type == FairyRenderType::Font)
    {
        this->fontContext = rhv.fontContext;
    }
    else if (this->type == FairyRenderType::Mask)
    {
        this->maskContext = rhv.maskContext;
    }
    else if(this->type == FairyRenderType::Shape)
    {
        this->shapeContext = rhv.shapeContext;
    }
    else
    {
        this->extensionObject = rhv.extensionObject;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------------------------
BatchNode::BatchNode(FairyRenderType renderType, FairyNode node) :
    lastNode(nullptr),
    nextNode(nullptr),
    type(renderType),
    boundsRect(OBBRect::ZERO),
    fairyNode(node)
{
    this->arrObj.push_back(node.go);
}

BatchNode::BatchNode(FairyRenderType renderType, FontNode node) :
    lastNode(nullptr),
    nextNode(nullptr),
    type(renderType),
    boundsRect(OBBRect::ZERO),
    fontNode(node)
{
    this->arrObj.push_back(node.go);
}

BatchNode::BatchNode(FairyRenderType renderType, MaskNode node) :
    lastNode(nullptr),
    nextNode(nullptr),
    type(renderType),
    boundsRect(OBBRect::ZERO),
    maskNode(node) {}

BatchNode::BatchNode(FairyRenderType renderType, GObject* node) :
    lastNode(nullptr),
    nextNode(nullptr),
    type(renderType),
    boundsRect(OBBRect::ZERO),
    extensionObject(node) {}

BatchNode::BatchNode(FairyRenderType renderType, ShapeNode node) :
    lastNode(nullptr),
    nextNode(nullptr),
    type(renderType),
    boundsRect(OBBRect::ZERO),
    shapeNode(node) {}


BatchNode::BatchResult BatchNode::CanBatch(BatchNode* node)
{
    if (this->type == FairyRenderType::Mask)
    {
        return BatchResult::Fail;
    }

    if (this->type != FairyRenderType::Fairy && this->type != FairyRenderType::Font) return BatchResult::Fail;

    auto isIntersects = false;
    auto insertRect = node->boundsRect;
    for(auto& rect : this->arrRect)
    {
        isIntersects = rect.intersects(insertRect);
        if (isIntersects) break;
    }
    
    if (this->type != node->type)
    {
        if(isIntersects)
        {
            return BatchResult::Fail;
        }
        else
        {
            return BatchResult::Continue;
        }
    }
    else
    {
        auto texFlag = false; 
        auto grayFlag = false; 
        if(this->type == FairyRenderType::Fairy)
        {
            texFlag = this->fairyNode.texture == node->fairyNode.texture;
            grayFlag = this->fairyNode.isGrayed == node->fairyNode.isGrayed;
        }
        else if (this->type == FairyRenderType::Font)
        {
            bool outLine = false;
            bool shadow = false;
            if(this->fontNode.isOutLine == false &&
                this->fontNode.isOutLine == node->fontNode.isOutLine &&
                this->fontNode.isShadow == false &&
                this->fontNode.isShadow == node->fontNode.isShadow)
            {
                outLine = true;
                shadow = true;
            }
            else
            {
                if (this->fontNode.isOutLine == true && 
                    this->fontNode.isOutLine == node->fontNode.isOutLine &&
                    this->fontNode.outLineWidth == node->fontNode.outLineWidth &&
                    this->fontNode.outLineColor == node->fontNode.outLineColor)
                {
                    outLine = true;
                }
                if(this->fontNode.isShadow == true && 
                    this->fontNode.isShadow == node->fontNode.isShadow &&
                    this->fontNode.shadowColor == node->fontNode.shadowColor &&
                    this->fontNode.shadowOffset == node->fontNode.shadowOffset)
                {
                    shadow = true;
                }
            }

            if(shadow && outLine)
            {
                texFlag = this->fontNode.texture == node->fontNode.texture;
                grayFlag = true;// this->fontNode.isGrayed == node->fontNode.isGrayed;
            }
            else
            {
                return BatchResult::Fail;
            }
        }
        if (texFlag && grayFlag)
        {
            return isIntersects ? BatchResult::Succ_Intersects : BatchResult::Succ;
        }
        else
        {
            if (isIntersects)
            {
                return BatchResult::Fail;
            }
            else
            {
                return BatchResult::Continue;
            }
        }
    }

    return BatchResult::Fail;
}

void BatchNode::AddNode(BatchNode* node)
{
    this->arrRect.push_back(node->boundsRect);
    if (this->type == FairyRenderType::Fairy)
    {
        this->arrObj.push_back(node->fairyNode.go);
    }
    else if (this->type == FairyRenderType::Font)
    {
        this->arrObj.push_back(node->fontNode.go);
    }
}

void BatchNode::SetLastNode(BatchNode* node)
{
    this->lastNode = node;
    if(node != nullptr)
    {
        node->nextNode = this;
    }
}

void BatchNode::SetNextNode(BatchNode* node)
{
    this->nextNode = node;
    if(node != nullptr)
    {
        node->lastNode = this;
    }
}


BatchNode* BatchNode::GetRootNode()
{
    auto root = this;
    while(root->lastNode != nullptr)
    {
        root = root->lastNode;
    }
    return root;
}

void BatchNode::InitRect()
{
    if(this->type == FairyRenderType::Fairy)
    {
        this->boundsRect = fairyNode.obbRect;
    }
    else if (this->type == FairyRenderType::Font)
    {
        this->boundsRect = fontNode.obbRect;
    }
    else if(this->type == FairyRenderType::Mask)
    {
        Rect rect = maskNode.rect;
        this->boundsRect = OBBRect(rect.getMidX(), rect.getMidY(), rect.size.width, rect.size.height);
    }
    else if(this->type == FairyRenderType::Shape)
    {
        this->boundsRect = shapeNode.obbRect;
    }
    else
    {
        Rect rect = extensionObject->localToGlobal(Rect(Vec2::ZERO, extensionObject->getSize()));
        rect.size = rect.size * RenderDataProcess::Zoom;
        rect.origin = rect.origin * RenderDataProcess::Zoom;
        this->boundsRect = OBBRect(rect.getMidX(), rect.getMidY(), rect.size.width, rect.size.height);
    }
    this->arrRect.push_back(this->boundsRect);
}

BatchNode* BatchNode::CheckInsertNode(BatchNode* checkNode, BatchNode* insertNode)
{
    insertNode->InitRect();

    if (checkNode == nullptr) return insertNode;

    if (insertNode->type != FairyRenderType::Fairy && insertNode->type != FairyRenderType::Font)
    {
        insertNode->SetLastNode(checkNode);
        return insertNode;
    }

    BatchNode* lastSucNode = nullptr;
    BatchNode* tmpNode = checkNode;
    while(tmpNode)
    {
        BatchResult ret = tmpNode->CanBatch(insertNode);
        if(ret == BatchResult::Succ)
        {
            lastSucNode = tmpNode;
            tmpNode = tmpNode->lastNode;
        }
        else if(ret == BatchResult::Succ_Intersects)
        {
            lastSucNode = tmpNode;
            break;
        }
        else if(ret == BatchResult::Continue)
        {
            tmpNode = tmpNode->lastNode;
        }
        else
        {
            break;
        }
    }

    if (lastSucNode)
    {
        lastSucNode->AddNode(insertNode);
        delete insertNode;
    }
    else
    {
        insertNode->SetLastNode(checkNode);
        return insertNode;
    }

    return checkNode;
}

void BatchNode::RefresDisplayObjs(BatchNode* topNode)
{
    if (topNode == nullptr) return;

    AZ::s16 curOrderInLayer = 0;
    auto node = topNode->GetRootNode();
    while (node != nullptr)
    {
        if (node->type == FairyRenderType::Fairy)
        {
            DisplayObjRenderContext context;
            context.type = FairyRenderType::Fairy;
            context.texture = node->fairyNode.texture;
            context.isGrayed = node->fairyNode.isGrayed;
            context.maskIndex = node->fairyNode.m_maskIndex;
            context.orderInLayer = curOrderInLayer;
            for (auto go : node->arrObj)
            {
                go->GetDisplayObject()->RefreshNgraphicsData(context);
                // RenderDataProcess::CaculateRenderData(go, RenderDataProcess::GetSpriteFrame(go), meshContext, currentCount);
            }
        }
        else if (node->type == FairyRenderType::Font)
        {
            DisplayObjRenderContext context;
            context.type = FairyRenderType::Font;
            context.texture = node->fontNode.texture;
            context.isOutLine = node->fontNode.isOutLine;
            context.isShadow = node->fontNode.isShadow;
            if (context.isOutLine)
            {
                context.outLineColor = node->fontNode.outLineColor;
                context.outLineWidth = node->fontNode.outLineWidth;
            }
            if (context.isShadow)
            {
                context.shadowOffset = node->fontNode.shadowOffset;
                context.shadowColor = node->fontNode.shadowColor;
            }
            context.maskIndex = node->fontNode.m_maskIndex;
            context.orderInLayer = curOrderInLayer;
            for (auto go : node->arrObj)
            {
                go->GetDisplayObject()->RefreshNgraphicsData(context);
                // RenderDataProcess::CaculateRenderData(go, RenderDataProcess::GetSpriteFrame(go), meshContext, currentCount);
            }
        }
        else if (node->type == FairyRenderType::Mask)
        {
            if (node->maskNode.index != 0)
            {
                DisplayObjRenderContext context;
                context.type = FairyRenderType::Mask;
                context.maskIndex = node->maskNode.index;
                context.orderInLayer = curOrderInLayer;
                node->maskNode.go->GetDisplayObject()->RefreshNgraphicsData(context);
            }
        }
        else if (node->type == FairyRenderType::Shape)
        {
            DisplayObjRenderContext context;
            context.type = FairyRenderType::Shape;
            context.maskIndex = node->shapeNode.m_maskIndex;
            context.orderInLayer = curOrderInLayer;
            node->shapeNode.go->GetDisplayObject()->RefreshNgraphicsData(context);
        }
        else
        {
            // RenderQueueObject rqo(node->type, node->extensionObject);
            // renderContext.push_back(rqo);
        }

        node = node->nextNode;
        curOrderInLayer++;
    }

    node = topNode->GetRootNode();
    while (node != nullptr)
    {
        auto tmp = node->nextNode;
        delete node;
        node = tmp;
    }
}

void BatchNode::OutputVertexAndIndices(BatchNode* topNode, MeshContext& meshContext, vector<RenderQueueObject>& renderContext)
{
    if (topNode == nullptr) return;

    int start = 0;
    int currentCount = 0;
    auto node = topNode->GetRootNode();
    while(node != nullptr)
    {
        if(node->type == FairyRenderType::Fairy)
        {
            for(auto go : node->arrObj)
            {
                RenderDataProcess::CaculateRenderData(go, RenderDataProcess::GetSpriteFrame(go), meshContext, currentCount);
            }
            RenderContext rc;
            rc.texture = node->fairyNode.texture;
            rc.startIndex = start;
            rc.numIndex = currentCount;
            rc.baseVertex = meshContext.vertexCount;
            rc.isGrayed = node->fairyNode.isGrayed;
            rc.maskIndex = node->fairyNode.m_maskIndex;
            RenderQueueObject rqo(FairyRenderType::Fairy, rc);
            renderContext.push_back(rqo);

            start += currentCount;
            currentCount = 0;
        }
        else if (node->type == FairyRenderType::Font)
        {
            for (auto go : node->arrObj)
            {
                RenderDataProcess::CaculateRenderData(go, RenderDataProcess::GetSpriteFrame(go), meshContext, currentCount);
            }
            FontContext fc;
            fc.texture = node->fontNode.texture;
            fc.startIndex = start;
            fc.numIndex = currentCount;
            fc.baseVertex = meshContext.vertexCount;
            fc.isOutLine = node->fontNode.isOutLine;
            fc.isShadow = node->fontNode.isShadow;
            if(fc.isOutLine)
            {
                fc.outLineColor = node->fontNode.outLineColor;
                fc.outLineWidth = node->fontNode.outLineWidth;
            }
            if(fc.isShadow)
            {
                fc.shadowOffset = node->fontNode.shadowOffset;
                fc.shadowColor = node->fontNode.shadowColor;
            }
            fc.maskIndex = node->fontNode.m_maskIndex;
            RenderQueueObject rqo(FairyRenderType::Font, fc);
            renderContext.push_back(rqo);

            start += currentCount;
            currentCount = 0;
        }
        else if (node->type == FairyRenderType::Mask)
        {
            if (node->maskNode.index != 0)
            {
                MaskContext mc;
                mc.applyMask = node->maskNode.index;
                RenderDataProcess::CaculateMaskRect(node->maskNode.go, meshContext, currentCount);

                mc.startIndex = start;
                mc.numIndex = currentCount;
                mc.baseVertex = meshContext.vertexCount;

                RenderQueueObject rqo(FairyRenderType::Mask, mc);
                renderContext.push_back(rqo);
                start += currentCount;
                currentCount = 0;
            }
        }
        else if (node->type == FairyRenderType::Shape)
        {
            ShapeContext sc;
            RenderDataProcess::CaculateShapeRect(node->shapeNode.go, meshContext, currentCount);
            sc.startIndex = start;
            sc.numIndex = currentCount;
            sc.baseVertex = meshContext.vertexCount;
            sc.maskIndex = node->shapeNode.m_maskIndex;
            if (currentCount != 0)
            {
                RenderQueueObject rqo(FairyRenderType::Shape, sc);
                renderContext.push_back(rqo);
            }
            start += currentCount;
            currentCount = 0;
        }
        else
        {
            RenderQueueObject rqo(node->type, node->extensionObject);
            renderContext.push_back(rqo);
        }

        node = node->nextNode;
    }

    node = topNode->GetRootNode();
    while(node != nullptr)
    {
        auto tmp = node->nextNode;
        delete node;
        node = tmp;
    }
}
NS_FGUI_END
