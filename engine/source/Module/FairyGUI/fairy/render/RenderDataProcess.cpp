#include "RenderDataProcess.h"
#include "display/SpriteFrame.h"
#include "display/Texture2D.h"
#include "display/BitMapFont.h"
#include "display/FonsFont.h"
#include "display/Animation.h"
#include "FGuiCentral.h"
#include "utils/TextParser.h"
#include <AzCore/std/containers/vector.h>
#include <bx/math.h>
#include "EBus/FairySystemComponentBus.h"


using namespace AZStd;
using namespace AZ;

NS_FGUI_BEGIN

int RenderDataProcess::DefaultWidth = 800;
int RenderDataProcess::DefaultHeight = 600;
int RenderDataProcess::Width = -1;
int RenderDataProcess::Height =-1;
float RenderDataProcess::Zoom = 1.0f;

static const float PI = 3.1415926f;

Vec2 RenderDataProcess::OffSet; 
AZ::Transform RenderDataProcess::ProjectionMatrix;

void RenderDataProcess::SetDefaultSize(int width, int height)
{
    DefaultWidth = width;
    DefaultHeight = height;
    ResetZoom(width, height);
}

void RenderDataProcess::ResetZoom(int width, int height)
{
    if (Width == width && Height == height) return;
    Width = width;
    Height = height;

    float zoomX = (float)Width / (float)DefaultWidth;
    float zoomY = (float)Height / (float)DefaultHeight;
    Zoom = zoomX > zoomY ? zoomY : zoomX;

    OffSet = Vec2::ZERO;
    if (zoomX > zoomY)
    {
        OffSet.x = (float)DefaultWidth * (zoomX - Zoom) / 2 / Zoom;
    }
    else
    {  
        OffSet.y = (float)DefaultHeight * (zoomY - Zoom) / 2 / Zoom;
    }

    // float proj[16];
    // float scaleX = zoomX * 10 / DefaultHeight;
    // float scaleY = zoomY * 10 / DefaultHeight;
    
    // bx::mtxOrtho(proj, -RenderDataProcess::OffSet.x, float(RenderDataProcess::Width) / RenderDataProcess::Zoom - RenderDataProcess::OffSet.x, float(RenderDataProcess::Height) / RenderDataProcess::Zoom - RenderDataProcess::OffSet.y, -RenderDataProcess::OffSet.y, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
    // ProjectionMatrix = AZ::Transform::CreateFromRows(Vector4(proj[0], proj[4], proj[8], proj[12]), Vector4(proj[1], proj[5], proj[9], proj[13]), Vector4(proj[2], proj[6], proj[10], proj[14]));
    // ProjectionMatrix = AZ::Transform::CreateFromRows(Vector4(1.0f / 100 * 5.0f / 4.0f , 0, 0, -(float)DefaultWidth / 200.0f * 5.0f / 4.0f), Vector4(0, 1.0f / 100 * 5.0f /4.0f, 0, -(float)DefaultHeight / 200.0f * 5.0f /4.0f), Vector4(0, 0, 1, 0));
    // EBUS_EVENT(Module::FairySystemRequestBus, ChangeStageCameraPos, (float)DefaultWidth / 200.0f, (float)DefaultHeight / 200.0f);
}

bool RenderDataProcess::IsNeedRender(GObject* go)
{
    GLoader* p1 = go->SafeCast<GLoader>();
    GImage* p2 = go->SafeCast<GImage>();
    GTextField* p3 = go->SafeCast<GTextField>();
    GMovieClip* p4 = go->SafeCast<GMovieClip>();   
    
    return p1 || p2 || p3 || p4;
}

SpriteFrame* RenderDataProcess::GetSpriteFrame(GObject* go)
{
    GLoader* ploader = go->SafeCast<GLoader>();
    if(ploader && ploader->getSpriteFrame())
    {
        return ploader->getSpriteFrame();
    }

    GMovieClip* pmovieclip = go->SafeCast<GMovieClip>();
    if (pmovieclip)
    {
        PackageItem* item = go->getPackageItem();
        if(item)
        {
            int frame = pmovieclip->getCurrentFrame();
            if(frame < item->animation->_frames.size())
            {
                auto animF = item->animation->_frames[frame];
                if (animF)
                {
                    return animF->_spriteframe;
                }
            }
        }
    }

    GTextField* ptext = go->SafeCast<GTextField>();
    if(ptext)
    {
        PackageItem* item = UIPackage::getItemByURL(ptext->getTextFormat()->face);
        if(item)
        {
            return item->refSpriteFrame;
        }
        else
        {
            return FonsFont::getInstance()->GetTextSpriteFrame(ptext->getTextFormat()->face);
        }
    }

    PackageItem* item = go->getPackageItem();
    if(item)
    {
        return item->spriteFrame;
    }
    return nullptr;
}

OBBRect RenderDataProcess::CaculateRenderRect(GObject* go)
{
    Rect rect;
    rect.size = go->getSize();
    rect.origin = Vec2::ZERO;
    if (GLoader* ploader = go->SafeCast<GLoader>())
    {
        rect = ploader->getRenderSize();
    }
    if (GTextField* ptext = go->SafeCast<GTextField>())
    {
        AZStd::string str = ptext->getText();
        PackageItem* item = UIPackage::getItemByURL(ptext->getTextFormat()->face);
        if(item)
            rect.size = CaculateTextFormat(item, str, ptext->getTextFormat());
    }
    if (GMovieClip* pmovieclip = go->SafeCast<GMovieClip>())
    {
        auto pSpriteFrame = RenderDataProcess::GetSpriteFrame(go);
        if(pSpriteFrame)
            rect = pSpriteFrame->getRectInPixels();
    }
    rect = go->localToGlobal(rect);
    rect.size = rect.size * RenderDataProcess::Zoom;
    rect.origin = rect.origin * RenderDataProcess::Zoom;

    float rotation = go->getRotation();
    float scaleX = go->getScaleX();
    float scaleY = go->getScaleY();
    GComponent* pParent = go->getParent();
    while (pParent)
    {
        rotation += pParent->getRotation();
        scaleX *= pParent->getScaleX();
        scaleY *= pParent->getScaleY();
        pParent = pParent->getParent();
    }

    OBBRect obbRect = OBBRect(rect.getMidX(), rect.getMidY(), rect.size.width * scaleX, rect.size.height * scaleY, rotation * N_PI / 180.0f);
    return obbRect;
}

bool RenderDataProcess::IsTileRender(GObject* go)
{
    PackageItem* item = go->getPackageItem();
    if(item)
    {
        return item->scaleByTile;
    }

    return false;
}

bool RenderDataProcess::IsTextAutoSize(GObject* go)
{
    if(GBasicTextField* ptext = go->SafeCast<GBasicTextField>())
    {
        if(ptext->getAutoSize() == TextAutoSize::NONE)
        {
            TextParser tp = ptext->GetTextParser();
            Size allSize = tp.GetAllSize();
            Size componentSize = go->getSize();
            if (allSize.width <= componentSize.width && allSize.height <= componentSize.height)
                return true;

            return false;
        }
    }
    return true;
}

void RenderDataProcess::CaculateRenderData(GObject* go, SpriteFrame* pSpriteFrame, MeshContext& meshContext, int& indexCount)
{
    if(go->SafeCast<GTextField>())
    {
        DoNormalFont(go, pSpriteFrame, meshContext, indexCount);
        return;
    }

    if (!pSpriteFrame) return;

    PackageItem* item = nullptr;
    bool isScale9Grid = false;
    Rect rectScale9Grid;

    RenderNode rn;
    rn.renderSize.size = go->getSize();
    rn.sourceSize = go->sourceSize;
    rn.texture = pSpriteFrame->texture;
    rn.rect = pSpriteFrame->_rect;
    rn.isClipping = false;
    rn.flipType = FlipType::NONE;
    rn.fillMethod = FillMethod::NONE;
    rn.renderSize.origin = Vec2::ZERO;
    rn.isTile = RenderDataProcess::IsTileRender(go);
    
    GLoader* ploader = go->SafeCast<GLoader>();
    if (ploader)
    {
        rn.renderSize.size = ploader->getContentSize();
        rn.color = ploader->getColor();

        item = ploader->getPackageItem();
        if (item)
        {
            if (item->type == PackageItemType::IMAGE)
            {
                if (item->scale9Grid)
                {
                    rectScale9Grid = ploader->getScale9GridRect();
                    isScale9Grid = true;
                }
                rn.fillMethod = ploader->getFillMethod();
                if (rn.fillMethod != FillMethod::NONE)
                {
                    rn.amount = ploader->getfillAmount();
                    rn.origin = ploader->getfillOrigin();
                    rn.clockwise = ploader->getfillClockwise();
                }
            }
            else if(item->type == PackageItemType::MOVIECLIP)
            {
                rn.renderSize = ploader->getRenderSize();
            }
        }
    }

    GImage* pimage = go->SafeCast<GImage>();
    if (pimage)
    {
        rn.color = pimage->getColor();

        item = pimage->getPackageItem();
        if (item)
        {
            if(item->scale9Grid)
            {
                rectScale9Grid = pimage->getScale9GridRect();
                isScale9Grid = true;
            }
        }
        rn.flipType = pimage->getFlip();
        rn.fillMethod = pimage->getFillMethod();
        if (rn.fillMethod!= FillMethod::NONE)
        {
            rn.amount = pimage->getfillAmount();
            rn.origin = pimage->getfillOrigin();
            rn.clockwise = pimage->getfillClockwise();
        }
    }

    GMovieClip* pmovieclip = go->SafeCast<GMovieClip>();
    if (pmovieclip)
    {
        rn.color = pmovieclip->getColor();

        item = pmovieclip->getPackageItem();
        rn.flipType = pmovieclip->getFlip();
        Size curSize = go->getSize();
        rn.renderSize = pSpriteFrame->getRectInPixels();
        Size sourceSize = pSpriteFrame->_size;
        float wValue = curSize.width / sourceSize.width;
        float hValue = curSize.height / sourceSize.height;

        rn.renderSize.origin.x *= wValue; 
        rn.renderSize.origin.y *= hValue; 
        rn.renderSize.size.width *= wValue; 
        rn.renderSize.size.height *= hValue; 
    }

    rn.color.a = go->getAlpha() * 255.0f;
    rn.scale9grid = rectScale9Grid;
    rn.isScale9Grid = isScale9Grid;

    if (rn.fillMethod != FillMethod::NONE)
    {
        RenderDataProcess::DoFillMethod(rn, go->GetGlobalMatrix44(), meshContext, indexCount);
        return;
    }

    if (isScale9Grid)
    {
        RenderDataProcess::DoScale9Grid(rn, go->GetGlobalMatrix44(), meshContext, indexCount);
    }
    else
    {
        if (rn.isTile)
        {
            RenderDataProcess::DoTile(rn, go->GetGlobalMatrix44(), meshContext, indexCount);
        }
        else
        {
            RenderDataProcess::DoNormal(rn, go->GetGlobalMatrix44(), meshContext, indexCount);
        }
    }
}

void RenderDataProcess::DoFillMethod(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    float amount = bx::clamp(renderNode.amount, 0.0f, 1.0f);
    switch (renderNode.fillMethod)
    {
    case FillMethod::HORIZONTAL:
        RenderDataProcess::FillHorizontal(renderNode, amount, matrix, meshContext, indexCount);
        break;
    case FillMethod::VERTICAL:
        RenderDataProcess::FillVertical(renderNode, amount, matrix, meshContext, indexCount);
        break;
    case FillMethod::RADIAL90:
        RenderDataProcess::FillRadial90(renderNode, amount, matrix, meshContext, indexCount);
        break;
    case FillMethod::RADIAL180:
        RenderDataProcess::FillRadial180(renderNode, (Origin180)renderNode.origin, amount, matrix, meshContext, indexCount);
        break;
    case FillMethod::RADIAL360:
        RenderDataProcess::FillRadial360(renderNode, amount, matrix, meshContext, indexCount);
        break;
    }

}

void RenderDataProcess::FillHorizontal(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    float u1 = renderNode.rect.getMinX() / (float)renderNode.texture->getWidth();
    float u2 = renderNode.rect.getMaxX() / (float)renderNode.texture->getWidth();
    float v1 = renderNode.rect.getMinY() / (float)renderNode.texture->getHeight();
    float v2 = renderNode.rect.getMaxY() / (float)renderNode.texture->getHeight();
    float minX = renderNode.renderSize.origin.x;
    float minY = renderNode.renderSize.origin.y; 
    float maxX = renderNode.renderSize.origin.x + renderNode.renderSize.size.width;
    float maxY = renderNode.renderSize.origin.y + renderNode.renderSize.size.height;
    if ((OriginHorizontal)renderNode.fillMethod == OriginHorizontal::Left)
    {
        u2 = (u2 - u1) * amount + u1;
        maxX = (maxX - minX) * amount + minX;
    }
    else
    {
        u1 = (u2 - u1) * (1.0f - amount) + u1;
        minX = (maxX - minX) * (1.0f - amount) + minX;
    }
    Vec2 pos[4] = {}, uv[4] = {};
    GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);
    SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
    indexCount += 6;
}

void RenderDataProcess::FillVertical(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    float u1 = renderNode.rect.getMinX() / (float)renderNode.texture->getWidth();
    float u2 = renderNode.rect.getMaxX() / (float)renderNode.texture->getWidth();
    float v1 = renderNode.rect.getMinY() / (float)renderNode.texture->getHeight();
    float v2 = renderNode.rect.getMaxY() / (float)renderNode.texture->getHeight();
    float minX = renderNode.renderSize.origin.x;
    float minY = renderNode.renderSize.origin.y;
    float maxX = renderNode.renderSize.origin.x + renderNode.renderSize.size.width;
    float maxY = renderNode.renderSize.origin.y + renderNode.renderSize.size.height;
    if ((OriginVertical)renderNode.fillMethod == OriginVertical::Bottom)
    {
        v2 = (v2 - v1) * amount + v1;
        maxY = (maxY - minY) * amount + minY;
    }
    else
    {
        v1 = (v2 - v1) * (1.0f - amount) + v1;
        minY = (maxY - minY) * (1.0f - amount) + minY;
    }
    Vec2 pos[4] = {}, uv[4] = {};
    GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);
    SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
    indexCount += 6;
}

void RenderDataProcess::FillRadial90(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    Rect t(0,0,1,1);
    RenderDataProcess::FillRadia90Test(renderNode, (Origin90)renderNode.origin, t, amount, matrix, meshContext, indexCount);
}

void RenderDataProcess::FillRadia90Test(RenderNode& renderNode, Origin90 FillType, Rect offset, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    if (amount < 0.001f)
    {
        // todo... maybe need create empty rect
        return;
    }
    if (amount > 0.999f)
    {
        float minU = renderNode.rect.getMinX() / (float)renderNode.texture->getWidth();
        float maxU = renderNode.rect.getMaxX() / (float)renderNode.texture->getWidth();
        float minV = renderNode.rect.getMinY() / (float)renderNode.texture->getHeight();
        float maxV = renderNode.rect.getMaxY() / (float)renderNode.texture->getHeight();

        // float minX = renderNode.renderSize.origin.x, minY = renderNode.renderSize.origin.y, maxX = renderNode.renderSize.origin.x + renderNode.renderSize.size.width, maxY = renderNode.renderSize.origin.y + renderNode.renderSize.size.height;

        float u1 = minU + (maxU - minU) * offset.origin.x;
        float u2 = u1 + (maxU - minU) * offset.size.width;
        float v1 = minV + (maxV - minV) * offset.origin.y;
        float v2 = v1 + (maxV - minV) * offset.size.height;

        float x1 = renderNode.renderSize.origin.x + offset.origin.x * renderNode.renderSize.size.width;
        float x2 = x1 + offset.size.width * renderNode.renderSize.size.width;
        float y1 = renderNode.renderSize.origin.y + offset.origin.y * renderNode.renderSize.size.height;
        float y2 = y1 + offset.size.height * renderNode.renderSize.size.height;

        

        Vec2 pos[4] = {}, uv[4] = {};

        GetPosAndUV(matrix, x1, x2, y1, y2, u1, v1, u2, v2, pos, uv);

        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);

        indexCount += 6;
        return;
    }
    float minU = renderNode.rect.getMinX() / (float)renderNode.texture->getWidth();
    float maxU = renderNode.rect.getMaxX() / (float)renderNode.texture->getWidth();
    float minV = renderNode.rect.getMinY() / (float)renderNode.texture->getHeight();
    float maxV = renderNode.rect.getMaxY() / (float)renderNode.texture->getHeight();

    float minX = renderNode.renderSize.origin.x;
    float minY = renderNode.renderSize.origin.y;
    float maxX = renderNode.renderSize.origin.x + renderNode.renderSize.size.width;
    float maxY = renderNode.renderSize.origin.y + renderNode.renderSize.size.height;

    float u1 = minU + (maxU - minU) * offset.origin.x;
    float u2 = u1 + (maxU - minU) * offset.size.width;
    float v1 = minV + (maxV - minV) * offset.origin.y;
    float v2 = v1 + (maxV - minV) * offset.size.height;

    float x1 = renderNode.renderSize.origin.x + offset.origin.x * renderNode.renderSize.size.width;
    float x2 = x1 + offset.size.width * renderNode.renderSize.size.width;
    float y1 = renderNode.renderSize.origin.y + offset.origin.y * renderNode.renderSize.size.height;
    float y2 = y1 + offset.size.height * renderNode.renderSize.size.height;

    Vec2 pointPos[4] = {
        Vec2(x1, y1),
        Vec2(x2, y1),
        Vec2(x2, y2),
        Vec2(x1, y2),
    };
    Vec2 uv[4] = {
        Vec2(u1, v2),
        Vec2(u2, v2),
        Vec2(u2, v1),
        Vec2(u1, v1),
    };


    switch (FillType)
    {
    case Origin90::BottomLeft:
        if (renderNode.clockwise)
        {
            float v = bx::tan(PI / 2 * (1.0 - amount));
            float h = renderNode.renderSize.size.width * offset.size.width * v;
            if (h > renderNode.renderSize.size.height * offset.size.height)
            {
                float ratio = (h - renderNode.renderSize.size.height * offset.size.height) / h;
                pointPos[1].x -= renderNode.renderSize.size.width * offset.size.width * ratio;
                pointPos[2] = pointPos[3];
                uv[2].x -= (u2 - u1) * ratio;
                uv[1] = uv[2];
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[2].y -= h;
                uv[1].y -= (v2 - v1) * ratio;
            }
        }
        else
        {
            float v = bx::tan(PI / 2 * amount);
            float h = renderNode.renderSize.size.width * offset.size.width * v;
            if (h > renderNode.renderSize.size.height * offset.size.height)
            {
                float ratio = (h - renderNode.renderSize.size.height * offset.size.height) / h;
                pointPos[0].x += renderNode.renderSize.size.width * offset.size.width * (1.0f - ratio);
                uv[3].x += (u2 - u1) * (1.0f - ratio);
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[1].y += renderNode.renderSize.size.height * offset.size.height * (1.0f - ratio);
                pointPos[0] = pointPos[1];

                uv[2].y += (v2 - v1) *(1.0f - ratio);
                uv[3] = uv[2];
            }
        }
        break;
    case Origin90::BottomRight:
        if (renderNode.clockwise)
        {
            float v = bx::tan(PI / 2 * amount);
            float h = (renderNode.renderSize.size.width * offset.size.width) * v;
            if (h > (renderNode.renderSize.size.height * offset.size.height))
            {
                float ratio = (h - (renderNode.renderSize.size.height * offset.size.height)) / h;
                pointPos[1].x -= (renderNode.renderSize.size.width * offset.size.width) * (1 - ratio);
                uv[2].x -= (u2 - u1) * (1 - ratio);
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[0].y += (renderNode.renderSize.size.height * offset.size.height) * (1 - ratio);
                pointPos[1] = pointPos[0];

                uv[3].y += (v2 - v1) * (1 - ratio);
                uv[2] = uv[3];
            }
        }
        else
        {
            float v = bx::tan(PI / 2 * (1.0 - amount));
            float h = (renderNode.renderSize.size.width * offset.size.width) * v;
            if (h > (renderNode.renderSize.size.height * offset.size.height))
            {
                float ratio = (h - (renderNode.renderSize.size.height * offset.size.height)) / h;
                pointPos[0].x += (renderNode.renderSize.size.width * offset.size.width) * ratio;
                pointPos[3] = pointPos[0];

                uv[3].x += (u2 - u1) * ratio;
                uv[0] = uv[3];
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[3].y -= h;
                uv[0].y -= (v2 - v1) * ratio;
            }
        }
        break;
    case Origin90::TopLeft:
        if (renderNode.clockwise)
        {
            float v = bx::tan(PI / 2 * amount);
            float h = (renderNode.renderSize.size.width * offset.size.width) * v;
            if (h > (renderNode.renderSize.size.height * offset.size.height))
            {
                float ratio = (h - (renderNode.renderSize.size.height * offset.size.height)) / h;
                pointPos[3].x += (renderNode.renderSize.size.width * offset.size.width) * (1 - ratio);
                uv[0].x += (u2 - u1) * (1 - ratio);
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[2].y -= (renderNode.renderSize.size.height * offset.size.height) * (1 - ratio);
                pointPos[3] = pointPos[2];

                uv[1].y -= (v2 - v1) * (1 - ratio);
                uv[0] = uv[1];
            }
        }
        else
        {
            float v = bx::tan(PI / 2 * (1.0 - amount));
            float h = (renderNode.renderSize.size.width * offset.size.width) * v;
            if (h > (renderNode.renderSize.size.height * offset.size.height))
            {
                float ratio = (h - (renderNode.renderSize.size.height * offset.size.height)) / h;
                pointPos[2].x -= (renderNode.renderSize.size.width * offset.size.width) * ratio;
                pointPos[1] = pointPos[2];
                uv[1].x -= (u2 - u1) * ratio;
                uv[2] = uv[1];
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[1].y += h;
                uv[2].y += (v2 - v1) * ratio;
            }
        }
        break;
    case Origin90::TopRight:
        if (renderNode.clockwise)
        {
            float v = bx::tan(PI / 2 * (1.0 - amount));
            float h = (renderNode.renderSize.size.width * offset.size.width) * v;
            if (h > (renderNode.renderSize.size.height * offset.size.height))
            {
                float ratio = (h - (renderNode.renderSize.size.height * offset.size.height)) / h;
                pointPos[3].x += (renderNode.renderSize.size.width * offset.size.width) * ratio;
                pointPos[0] = pointPos[3];
                uv[0].x += (u2 - u1) * ratio;
                uv[3] = uv[0];
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[0].y += (renderNode.renderSize.size.height * offset.size.height) * ratio;
                uv[3].y += (v2 - v1) * ratio;
            }
        }
        else
        {
            float v = bx::tan(PI / 2 * amount);
            float h = (renderNode.renderSize.size.width * offset.size.width) * v;
            if (h > (renderNode.renderSize.size.height * offset.size.height))
            {
                float ratio = (h - (renderNode.renderSize.size.height * offset.size.height)) / h;
                pointPos[2].x -= (renderNode.renderSize.size.width * offset.size.width) * (1 - ratio);
                uv[1].x -= (u2 - u1) * (1 - ratio);
            }
            else
            {
                float ratio = h / (renderNode.renderSize.size.height * offset.size.height);
                pointPos[3].y -= (renderNode.renderSize.size.height * offset.size.height) * (1 - ratio);
                pointPos[2] = pointPos[3];
                uv[0].y -= (v2 - v1) * (1 - ratio);
                uv[1] = uv[0];
            }
        }
        break;
    }

    auto bottom_left = matrix *     AZ::Vector3(pointPos[0].x, pointPos[0].y, 0);
    auto bottom_right = matrix *    AZ::Vector3(pointPos[1].x, pointPos[1].y, 0);
    auto top_right = matrix *       AZ::Vector3(pointPos[2].x, pointPos[2].y, 0);
    auto top_left = matrix *        AZ::Vector3(pointPos[3].x, pointPos[3].y, 0);

    Vec2 pos[4] = {};

    Vec2 t_l(top_left.GetX(), top_left.GetY());
    Vec2 t_r(top_right.GetX(), top_right.GetY());
    Vec2 b_r(bottom_right.GetX(), bottom_right.GetY());
    Vec2 b_l(bottom_left.GetX(), bottom_left.GetY());

    pos[0] = t_l;
    pos[1] = t_r;
    pos[2] = b_r;
    pos[3] = b_l;

    SetVertexAndIndex(meshContext, pos, uv, renderNode.color);

    indexCount += 6;

}


void RenderDataProcess::FillRadial180(RenderNode& renderNode, Origin180 fillType, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    switch (fillType)
    {
    case Origin180::Top:
        if (amount <= 0.5f)
        {
            if (renderNode.clockwise)
            {
                Rect t(0.5f, 0.0f, 0.5f, 1.0f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t(0.0f, 0.0f, 0.5f, 1.0f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (renderNode.clockwise)
            {
                Rect t1(0.5f, 0.0f, 0.5f, 1.0f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 1.0f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.0f, 0.0f, 0.5f, 1.0f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 1.0f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, value, matrix, meshContext, indexCount);
            }
        }
        break;
    case Origin180::Bottom:
        if (amount <= 0.5f)
        {
            if (renderNode.clockwise)
            {
                Rect t(0.0f, 0.0f, 0.5f, 1.0f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t(0.5f, 0.0f, 0.5f, 1.0f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (renderNode.clockwise)
            {
                Rect t1(0.0f, 0.0f, 0.5f, 1.0f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 1.0f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.5f, 0.0f, 0.5f, 1.0f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 1.0f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, value, matrix, meshContext, indexCount);
            }
        }
        break;
    case Origin180::Left:
        if (amount <= 0.5f)
        {
            if (renderNode.clockwise)
            {
                Rect t(0.0f, 0.0f, 1.0f, 0.5f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t(0.0f, 0.5f, 1.0f, 0.5f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (renderNode.clockwise)
            {
                Rect t1(0.0f, 0.0f, 1.0f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 1.0f, 0.5f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.0f, 0.5f, 1.0f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 1.0f, 0.5f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, value, matrix, meshContext, indexCount);
            }
        }
        break;
    case Origin180::Right:
        if (amount <= 0.5f)
        {
            if (renderNode.clockwise)
            {
                Rect t(0.0f, 0.5f, 1.0f, 0.5f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t(0.0f, 0.0f, 1.0f, 0.5f);
                float value = amount * 2;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (renderNode.clockwise)
            {
                Rect t1(0.0f, 0.5f, 1.0f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 1.0f, 0.5f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.0f, 0.0f, 1.0f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 1.0f, 0.5f);
                float value = (amount - 0.5f) * 2.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, value, matrix, meshContext, indexCount);
            }
        }
        break;
    }
}

void RenderDataProcess::FillRadial360(RenderNode& renderNode, float amount, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    switch ((Origin360)renderNode.origin)
    {
    case Origin360::Top:
        if (renderNode.clockwise)
        {
            if (amount <= 0.25f)
            {
                Rect t(0.5f, 0.0f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.0f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t4, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (amount <= 0.25f)
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.5f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t4, value, matrix, meshContext, indexCount);
            }
        }
        break;
    case Origin360::Bottom:
        if (renderNode.clockwise)
        {
            if (amount <= 0.25f)
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.5f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t4, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (amount <= 0.25f)
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.0f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t4, value, matrix, meshContext, indexCount);
            }
        }
        break;
    case Origin360::Left:
        if (renderNode.clockwise)
        {
            if (amount <= 0.25f)
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.0f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t4, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (amount <= 0.25f)
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.0f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t4, value, matrix, meshContext, indexCount);
            }
        }
        break;
    case Origin360::Right:
        if (renderNode.clockwise)
        {
            if (amount <= 0.25f)
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.5f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft , t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.5f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t4, value, matrix, meshContext, indexCount);
            }
        }
        else
        {
            if (amount <= 0.25f)
            {
                Rect t1(0.5f, 0.0f, 0.5f, 0.5f);
                float value = amount * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.5f)
            {
                Rect t1(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 0.5f);
                float value = (amount - 0.25f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, value, matrix, meshContext, indexCount);
            }
            else if (amount <= 0.75f)
            {
                Rect t1(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.5f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t3, value, matrix, meshContext, indexCount);
            }
            else
            {
                Rect t1(0.5f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomLeft, t1, 1, matrix, meshContext, indexCount);
                Rect t2(0.0f, 0.0f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::BottomRight, t2, 1, matrix, meshContext, indexCount);
                Rect t3(0.0f, 0.5f, 0.5f, 0.5f);
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopRight, t3, 1, matrix, meshContext, indexCount);
                Rect t4(0.5f, 0.5f, 0.5f, 0.5f);
                float value = (amount - 0.75f) * 4.0f;
                RenderDataProcess::FillRadia90Test(renderNode, Origin90::TopLeft, t4, value, matrix, meshContext, indexCount);
            }
        }
        break;
    }
}


void RenderDataProcess::DoNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    float u1 = renderNode.rect.getMinX() / (float)renderNode.texture->getWidth();
    float u2 = renderNode.rect.getMaxX() / (float)renderNode.texture->getWidth();
    float v1 = renderNode.rect.getMinY() / (float)renderNode.texture->getHeight();
    float v2 = renderNode.rect.getMaxY() / (float)renderNode.texture->getHeight();

    float minX = renderNode.renderSize.origin.x, minY = renderNode.renderSize.origin.y, maxX = renderNode.renderSize.origin.x + renderNode.renderSize.size.width, maxY = renderNode.renderSize.origin.y + renderNode.renderSize.size.height;

    if (renderNode.flipType == FlipType::HORIZONTAL || renderNode.flipType == FlipType::BOTH)
    {
        auto tmp = minX;
        minX = maxX;
        maxX = tmp;
    }
    if (renderNode.flipType == FlipType::VERTICAL || renderNode.flipType == FlipType::BOTH)
    {
        auto tmp = minY;
        minY = maxY;
        maxY = tmp;
    }

    Vec2 pos[4] = {}, uv[4] = {};

    GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);

    SetVertexAndIndex(meshContext, pos, uv, renderNode.color);

    indexCount += 6;
}

void RenderDataProcess::DoMaskNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    float u1 = 0.0f;
    float u2 = 0.0f;
    float v1 = 0.0f;
    float v2 = 0.0f;

    float minX = renderNode.renderSize.origin.x, minY = renderNode.renderSize.origin.y, maxX = renderNode.renderSize.origin.x + renderNode.renderSize.size.width, maxY = renderNode.renderSize.origin.y + renderNode.renderSize.size.height;

    Vec2 pos[4] = {}, uv[4] = {};

    GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);

    SetVertexAndIndex(meshContext, pos, uv, renderNode.color);

    indexCount += 6;
}

void RenderDataProcess::DoShapeNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount, int lineSize, Color4B linecolor)
{
    float u1 = 0.0f;
    float u2 = 0.0f;
    float v1 = 0.0f;
    float v2 = 0.0f;
    float minX = renderNode.renderSize.origin.x, minY = renderNode.renderSize.origin.y, maxX = renderNode.renderSize.origin.x + renderNode.renderSize.size.width, maxY = renderNode.renderSize.origin.y + renderNode.renderSize.size.height;

    Vec2 pos[4] = {}, uv[4] = {};

    GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);

    SetVertexAndIndex(meshContext, pos, uv, renderNode.color);

    indexCount += 6;

    if (lineSize > 0)
    {
        Rect leftLineRect(minX - lineSize / 2, minY - lineSize / 2, lineSize, renderNode.renderSize.size.height + lineSize / 2 + lineSize / 2);
        RenderDataProcess::DoShapeBorderNormal(leftLineRect, matrix, meshContext, indexCount, linecolor);
        Rect RightLineRect(maxX - lineSize / 2- lineSize % 2, minY - lineSize / 2, lineSize, renderNode.renderSize.size.height + lineSize / 2 + lineSize / 2);
        RenderDataProcess::DoShapeBorderNormal(RightLineRect, matrix, meshContext, indexCount, linecolor);
        Rect TopLineRect(minX + lineSize / 2 + lineSize % 2, minY - lineSize / 2, renderNode.renderSize.size.width - lineSize / 2 - lineSize / 2 - lineSize % 2 - lineSize % 2, lineSize);
        RenderDataProcess::DoShapeBorderNormal(TopLineRect, matrix, meshContext, indexCount, linecolor);
        Rect BottomLineRect(minX + lineSize / 2 + lineSize % 2, maxY - lineSize / 2 - lineSize % 2, renderNode.renderSize.size.width - lineSize / 2 - lineSize / 2 - lineSize % 2 - lineSize % 2, lineSize);
        RenderDataProcess::DoShapeBorderNormal(BottomLineRect, matrix, meshContext, indexCount, linecolor);
    }
}

void RenderDataProcess::DoTile(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    float u1 = renderNode.rect.getMinX() / (float)renderNode.texture->getWidth();
    float u2 = renderNode.rect.getMaxX() / (float)renderNode.texture->getWidth();
    float v1 = renderNode.rect.getMinY() / (float)renderNode.texture->getHeight();
    float v2 = renderNode.rect.getMaxY() / (float)renderNode.texture->getHeight();

    Vec2 pos[4] = {}, uv[4] = {};

    int modValue = (int)renderNode.renderSize.size.width % (int)renderNode.sourceSize.width;
    int widthCount = renderNode.renderSize.size.width / renderNode.sourceSize.width + (modValue == 0 ? 0 : 1);
    modValue = (int)renderNode.renderSize.size.height % (int)renderNode.sourceSize.height;
    int heightCount = renderNode.renderSize.size.height / renderNode.sourceSize.height + (modValue == 0 ? 0 : 1);

    for (int i = 0; i < widthCount; i++)
    {
        float minX = renderNode.renderSize.origin.x + i * renderNode.sourceSize.width;
        float maxX = renderNode.renderSize.origin.x + renderNode.sourceSize.width + i * renderNode.sourceSize.width;

        if (renderNode.flipType == FlipType::HORIZONTAL || renderNode.flipType == FlipType::BOTH)
        {
            minX = renderNode.renderSize.size.width - minX;
            maxX = renderNode.renderSize.size.width - maxX;
        }

        for (int j = 0; j < heightCount; j++)
        {
            float minY = renderNode.renderSize.origin.y + j * renderNode.sourceSize.height;
            float maxY = renderNode.renderSize.origin.y + renderNode.sourceSize.height + j * renderNode.sourceSize.height;

            if (renderNode.flipType == FlipType::VERTICAL || renderNode.flipType == FlipType::BOTH)
            {
                minY = renderNode.renderSize.size.height - minY;
                maxY = renderNode.renderSize.size.height - maxY;
            }

            GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);

            SetVertexAndIndex(meshContext, pos, uv, renderNode.color);

            indexCount += 6;
        }
    }
}

void RenderDataProcess::DoScale9Grid(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    float u1 = renderNode.rect.getMinX() / (float)renderNode.texture->getWidth();
    float u2 = renderNode.rect.getMaxX() / (float)renderNode.texture->getWidth();
    float v1 = renderNode.rect.getMinY() / (float)renderNode.texture->getHeight();
    float v2 = renderNode.rect.getMaxY() / (float)renderNode.texture->getHeight();

    float _x1, _x2, _x3, _x4, _y1, _y2, _y3, _y4, _u1, _u2, _u3, _u4, _v1, _v2, _v3, _v4;
    _u1 = u1;
    _u2 = _u1 + renderNode.scale9grid.origin.x / (float)renderNode.texture->getWidth();
    _u3 = _u2 + renderNode.scale9grid.size.width / (float)renderNode.texture->getWidth();
    _u4 = u2;

    _v1 = v1;
    _v2 = _v1 + renderNode.scale9grid.origin.y / (float)renderNode.texture->getHeight();
    _v3 = _v2 + renderNode.scale9grid.size.height / (float)renderNode.texture->getHeight();
    _v4 = v2;
    
    _x1 = renderNode.renderSize.origin.x;
    _x4 = _x1 + renderNode.renderSize.size.width;
    _x2 = _x1 + renderNode.scale9grid.origin.x;
    _x3 = _x4 - (renderNode.rect.size.width - renderNode.scale9grid.origin.x - renderNode.scale9grid.size.width);
    if (_x2 > _x4) _x2 = (_x4 + _x1) / 2;
    if (_x3 < _x2) _x3 = _x2;

    _y1 = renderNode.renderSize.origin.y;
    _y4 = renderNode.renderSize.size.height + renderNode.renderSize.origin.y;
    _y2 = _y1 + renderNode.scale9grid.origin.y;
    _y3 = _y4 - (renderNode.rect.size.height - renderNode.scale9grid.origin.y - renderNode.scale9grid.size.height);
    if (_y2 > _y4) _y2 = (_y4 + _y1) / 2;
    if (_y3 < _y2) _y3 = _y2;
   
    if (renderNode.flipType == FlipType::HORIZONTAL || renderNode.flipType == FlipType::BOTH)
    {
        float tmp = _x1; _x1 = _x4; _x4 = tmp;
        _x2 = _x1 + _x4 - _x2;
        _x3 = _x1 + _x4 - _x3;
    }
    if (renderNode.flipType == FlipType::VERTICAL || renderNode.flipType == FlipType::BOTH)
    {
        float tmp = _y1; _y1 = _y4; _y4 = tmp;
        _y2 = _y1 + _y4 - _y2;
        _y3 = _y1 + _y4 - _y3;
    }

    Vec2 pos[4] = {}, uv[4] = {};
    //bottom-left
    if(_x1 != _x2 && _y1 != _y2)
    {
        GetPosAndUV(matrix, _x1, _x2, _y1, _y2, _u1, _v1, _u2, _v2, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //bottom-center
    if (_x2 != _x3 && _y1 != _y2)
    {
        GetPosAndUV(matrix, _x2, _x3, _y1, _y2, _u2, _v1, _u3, _v2, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //bottom-right
    if (_x3 != _x4 && _y1 != _y2)
    {
        GetPosAndUV(matrix, _x3, _x4, _y1, _y2, _u3, _v1, _u4, _v2, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //middle-left
    if (_x1 != _x2 && _y2 != _y3)
    {
        GetPosAndUV(matrix, _x1, _x2, _y2, _y3, _u1, _v2, _u2, _v3, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //middle-center
    if (_x2 != _x3 && _y2 != _y3)
    {
        GetPosAndUV(matrix, _x2, _x3, _y2, _y3, _u2, _v2, _u3, _v3, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //middle-right
    if (_x3 != _x4 && _y2 != _y3)
    {
        GetPosAndUV(matrix, _x3, _x4, _y2, _y3, _u3, _v2, _u4, _v3, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //top-left
    if (_x1 != _x2 && _y3 != _y4)
    {
        GetPosAndUV(matrix, _x1, _x2, _y3, _y4, _u1, _v3, _u2, _v4, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //top-center
    if (_x2 != _x3 && _y3 != _y4)
    {
        GetPosAndUV(matrix, _x2, _x3, _y3, _y4, _u2, _v3, _u3, _v4, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    //top-right
    if (_x3 != _x4 && _y3 != _y4)
    {
        GetPosAndUV(matrix, _x3, _x4, _y3, _y4, _u3, _v3, _u4, _v4, pos, uv);
        SetVertexAndIndex(meshContext, pos, uv, renderNode.color);
        indexCount += 6;
    }
    
}

void RenderDataProcess::DoNormalFont(GObject* go, SpriteFrame* pSpriteFrame, MeshContext& meshContext, int& indexCount)
{
    GTextField* ptext = go->SafeCast<GTextField>();
    if (ptext)
    {
        ptext->getTextFormat()->color.a = go->getAlpha() * 255.0f;
        PackageItem* item = UIPackage::getItemByURL(ptext->getTextFormat()->face);
        if (item)
        {
            AZStd::string str = ptext->getText();
            Size size = CaculateTextFormat(item, str, ptext->getTextFormat());

            float maxX = go->getSize().width, maxY = go->getSize().height;
            float minX = 0, minY = 0;

            Vec2 beginPos = Vec2::ZERO;
            if (ptext->getTextFormat()->align == TextHAlignment::LEFT)
            {
                beginPos.x = minX;
            }
            else if (ptext->getTextFormat()->align == TextHAlignment::CENTER)
            {
                beginPos.x = (minX + maxX) / 2 - size.width / 2;
            }
            else if (ptext->getTextFormat()->align == TextHAlignment::RIGHT)
            {
                beginPos.x = maxX - size.width;
            }
            if (ptext->getTextFormat()->verticalAlign == TextVAlignment::TOP)
            {
                beginPos.y = minY;
            }
            else if (ptext->getTextFormat()->verticalAlign == TextVAlignment::CENTER)
            {
                beginPos.y = (minY + maxY) / 2 + size.height / 2;
            }
            else if (ptext->getTextFormat()->verticalAlign == TextVAlignment::BOTTOM)
            {
                beginPos.y = maxY;
            }

            beginPos.x -= ptext->getTextFormat()->letterSpacing;
            beginPos.x += 2;

            const char* arr = str.c_str();
            while (*arr != '\0')
            {
                char v = *arr;
                if (v != '\n')
                {
                    FontLetterDefinition fld = item->bitmapFont->fontAtlas[v];
                    DoBitmapFont(fld, beginPos, ptext->getTextFormat(), go->GetGlobalMatrix44(), meshContext, indexCount);
                }
                ++arr;
            }
        }
        else
        {
            RenderDataProcess::DoFonsFont(ptext, ptext->getTextFormat(), go->GetGlobalMatrix44(), meshContext, indexCount);
        }
    }
} 
float BOLD_OFFSET[8] = 
{
    -0.5f, 0, 0.5f, 0, 0, -0.5f, 0, 0.5f
};

void RenderDataProcess::DoFonsFont(GObject* go, TextFormat* textFormat, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    GTextField* ptext = go->SafeCast<GTextField>();
    if (ptext == nullptr) return;

    TextParser tp = ptext->GetTextParser();
    
    float maxX = go->getSize().width, maxY = go->getSize().height;
    float minX = 0, minY = 0;

    AZ::Transform m = matrix;
    if(GBasicTextField* pBasicText = go->SafeCast<GBasicTextField>())
    {
        float scale = pBasicText->GetShrinkScale();
        m.MultiplyByScale(Vector3(scale, scale, scale));
    }

    Size size = tp.GetAllSize();
    int count = tp._vecLine.size();
    size.height += (MAX(0, count - 1)) * textFormat->lineSpacing;

    const int xOffset = 3;

    Vec2 beginPos = Vec2::ZERO;
    if (textFormat->verticalAlign == TextVAlignment::TOP)
    {
        beginPos.y = minY;
    }
    else if (textFormat->verticalAlign == TextVAlignment::CENTER)
    {
        beginPos.y = MAX(minY, (minY + maxY) / 2 - size.height / 2);
    }
    else if (textFormat->verticalAlign == TextVAlignment::BOTTOM)
    {
        beginPos.y = maxY - size.height;
    }    
    
    int vecCount = 0;

    for (auto& lineObj : tp._vecLine)
    {
        if(vecCount == 0 )
        {
            if (textFormat->align == TextHAlignment::LEFT)
            {
                beginPos.x = minX + xOffset;
            }
            else if (textFormat->align == TextHAlignment::CENTER)
            {
                beginPos.x = (minX + maxX) / 2 - lineObj._size.width / 2 + xOffset / 2;
            }
            else if (textFormat->align == TextHAlignment::RIGHT)
            {
                beginPos.x = maxX - lineObj._size.width - xOffset;
            }
        }

        ++vecCount;

        if (maxY < beginPos.y) return;

        float middle = beginPos.y + lineObj._size.height / 2;
        AZStd::vector<Vec2> fontVertex;
        FonsFont::CaculateFont(textFormat->face, beginPos, lineObj._fontSize, lineObj._text.c_str(), fontVertex);
        float totalLetterSpacing = 0.0f;
        auto it = fontVertex.begin();
        while (it != fontVertex.end())
        {
            float x1 = (*it).x + totalLetterSpacing;
            float y1 = (*it).y;

            ++it;
            float x2 = (*it).x + totalLetterSpacing;
            float y2 = (*it).y;

            y1 = middle * 2 - y1;
            y2 = middle * 2 - y2;

            ++it;
            float u1 = (*it).x;
            float v1 = (*it).y;

            ++it;
            float u2 = (*it).x;
            float v2 = (*it).y;

            Vec2 pos[4] = {}, uv[4] = {};

            if(lineObj._bold)
            {
                for(int b = 0; b < 4; b++)
                {
                    GetPosAndUV(m, x1 + BOLD_OFFSET[b * 2], x2 + BOLD_OFFSET[b * 2], y1 + BOLD_OFFSET[b * 2 + 1], y2 + BOLD_OFFSET[b * 2 + 1], u1, v1, u2, v2, pos, uv);

                    SetVertexAndIndex(meshContext, pos, uv, lineObj._color);

                    indexCount += 6;
                }
            }
            else
            {
                GetPosAndUV(m, x1, x2, y1, y2, u1, v1, u2, v2, pos, uv);

                SetVertexAndIndex(meshContext, pos, uv, lineObj._color);

                indexCount += 6;
            }

            ++it;
            totalLetterSpacing += (float)textFormat->letterSpacing;
        }
        if(lineObj._newLine)
        {
            beginPos.y += lineObj._size.height;
            beginPos.y += textFormat->lineSpacing;

            if (textFormat->align == TextHAlignment::LEFT)
            {
                beginPos.x = minX + xOffset;
            }
            else if (textFormat->align == TextHAlignment::CENTER)
            {
                beginPos.x = (minX + maxX) / 2 - lineObj._size.width / 2 + xOffset / 2;
            }
            else if (textFormat->align == TextHAlignment::RIGHT)
            {
                beginPos.x = maxX - lineObj._size.width - xOffset;
            }
        }
        else
        {
            beginPos.x += lineObj._size.width;
        }
    }
}

Size RenderDataProcess::CaculateTextFormat(PackageItem* item, AZStd::string string, TextFormat* textFormat)
{
    int width = 0, height = 0;

    const char* arr = string.c_str();
    while (*arr != '\0')
    {
        char v = *arr;
        if (v != '\n')
        {
            FontLetterDefinition fld = item->bitmapFont->fontAtlas[v];

            width += fld.width + textFormat->letterSpacing;
            height = height < fld.height ? fld.height : height;
        }
        ++arr;
    }
    width -= textFormat->letterSpacing;
    width += 4;

    return Size(width, height);
}

void RenderDataProcess::DoBitmapFont(FontLetterDefinition& fontDef, Vec2& beginPos, TextFormat* textFormat, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount)
{
    if (fontDef.texture == nullptr) return;

    float width = fontDef.width;
    float height = fontDef.height;

    float minX = beginPos.x + textFormat->letterSpacing;
    float maxX = minX + width;
    float minY = 0, maxY = 0;
    if(textFormat->verticalAlign == TextVAlignment::TOP)
    {
        minY = beginPos.y;
        maxY = minY + height;
    }
    else if (textFormat->verticalAlign == TextVAlignment::CENTER)
    {
        minY = beginPos.y - height;
        maxY = beginPos.y;
    }
    else if (textFormat->verticalAlign == TextVAlignment::BOTTOM)
    {
        maxY = beginPos.y;
        minY = maxY - height;
    }

    float u1 = fontDef.U / (float)fontDef.texture->getWidth();
    float u2 = (fontDef.U + fontDef.width) / (float)fontDef.texture->getWidth();
    float v1 = fontDef.V / (float)fontDef.texture->getHeight();
    float v2 = (fontDef.V + fontDef.height) / (float)fontDef.texture->getHeight();
    
    Vec2 pos[4] = {}, uv[4] = {};

    GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);

    SetVertexAndIndex(meshContext, pos, uv, Color4B::WHITE);

    indexCount += 6;

    beginPos.x = minX + fontDef.xAdvance;
}

void RenderDataProcess::SetVertexAndIndex(MeshContext& meshContext, Vec2 *pos, Vec2 *uv, Color4B color)
{
    meshContext.vt[meshContext.vertexCount] = { pos[0].x, pos[0].y, uv[0].x, uv[0].y, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
    meshContext.vt[meshContext.vertexCount + 1] = { pos[1].x, pos[1].y, uv[1].x, uv[1].y, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
    meshContext.vt[meshContext.vertexCount + 2] = { pos[2].x, pos[2].y, uv[2].x, uv[2].y, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
    meshContext.vt[meshContext.vertexCount + 3] = { pos[3].x, pos[3].y, uv[3].x, uv[3].y, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };

    meshContext.indices[meshContext.indexCount] = meshContext.vertexCount;
    meshContext.indices[meshContext.indexCount + 1] = meshContext.vertexCount + 1;
    meshContext.indices[meshContext.indexCount + 2] = meshContext.vertexCount + 2;
    meshContext.indices[meshContext.indexCount + 3] = meshContext.vertexCount;
    meshContext.indices[meshContext.indexCount + 4] = meshContext.vertexCount + 2;
    meshContext.indices[meshContext.indexCount + 5] = meshContext.vertexCount + 3;

    meshContext.vertexCount += 4;
    meshContext.indexCount += 6;
}

void RenderDataProcess::GetPosAndUV(const AZ::Transform& matrix, float minX, float maxX, float minY, float maxY, float u1, float v1, float u2, float v2, Vec2 *pos, Vec2 *uv)
{    
    // auto transMatrix = ProjectionMatrix * matrix;
    auto transMatrix = matrix;
    auto bottom_left = transMatrix * AZ::Vector3(minX, minY, 0);
    auto bottom_right = transMatrix * AZ::Vector3(maxX, minY, 0);
    auto top_right = transMatrix * AZ::Vector3(maxX, maxY, 0);
    auto top_left = transMatrix * AZ::Vector3(minX, maxY, 0);
    
    Vec2 t_l(top_left.GetX(), top_left.GetY());
    Vec2 t_r(top_right.GetX(), top_right.GetY());
    Vec2 b_r(bottom_right.GetX(), bottom_right.GetY());
    Vec2 b_l(bottom_left.GetX(), bottom_left.GetY());

    pos[0] = t_l;
    pos[1] = t_r;
    pos[2] = b_r;
    pos[3] = b_l;

    uv[0] = Vec2(u1, v2);
    uv[1] = Vec2(u2, v2);
    uv[2] = Vec2(u2, v1);
    uv[3] = Vec2(u1, v1);
}

void RenderDataProcess::CaculateMaskRect(GObject* go, MeshContext& meshContext, int& indexCount)
{
    RenderNode rn;
    rn.renderSize.size = go->getSize();
    rn.sourceSize = go->sourceSize;

    rn.flipType = FlipType::NONE;
    rn.fillMethod = FillMethod::NONE;
    rn.renderSize.origin = Vec2::ZERO;
    rn.color.a = 0.0f;

    //if (rn.fillMethod != FillMethod::NONE)
    //{
    //    RenderDataProcess::DoFillMethod(rn, go->GetGlobalMatrix44(), meshContext, indexCount);
    //    return;
    //}
    //else
    //{
    RenderDataProcess::DoMaskNormal(rn, go->GetGlobalMatrix44(), meshContext, indexCount);
    //}
}

void RenderDataProcess::CaculateShapeRect(GObject* go, MeshContext& meshContext, int& indexCount)
{
    GGraph* graph = go->SafeCast<GGraph>();
    if (graph == nullptr || graph->GetType() == 0)
    {
        return;
    }
    RenderNode rn;
    rn.renderSize.size = go->getSize();
    rn.sourceSize = go->sourceSize;

    rn.flipType = FlipType::NONE;
    rn.fillMethod = FillMethod::NONE;
    rn.renderSize.origin = Vec2::ZERO;
    Color4B col(graph->GetFillColor());
    rn.color = col;
    if (graph->GetType() == 1)
    {
        RenderDataProcess::DoShapeNormal(rn, go->GetGlobalMatrix44(), meshContext, indexCount, graph->GetLineSize(), Color4B(graph->GetLineColor()));
    }
    else
    {
        RenderDataProcess::DoShapeCircleNormal(rn, go->GetGlobalMatrix44(), meshContext, indexCount, graph->GetLineSize(), Color4B(graph->GetLineColor()));
    }
}

void RenderDataProcess::DoShapeBorderNormal(Rect& rect, const AZ::Transform& matrix, MeshContext& meshContext, int& indexCount, Color4B color)
{
    float u1 = 0.0f;
    float u2 = 0.0f;
    float v1 = 0.0f;
    float v2 = 0.0f;
    float minX = rect.origin.x, minY = rect.origin.y, maxX = rect.origin.x + rect.size.width, maxY = rect.origin.y + rect.size.height;

    Vec2 pos[4] = {}, uv[4] = {};

    GetPosAndUV(matrix, minX, maxX, minY, maxY, u1, v1, u2, v2, pos, uv);

    SetVertexAndIndex(meshContext, pos, uv, color);

    indexCount += 6;
}

inline int calculatePiceses(float r)
{
    int piceses = (int)sqrtf(r * 20.0f);
    return MAX(piceses, 8);
}

void RenderDataProcess::DoShapeCircleNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& totalIndexCount, int lineSize, Color4B linecolor)
{
    float radius = MAX(renderNode.renderSize.size.width, renderNode.renderSize.size.height) / 2.0f;
    int piceses = calculatePiceses(radius);

    const int vertexCount = piceses + 2;
    const int indexCount = piceses * 3;

    float angle_shift = 2.0f / piceses * PI;
    vector<Vec2> pos;
    float halfWidth = renderNode.renderSize.size.width / 2.0f;
    float halfHeight = renderNode.renderSize.size.height / 2.0f;
    pos.push_back(Vec2(halfWidth, halfHeight));
    float phi = PI / -2.0f;
    for (int i = 0; i <= piceses; i++, phi += angle_shift)
    {
        Vec2 piceseVec2(halfWidth + halfWidth*cosf(phi), halfHeight + halfHeight * sinf(phi));
        pos.push_back(piceseVec2);
    }
    for (int i = 0; i < pos.size(); i++)
    {
        AZ::Vector3 targetPos(pos[i].x, pos[i].y, 0.0f);
        auto realPos = matrix * targetPos;
        pos[i].x = realPos.GetX(); pos[i].y = realPos.GetY();
    }
    auto& color = renderNode.color;
    for (int i = 0;i < vertexCount;i++)
    {
        meshContext.vt[meshContext.vertexCount + i] = { pos[i].x, pos[i].y, 0.0f, 0.0f, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
    }

    for (int i = 0;i < piceses;i++)
    {
        meshContext.indices[meshContext.indexCount + i * 3] = meshContext.vertexCount;
        meshContext.indices[meshContext.indexCount + i * 3 + 1] = meshContext.vertexCount + i + 1;
        meshContext.indices[meshContext.indexCount + i * 3 + 2] = meshContext.vertexCount + i + 2;
    }
    meshContext.vertexCount += vertexCount;
    meshContext.indexCount += indexCount;

    totalIndexCount += indexCount;
    if (lineSize)
    {
        RenderDataProcess::DoShapeCircleBorderNormal(renderNode, matrix, meshContext, totalIndexCount, lineSize, linecolor);
    }
}

void RenderDataProcess::DoShapeCircleBorderNormal(RenderNode& renderNode, const AZ::Transform& matrix, MeshContext& meshContext, int& totalIndexCount, int lineSize, Color4B linecolor)
{
    Vec2 midPoint(renderNode.renderSize.size.width / 2.0f, renderNode.renderSize.size.height / 2.0f);
    Vec2 outerRect(renderNode.renderSize.size.width / 2.0f + lineSize / 2, renderNode.renderSize.size.height / 2.0f + lineSize / 2);
    Vec2 innerRect(renderNode.renderSize.size.width / 2.0f - lineSize / 2 - lineSize % 2, renderNode.renderSize.size.height / 2.0f - lineSize / 2 - lineSize % 2);

    float outerRadius = MAX(outerRect.x, outerRect.y);
    int piceses = calculatePiceses(outerRadius);

    const int vertexCount = (piceses + 1) * 2;
    const int indexCount = piceses * 6;

    float angle_shift = 2.0f / piceses * PI;
    vector<Vec2> pos;

    float phi = PI / -2.0f;
    for (int i = 0; i <= piceses; i++, phi += angle_shift)
    {
        Vec2 innerPiceseVec2(midPoint.x + innerRect.x*cosf(phi), midPoint.y + innerRect.y * sinf(phi));
        pos.push_back(innerPiceseVec2);
        Vec2 outerPiceseVec2(midPoint.x + outerRect.x*cosf(phi), midPoint.y + outerRect.y * sinf(phi));
        pos.push_back(outerPiceseVec2);
    }
    for (int i = 0; i < pos.size(); i++)
    {
        AZ::Vector3 targetPos(pos[i].x, pos[i].y, 0.0f);
        auto realPos = matrix * targetPos;
        pos[i].x = realPos.GetX(); pos[i].y = realPos.GetY();
    }

    for (int i = 0; i < vertexCount; i++)
    {
        meshContext.vt[meshContext.vertexCount + i] = { pos[i].x, pos[i].y, 0.0f, 0.0f, linecolor.r / 255.0f, linecolor.g / 255.0f, linecolor.b / 255.0f, linecolor.a / 255.0f };
    }

    for (int i = 0; i < piceses; i++)
    {
        meshContext.indices[meshContext.indexCount + i * 6]        = meshContext.vertexCount + i * 2;
        meshContext.indices[meshContext.indexCount + i * 6 + 1]    = meshContext.vertexCount + i * 2 + 1;
        meshContext.indices[meshContext.indexCount + i * 6 + 2]    = meshContext.vertexCount + i * 2 + 3;
        meshContext.indices[meshContext.indexCount + i * 6 + 3]    = meshContext.vertexCount + i * 2;
        meshContext.indices[meshContext.indexCount + i * 6 + 4]    = meshContext.vertexCount + i * 2 + 3;
        meshContext.indices[meshContext.indexCount + i * 6 + 5]    = meshContext.vertexCount + i * 2 + 2;
    }

    meshContext.vertexCount += vertexCount;
    meshContext.indexCount += indexCount;

    totalIndexCount += indexCount;
}


void RenderDataProcess::GetFontEffect(FontNode& fn)
{
    if(GBasicTextField* ptext = fn.go->SafeCast<GBasicTextField>())
    {
        auto tf = ptext->getTextFormat();
        if(tf)
        {
            if(tf->hasEffect(TextFormat::OUTLINE))
            {
                fn.isOutLine = true;
                fn.outLineWidth = tf->outlineSize;
                fn.outLineColor = (Color4B)tf->outlineColor;
            }
            if (tf->hasEffect(TextFormat::SHADOW))
            {
                fn.isShadow = true;
                fn.shadowOffset = tf->shadowOffset;
                fn.shadowColor = (Color4B)tf->shadowColor;
            }
        }
    }
}


NS_FGUI_END
