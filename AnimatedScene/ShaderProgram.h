#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#define NOMINMAX
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>

// Class for holding vertex/fragment shader
class ShaderProgram {
private:

	GLuint m_vertexShader;
	GLuint m_fragmentShader;
	GLuint m_program;

	void DestroyAll();

	// Reset all values to 0, doesn't destroy anything
	void ResetAll();

	static GLuint LoadShader(const std::string& shaderPath, GLenum shaderType);
	void CreateProgram();

public:

	ShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	virtual ~ShaderProgram();

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& s);

	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram& operator=(ShaderProgram&& s);

	void SetActive() const { glUseProgram(m_program); }
	void SetInactive() const { glUseProgram(0); }

	GLuint GetVertexShader() const { return m_vertexShader; }
	GLuint GetFragmentShader() const { return m_fragmentShader; }
	GLuint GetProgram() const { return m_program; }

	GLint GetUniformLocation(const std::string& name) const { return glGetUniformLocation(m_program, name.c_str()); }
	GLint GetAttribLocation(const std::string& name) const { return glGetAttribLocation(m_program, name.c_str()); }
	GLint GetUniformBlockIndex(const std::string& name) const { return glGetUniformBlockIndex(m_program, name.c_str()); }

	void UniformBlockBinding(GLint blockIndex, GLint blockBinding) const { glUniformBlockBinding(m_program, blockIndex, blockBinding); }
};

#endif