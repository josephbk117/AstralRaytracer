#pragma once
#include "Utils/Common.h"

class DrawingPanel
{
	public:
		DrawingPanel();
		~DrawingPanel();
		void init(float32 width, float32 height);
		void setTextureID(uint32 textureID, uint32 index= 0);
		[[nodiscard]]
		uint32 getTextureID(uint32 index= 0) const noexcept;
		void   draw() const noexcept;
	private:
		float32               m_width = 0.0f;
		float32               m_height= 0.0f;
		uint32                m_vaoID = 0;
		uint32                m_vboID = 0;
		std::array<uint32, 2> m_textureID;
};