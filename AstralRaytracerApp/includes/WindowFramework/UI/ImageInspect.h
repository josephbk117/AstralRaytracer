#pragma once
#include "Raytracer/TextureData.h"
#include "Utils/Common.h"

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_internal.h>

namespace AstralRaytracer
{
	namespace UI
	{
		inline void histogram(const TextureDataRGBF& texData)
		{
			const int32 width = texData.getWidth();
			const int32 height= texData.getHeight();

			std::array<std::array<uint32, 256>, 3> count= {{{0}}};

			const float32* ptrCols= texData.getTextureData().data();

			ImGui::InvisibleButton("histogram", ImVec2(512, 256));
			for(int32 l= 0; l < height * width; l++)
			{
				const int32 val0=
						glm::min(static_cast<int32>(*ptrCols++ * 255.0f), 255); // Convert HDR to 0-255 range
				const int32 val1=
						glm::min(static_cast<int32>(*ptrCols++ * 255.0f), 255); // Convert HDR to 0-255 range
				const int32 val2=
						glm::min(static_cast<int32>(*ptrCols++ * 255.0f), 255); // Convert HDR to 0-255 range

				count[0][val0]++;
				count[1][val1]++;
				count[2][val2]++;
			}

			uint32        maxv  = count[0][0];
			const uint32* pCount= &count[0][0];
			for(int32 i= 0; i < 3 * 256; i++, pCount++)
			{
				maxv= (maxv > *pCount) ? maxv : *pCount;
			}

			ImDrawList* const drawList= ImGui::GetWindowDrawList();
			const ImVec2      rmin    = ImGui::GetItemRectMin();
			const ImVec2      rmax    = ImGui::GetItemRectMax();
			const ImVec2      size    = ImGui::GetItemRectSize();
			const float32     hFactor = size.y / static_cast<float32>(maxv);

			for(int32 i= 0; i <= 10; i++)
			{
				const float32 ax= rmin.x + (size.x / 10.f) * static_cast<float32>(i);
				const float32 ay= rmin.y + (size.y / 10.f) * static_cast<float32>(i);
				drawList->AddLine(ImVec2(rmin.x, ay), ImVec2(rmax.x, ay), 0x80808080);
				drawList->AddLine(ImVec2(ax, rmin.y), ImVec2(ax, rmax.y), 0x80808080);
			}

			const float32 barWidth= (size.x / 256.f);
			for(int32 j= 0; j < 256; j++)
			{
				// pixel count << 2 + color index(on 2 bits)
				std::array<uint32_t, 3> cols= {
						{(count[0][j] << 2), (count[1][j] << 2) + 1, (count[2][j] << 2) + 2}};
				if(cols[0] > cols[1])
					std::swap(cols[0], cols[1]);
				if(cols[1] > cols[2])
					std::swap(cols[1], cols[2]);
				if(cols[0] > cols[1])
					std::swap(cols[0], cols[1]);
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
				const float32 left         = rmin.x + barWidth * static_cast<float32>(j);
				const float32 right        = left + barWidth;
				for(int32 i= 0; i < 3; i++)
				{
					if(heights[i] >= currentHeight)
					{
						continue;
					}
					drawList->AddRectFilled(ImVec2(left, currentHeight), ImVec2(right, heights[i]),
																	colors[i]);
					currentHeight= heights[i];
				}
			}
		}

		inline void inspect(const TextureDataRGBF& texData, ImVec2 mouseUVCoord,
												ImVec2 displayedTextureSize)
		{
			const int32 width = texData.getWidth();
			const int32 height= texData.getHeight();

			ImGui::BeginTooltip();
			ImGui::BeginGroup();
			ImDrawList* const    drawList          = ImGui::GetWindowDrawList();
			static const float32 zoomRectangleWidth= 160.0f;

			// bitmap zoom
			ImGui::InvisibleButton("AnotherInvisibleMan", ImVec2(zoomRectangleWidth, zoomRectangleWidth));
			const ImRect pickRc(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
			drawList->AddRectFilled(pickRc.Min, pickRc.Max, 0xFF000000);
			const int32   zoomSize = 4;
			const float32 quadWidth= zoomRectangleWidth / float32(zoomSize * 2 + 1);
			const ImVec2  quadSize(quadWidth, quadWidth);
			const float32 coOrdX= ImClamp(mouseUVCoord.x * width, static_cast<float32>(zoomSize),
																		static_cast<float32>(width - zoomSize));
			const float32 coOrdY= ImClamp(mouseUVCoord.y * height, static_cast<float32>(zoomSize),
																		static_cast<float32>(height - zoomSize));

			union PixelUnion
			{
				std::array<uint8, 4> data;
				uint32               texel;
			};

			for(int32 y= -zoomSize; y <= zoomSize; y++)
			{
				for(int32 x= -zoomSize; x <= zoomSize; x++)
				{
					const ColourData&  colData= texData.getTexelColor(static_cast<uint32>(coOrdX + x),
																														static_cast<uint32>(coOrdY - y));
					const glm::u8vec3& pixData= colData.getColour_8_BitClamped(); // Adjusted for HDR values
					PixelUnion         pixel;
					pixel.data[0]= pixData.r;
					pixel.data[1]= pixData.g;
					pixel.data[2]= pixData.b;
					pixel.data[3]= 255;

					ImVec2 param;
					param.x= (x + zoomSize) * quadSize.x;
					param.y= (y + zoomSize) * quadSize.y;

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

			glm::vec3 colData=
					texData.getTexelColor(static_cast<uint32>(coOrdX), static_cast<uint32>(coOrdY));

			PixelUnion pixel;
			pixel.data[0]= static_cast<uint8>(colData.r * 255.0f); // Convert HDR to 0-255 range
			pixel.data[1]= static_cast<uint8>(colData.g * 255.0f); // Convert HDR to 0-255 range
			pixel.data[2]= static_cast<uint8>(colData.b * 255.0f); // Convert HDR to 0-255 range
			pixel.data[3]= 255;

			const ImVec4 color= ImColor(pixel.texel);
			ImVec4       colHSV;
			ImGui::ColorConvertRGBtoHSV(color.x, color.y, color.z, colHSV.x, colHSV.y, colHSV.z);
			ImGui::Text("U %1.2f V %1.2f", mouseUVCoord.x, mouseUVCoord.y);
			ImGui::Text("Coord %d %d", static_cast<int32>(mouseUVCoord.x * width),
									static_cast<int32>(mouseUVCoord.y * height));
			ImGui::Separator();
			ImGui::Text("R 0x%02x  G 0x%02x  B 0x%02x", static_cast<int32>(color.x * 255.0f),
									static_cast<int32>(color.y * 255.0f), static_cast<int32>(color.z * 255.0f));
			ImGui::Text("R %1.2f G %1.2f B %1.2f", color.x, color.y, color.z);
			ImGui::Separator();
			ImGui::Text("H 0x%02x  S 0x%02x  V 0x%02x", static_cast<int32>(colHSV.x * 255.0f),
									static_cast<int32>(colHSV.y * 255.0f), static_cast<int32>(colHSV.z * 255.0f));
			ImGui::Text("H %1.2f S %1.2f V %1.2f", colHSV.x, colHSV.y, colHSV.z);
			ImGui::Separator();
			ImGui::Text("Alpha 0x%02x", static_cast<int32>(color.w * 255.0f));
			ImGui::Text("Alpha %1.2f", color.w);
			ImGui::Separator();
			ImGui::Text("Size %d, %d", static_cast<int32>(displayedTextureSize.x),
									static_cast<int32>(displayedTextureSize.y));
			ImGui::EndGroup();
			histogram(texData);
			ImGui::EndTooltip();
		}
	} // namespace UI
} // namespace AstralRaytracer
