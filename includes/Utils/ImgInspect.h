#pragma once
#include "Common.h"
#include "Raytracer/TextureData.h"

#include <glm.hpp>
#include <imgui.h>
#include <imgui_internal.h>

namespace ImageInspect
{
	inline void histogram(const TextureData& texData)
	{
		int32 width = texData.getWidth();
		int32 height= texData.getHeight();

		uint32 count[3][256]= {0};

		const uint8* ptrCols= texData.getTextureData().data();

		ImGui::InvisibleButton("histogram", ImVec2(512, 256));
		for(int32 l= 0; l < height * width; l++)
		{
			count[0][*ptrCols++]++;
			count[1][*ptrCols++]++;
			count[2][*ptrCols++]++;
		}

		uint32  maxv  = count[0][0];
		uint32* pCount= &count[0][0];
		for(int32 i= 0; i < 3 * 256; i++, pCount++)
		{
			maxv= (maxv > *pCount) ? maxv : *pCount;
		}

		ImDrawList*   drawList= ImGui::GetWindowDrawList();
		const ImVec2  rmin    = ImGui::GetItemRectMin();
		const ImVec2  rmax    = ImGui::GetItemRectMax();
		const ImVec2  size    = ImGui::GetItemRectSize();
		const float32 hFactor = size.y / float32(maxv);

		for(int32 i= 0; i <= 10; i++)
		{
			float32 ax= rmin.x + (size.x / 10.f) * float32(i);
			float32 ay= rmin.y + (size.y / 10.f) * float32(i);
			drawList->AddLine(ImVec2(rmin.x, ay), ImVec2(rmax.x, ay), 0x80808080);
			drawList->AddLine(ImVec2(ax, rmin.y), ImVec2(ax, rmax.y), 0x80808080);
		}

		const float32 barWidth= (size.x / 256.f);
		for(int32 j= 0; j < 256; j++)
		{
			// pixel count << 2 + color index(on 2 bits)
			uint32_t cols[3]= {(count[0][j] << 2), (count[1][j] << 2) + 1, (count[2][j] << 2) + 2};
			if(cols[0] > cols[1])
				ImSwap(cols[0], cols[1]);
			if(cols[1] > cols[2])
				ImSwap(cols[1], cols[2]);
			if(cols[0] > cols[1])
				ImSwap(cols[0], cols[1]);
			float32 heights[3];
			uint32  colors[3];
			uint32  currentColor= 0xFFFFFFFF;
			for(int32 i= 0; i < 3; i++)
			{
				heights[i]= rmax.y - (cols[i] >> 2) * hFactor;
				colors[i] = currentColor;
				currentColor-= 0xFF << ((cols[i] & 3) * 8);
			}

			float32       currentHeight= rmax.y;
			const float32 left         = rmin.x + barWidth * float32(j);
			const float32 right        = left + barWidth;
			for(int32 i= 0; i < 3; i++)
			{
				if(heights[i] >= currentHeight)
				{
					continue;
				}
				drawList->AddRectFilled(ImVec2(left, currentHeight), ImVec2(right, heights[i]), colors[i]);
				currentHeight= heights[i];
			}
		}
	}

	inline void inspect(const TextureData& texData, ImVec2 mouseUVCoord, ImVec2 displayedTextureSize)
	{
		const int32 width = texData.getWidth();
		const int32 height= texData.getHeight();

		ImGui::BeginTooltip();
		ImGui::BeginGroup();
		ImDrawList*          drawList          = ImGui::GetWindowDrawList();
		static const float32 zoomRectangleWidth= 160.0f;

		// bitmap zoom
		ImGui::InvisibleButton("AnotherInvisibleMan", ImVec2(zoomRectangleWidth, zoomRectangleWidth));
		const ImRect pickRc(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		drawList->AddRectFilled(pickRc.Min, pickRc.Max, 0xFF000000);
		const int32   zoomSize = 4;
		const float32 quadWidth= zoomRectangleWidth / float32(zoomSize * 2 + 1);
		const ImVec2  quadSize(quadWidth, quadWidth);
		const uint32  coOrdX= ImClamp(int32(mouseUVCoord.x * width), zoomSize, width - zoomSize);
		const uint32  coOrdY= ImClamp(int32(mouseUVCoord.y * height), zoomSize, height - zoomSize);

		union PixelUnion
		{
			uint8  data[4];
			uint32 texel;
		};

		for(int32 y= -zoomSize; y <= zoomSize; y++)
		{
			for(int32 x= -zoomSize; x <= zoomSize; x++)
			{
				const ColourData&  colData= texData.getTexelColor(coOrdX + x, coOrdY - y);
				const glm::u8vec3& pixData= colData.getColour_8_BitClamped();
				PixelUnion         pixel;
				pixel.data[0]= pixData.x;
				pixel.data[1]= pixData.y;
				pixel.data[2]= pixData.z;
				pixel.data[3]= 255;

				ImVec2 param;
				param.x= float32(x + zoomSize) * quadSize.x;
				param.y= float32(y + zoomSize) * quadSize.y;

				ImVec2 pos;
				pos.x= pickRc.Min.x + param.x;
				pos.y= pickRc.Min.y + param.y;

				ImVec2 param2;
				param2.x= pos.x + quadSize.x;
				param2.y= pos.y + quadSize.y;

				drawList->AddRectFilled(pos, param2, pixel.texel);
			}
		}
		ImGui::SameLine();

		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();

		ColourData  colData= texData.getTexelColor(coOrdX, coOrdY);
		glm::u8vec3 pixData= colData.getColour_8_BitClamped();

		PixelUnion pixel;
		pixel.data[0]= pixData.x;
		pixel.data[1]= pixData.y;
		pixel.data[2]= pixData.z;
		pixel.data[3]= 255;

		const ImVec4 color= ImColor(pixel.texel);
		ImVec4       colHSV;
		ImGui::ColorConvertRGBtoHSV(color.x, color.y, color.z, colHSV.x, colHSV.y, colHSV.z);
		ImGui::Text("U %1.2f V %1.2f", mouseUVCoord.x, mouseUVCoord.y);
		ImGui::Text("Coord %d %d", int32(mouseUVCoord.x * width), int32(mouseUVCoord.y * height));
		ImGui::Separator();
		ImGui::Text("R 0x%02x  G 0x%02x  B 0x%02x", int32(color.x * 255.0f), int32(color.y * 255.0f),
								int32(color.z * 255.0f));
		ImGui::Text("R %1.2f G %1.2f B %1.2f", color.x, color.y, color.z);
		ImGui::Separator();
		ImGui::Text("H 0x%02x  S 0x%02x  V 0x%02x", int32(colHSV.x * 255.0f), int32(colHSV.y * 255.0f),
								int32(colHSV.z * 255.0f));
		ImGui::Text("H %1.2f S %1.2f V %1.2f", colHSV.x, colHSV.y, colHSV.z);
		ImGui::Separator();
		ImGui::Text("Alpha 0x%02x", int32(color.w * 255.0f));
		ImGui::Text("Alpha %1.2f", color.w);
		ImGui::Separator();
		ImGui::Text("Size %d, %d", int32(displayedTextureSize.x), int32(displayedTextureSize.y));
		ImGui::EndGroup();
		histogram(texData);
		ImGui::EndTooltip();
	}
} // namespace ImageInspect
