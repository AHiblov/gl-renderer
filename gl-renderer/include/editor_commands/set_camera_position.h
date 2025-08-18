/* set_camera_position.h
 * Editor command for setting initial camera position
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <glm/vec3.hpp>

#include "editor_scene_modifier.h"
#include "editor_commands/editor_command.h"

namespace renderer::editor_commands
{

class SetCameraPosition: public EditorCommand
{
public:
	SetCameraPosition(renderer::EditorSceneModifier *sceneModifier, const glm::vec3 &camPos, float horizontalRotation, float verticalRotation);
	virtual ~SetCameraPosition();

	virtual void execute() override;

private:
	renderer::EditorSceneModifier *editorSceneModifier; //Non-owning pointer

	const glm::vec3 &cameraPosition;
	float horizontalRotationRadians;
	float verticalRotationRadians;
};

}
