/* main.cpp
 * 3D renderer entry point
 *
 * Author: Artem Hiblov
 */

#include "log.h"
#include "main_component.h"
#include "graphics_lib/backend_teller.h"
#include "utils/commandline_parser.h"

using namespace std;
using namespace renderer;
using namespace renderer::graphics_lib;
using namespace renderer::utils;

int main(int argc, const char **argv)
{
	AppParameters appParameters;

	bool status = parseCommandline(argc, argv, appParameters);
	if(!status)
	{
		Log::getInstance().error("Can't parse commandline arguments");
		return 1;
	}

	{
		Log::getInstance().info(string("Graphics library: ") + getGraphicsLibraryString());

		MainComponent mainComponent(appParameters);
		status = mainComponent.initBase();
		if(!status)
			return 1;

		mainComponent.showSplash();

		status = mainComponent.initAll();
		if(!status)
			return 1;

		mainComponent.run();
	}

	Log::getInstance().info("The application is deinitialized normally");
    return 0;
}
