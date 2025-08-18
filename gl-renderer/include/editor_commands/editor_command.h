/* editor_command.h
 * Define interface for all the editor commands
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::editor_commands
{

struct EditorCommand
{
	virtual ~EditorCommand()
	{}

	virtual void execute() = 0;
};

}
