/* insert_instance_group.cpp
 * Editor command for instance group insertion
 *
 * Author: Artem Hiblov
 */

#include "editor_commands/insert_instance_group.h"

using namespace std;
using namespace renderer;
using namespace renderer::editor_commands;
using namespace renderer::managers;

InsertInstanceGroup::InsertInstanceGroup(EditorSceneModifier *sceneModifier, int chunkIdx, const glm::vec3 &insertionPos, int fradius, TerrainManager *terrainMgr, const string &objName,
	const string &shader):
	editorSceneModifier(sceneModifier), chunkIndex(chunkIdx), insertionPosition(insertionPos), radius(fradius), terrainManager(terrainMgr), objectName(objName), shaderFeature(shader)
{
}

InsertInstanceGroup::~InsertInstanceGroup()
{
	editorSceneModifier = nullptr;
}

void InsertInstanceGroup::execute()
{
	if(!editorSceneModifier)
		return;

	editorSceneModifier->insertInstanceGroupIntoScene(chunkIndex, insertionPosition, radius, terrainManager, objectName, shaderFeature);
}
