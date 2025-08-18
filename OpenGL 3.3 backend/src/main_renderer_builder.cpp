/* main_renderer_builder.cpp
 * Initializes all the shaders for ForardRenderer and DeferredRenderer classes
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/main_renderer_builder.h"

#include <GL/glew.h>

#include "common_constants.h"
#include "log.h"
#include "graphics_lib/deferred_renderer.h"
#include "graphics_lib/forward_renderer.h"
#include "graphics_lib/uniform_setters.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::managers;

namespace
{
	const char *SHADER_NAME_SKY = "sky";
	const char *DEFERRED_LIGHT_PASS_DIRECTIONAL_SHADER_NAME = "directional-light-pass";
	const char *DEFERRED_FOG_LIGHT_PASS_DIRECTIONAL_SHADER_NAME = "directional-fog-light-pass";
	const char *STENCIL_PASS_SHADER_NAME = "stencil";
	const char *DEFERRED_LIGHT_PASS_POINT_SHADER_NAME = "point-light-pass";

	//Common part
	const char *COLOUR_TEXTURE_UNIFORM_NAME = "colourTexture";

	//3D shaders

	const char *MODEL_UNIFORM_NAME = "model";
	const char *VIEW_UNIFORM_NAME = "view";
	const char *PROJECTION_UNIFORM_NAME = "projection";

	//Directional light only
	const char *ROTATION_UNIFORM_NAME = "rotation";
	const char *LIGHT_DIRECTION_UNIFORM_NAME = "lightDirection";
	const char *LIGHT_DIFFUSE_COLOUR_UNIFORM_NAME = "diffuseLightColour";
	const char *LIGHT_AMBIENT_COLOUR_UNIFORM_NAME = "ambientLightColour";

	//Point light only
	const char *LIGHT_POSITION_WORLDSPACE_UNIFORM_NAME = "lightPositionWld";

	//Shader-specific
	const char *NORMAL_TEXTURE_UNIFORM_NAME = "normalTexture";
	const char *TIME_UNIFORM_NAME = "time";
	const char *CAMERA_POSITION_UNIFORM_NAME = "cameraPosition";
	const char *MATERIAL_ALPHA_UNIFORM_NAME = "materialAlpha";
	const char *FOG_DENSITY_UNIFORM_NAME = "fogDensity";
	const char *FOG_COLOUR_UNIFORM_NAME = "fogColour";

	//Deferred shaders
	const char *POSITION_COMPONENT_UNIFORM_NAME = "positionComponent";
	const char *NORMAL_COMPONENT_UNIFORM_NAME = "normalComponent";
	const char *DIFFUSE_COMPONENT_UNIFORM_NAME = "diffuseComponent";
	const char *SCREEN_SIZE_UNIFORM_NAME = "screenSize";
	const char *LIGHT_PARAMETERS_UNIFORM_NAME = "lightParameters";

	const char *OBJECT_LIGHT_SPHERE_NAME = "light-sphere";

	constexpr float MATERIAL_ALPHA_X = 0.1f;
	constexpr float MATERIAL_ALPHA_Y = 0.1f;


	void initDirectionalShaderUniforms(ShaderIds &shaderId);
	void initPointShaderUniforms(ShaderIds &shaderId);
	void initNormalmapShaderUniforms(ShaderIds &shaderId);
	void initSmallWavesShaderUniforms(ShaderIds &shaderId);
	void initGlitterShaderUniforms(ShaderIds &shaderId);
	void initFogShaderUniforms(ShaderIds &shaderId);
	void initSkyShaderUniforms(ShaderIds &shaderId);
	void initStencilPassShaderUniforms(ShaderIds &shaderId);
	void initDeferredDirectionalLightPassShaderUniforms(ShaderIds &shaderId, bool useFog);
	void initDeferredPointLightPassShaderUniforms(ShaderIds &shaderId, bool useFog);
}

unique_ptr<Base3DRenderer> renderer::graphics_lib::buildMainRenderer(ObjectManager *objectManager, ShaderManager *shaderManager, int screenWidth, int screenHeight, const Light &light,
	const Fog &fog, const map<int, unsigned long long> &shaderFlags, vector<ShaderIds> &shaderIds, bool isDirectional, bool isDeferred)
{
	const float screenRatio = static_cast<float>(screenWidth) / screenHeight;

	for(auto &[index, flags]: shaderFlags)
	{
		ShaderIds &current = shaderIds[index];
		glUseProgram(current.id);

		current.unifTextureId = glGetUniformLocation(current.id, COLOUR_TEXTURE_UNIFORM_NAME);

		if(flags & ShaderFlags::FEATURE_DIRECTIONAL_LIGHT)
		{
			initDirectionalShaderUniforms(current);
			setDirectionalShaderUniforms(current, screenRatio, light);
		}

		if(flags & ShaderFlags::FEATURE_POINT_LIGHT)
		{
			initPointShaderUniforms(current);
			setPointShaderUniforms(current, screenRatio, light);
		}

		if(flags & ShaderFlags::FEATURE_NORMALMAP)
		{
			initNormalmapShaderUniforms(current);
		}

		if(flags & ShaderFlags::FEATURE_SMALL_WAVES)
		{
			initSmallWavesShaderUniforms(current);
		}

		if(flags & ShaderFlags::FEATURE_GLITTER)
		{
			initGlitterShaderUniforms(current);
			setGlitterShaderUniforms(current, MATERIAL_ALPHA_X, MATERIAL_ALPHA_Y);
		}

		if(flags | ShaderFlags::FEATURE_FOG) //Can be combined with other
		{
			initFogShaderUniforms(current);
			setFogShaderUniforms(current, fog);
		}
	}

	ShaderIds skyShader;
	shaderManager->getShaderId(SHADER_NAME_SKY, skyShader);
	glUseProgram(skyShader.id);
	initSkyShaderUniforms(skyShader);
	setSkyShaderUniforms(skyShader, screenRatio);

	unique_ptr<Base3DRenderer> mainRenderer;
	if(isDeferred)
	{
		Log::getInstance().info("Initializing deferred renderer");

		ShaderIds directionalLightPassId;
		shaderManager->getShaderId(fog.enable ? DEFERRED_FOG_LIGHT_PASS_DIRECTIONAL_SHADER_NAME: DEFERRED_LIGHT_PASS_DIRECTIONAL_SHADER_NAME, directionalLightPassId);
		glUseProgram(directionalLightPassId.id);
		if(isDirectional)
		{
			initDeferredDirectionalLightPassShaderUniforms(directionalLightPassId, fog.enable);
			setDeferredDirectionalLightPassShaderUniforms(directionalLightPassId, screenWidth, screenHeight, light, fog);
		}
		else
		{
			initDeferredPointLightPassShaderUniforms(directionalLightPassId, fog.enable);
			setDeferredPointLightPassShaderUniforms(directionalLightPassId, screenWidth, screenHeight, light);
		}

		bool isDeferredLightDirectional = light.lightType == "directional";
		ShaderIds stencilPassId;
		ObjectRenderingData lightSphere;
		ShaderIds pointLightPassId;
		if(!isDeferredLightDirectional)
		{
			objectManager->getRenderingData(OBJECT_LIGHT_SPHERE_NAME, lightSphere);

			shaderManager->getShaderId(STENCIL_PASS_SHADER_NAME, stencilPassId);
			glUseProgram(stencilPassId.id);
			initStencilPassShaderUniforms(stencilPassId);
			setStencilPassShaderUniforms(stencilPassId, static_cast<float>(screenWidth) / screenHeight);

			shaderManager->getShaderId(DEFERRED_LIGHT_PASS_POINT_SHADER_NAME, pointLightPassId);
			glUseProgram(pointLightPassId.id);
			initDeferredPointLightPassShaderUniforms(pointLightPassId, false);
			setDeferredPointLightPassShaderUniforms(pointLightPassId, screenWidth, screenHeight, light);
		}

		mainRenderer = make_unique<DeferredRenderer>(shaderIds, skyShader, shaderFlags, lightSphere, isDirectional, isDeferredLightDirectional, fog.enable, directionalLightPassId, stencilPassId,
			pointLightPassId, glm::vec3(light.x, light.y, light.z), screenWidth, screenHeight);

		if(!isDeferredLightDirectional) //Use only ambient component if point light is specified
			mainRenderer->setDiffuseLightColour(LIGHT_DIFFUSE_NIGHT_COLOUR);
	}
	else
	{
		Log::getInstance().info("Initializing forward renderer");

		mainRenderer = make_unique<ForwardRenderer>(shaderIds, skyShader, shaderFlags, isDirectional);
	}

	return mainRenderer;
}

namespace
{
	void initDirectionalShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifModel = glGetUniformLocation(shaderId.id, MODEL_UNIFORM_NAME);
		shaderId.unifView = glGetUniformLocation(shaderId.id, VIEW_UNIFORM_NAME);
		shaderId.unifProjection = glGetUniformLocation(shaderId.id, PROJECTION_UNIFORM_NAME);

		shaderId.unifRotation = glGetUniformLocation(shaderId.id, ROTATION_UNIFORM_NAME);
		shaderId.unifLightDirection = glGetUniformLocation(shaderId.id, LIGHT_DIRECTION_UNIFORM_NAME);
		shaderId.unifDiffuseLightColour = glGetUniformLocation(shaderId.id, LIGHT_DIFFUSE_COLOUR_UNIFORM_NAME);
		shaderId.unifAmbientLightColour = glGetUniformLocation(shaderId.id, LIGHT_AMBIENT_COLOUR_UNIFORM_NAME);
	}

	void initPointShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifModel = glGetUniformLocation(shaderId.id, MODEL_UNIFORM_NAME);
		shaderId.unifView = glGetUniformLocation(shaderId.id, VIEW_UNIFORM_NAME);
		shaderId.unifProjection = glGetUniformLocation(shaderId.id, PROJECTION_UNIFORM_NAME);

		shaderId.unifLightPosition = glGetUniformLocation(shaderId.id, LIGHT_POSITION_WORLDSPACE_UNIFORM_NAME);
	}

	void initNormalmapShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifNormalTextureId = glGetUniformLocation(shaderId.id, NORMAL_TEXTURE_UNIFORM_NAME);
	}

	void initSmallWavesShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifTime = glGetUniformLocation(shaderId.id, TIME_UNIFORM_NAME);
	}

	void initGlitterShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifCameraPosition = glGetUniformLocation(shaderId.id, CAMERA_POSITION_UNIFORM_NAME);
		shaderId.unifMaterialAlpha = glGetUniformLocation(shaderId.id, MATERIAL_ALPHA_UNIFORM_NAME);
	}

	void initFogShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifFogDensity = glGetUniformLocation(shaderId.id, FOG_DENSITY_UNIFORM_NAME);
		shaderId.unifFogColour = glGetUniformLocation(shaderId.id, FOG_COLOUR_UNIFORM_NAME);
	}

	void initSkyShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifTextureId = glGetUniformLocation(shaderId.id, COLOUR_TEXTURE_UNIFORM_NAME);

		shaderId.unifModel = glGetUniformLocation(shaderId.id, MODEL_UNIFORM_NAME);
		shaderId.unifView = glGetUniformLocation(shaderId.id, VIEW_UNIFORM_NAME);
		shaderId.unifProjection = glGetUniformLocation(shaderId.id, PROJECTION_UNIFORM_NAME);
	}

	void initStencilPassShaderUniforms(ShaderIds &shaderId)
	{
		shaderId.unifModel = glGetUniformLocation(shaderId.id, MODEL_UNIFORM_NAME);
		shaderId.unifView = glGetUniformLocation(shaderId.id, VIEW_UNIFORM_NAME);
		shaderId.unifProjection = glGetUniformLocation(shaderId.id, PROJECTION_UNIFORM_NAME);
	}

	void initDeferredDirectionalLightPassShaderUniforms(ShaderIds &shaderId, bool useFog)
	{
		shaderId.unifLightDirection = glGetUniformLocation(shaderId.id, LIGHT_DIRECTION_UNIFORM_NAME);
		shaderId.unifDiffuseLightColour = glGetUniformLocation(shaderId.id, LIGHT_DIFFUSE_COLOUR_UNIFORM_NAME);
		shaderId.unifAmbientLightColour = glGetUniformLocation(shaderId.id, LIGHT_AMBIENT_COLOUR_UNIFORM_NAME);

		if(useFog)
		{
			shaderId.unifView = glGetUniformLocation(shaderId.id, VIEW_UNIFORM_NAME);

			shaderId.unifFogDensity = glGetUniformLocation(shaderId.id, FOG_DENSITY_UNIFORM_NAME);
			shaderId.unifFogColour = glGetUniformLocation(shaderId.id, FOG_COLOUR_UNIFORM_NAME);

			shaderId.unifPositionComponentId = glGetUniformLocation(shaderId.id, POSITION_COMPONENT_UNIFORM_NAME);
		}

		shaderId.unifNormalComponentId = glGetUniformLocation(shaderId.id, NORMAL_COMPONENT_UNIFORM_NAME);
		shaderId.unifDiffuseComponentId = glGetUniformLocation(shaderId.id, DIFFUSE_COMPONENT_UNIFORM_NAME);
	}

	void initDeferredPointLightPassShaderUniforms(ShaderIds &shaderId, bool useFog)
	{
		shaderId.unifModel = glGetUniformLocation(shaderId.id, MODEL_UNIFORM_NAME);
		shaderId.unifView = glGetUniformLocation(shaderId.id, VIEW_UNIFORM_NAME);
		shaderId.unifProjection = glGetUniformLocation(shaderId.id, PROJECTION_UNIFORM_NAME);

		shaderId.unifLightPosition = glGetUniformLocation(shaderId.id, LIGHT_POSITION_WORLDSPACE_UNIFORM_NAME);
		shaderId.unifDiffuseLightColour = glGetUniformLocation(shaderId.id, LIGHT_DIFFUSE_COLOUR_UNIFORM_NAME);
		shaderId.unifAmbientLightColour = glGetUniformLocation(shaderId.id, LIGHT_AMBIENT_COLOUR_UNIFORM_NAME);

		shaderId.unifPositionComponentId = glGetUniformLocation(shaderId.id, POSITION_COMPONENT_UNIFORM_NAME);
		shaderId.unifNormalComponentId = glGetUniformLocation(shaderId.id, NORMAL_COMPONENT_UNIFORM_NAME);
		shaderId.unifDiffuseComponentId = glGetUniformLocation(shaderId.id, DIFFUSE_COMPONENT_UNIFORM_NAME);

		shaderId.unifScreenSize = glGetUniformLocation(shaderId.id, SCREEN_SIZE_UNIFORM_NAME);
		shaderId.unifLightParameters = glGetUniformLocation(shaderId.id, LIGHT_PARAMETERS_UNIFORM_NAME);
	}
}
