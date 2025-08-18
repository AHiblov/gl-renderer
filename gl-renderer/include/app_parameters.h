/* app_parameters.h
 * Parameters which configure how renderer works
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

namespace renderer
{

struct AppParameters
{
	AppParameters():
		screenWidth(1366), screenHeight(768), isFullScreen(false), useSmoothing(false), enableDebug(false), isEditorMode(false)
	{
	}

	unsigned int screenWidth;
	unsigned int screenHeight;
	std::string scenePath;
	bool isFullScreen;
	bool useSmoothing;
	bool enableDebug;
	bool isEditorMode;
};

}
