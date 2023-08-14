#pragma once
#include "Utils/Common.h"

#include <glbinding/gl/gl.h>
#include <glm.hpp>
#include <string>

namespace AstralRaytracer
{
	class BaseShaderProgram
	{
		public:
		BaseShaderProgram()= default;
		virtual ~BaseShaderProgram(){};

		static void applyShaderUniformMatrix(int32 uniformId, const glm::mat4& matrixValue);
		static void applyShaderVector3(int32 uniformId, const glm::vec3& value);
		static void applyShaderFloat(int32 uniformId, float32 value);
		static void applyShaderInt(int32 uniformId, int32 value);
		static void applyShaderBool(int32 uniformId, bool value);

		virtual int32     getId() { return m_programId; }
		virtual gl::GLint getUniformLocation(const std::string& uniformName) const;

		protected:
		virtual void compile(const std::string& filePath, uint32 id)= 0;
		virtual void link(uint32 shaderId)                          = 0;
		virtual void use()                                          = 0;
		virtual void unuse()                                        = 0;

		int32 m_programId= -1;
	};
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	class ShaderProgramVSFS: public BaseShaderProgram
	{
		public:
		ShaderProgramVSFS();
		virtual ~ShaderProgramVSFS();
		void         compileShadersVSFS(const std::string& vertexShaderPath,
																		const std::string& fragmentShaderPath);
		void         linkShaders();
		void         addAttribute(const std::string& attributeName);
		virtual void use() override;
		virtual void unuse() override;

		private:
		virtual void compile(const std::string& filePath, uint32 id) override;

		uint32 vertexShaderID, fragmentShaderID;
		int32  attributeCount;
	};
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	class ComputeShaderProgram: public BaseShaderProgram
	{
		public:
		void compileAndLinkShader(const std::string& computeShaderPath);
		void use() override;
		void unuse() override;

		private:
		void link(uint32 shaderId) override;
		void compile(const std::string& filePath, uint32 id) override;
	};
} // namespace AstralRaytracer