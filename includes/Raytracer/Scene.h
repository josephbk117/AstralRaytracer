#include "Traceable/Traceable.h"
#include "Utils/Common.h"

#include <memory>
#include <vector>

namespace AstralRaytracer
{
	class Scene
	{
		public:
		/*template<typename T>
		T* addTraceableToScene(Traceable& traceable)
		{
			return static_cast<T*>(m_sceneTraceables.emplace_back(traceable).get());
		}
		const Traceable* const getTraceableInScene(uint32 index);
		uint32 getTraceableCountInScene() const { return m_sceneTraceables.size(); }*/
		std::vector<std::unique_ptr<Traceable>> m_sceneTraceables;
	};
} // namespace AstralRaytracer