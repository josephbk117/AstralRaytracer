#pragma once
#include "Utils/Common.h"

#include <glbinding/gl/gl.h>
#include <glm.hpp>
#include <string>

class ShaderProgram
{
	public:
	ShaderProgram();
	~ShaderProgram();
	void compileShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void linkShaders();
	void addAttribute(const std::string& attributeName);
	void use();
	void unuse();
	gl::GLint   getUniformLocation(const std::string& uniformName) const;
	static void applyShaderUniformMatrix(int32 uniformId, const glm::mat4& matrixValue);
	static void applyShaderVector3(int32 uniformId, const glm::vec3& value);
	static void applyShaderFloat(int32 uniformId, float32 value);
	static void applyShaderInt(int32 uniformId, int32 value);
	static void applyShaderBool(int32 uniformId, bool value);

	private:
	uint32 programID, vertexShaderID, fragmentShaderID;
	void   compileShader(const std::string& filePath, uint32 ID);
	int32  attributeCount;
};
