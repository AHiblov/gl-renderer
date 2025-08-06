/* commandline_parser.h
 * Commandline arguments analyzer
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "app_parameters.h"

namespace renderer::utils
{

/*
@brief Parses commandline and fills in the structure
*/
bool parseCommandline(int argc, const char **argv, renderer::AppParameters &parameters);

}
