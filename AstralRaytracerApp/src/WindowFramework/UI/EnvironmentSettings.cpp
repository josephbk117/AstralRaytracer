#include "WindowFramework/UI/EnvironmentSettings.h"

#include "Raytracer/Camera.h"
#include "WindowFramework/Window.h"

void AstralRaytracer::UI::EnvironmentSettings::display(
		AppStateInfo& appStateInfo,
		Camera&       cam,
		const Window& window
)
{
	ImGui::PushFont(window.getSecondaryFont());
	ImGui::SeparatorText("ENVIRONMENT");
	ImGui::PopFont();

	ImGui::BeginChild(
			"Environment", ImVec2(0.0f, 0.0f), false,
			ImGuiWindowFlags_AlwaysAutoResize
	);

	ImGui::Text("Camera");
	float32 focalLength= cam.getFocalLength();
	if(ImGui::DragFloat("Focal Length", &focalLength, 0.1f, 0.01f, 100.0f, "%.2f mm"))
	{
		cam.setFocalLength(focalLength);
		appStateInfo.isSceneDirty= true;
	}

	float32 vFov= glm::degrees(cam.getVerticalFov());
	if(ImGui::SliderFloat("Vertical FOV", &vFov, 1.0f, 179.0f, "%.2f Deg"))
	{
		cam.setVerticalFov(glm::radians(vFov), cam.getSensorHeight());
		appStateInfo.isSceneDirty= true;
	}

	float32 sensorHeight= cam.getSensorHeight();
	if(ImGui::DragFloat("Sensor Height", &sensorHeight, 0.1f, 0.01f, 100.0f, "%.2f mm"))
	{
		cam.setSensorHeight(sensorHeight);
		appStateInfo.isSceneDirty= true;
	}

	float32 fStop= cam.getFStop();
	if(ImGui::SliderFloat("F-Stop", &fStop, 0.1f, 100.0f, "%.2f"))
	{
		cam.setFStop(fStop);
		appStateInfo.isSceneDirty= true;
	}

	float32 aperture= cam.getApertureDiameter();
	if(ImGui::SliderFloat("Aperture Diameter", &aperture, 0.01f, 10.0f, "%.2f mm"))
	{
		cam.setAperture(aperture);
		appStateInfo.isSceneDirty= true;
	}

	constexpr float32 max_float= std::numeric_limits<float32>::max();

	float32 focusDistance= cam.getFocusDistance();
	if(ImGui::DragFloat("Focus Distance", &focusDistance, 0.25f, 0.0f, max_float, "%.2f m"))
	{
		cam.setFocusDistance(focusDistance);
		appStateInfo.isSceneDirty= true;
	}

	if(appStateInfo.isSceneDirty)
	{
		cam.update(appStateInfo.rendererResolution);
	}

	ImGui::EndChild();
}
