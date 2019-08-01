#pragma once
#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/fairytypes.h"
#include "FairyGUI/fairy/base/Base.h"
#include "FairyGUI/fairy/render/OBBRect.h"
#include "FairyGUI/fairy/ui/FieldTypes.h"
#include "FairyGUI/fairy/display/Texture2D.h"

NS_FGUI_BEGIN

class GObject;

class MeshContext
{
public:
    struct Vertex
    {
        float x, y, u, v;
        float r, g, b, a;
    };

    MeshContext()
    {
        static const int vertexCnt = 1 << 14;
        vertexCount = 0;
        indexCount = 0;
        vt = new Vertex[vertexCnt * 4];
        indices = new uint16_t[vertexCnt * 6];
    }
    ~MeshContext()
    {
        delete[] vt;
        delete[] indices;
    }
    void clear()
    {
        vertexCount = 0;
        indexCount = 0;
    }

    Vertex* vt;
    int vertexCount;
    uint16_t* indices;
    int indexCount;
};

struct RenderNode
{
    Rect renderSize;
    Size sourceSize;
    Color4B color;
    Rect rect;
    Rect scale9grid;
    bool isScale9Grid;
    bool isClipping;
    FlipType flipType;
    FillMethod fillMethod;
    float amount;
    int origin; 
    bool clockwise;
    bool isTile;

    Texture2D* texture;
};

struct FairyNode
{
    ~FairyNode() {}
    OBBRect obbRect = OBBRect::ZERO;
    GObject* go = nullptr;
    Texture2D* texture = nullptr;
    bool isGrayed = false;
    int m_maskIndex = 0;
};

struct ShapeNode
{
    ~ShapeNode() {}
    OBBRect obbRect = OBBRect::ZERO;
    GObject* go = nullptr;
    
    bool isGrayed = false;
    int m_maskIndex = 0;
};

struct FontNode
{
    ~FontNode() {}
    OBBRect obbRect = OBBRect::ZERO;
    Color4B outLineColor = Color4B::WHITE;
    Color4B shadowColor = Color4B::WHITE;
    Vec2 shadowOffset = Vec2::ZERO;
    GObject* go = nullptr;
    Texture2D* texture = nullptr;
    int outLineWidth = 1;
    bool isOutLine = false;
    bool isShadow = false;
    int m_maskIndex = 0;
};

struct MaskNode
{
    GObject* go = nullptr;
    MaskNode* parent = nullptr;
    Rect rect = Rect::ZERO;
    int index= 0;
};

struct RenderContext
{
    int baseVertex = 0;
    int startIndex = 0;
    int numIndex = 0;
    Texture2D* texture = nullptr;

    bool isGrayed = false;
    int maskIndex = 0;
};

struct ShapeContext
{
    int baseVertex = 0;
    int startIndex = 0;
    int numIndex = 0;

    bool isGrayed = false;
    int maskIndex = 0;
};

struct FontContext
{
    Color4B outLineColor = Color4B::WHITE;
    Color4B shadowColor = Color4B::WHITE;
    Vec2 shadowOffset = Vec2::ZERO;
    int outLineWidth = 1;

    int baseVertex = 0;
    int startIndex = 0;
    int numIndex = 0;
    Texture2D* texture = nullptr;

    bool isOutLine = false;
    bool isShadow = false;

    int maskIndex = 0;
};

struct MaskContext
{
    int applyMask;
    int baseVertex = 0;
    int startIndex = 0;
    int numIndex = 0;
};

struct RenderQueueObject
{
    RenderQueueObject(FairyRenderType t, RenderContext rc);
    RenderQueueObject(FairyRenderType t, FontContext fc);
    RenderQueueObject(FairyRenderType t, GObject* go);
    RenderQueueObject(FairyRenderType t, ShapeContext sc);
    RenderQueueObject(FairyRenderType t, MaskContext mask);
    ~RenderQueueObject();
    RenderQueueObject(const RenderQueueObject& rhv);

    RenderQueueObject &operator =(const RenderQueueObject &);

    FairyRenderType type;
    union
    {
        RenderContext fairyContext;
        FontContext fontContext;
        MaskContext maskContext;
        ShapeContext shapeContext;
        GObject* extensionObject;
    };
};


struct DisplayObjRenderContext
{
    FairyRenderType type = FairyRenderType::Fairy;
    
    Texture2D* texture = nullptr;
    bool isGrayed = false;
    int maskIndex = 0;

    Color4B outLineColor = Color4B::WHITE;
    Color4B shadowColor = Color4B::WHITE;
    Vec2 shadowOffset = Vec2::ZERO;
    int outLineWidth = 1;
    bool isOutLine = false;
    bool isShadow = false;

    AZ::s16 orderInLayer = 0;
};
//----------------------------------------------------------------------

NS_FGUI_END