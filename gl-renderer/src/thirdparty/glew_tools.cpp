/* glew_tools.cpp
 * GLEW library initialization
 *
 * Author: Artem Hiblov
 */

#include "thirdparty/glew_tools.h"

#include "log.h"

using namespace renderer;
using namespace renderer::thirdparty;

bool renderer::thirdparty::initGlew()
{
	if(glewInit() != GLEW_OK)
	{
		Log::getInstance().error("Can't initialize GLEW");
		return false;
	}

	return true;
}