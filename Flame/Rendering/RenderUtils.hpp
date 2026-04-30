#pragma once

#include "../Framework/ImGui/imgui.h"

namespace RenderUtils
{
	inline float modelview[16] = {};
	inline float projection[16] = {};
	inline int viewport[4] = {};
	inline bool matricesValid = false;

	inline float espFontSize = 11.0f;

	void CaptureViewport();
	void UpdateMatrices(float yaw, float pitch, float fov);
	bool WorldToScreen(double x, double y, double z, ImVec2 &screenPos);
	bool WorldToScreen(double worldX, double worldY, double worldZ, double camX,double camY, double camZ, ImVec2 &screenPos);
	void DrawTextShadow(ImDrawList* dl, ImFont* font, float size, ImVec2 pos,ImU32 color, const char* text,ImU32 shadowColor = IM_COL32(0, 0, 0, 180));
	float TextWidth(ImFont* font, float size, const char* text);
}
