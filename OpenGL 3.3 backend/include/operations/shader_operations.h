/* shader_operations.h
 * Compiles shaders and logs errors if any
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

namespace renderer::graphics_lib::operations
{
/*
@brief Compiles and links shaders
*/
bool makeShader(const std::string &vertexShader, const std::string &fragmentShader, unsigned int &shaderId, unsigned int &vertexShaderId, unsigned int &fragmentShaderId);
}
