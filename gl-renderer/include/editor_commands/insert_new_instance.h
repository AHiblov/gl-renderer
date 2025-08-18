/* insert_new_instance.h
 * Editor command for insertion one instance into scene
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

#include "editor_scene_modifier.h"
#include "editor_commands/editor_command.h"

namespace renderer::editor_commands
{

class InsertNewInstance: public EditorCommand
{
public:
	InsertNewInstance(renderer::EditorSceneModifier *sceneModifier, int chunkIdx, int objectIdx, int instanceIdx, float posX, float posY, float posZ, float rot, const std::string &objName,
		const std::string &shader);
	virtual ~InsertNewInstance();

	virtual void execute() override;

private:
	renderer::EditorSceneModifier *editorSceneModifier; //Non-owning pointer

	int chunkIndex;
	int objectIndex;
	int instanceIndex;
	float x;
	float y;
	float z;
	float rotation;
	const std::string &objectName;
	const std::string &shaderFeature;
};

}
