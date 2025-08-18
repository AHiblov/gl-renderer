/* shader_ids.h
 * Keeps shader program ID and related uniform IDs
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::graphics_lib::videocard_data
{

struct ShaderIds
{
	//Common part

	unsigned int id = -1u;

	unsigned int unifTextureId = -1u;

	//3D shaders

	unsigned int unifModel = -1u;
	unsigned int unifView = -1u;
	unsigned int unifProjection = -1u;

	//Directional light only
	unsigned int unifLightDirection = -1u;
	unsigned int unifRotation = -1u;
	unsigned int unifDiffuseLightColour = -1u;
	unsigned int unifAmbientLightColour = -1u;

	//Point light only
	unsigned int unifLightPosition = -1u;

	//Shader-specific
	unsigned int unifNormalTextureId = -1u; //Normalmap
	unsigned int unifTime = -1u; //Small waves
	unsigned int unifCameraPosition = -1u; //Glitter
	unsigned int unifMaterialAlpha = -1u; //Glitter
	unsigned int unifFogDensity = -1u; //Fog
	unsigned int unifFogColour = -1u; //Fog

	//Deferred
	unsigned int unifPositionComponentId = -1u;
	unsigned int unifNormalComponentId = -1u;
	unsigned int unifDiffuseComponentId = -1u;
	unsigned int unifScreenSize = -1u;
	unsigned int unifLightParameters = -1u;

	//2D shader

	unsigned int unifScreenRatio = -1u;


	//Light type
	bool isDirectionalLight = false;
};

}
