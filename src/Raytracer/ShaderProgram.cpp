#include "Raytracer/ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace AstralRaytracer
{
	void BaseShaderProgram::applyShaderUniformMatrix(int32 uniformId, const glm::mat4& matrixValue)
	{
		gl::glUniformMatrix4fv(uniformId, 1, gl::GL_FALSE, &matrixValue[0][0]);
	}

	void BaseShaderProgram::applyShaderVector3(int32 uniformId, const glm::vec3& value)
	{
		gl::glUniform3f(uniformId, value.x, value.y, value.z);
	}

	void BaseShaderProgram::applyShaderFloat(int32 uniformId, float32 value)
	{
		gl::glUniform1f(uniformId, value);
	}

	void BaseShaderProgram::applyShaderInt(int32 uniformId, int32 value)
	{
		gl::glUniform1i(uniformId, value);
	}

	void BaseShaderProgram::applyShaderBool(int32 uniformId, bool value)
	{
		gl::glUniform1i(uniformId, value);
	}

	gl::GLint BaseShaderProgram::getUniformLocation(const std::string& uniformName) const
	{
		return gl::glGetUniformLocation(m_programId, uniformName.c_str());
	}

} // namespace AstralRaytracer

namespace AstralRaytracer
{

	ShaderProgramVSFS::ShaderProgramVSFS()
	{
		vertexShaderID  = 0;
		fragmentShaderID= 0;
		attributeCount  = 0;
	}

	ShaderProgramVSFS::~ShaderProgramVSFS() {}

	void ShaderProgramVSFS::compileShadersVSFS(const std::string& vertexShaderPath,
																						 const std::string& fragmentShaderPath)
	{
		m_programId   = gl::glCreateProgram();
		vertexShaderID= gl::glCreateShader(gl::GL_VERTEX_SHADER);
		if(vertexShaderID == 0)
			std::cout << "ERROR : Vertex shader creation";
		fragmentShaderID= gl::glCreateShader(gl::GL_FRAGMENT_SHADER);
		if(vertexShaderID == 0)
			std::cout << "ERROR : Fragment shader creation";
		compile(vertexShaderPath, vertexShaderID);
		compile(fragmentShaderPath, fragmentShaderID);
	}

	void ShaderProgramVSFS::linkShaders()
	{
		gl::glAttachShader(m_programId, vertexShaderID);
		gl::glAttachShader(m_programId, fragmentShaderID);

		gl::glLinkProgram(m_programId);
		gl::GLint isLinked= 0;
		gl::glGetProgramiv(m_programId, gl::GL_LINK_STATUS, &isLinked);
		if(isLinked == static_cast<gl::GLint>(gl::GL_FALSE))
		{
			gl::GLint maxLength= 0;
			gl::glGetProgramiv(m_programId, gl::GL_INFO_LOG_LENGTH, (int32*)&isLinked);
			std::vector<char> infoLog(maxLength);
			std::cout << &(infoLog[0]);
			gl::glGetProgramInfoLog(m_programId, maxLength, &maxLength, &infoLog[0]);
			gl::glDeleteProgram(m_programId);
			gl::glDeleteShader(vertexShaderID);
			gl::glDeleteShader(fragmentShaderID);
		}
		else
		{
			std::cout << "\nLinked successfully";
		}

		gl::glDetachShader(m_programId, vertexShaderID);
		gl::glDetachShader(m_programId, fragmentShaderID);
		gl::glDeleteShader(vertexShaderID);
		gl::glDeleteShader(fragmentShaderID);
	}

	void ShaderProgramVSFS::addAttribute(const std::string& attributeName)
	{
		gl::glBindAttribLocation(m_programId, attributeCount++, attributeName.c_str());
	}

	void ShaderProgramVSFS::use()
	{
		gl::glUseProgram(m_programId);
		for(int32 i= 0; i < attributeCount; i++)
		{
			gl::glEnableVertexAttribArray(i);
		}
	}

	void ShaderProgramVSFS::unuse()
	{
		gl::glUseProgram(0);
		for(int32 i= 0; i < attributeCount; i++)
		{
			gl::glDisableVertexAttribArray(i);
		}
	}

	void ShaderProgramVSFS::compile(const std::string& filePath, uint32 ID)
	{
		std::ifstream shaderFile(filePath);
		if(shaderFile.fail())
		{
			perror(filePath.c_str());
			std::cout << "ERROR : file : " << filePath << " couldn't be loaded";
		}
		std::string fileContents= "";
		std::string line;
		while(std::getline(shaderFile, line))
		{
			fileContents+= line + "\n";
		}
		shaderFile.close();
		const char* charPointer= fileContents.c_str();
		gl::glShaderSource(ID, 1, &charPointer, nullptr);
		gl::glCompileShader(ID);

		gl::GLint success= 0;
		gl::glGetShaderiv(ID, gl::GL_COMPILE_STATUS, &success);

		if(success == static_cast<gl::GLint>(gl::GL_FALSE))
		{
			gl::GLint maxLength= 0;
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
			std::cout << "Compiled Correctly : " << filePath << "\n";
		}
	}
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	void ComputeShaderProgram::compileAndLinkShader(const std::string& computeShaderPath)
	{
		m_programId           = gl::glCreateProgram();
		uint32 computeShaderId= gl::glCreateShader(gl::GL_COMPUTE_SHADER);
		if(computeShaderId == 0)
			std::cout << "ERROR : Compute shader creation";
		compile(computeShaderPath, computeShaderId);
		link(computeShaderId);
	}

	void ComputeShaderProgram::compile(const std::string& filePath, uint32 id)
	{
		std::ifstream shaderFile(filePath);
		if(shaderFile.fail())
		{
			perror(filePath.c_str());
			std::cout << "ERROR : file : " << filePath << " couldn't be loaded";
		}
		std::string fileContents= "";
		std::string line;
		while(std::getline(shaderFile, line))
		{
			fileContents+= line + "\n";
		}
		shaderFile.close();
		const char* charPointer= fileContents.c_str();
		gl::glShaderSource(id, 1, &charPointer, nullptr);
		gl::glCompileShader(id);

		gl::GLint success= 0;
		gl::glGetShaderiv(id, gl::GL_COMPILE_STATUS, &success);

		if(success == static_cast<gl::GLint>(gl::GL_FALSE))
		{
			gl::GLint maxLength= 0;
			gl::glGetShaderiv(id, gl::GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char> errorLog(maxLength);
			gl::glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

			std::cout << filePath << " failed to compile";
			std::cout << &(errorLog[0]);

			gl::glDeleteShader(id);
			return;
		}
		else
		{
			std::cout << "Compiled Correctly : " << filePath << "\n";
		}
	}

	void ComputeShaderProgram::link(uint32 shaderId)
	{
		gl::glAttachShader(m_programId, shaderId);
		gl::glLinkProgram(m_programId);
		gl::GLint isLinked= 0;
		gl::glGetProgramiv(m_programId, gl::GL_LINK_STATUS, &isLinked);
		if(isLinked == static_cast<gl::GLint>(gl::GL_FALSE))
		{
			gl::GLint maxLength= 0;
			gl::glGetProgramiv(m_programId, gl::GL_INFO_LOG_LENGTH, (int32*)&isLinked);
			std::vector<char> infoLog(maxLength);
			std::cout << &(infoLog[0]);
			gl::glGetProgramInfoLog(m_programId, maxLength, &maxLength, &infoLog[0]);
			gl::glDeleteProgram(m_programId);
			gl::glDeleteShader(shaderId);
		}
		else
		{
			std::cout << "\nLinked successfully";
		}

		gl::glDetachShader(m_programId, shaderId);
		gl::glDeleteShader(shaderId);
	}

	void ComputeShaderProgram::use() 
	{
		gl::glUseProgram(m_programId);
	}

	void ComputeShaderProgram::unuse() {}

	void ComputeShaderProgram::dispatch(uint32 x, uint32 y, uint32 z) 
	{
		gl::glDispatchCompute(x, y, z);
		gl::glMemoryBarrier(gl::MemoryBarrierMask::GL_ALL_BARRIER_BITS);
	}

} // namespace AstralRaytracer