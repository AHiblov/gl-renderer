/* shader_loader.cpp
 * Stuff for reading and parsing shader description and shader files
 *
 * Author: Artem Hiblov
 */

#include "loaders/shader_loader.h"

#include <fstream>
#include <sstream>

#include "log.h"

using namespace std;

bool renderer::loaders::loadShader(const string &path, string &code)
{
	if(path.empty())
	{
		Log::getInstance().error("Shader file path is not provided");
		return false;
	}

	ifstream data(path);
	if(!data.is_open())
	{
		Log::getInstance().error(path + " can't be opened");
		return false;
	}

	stringstream buffer;
	buffer << data.rdbuf();
	code = buffer.str();

	data.close();
	return true;
}
