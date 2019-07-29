#ifndef __HITTEST_H__
#define __HITTEST_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "utils/ByteArray.h"

NS_FGUI_BEGIN

class GComponent;

class IHitTest
{
    FUI_RTTI_BASE(IHitTest)
public:
    virtual ~IHitTest() {}

    virtual bool hitTest(GComponent* obj, const Vec2& localPoint)
    {
        return true;
    }
};

class PixelHitTestData
{
public:
    int pixelWidth;
    float scale;
    unsigned char* pixels;
    size_t pixelsLength;

    PixelHitTestData();
    ~PixelHitTestData();

    void load(ByteArray& ba);
};

class PixelHitTest : public IHitTest
{
public:
    PixelHitTest(PixelHitTestData* data, int offsetX, int offsetY);

    virtual bool hitTest(GComponent* obj, const Vec2& localPoint) override;

    int offsetX;
    int offsetY;
    float scaleX;
    float scaleY;

private:
    PixelHitTestData* _data;
};

NS_FGUI_END

#endif