/* set_camera_position.cpp
 * Editor command for setting initial camera position
 *
 * Author: Artem Hiblov
 */

#include "editor_commands/set_camera_position.h"

using namespace renderer;
using namespace renderer::editor_commands;

SetCameraPosition::SetCameraPosition(EditorSceneModifier *sceneModifier, const glm::vec3 &camPos, float horizontalRotation, float verticalRotation):
	editorSceneModifier(sceneModifier), cameraPosition(camPos), horizontalRotationRadians(horizontalRotation), verticalRotationRadians(verticalRotation)
{
}

SetCameraPosition::~SetCameraPosition()
{
	editorSceneModifier = nullptr;
}

void SetCameraPosition::execute()
{
	if(!editorSceneModifier)
		return;

	editorSceneModifier->setInitialCameraPosition(cameraPosition, horizontalRotationRadians, verticalRotationRadians);
}
