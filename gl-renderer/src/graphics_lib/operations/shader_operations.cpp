/* shader_operations.cpp
 * Compiles shaders and logs errors if any
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/shader_operations.h"

#include <cstring>
#include <memory>

#include <GL/glew.h>

#include "log.h"

using namespace std;
using namespace renderer;
using namespace renderer::graphics_lib;

namespace
{
	enum EShaderType
	{
		shader_vertex,
		shader_fragment
	};

	/*
	@brief Compiles shader
	*/
	bool compileShader(const string &source, EShaderType type, unsigned int &id);

	/*
	@brief Links vertex and fragment shader
	*/
	bool linkShader(unsigned int vertexShaderId, unsigned int fragmentShaderId, unsigned int &linkedShaderId);
}

bool renderer::graphics_lib::operations::makeShader(const string &vertexShader, const string &fragmentShader, unsigned int &shaderId, unsigned int &vertexShaderId, unsigned int &fragmentShaderId)
{
	if(vertexShader.empty() || fragmentShader.empty())
	{
		Log::getInstance().error("Shader source code is not provided");
		return false;
	}

	unsigned int compiledVertexShader = -1u, compiledFragmentShader = -1u;

	if(!compileShader(vertexShader, shader_vertex, compiledVertexShader))
	{
		Log::getInstance().error("Vertex shader compiling error");
		return false;
	}
	if(!compileShader(fragmentShader, shader_fragment, compiledFragmentShader))
	{
		Log::getInstance().error("Fragment shader compiling error");
		return false;
	}

	unsigned int newShaderId = -1u;
	if(!linkShader(compiledVertexShader, compiledFragmentShader, newShaderId))
	{
		Log::getInstance().error("Can't link shaders");
		return false;
	}

	shaderId = newShaderId;
	vertexShaderId = compiledVertexShader;
	fragmentShaderId = compiledFragmentShader;

	return true;
}

namespace
{
	bool compileShader(const string &source, EShaderType type, unsigned int &id)
	{
		if(source.empty())
		{
			Log::getInstance().error("Shader source is empty");
			return false;
		}

		unsigned int shaderId = -1u;

		shaderId = glCreateShader(type == shader_vertex ? GL_VERTEX_SHADER: GL_FRAGMENT_SHADER);
		const char *sourceCPtr = source.c_str();
		glShaderSource(shaderId, 1, &sourceCPtr, nullptr);
		glCompileShader(shaderId);

		int compileStatus = GL_FALSE;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
		if(compileStatus == GL_FALSE)
		{
			int messageLength = 0;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &messageLength);

			unique_ptr<char[]> errorMessage(new char[messageLength + 1]);
			memset(errorMessage.get(), 0, messageLength + 1);

			glGetShaderInfoLog(shaderId, messageLength, nullptr, errorMessage.get());
			Log::getInstance().error(string((type == shader_vertex ? "Vertex": "Fragment")) + " shader compilation error: " + errorMessage.get());

			return false;
		}

		id = shaderId;
		return true;
	}

	bool linkShader(unsigned int vertexShaderId, unsigned int fragmentShaderId, unsigned int &linkedShaderId)
	{
		unsigned int programId = glCreateProgram();
		glAttachShader(programId, vertexShaderId);
		glAttachShader(programId, fragmentShaderId);
		glLinkProgram(programId);

		int linkStatus = GL_FALSE;
		glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
		if(linkStatus == GL_FALSE)
		{
			int messageLength = 0;
			glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &messageLength);

			unique_ptr<char[]> errorMessage(new char[messageLength + 1]);
			memset(errorMessage.get(), 0, messageLength + 1);

			glGetProgramInfoLog(programId, messageLength, nullptr, errorMessage.get());
			Log::getInstance().error(string("Link error: ") + errorMessage.get());

			return false;
		}

		linkedShaderId = programId;
		return true;
	}
}
