#include <iostream>
#include <cstring>

#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"
#include "WindowFramework/Input.h"
#include "Raytracer/TextureData.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/DrawingPanel.h"
#include "Raytracer/ShaderProgram.h"
#include "Raytracer/Camera.h"
#include <ext/quaternion_geometric.hpp>

int main()
{
    AstralRaytracer::Window window("new window test");
	window.initialize();
	AstralRaytracer::Input::initialize(window);

	//----------------

	const uint32 xImageSize = 380;
	const uint32 yImageSize = 190;
	const uint8	componentCount = 3;
	const uint32 xTotalPixelCount = xImageSize * 3;

	const uint32 NS = 40;

	// Destructors need to be called before context is removed
	{
		TextureData texData(xImageSize, yImageSize, componentCount);
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
		AstralRaytracer::Camera cam(45.0f, 1.0f, 10.0f);

		float64 prevTime = AstralRaytracer::Input::getTimeSinceStart();
		while(!window.shouldWindowClose())
		{
			//-------
			//Perform ray tracing update to image

			cam.update(AstralRaytracer::Input::getTimeSinceStart() - prevTime);
			prevTime = AstralRaytracer::Input::getTimeSinceStart();

			const glm::mat4& invProj = cam.getInverseProjection();
			const glm::mat4& invView = cam.getInverseView();

			for (uint32 index = 0; index < xTotalPixelCount * yImageSize; index += componentCount)
			{				
				glm::vec2 coord { (index % xTotalPixelCount) / (float32)xTotalPixelCount, (index / xTotalPixelCount) / (float32)yImageSize };
				coord = (coord * 2.0f) - 1.0f;

				glm::vec4 target = invProj * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
				glm::vec3 rayDir = glm::vec3(invView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));

				texData.setTexelColorAtPixelIndex(index, rayDir.x * 255, rayDir.y * 255, rayDir.z * 255);
			}

			//-------

			gl::glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			gl::glClear(gl::GL_COLOR_BUFFER_BIT);

			shader.use();
			shader.applyShaderUniformMatrix(drawingPanelModelMatrix, drawingPanel.getTransform().getMatrix());

			TextureManager::updateTexture(texData, textureId);

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