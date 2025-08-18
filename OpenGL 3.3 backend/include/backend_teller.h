/* backend_teller.h
 * Reports graphics library name and version
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::graphics_lib
{

const char* getGraphicsLibraryString()
{
	return "OpenGL 3.3";
}

}
