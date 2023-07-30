#include "RayTracer/ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <vector>

ShaderProgram::ShaderProgram()
{
	programID = 0;
	vertexShaderID = 0;
	fragmentShaderID = 0;
	attributeCount = 0;
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::compileShaders(const std::string & vertexShaderPath, const std::string & fragmentShaderPath)
{
	programID = gl::glCreateProgram();
	vertexShaderID = gl::glCreateShader(gl::GL_VERTEX_SHADER);
	if (vertexShaderID == 0)
		std::cout << "ERROR : Vertex shader creation";
	fragmentShaderID = gl::glCreateShader(gl::GL_FRAGMENT_SHADER);
	if (vertexShaderID == 0)
		std::cout << "ERROR : Fragment shader creation";
	compileShader(vertexShaderPath, vertexShaderID);
	compileShader(fragmentShaderPath, fragmentShaderID);
}

void ShaderProgram::linkShaders()
{
	gl::glAttachShader(programID, vertexShaderID);
	gl::glAttachShader(programID, fragmentShaderID);

	gl::glLinkProgram(programID);
	gl::GLint isLinked = 0;
	gl::glGetProgramiv(programID, gl::GL_LINK_STATUS, &isLinked);
	if (isLinked == static_cast<gl::GLint>(gl::GL_FALSE))
	{
		gl::GLint maxLength = 0;
		gl::glGetProgramiv(programID, gl::GL_INFO_LOG_LENGTH, (int32 *)&isLinked);
		std::vector<char> infoLog(maxLength);
		std::cout << &(infoLog[0]);
		gl::glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);
		gl::glDeleteProgram(programID);
		gl::glDeleteShader(vertexShaderID);
		gl::glDeleteShader(fragmentShaderID);
	}
	else
	{
		std::cout << "\nLinked successfully";
	}

	gl::glDetachShader(programID, vertexShaderID);
	gl::glDetachShader(programID, fragmentShaderID);
	gl::glDeleteShader(vertexShaderID);
	gl::glDeleteShader(fragmentShaderID);
}

void ShaderProgram::addAttribute(const std::string & attributeName)
{
	gl::glBindAttribLocation(programID, attributeCount++, attributeName.c_str());
}

void ShaderProgram::use()
{
	gl::glUseProgram(programID);
	for (int32 i = 0; i < attributeCount; i++)
	{
		gl::glEnableVertexAttribArray(i);
	}
}

void ShaderProgram::unuse()
{
	gl::glUseProgram(0);
	for (int32 i = 0; i < attributeCount; i++)
	{
		gl::glDisableVertexAttribArray(i);
	}
}

gl::GLint ShaderProgram::getUniformLocation(const std::string & uniformName)const
{
	return gl::glGetUniformLocation(programID, uniformName.c_str());
}

void ShaderProgram::applyShaderUniformMatrix(int32 uniformId, const glm::mat4& matrixValue)
{
	gl::glUniformMatrix4fv(uniformId, 1, gl::GL_FALSE, &matrixValue[0][0]);
}

void ShaderProgram::applyShaderVector3(int32 uniformId, const glm::vec3& value)
{
	gl::glUniform3f(uniformId, value.x, value.y, value.z);
}

void ShaderProgram::applyShaderFloat(int32 uniformId, float32 value)
{
	gl::glUniform1f(uniformId, value);
}

void ShaderProgram::applyShaderInt(int32 uniformId, int32 value)
{
	gl::glUniform1i(uniformId, value);
}

void ShaderProgram::applyShaderBool(int32 uniformId, bool value)
{
	gl::glUniform1i(uniformId, value);
}

void ShaderProgram::compileShader(const std::string & filePath, uint32 ID)
{
	std::ifstream shaderFile(filePath);
	if (shaderFile.fail())
	{
		perror(filePath.c_str());
		std::cout << "ERROR : file : " << filePath << " couldn't be loaded";
	}
	std::string fileContents = "";
	std::string line;
	while (std::getline(shaderFile, line))
	{
		fileContents += line + "\n";
	}
	shaderFile.close();
	const char* charPointer = fileContents.c_str();
	gl::glShaderSource(ID, 1, &charPointer, nullptr);
	gl::glCompileShader(ID);

	gl::GLint success = 0;
	gl::glGetShaderiv(ID, gl::GL_COMPILE_STATUS, &success);

	if (success == static_cast<gl::GLint>(gl::GL_FALSE))
	{
		gl::GLint maxLength = 0;
		gl::glGetShaderiv(ID, gl::GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		gl::glGetShaderInfoLog(ID, maxLength, &maxLength, &errorLog[0]);

		std::cout << filePath << " failed to compile";
		std::cout << &(errorLog[0]);

		gl::glDeleteShader(ID);
		return;
	}
	else
	{
		std::cout << "Compiled Correctly : " << filePath <<"\n";
	}
}