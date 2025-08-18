/* commandline_parser.cpp
 * Commandline arguments analyzer
 *
 * Author: Artem Hiblov
 */

#include "utils/commandline_parser.h"

#include <cstring>
#include <string>

#include "log.h"

using namespace renderer;
using namespace renderer::utils;

namespace
{
	const char *ARGUMENT_SCENE = "scene";
	const char *ARGUMENT_RESOLUTION = "resolution";
	const char *ARGUMENT_FULLSCREEN = "fullscreen";
	const char *ARGUMENT_SMOOTH = "smooth";
	const char *ARGUMENT_DEBUG = "debug";
	const char *ARGUMENT_EDITOR = "editor";
}

bool renderer::utils::parseCommandline(int argc, const char **argv, AppParameters &parameters)
{
	if(argc < 2) //Usually means there are no custom parameters at all
	{
		Log::getInstance().error("Path to scene is not provided");
		return false;
	}

	if(!argv)
	{
		Log::getInstance().error("No arguments provided");
		return false;
	}

	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], ARGUMENT_RESOLUTION) == 0)
		{
			if(i + 2 >= argc)
			{
				Log::getInstance().error("Not enough dimension parameters are provided");
				return false;
			}

			parameters.screenWidth = atoi(argv[i+1]);
			parameters.screenHeight = atoi(argv[i+2]);

			i += 2; //not += 3, because i++ will move index to the next argument
		}
		else if(strcmp(argv[i], ARGUMENT_SCENE) == 0)
		{
			if(i + 1 >= argc)
			{
				Log::getInstance().error("No scene path parameter is provided");
				return false;
			}

			parameters.scenePath = argv[i+1];

			i += 1; //i++ will move index to the next argument
		}
		else if(strcmp(argv[i], ARGUMENT_FULLSCREEN) == 0)
		{
			parameters.isFullScreen = true;
		}
		else if(strcmp(argv[i], ARGUMENT_SMOOTH) == 0)
		{
			parameters.useSmoothing = true;
		}
		else if(strcmp(argv[i], ARGUMENT_DEBUG) == 0)
		{
			parameters.enableDebug = true;
		}
		else if(strcmp(argv[i], ARGUMENT_EDITOR) == 0)
		{
			parameters.isEditorMode = true;
		}
		else Log::getInstance().warning(std::string("Unknown parameter \"") + argv[i] + "\", ignored");
	}

	return true;
}
