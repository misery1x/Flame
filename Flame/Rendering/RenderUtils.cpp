#include "RenderUtils.hpp"
#include <cstring>
#include <cmath>

#include <Windows.h>
#include <gl/GL.h>

#ifndef GL_VIEWPORT
#define GL_VIEWPORT 0x0BA2
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void BuildPerspective(float* out, float fovDeg, float aspect, float zNear, float zFar)
{
    memset(out, 0, 16 * sizeof(float));
    float f = 1.0f / tanf(fovDeg * (float)M_PI / 360.0f);
    out[0]  = f / aspect;
    out[5]  = f;
    out[10] = (zFar + zNear) / (zNear - zFar);
    out[11] = -1.0f;
    out[14] = (2.0f * zFar * zNear) / (zNear - zFar);
}

static void BuildModelView(float* out, float yaw, float pitch)
{
    memset(out, 0, 16 * sizeof(float));
    float yawRad   = yaw   * (float)M_PI / 180.0f;
    float pitchRad = pitch * (float)M_PI / 180.0f;
    float cp = cosf(pitchRad), sp = sinf(pitchRad);
    float cy = cosf(yawRad),   sy = sinf(yawRad);

    out[0]  = -cy;        out[1]  = -sp * sy;   out[2]  =  cp * sy;  out[3]  = 0.0f;
    out[4]  =  0.0f;      out[5]  =  cp;        out[6]  =  sp;       out[7]  = 0.0f;
    out[8]  = -sy;        out[9]  =  sp * cy;   out[10] = -cp * cy;  out[11] = 0.0f;
    out[12] =  0.0f;      out[13] =  0.0f;      out[14] =  0.0f;     out[15] = 1.0f;
}

namespace RenderUtils
{
    void CaptureViewport()
    {
        glGetIntegerv(GL_VIEWPORT, viewport);
    }

    void UpdateMatrices(float yaw, float pitch, float fov)
    {
        matricesValid = false;
        if (viewport[2] <= 0 || viewport[3] <= 0) return;
        float aspect = (float)viewport[2] / (float)viewport[3];
        BuildModelView(modelview, yaw, pitch);
        BuildPerspective(projection, fov, aspect, 0.05f, 512.0f);
        matricesValid = true;
    }

    bool WorldToScreen(double x, double y, double z, ImVec2& screenPos)
    {
        if (!matricesValid) return false;

        double vX = modelview[0]*x + modelview[4]*y + modelview[8]*z  + modelview[12];
        double vY = modelview[1]*x + modelview[5]*y + modelview[9]*z  + modelview[13];
        double vZ = modelview[2]*x + modelview[6]*y + modelview[10]*z + modelview[14];
        double vW = modelview[3]*x + modelview[7]*y + modelview[11]*z + modelview[15];

        double cX = projection[0]*vX + projection[4]*vY + projection[8]*vZ  + projection[12]*vW;
        double cY = projection[1]*vX + projection[5]*vY + projection[9]*vZ  + projection[13]*vW;
        double cW = projection[3]*vX + projection[7]*vY + projection[11]*vZ + projection[15]*vW;

        if (cW <= 0.001) return false;

        screenPos.x = (float)(viewport[0] + (cX / cW + 1.0) * viewport[2] / 2.0);
        screenPos.y = (float)(viewport[1] + (1.0 - cY / cW) * viewport[3] / 2.0);
        return true;
    }

    bool WorldToScreen(double worldX, double worldY, double worldZ,double camX, double camY, double camZ, ImVec2& screenPos)
    {
        return WorldToScreen(worldX - camX, worldY - camY, worldZ - camZ, screenPos);
    }

    void DrawTextShadow(ImDrawList* dl, ImFont* font, float size, ImVec2 pos,ImU32 color, const char* text, ImU32 shadowColor)
    {
        dl->AddText(font, size, ImVec2(pos.x + 1.0f, pos.y + 1.0f), shadowColor, text);
        dl->AddText(font, size, pos, color, text);
    }

    float TextWidth(ImFont* font, float size, const char* text)
    {
        return font->CalcTextSizeA(size, FLT_MAX, 0.0f, text).x;
    }
}
