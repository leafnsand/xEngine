#pragma once
#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/fairytypes.h"
#include "FairyGUI/fairy/FairyGUI.h"
#include "FairyGUI/fairy/render/RenderTypes.h"

NS_FGUI_BEGIN
class BatchNode
{
public:
    enum BatchResult
    {
        Succ,
        Succ_Intersects,
        Continue,
        Fail,
    };

    BatchNode(FairyRenderType type, FairyNode node);
    BatchNode(FairyRenderType type, FontNode node);
    BatchNode(FairyRenderType type, MaskNode node);
    BatchNode(FairyRenderType type, ShapeNode node);
    BatchNode(FairyRenderType type, GObject* node);
    ~BatchNode(){}

    static BatchNode* CheckInsertNode(BatchNode* checkNode, BatchNode* insertNode);
    static void OutputVertexAndIndices(BatchNode* topNode, MeshContext& meshContext, vector<RenderQueueObject>& renderContext);
    static void RefresDisplayObjs(BatchNode* topNode);

    void AddNode(BatchNode* node);
    void SetLastNode(BatchNode* node);
    void SetNextNode(BatchNode* node);
    BatchNode* GetRootNode();

    BatchResult CanBatch(BatchNode* node);

    void InitRect();

    BatchNode* lastNode;
    BatchNode* nextNode;
    FairyRenderType type;
    OBBRect boundsRect;
    vector<OBBRect> arrRect;
    vector<GObject*> arrObj;
    union
    {
        FairyNode fairyNode;
        FontNode fontNode;
        MaskNode maskNode;
        ShapeNode shapeNode;
        GObject* extensionObject;
    };
};

NS_FGUI_END
