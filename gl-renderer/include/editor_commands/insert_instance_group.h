/* insert_instance_group.h
 * Editor command for instance group insertion
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

#include <glm/vec3.hpp>

#include "editor_scene_modifier.h"
#include "editor_commands/editor_command.h"
#include "managers/terrain_manager.h"

namespace renderer::editor_commands
{

class InsertInstanceGroup: public EditorCommand
{
public:
	InsertInstanceGroup(renderer::EditorSceneModifier *sceneModifier, int chunkIdx, const glm::vec3 &insertionPos, int fradius, renderer::managers::TerrainManager *terrainMgr, const std::string &objName,
		const std::string &shader);
	virtual ~InsertInstanceGroup();

	virtual void execute() override;

private:
	renderer::EditorSceneModifier *editorSceneModifier; //Non-owning pointer

	int chunkIndex;
	const glm::vec3 &insertionPosition;
	int radius;
	renderer::managers::TerrainManager *terrainManager;
	const std::string &objectName;
	const std::string &shaderFeature;
};

}
