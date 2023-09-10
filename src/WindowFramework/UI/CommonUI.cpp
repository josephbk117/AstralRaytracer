#include "WindowFramework/UI/CommonUI.h"

namespace AstralRaytracer
{
	namespace UI
	{
		bool displayXYZFloat(glm::vec3& vec3, const std::string& label)
		{
			return ImGui::InputFloat3(label.c_str(), reinterpret_cast<float*>(&vec3));
		}

		bool displayTransform(Traceable& traceable)
		{
			glm::vec3 pos  = traceable.getPosition();
			float32   rot  = traceable.getRotation();
			glm::vec3 scale= traceable.getScale();

			bool modified = false;
			if(displayXYZFloat(pos, "Position"))
			{
				traceable.setPosition(pos);
				modified = true;
			}

			if(ImGui::SliderFloat("Rotation", &rot, -180.f, 180.0f, "%.2f"))
			{
				traceable.setRotation(rot);
				modified = true;
			}

			if(displayXYZFloat(scale, "Scale"))
			{
				traceable.setScale(scale);
				modified = true;
			}

			return modified;
		}
	} // namespace UI
} // namespace AstralRaytracer