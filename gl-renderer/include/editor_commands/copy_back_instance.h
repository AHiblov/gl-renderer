/* copy_back_instance.h
 * Editor command for copying modified/inserted instance back into scene
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "editor_scene_modifier.h"
#include "editor_commands/editor_command.h"

namespace renderer::editor_commands
{

class CopyBackInstance: EditorCommand
{
public:
	CopyBackInstance(renderer::EditorSceneModifier *sceneModifier, int chunkIdx, int objectIdx, int instanceIdx, const float *selectedInst);
	virtual ~CopyBackInstance();

	virtual void execute() override;

private:
	renderer::EditorSceneModifier *editorSceneModifier; //Non-owning pointer

	int chunkIndex;
	int objectIndex;
	int instanceIndex;
	const float *selectedInstance; //Non-owning pointer
};

}
