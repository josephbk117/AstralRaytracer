#pragma once

#include "Compositor/Compositor.h"
#include "Raytracer/Camera.h"
#include "Raytracer/Renderer.h"
#include "Utils/AssetManager.h"
#include "Utils/Common.h"
#include "WindowFramework/Input.h"
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"

class Application
{
	public:
		Application(spdlog::level::level_enum logLevel= spdlog::level::trace);
		void initialize();
		void run();
		void shutdown();
	private:
		AstralRaytracer::AssetManager m_assetManager;
		AstralRaytracer::Window       m_window{ "Astral Raytracer" };
		AstralRaytracer::Renderer     m_renderer;
		AstralRaytracer::Camera       m_cam{ 15.0f, 15.0f, 10.0f, 0.0f };
		AstralRaytracer::Scene        m_scene;
		AstralRaytracer::Compositor   m_compositor;
};