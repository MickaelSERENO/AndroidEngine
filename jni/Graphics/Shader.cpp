#include "Shader.h"

Shader::Shader() : m_programID(0), m_vertexID(0), m_fragID(0)
{}

Shader::~Shader()
{
	glDeleteProgram(m_programID);
	if(m_vertexID != 0)
		glDeleteShader(m_vertexID);
	if(m_fragID != 0)
		glDeleteShader(m_fragID);
}

Shader* Shader::loadFromFiles(File vertexFile, File fragFile)
{
	return loadFromFiles(vertexFile, fragFile, std::map<std::string, int>());
}

Shader* Shader::loadFromFiles(File vertexFile, File fragFile, const std::map<std::string, int>& attrib)
{
	std::string vertexCode;
	std::string fragCode;

	char* line;
	while((line = vertexFile.readLine()) != NULL)
	{
		if(line == NULL)
			break;
		vertexCode.append(line);	
		free(line);
	}

	while((line = fragFile.readLine()) != NULL)
	{
		if(line == NULL)
			break;
		fragCode.append(line);	
		free(line);
	}

	return loadFromStrings(vertexCode, fragCode, attrib);
}

Shader* Shader::loadFromStrings(const std::string& vertexString, const std::string& fragString)
{
	return loadFromStrings(vertexString, fragString, std::map<std::string, int>());
}

Shader* Shader::loadFromStrings(const std::string& vertexString, const std::string& fragString, const std::map<std::string, int>& attrib)
{
	Shader* shader = new Shader();
	shader->m_programID = glCreateProgram();
	shader->m_vertexID = loadShader(vertexString, GL_VERTEX_SHADER);
	shader->m_fragID = loadShader(fragString, GL_FRAGMENT_SHADER);

	glAttachShader(shader->m_programID, shader->m_vertexID);
	glAttachShader(shader->m_programID, shader->m_fragID);

	shader->bindAttribLocation(attrib);
	glLinkProgram(shader->m_programID);
	int linkStatus;
	glGetProgramiv(shader->m_programID, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == GL_FALSE)
	{
		char* error = (char*) malloc(ERROR_MAX_LENGTH * sizeof(char));
		int length=0;
		glGetProgramInfoLog(shader->m_programID, ERROR_MAX_LENGTH, &length, error);
		LOG_ERROR("Could not link shader-> : \n %s", error);

		delete shader;
		return NULL;
	}

	return shader;
}

int Shader::loadShader(const std::string& code, int type)
{
	int shader = glCreateShader(type);
	const GLchar* s = code.c_str();
	glShaderSource(shader, 1, &s, 0);
	glCompileShader(shader);

	int compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if(compiled == GL_FALSE)
	{
		char* error = (char*) malloc(ERROR_MAX_LENGTH * sizeof(char));
		int length=0;
		glGetShaderInfoLog(shader, ERROR_MAX_LENGTH, &length, error);

		LOG_ERROR("Could not compile shader %d : \n %s", type, error);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void Shader::bindAttribLocation(const std::map<std::string, int>& attrib)
{
	for(std::map<std::string, int>::const_iterator it = attrib.begin(); it != attrib.end(); it++)
		glBindAttribLocation(m_programID, it->second, it->first.c_str());
}

int Shader::getProgramID()
{
	return m_programID;
}

int Shader::getVertexID()
{
	return m_vertexID;
}

int Shader::getFragID()
{
	return m_fragID;
}