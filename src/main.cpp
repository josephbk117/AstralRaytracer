#include <iostream>
#include <cstring>

#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"
#include "Raytracer/TextureData.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/DrawingPanel.h"
#include "Raytracer/ShaderProgram.h"

int main()
{
    AstralRaytracer::Window window("new window test");
	window.initialize();

	//----------------

	const uint32 xImageSize = 380;
	const uint32 yImageSize = 190;
	const uint32 xTotalPixelCount = xImageSize * 3;

	const uint32 NS = 40;

	// Destructors need to be called before context is removed
	{
		TextureData texData;
		{
			unsigned char* data = new unsigned char[xImageSize * yImageSize * 3];
			texData.setTextureData(data, xImageSize, yImageSize, 3);
		}
		const uint32 textureId = TextureManager::loadTextureFromData(texData, false);

		DrawingPanel drawingPanel;
		drawingPanel.init(1, 1);
		drawingPanel.setTextureID(textureId);
		drawingPanel.getTransform().setPosition(0, 0);
		drawingPanel.getTransform().setScale(glm::vec2(1, 1));

		ShaderProgram shader;
		shader.compileShaders("resources/simpleDisplay.vs", "resources/simpleDisplay.fs");
		shader.linkShaders();
		shader.use();

		int32 drawingPanelModelMatrix = shader.getUniformLocation("model");

		//----------------

		while(!window.shouldWindowClose())
		{
			//-------
			//Perform ray tracing update to image

			uint8* const texDataPtr = texData.getTextureData();
			for (uint32 index = 0; index < xTotalPixelCount * yImageSize; index += 3)
			{				
				glm::vec2 coord { (index % xTotalPixelCount) / (float32)xTotalPixelCount, (index / xTotalPixelCount) / (float32)yImageSize };
				texDataPtr[index] = coord.x * 255;
				texDataPtr[index+1] = coord.y * 255;
				texDataPtr[index+2] = 0;
			}

			//-------

			gl::glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			gl::glClear(gl::GL_COLOR_BUFFER_BIT);

			shader.use();
			shader.applyShaderUniformMatrix(drawingPanelModelMatrix, drawingPanel.getTransform().getMatrix());

			gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
			gl::glTexSubImage2D(gl::GL_TEXTURE_2D, 0, 0, 0, xImageSize, yImageSize, TextureManager::getTextureFormatFromData(3), gl::GL_UNSIGNED_BYTE, texDataPtr);

			window.startUI();

			ImGui::Begin("GameWindow");
			{
				ImGui::BeginChild("GameRender");
				ImVec2 wsize = ImGui::GetWindowSize();
				ImGui::Image(reinterpret_cast<ImTextureID>(drawingPanel.getTextureID()), wsize, ImVec2(0, 1), ImVec2(1, 0));
				ImGui::EndChild();
			}
			ImGui::End();

			window.endUI();

			window.swapBuffers();
			window.pollEvents();
		}
	}

	window.shutdown();
    return 0;
}