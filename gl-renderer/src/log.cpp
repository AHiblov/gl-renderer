/* log.cpp
 * Logging class implemented as Singleton
 *
 * Author: Artem Hiblov
 */

#include "log.h"

using namespace std;
using namespace renderer;

Log& Log::getInstance()
{
	static Log instance; //Constructed on the first call

	return instance;
}

void Log::info(const string &message)
{
	cout << message << endl;
}

void Log::warning(const string &message)
{
	cout << "Warning: " << message << endl;
}

void Log::error(const string &message)
{
	cout << "Error: " << message << endl;
}

Log::Log()
{
}

Log::~Log()
{
}
