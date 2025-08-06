/* copy_back_instance.cpp
 * Editor command for copying modified/inserted instance back into scene
 *
 * Author: Artem Hiblov
 */

#include "editor_commands/copy_back_instance.h"

using namespace renderer::editor_commands;

CopyBackInstance::CopyBackInstance(EditorSceneModifier *sceneModifier, int chunkIdx, int objectIdx, int instanceIdx, const float *selectedInst):
	editorSceneModifier(sceneModifier), chunkIndex(chunkIdx), objectIndex(objectIdx), instanceIndex(instanceIdx), selectedInstance(selectedInst)
{
}

CopyBackInstance::~CopyBackInstance()
{
	editorSceneModifier = nullptr;
	selectedInstance = nullptr;
}

void CopyBackInstance::execute()
{
	if(!editorSceneModifier)
		return;

	editorSceneModifier->copyBackSelectedInstance(chunkIndex, objectIndex, instanceIndex, selectedInstance);
}
