/* log.h
 * Logging class implemented as Singleton
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <iostream>
#include <string>

namespace renderer
{

class Log
{
public:
	static Log& getInstance();

	void info(const std::string &message);
	void warning(const std::string &message);
	void error(const std::string &message);

private:
	Log();
	~Log();
};

}
