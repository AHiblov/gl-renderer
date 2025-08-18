/* message_callback.h
 * OpenGL 4.3+ message handler
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::graphics_lib
{

void graphicsLibMessageCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char *message, const void *userParam);

}
