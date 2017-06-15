#include "ShaderProgram.h"

#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>

ShaderProgram::ShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	ResetAll();
	m_vertexShader = LoadShader(vertexShaderPath, GL_VERTEX_SHADER);
	m_fragmentShader = LoadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
	CreateProgram();
}

ShaderProgram::~ShaderProgram()
{
	DestroyAll();
}

ShaderProgram::ShaderProgram(ShaderProgram&& s)
{
	DestroyAll();
	m_vertexShader = s.m_vertexShader;
	m_fragmentShader = s.m_fragmentShader;
	m_program = s.m_program;
	s.ResetAll();
}

void ShaderProgram::DestroyAll()
{
	if (m_vertexShader != 0) {
		glDeleteShader(m_vertexShader);
	}
	if (m_fragmentShader != 0) {
		glDeleteShader(m_fragmentShader);
	}
	if (m_program != 0) {
		glDeleteProgram(m_program);
	}
	ResetAll();
}

void ShaderProgram::ResetAll()
{
	m_vertexShader = 0;
	m_fragmentShader = 0;
	m_program = 0;
}

GLuint ShaderProgram::LoadShader(const std::string& shaderPath, GLenum shaderType)
{
	std::ifstream shaderFile(shaderPath);
	
	if (!shaderFile.good()) {
		throw std::runtime_error("Unable to load shader " + shaderPath);
	}

	std::stringstream ss;
	ss << shaderFile.rdbuf();
	
	if (!ss.good()) {
		throw std::runtime_error("Shader bad content " + shaderPath);
	}

	auto shader = glCreateShader(shaderType);

	if (shader == 0) {
		throw std::runtime_error("Unable to create shader using glCreateShader " + shaderPath);
	}

	std::string&& content = ss.str();
	const char* shaderContent = content.c_str();

	glShaderSource(shader, 1, &shaderContent, nullptr);
	glCompileShader(shader);

	int compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE) {
		int errLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errLength);

		auto msg = std::unique_ptr<char[]>(new char[errLength]);
		glGetShaderInfoLog(shader, errLength, nullptr, msg.get());

		glDeleteShader(shader);

		throw std::runtime_error("Unable to compile shader using glCompileShader " + shaderPath + " Error msg: " + msg.get());
	}

	return shader;
}

void ShaderProgram::CreateProgram()
{
	m_program = glCreateProgram();

	if (m_program == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create OpenGL program");
	}

	glAttachShader(m_program, m_vertexShader);
	glAttachShader(m_program, m_fragmentShader);
	glLinkProgram(m_program);

	int linkStatus;
	glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE) {
		int errLength;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &errLength);

		auto msg = std::unique_ptr<char[]>(new char[errLength]);
		glGetProgramInfoLog(m_program, errLength, nullptr, msg.get());

		// clean up all memory
		DestroyAll();

		throw std::runtime_error("Unable to link program with shaders: " + std::string(msg.get()));
	}
}