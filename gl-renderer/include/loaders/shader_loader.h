/* shader_loader.h
 * Stuff for reading and parsing shader description and shader files
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>


namespace renderer::loaders
{

/*
@brief Reads shader from file
*/
bool loadShader(const std::string &path, std::string &code);

}