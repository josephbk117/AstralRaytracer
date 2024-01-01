#include "WindowFramework/UI/CommonUI.h"

namespace AstralRaytracer
{
	namespace UI
	{
		ImVec2 toImVec2(const glm::vec2& vec2) { return ImVec2(vec2.x, vec2.y); }

		ImVec2 toImVec2(const glm::u32vec2& vec2) { return ImVec2(vec2.x, vec2.y); }

		glm::vec2 toVec2(const ImVec2& vec2) { return glm::vec2(vec2.x, vec2.y); }

		glm::u32vec2 toU32Vec2(const ImVec2& vec2) { return glm::u32vec2(vec2.x, vec2.y); }

		bool displayXYZFloat(glm::vec3& vec3, const std::string& label, float32 speed /*=1.0f*/)
		{
			return ImGui::DragFloat3(label.c_str(), reinterpret_cast<float32*>(&vec3), speed);
		}

		bool displayTransform(Traceable& traceable)
		{
			glm::vec3 pos  = traceable.getPosition();
			float32   rot  = traceable.getRotation();
			glm::vec3 scale= traceable.getScale();

			bool modified= false;
			if(displayXYZFloat(pos, "Position", 0.1f))
			{
				traceable.setPosition(pos);
				modified= true;
			}

			if(ImGui::DragFloat("Rotation", &rot, 1.0f, -180.f, 180.0f, "%.2f"))
			{
				traceable.setRotation(rot);
				modified= true;
			}

			if(displayXYZFloat(scale, "Scale", 0.1f))
			{
				traceable.setScale(scale);
				modified= true;
			}

			return modified;
		}
	} // namespace UI
} // namespace AstralRaytracer