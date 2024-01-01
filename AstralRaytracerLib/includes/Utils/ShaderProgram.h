#pragma once
#include "Common.h"

#include <glbinding/gl/gl.h>
#include <string>
#include <unordered_map>
#include <variant>

using UniformDataType= std::variant<
		int32,
		uint32,
		float32,
		glm::ivec2,
		glm::uvec2,
		glm::vec2,
		glm::ivec3,
		glm::uvec3,
		glm::vec3,
		glm::mat4>;

struct UniformData
{
		uint32          uniformId= 0;
		float32         min      = 0.0f;
		float32         max      = 1.0f;
		UniformDataType data;
};

class ShaderProgram
{
	public:
		ShaderProgram();
		~ShaderProgram();
		void compileShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		void compileShadersFromSrcCode(
				const std::string& vertexShaderSrcCode,
				const std::string& fragmentShaderSrcCode
		);
		void linkShaders();
		void addAttribute(const std::string& attributeName);
		void use() const;
		void unuse() const;

		[[nodiscard]]
		const std::unordered_map<std::string, UniformData>& getUniformData() const;
		[[nodiscard]]
		gl::GLint getUniformLocation(const std::string& uniformName) const;

		void setUniformValue(
				const std::string&     uniformName,
				const UniformDataType& data,
				float32                min,
				float32                max
		);
		void        updateUniformData(const std::string& uniformName, const UniformDataType& data);
		static void applyShaderUniformMatrix(int32 uniformId, const glm::mat4& matrixValue);
		static void applyShaderVector3(int32 uniformId, const glm::vec3& value);
		static void applyShaderFloat(int32 uniformId, float32 value);
		static void applyShaderInt(int32 uniformId, int32 value);
		static void applyShaderInt2(int32 uniformId, const glm::ivec2& value);
		static void applyShaderUInt(int32 uniformId, uint32 value);
		static void applyShaderUInt2(int32 uniformId, const glm::uvec2& value);
		static void applyShaderBool(int32 uniformId, bool value);
	private:
		uint32                                       programID, vertexShaderID, fragmentShaderID;
		int32                                        attributeCount;
		std::unordered_map<std::string, UniformData> m_uniformMap;
		void compileShaderFromFilePath(const std::string& filePath, uint32 ID);
		bool compileShaderSourceCode(const std::string& srcCode, uint32 ID);
};
