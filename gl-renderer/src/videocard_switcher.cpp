/* videocard_switcher.cpp
 * Enforces using dedicated nVidia videocard
 *
 * Author: Artem hiblov
 */

//Include this file only in Windows version of the project

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}