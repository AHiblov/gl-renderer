/* insert_new_instance.cpp
 * Editor command for insertion one instance into scene
 *
 * Author: Artem Hiblov
 */

#include "editor_commands/insert_new_instance.h"

using namespace std;

using namespace renderer;
using namespace renderer::editor_commands;

InsertNewInstance::InsertNewInstance(EditorSceneModifier *sceneModifier, int chunkIdx, int objectIdx, int instanceIdx, float posX, float posY, float posZ, float rot, const string &objName,
	const string &shader):
	editorSceneModifier(sceneModifier), chunkIndex(chunkIdx), objectIndex(objectIdx), instanceIndex(instanceIdx), x(posX), y(posY), z(posZ), rotation(rot), objectName(objName),
	shaderFeature(shader)
{
}

InsertNewInstance::~InsertNewInstance()
{
	editorSceneModifier = nullptr;
}

void InsertNewInstance::execute()
{
	if(!editorSceneModifier)
		return;

	editorSceneModifier->insertNewInstanceIntoScene(chunkIndex, objectIndex, instanceIndex, x, y, z, rotation, objectName, shaderFeature);
}
