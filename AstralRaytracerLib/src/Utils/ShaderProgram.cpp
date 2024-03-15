#include "Utils/ShaderProgram.h"

#include "Utils/Common.h"

#include <vector>

namespace AstralRaytracer
{
	void ShaderProgram::compileShaders(
			const std::string& vertexShaderPath,
			const std::string& fragmentShaderPath
	)
	{
		Errors::GenericError error= Errors::GenericError::INVALID;
		initShader(error);

		compileShaderFromFilePath(vertexShaderPath, vertexShaderID);
		compileShaderFromFilePath(fragmentShaderPath, fragmentShaderID);
	}

	void ShaderProgram::compileShadersFromSrcCode(
			const std::string& vertexShaderSrcCode,
			const std::string& fragmentShaderSrcCode
	)
	{
		Errors::GenericError error= Errors::GenericError::INVALID;
		initShader(error);

		if(!compileShaderSourceCode(vertexShaderSrcCode, vertexShaderID))
		{
			ASTRAL_LOG_ERROR("Vertex shader compilation failed");
		}
		if(!compileShaderSourceCode(fragmentShaderSrcCode, fragmentShaderID))
		{
			ASTRAL_LOG_ERROR("Fragment shader compilation failed");
		}
	}

	void ShaderProgram::linkShaders()
	{
		gl::glAttachShader(programID, vertexShaderID);
		gl::glAttachShader(programID, fragmentShaderID);

		gl::glLinkProgram(programID);
		gl::GLint isLinked= 0;
		gl::glGetProgramiv(programID, gl::GL_LINK_STATUS, &isLinked);
		if(isLinked == static_cast<gl::GLint>(gl::GL_FALSE))
		{
			gl::GLint maxLength= 0;
			gl::glGetProgramiv(programID, gl::GL_INFO_LOG_LENGTH, (int32*)&isLinked);
			std::vector<char> infoLog(maxLength);
			ASTRAL_LOG_DEBUG(&(infoLog[0]));
			gl::glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);
			gl::glDeleteProgram(programID);
			gl::glDeleteShader(vertexShaderID);
			gl::glDeleteShader(fragmentShaderID);
		}
		else
		{
			ASTRAL_LOG_TRACE("Shader linked successfully");
		}

		gl::glDetachShader(programID, vertexShaderID);
		gl::glDetachShader(programID, fragmentShaderID);
		gl::glDeleteShader(vertexShaderID);
		gl::glDeleteShader(fragmentShaderID);
	}

	void ShaderProgram::addAttribute(const std::string& attributeName)
	{
		gl::glBindAttribLocation(programID, attributeCount++, attributeName.c_str());
	}

	void ShaderProgram::use() const
	{
		gl::glUseProgram(programID);
		for(int32 i= 0; i < attributeCount; i++)
		{
			gl::glEnableVertexAttribArray(i);
		}
	}

	void ShaderProgram::unuse() const
	{
		gl::glUseProgram(0);
		for(int32 i= 0; i < attributeCount; i++)
		{
			gl::glDisableVertexAttribArray(i);
		}
	}

	const std::unordered_map<std::string, UniformData>& ShaderProgram::getUniformData() const
	{
		return m_uniformMap;
	}

	gl::GLint ShaderProgram::getUniformLocation(const std::string& uniformName) const
	{
		return gl::glGetUniformLocation(programID, uniformName.c_str());
	}

	void ShaderProgram::setUniformValue(
			const std::string&     uniformName,
			const UniformDataType& data,
			float32                min,
			float32                max
	)
	{
		uint32 uniformId= 0;

		auto it= m_uniformMap.find(uniformName);
		if(it != m_uniformMap.end())
		{
			uniformId= it->second.uniformId;
		}
		else
		{
			uniformId                = getUniformLocation(uniformName);
			m_uniformMap[uniformName]= { uniformId, min, max, data };
		}

		m_uniformMap[uniformName]= { uniformId, min, max, data };

		updateUniformData(uniformName, data);
	}

	void ShaderProgram::updateUniformData(const std::string& uniformName, const UniformDataType& data)
	{
		const int32 uniformId= m_uniformMap[uniformName].uniformId;

		m_uniformMap[uniformName].data= data;

		using namespace AstralRaytracer;

		if(std::get_if<int32>(&data))
		{
			applyShaderInt(uniformId, std::get<int32>(data));
			return;
		}

		if(std::get_if<uint32>(&data))
		{
			applyShaderUInt(uniformId, std::get<uint32>(data));
			return;
		}

		if(std::get_if<float32>(&data))
		{
			applyShaderFloat(uniformId, std::get<float32>(data));
			return;
		}
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

	void ShaderProgram::applyShaderInt2(int32 uniformId, const glm::ivec2& value)
	{
		gl::glUniform2i(uniformId, value.x, value.y);
	}

	void ShaderProgram::applyShaderUInt(int32 uniformId, uint32 value)
	{
		gl::glUniform1ui(uniformId, value);
	}

	void ShaderProgram::applyShaderUInt2(int32 uniformId, const glm::uvec2& value)
	{
		gl::glUniform2ui(uniformId, value.x, value.y);
	}

	void ShaderProgram::applyShaderBool(int32 uniformId, bool value)
	{
		gl::glUniform1i(uniformId, value);
	}

	uint32 ShaderProgram::createProgram(Errors::GenericError& error) const
	{
		uint32 programID= gl::glCreateProgram();
		error= (programID == 0) ? Errors::GenericError::FAILURE : Errors::GenericError::SUCCESS;
		return programID;
	}

	uint32 ShaderProgram::createVertexShader(Errors::GenericError& error) const
	{
		uint32 shaderID= gl::glCreateShader(gl::GL_VERTEX_SHADER);
		error= (shaderID == 0) ? Errors::GenericError::FAILURE : Errors::GenericError::SUCCESS;
		return shaderID;
	}

	uint32 ShaderProgram::createFragmentShader(Errors::GenericError& error) const
	{
		uint32 shaderID= gl::glCreateShader(gl::GL_FRAGMENT_SHADER);
		error= (shaderID == 0) ? Errors::GenericError::FAILURE : Errors::GenericError::SUCCESS;
		return shaderID;
	}

	void ShaderProgram::initShader(Errors::GenericError& error)
	{
		Errors::GenericError programError= Errors::GenericError::INVALID;
		Errors::GenericError vsError     = Errors::GenericError::INVALID;
		Errors::GenericError fsError     = Errors::GenericError::INVALID;

		programID= createProgram(programError);
		if(programError != Errors::GenericError::SUCCESS)
		{
			ASTRAL_LOG_ERROR("Program creation failed");
		}

		vertexShaderID= createVertexShader(vsError);
		if(vsError != Errors::GenericError::SUCCESS)
		{
			ASTRAL_LOG_ERROR("Vertex shader creation failed");
		}

		fragmentShaderID= createFragmentShader(fsError);
		if(fsError != Errors::GenericError::SUCCESS)
		{
			ASTRAL_LOG_ERROR("Fragment shader creation failed");
		}

		error= std::max({ programError, vsError, fsError });

		ASTRAL_ASSERTM(error == Errors::GenericError::SUCCESS, "Shader creation failed");
	}

	void ShaderProgram::compileShaderFromFilePath(const std::string& filePath, uint32 ID)
	{
		std::string fileContents;

		Errors::GenericError error= getFileContent(filePath, fileContents);

		if(error != Errors::GenericError::SUCCESS)
		{
			ASTRAL_LOG_ERROR("FilePath " + filePath + " couldn't be loaded");
			return;
		}

		if(compileShaderSourceCode(fileContents, ID))
		{
			ASTRAL_LOG_TRACE(filePath + " compiled successfully");
			return;
		}

		ASTRAL_LOG_TRACE(filePath + " failed to compile");
	}

	bool ShaderProgram::compileShaderSourceCode(const std::string& srcCode, uint32 ID)
	{
		const char* charPointer= srcCode.c_str();
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
			ASTRAL_LOG_DEBUG(&(errorLog[0]));

			gl::glDeleteShader(ID);
			return false;
		}

		return true;
	}
} // namespace AstralRaytracer