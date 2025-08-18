/* message_callback.cpp
 * OpenGL 4.3+ message handler
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/message_callback.h"

#include <string>

#include <GL/glew.h>

#include "log.h"

using namespace std;
using namespace renderer::graphics_lib;

void renderer::graphics_lib::graphicsLibMessageCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char *message, const void *userParam)
{
	string fullMessage("Library: ");

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		fullMessage += "(API)";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		fullMessage += "(window system)";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		fullMessage += "(shader compiler)";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		fullMessage += "(third party)";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		fullMessage += "(application)";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		fullMessage += "(other)";
		break;
	}

	fullMessage += " ";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		fullMessage += "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		fullMessage += "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		fullMessage += "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		fullMessage += "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		fullMessage += "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_MARKER:
		fullMessage += "MARKER";
		break;
	case GL_DEBUG_TYPE_OTHER:
		fullMessage += "OTHER";
		break;
	}

	fullMessage += ": ";
	fullMessage += to_string(id);
	fullMessage += ": ";
	fullMessage += message;

	Log::getInstance().info(fullMessage);
}
