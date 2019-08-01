#pragma once
#include "FairyGUI/fairy/FairyGUI.h"
#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/render/RenderTypes.h"

NS_FGUI_BEGIN
struct FontLetterDefinition;

class RenderDataProcess
{
public:
    static void SetDefaultSize(int width, int height);
    static void ResetZoom(int width, int height);

    static bool IsNeedRender(GObject* go);

    static SpriteFrame* GetSpriteFrame(GObject* go);

    static void CaculateRenderData(GObject* go, SpriteFrame* pSpriteFrame, MeshContext& meshContext, int& indexCount);

    static void CaculateMaskRect(GObject* go,  MeshContext& meshContext, int& indexCount);

    static void CaculateShapeRect(GObject* go,  MeshContext& meshContext, int& indexCount);

    static OBBRect CaculateRenderRect(GObject* go);

    static void GetFontEffect(FontNode& fn);

    static bool IsTileRender(GObject* go);

    static bool IsTextAutoSize(GObject* go);

    static float Zoom;
    static int Width;
    static int Height;
    static int DefaultWidth;
    static int DefaultHeight;
    static Vec2 OffSet;
    static AZ::Transform ProjectionMatrix;
private:
    static void DoNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void DoMaskNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void DoShapeNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount,int lineSize, Color4B linecolor);
    static void DoShapeBorderNormal(Rect& rect, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount, Color4B linecolor);
    static void DoShapeCircleNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount, int lineSize, Color4B linecolor);
    static void DoShapeCircleBorderNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount, int lineSize, Color4B linecolor);
    static void DoScale9Grid(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void DoTile(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void DoFillMethod(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void SetVertexAndIndex(MeshContext& meshContext, Vec2 *pos, Vec2 *uv, Color4B color);
    static void FillHorizontal(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void FillVertical(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void FillRadial90(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void FillRadial180(RenderNode& renderNode, Origin180 fillType, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void FillRadial360(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);

    static void FillRadia90Test(RenderNode& renderNode, Origin90 FillType,Rect offset, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);

    static void DoNormalFont(GObject* go, SpriteFrame* pSpriteFrame, MeshContext& meshContext, int& indexCount);
    static void DoFonsFont(GObject* go, TextFormat* textFormat, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static void DoBitmapFont(FontLetterDefinition& fontDef, Vec2& beginPos, TextFormat* textFormat, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount);
    static Size CaculateTextFormat(PackageItem* item, AZStd::string string, TextFormat* textFormat);

    static void GetPosAndUV(const AZ::Transform& matrix, float minX, float maxX, float minY, float maxY, float u1, float v1, float u2, float v2, Vec2 *pos, Vec2 *uv);
};


NS_FGUI_END