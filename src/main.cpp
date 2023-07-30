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

	const int NX = 380;
	const int NY = 190;
	const int NS = 40;

	unsigned char *data = new unsigned char[NX * NY * 3];

	std::memset(data, 100, NX * NY);

	// Destructors need to be called before context is removed
	{
		TextureData texData;
		texData.setTextureData(data, NX, NY, 3);
		unsigned int textureId = TextureManager::loadTextureFromData(texData, false);

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
			gl::glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			gl::glClear(gl::GL_COLOR_BUFFER_BIT);

			shader.use();
			shader.applyShaderUniformMatrix(drawingPanelModelMatrix, drawingPanel.getTransform().getMatrix());

			gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
			gl::glTexSubImage2D(gl::GL_TEXTURE_2D, 0, 0, 0, NX, NY, TextureManager::getTextureFormatFromData(3), gl::GL_UNSIGNED_BYTE, data);

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