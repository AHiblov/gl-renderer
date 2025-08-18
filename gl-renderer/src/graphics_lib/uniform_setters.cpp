/* uniform_setters.cpp
 * Functions for setting uniforms to all shaders
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/uniform_setters.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common_constants.h"
#include "utils/math_tools.h"

using namespace renderer::data;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::utils;

namespace
{
	constexpr float PROJECTION_FOV = 45.f;
	constexpr float PROJECTION_NEAR_CLIPPING = 0.5f;
	constexpr float PROJECTION_FAR_CLIPPING = 125.f;

	constexpr float FOG_DENSITY = 0.04f;

	const glm::vec3 LIGHT_DIFFUSE_COLOUR_DEFAULT = glm::vec3(1.f, 1.f, 1.f);
	const glm::vec3 LIGHT_AMBIENT_COLOUR_DEFAULT = glm::vec3(1.f, 1.f, 1.f);
}

void renderer::graphics_lib::setDirectionalShaderUniforms(ShaderIds &shaderId, float screenRatio, const Light &light)
{
	const glm::mat4 projection = glm::perspective(PROJECTION_FOV, screenRatio, PROJECTION_NEAR_CLIPPING, PROJECTION_FAR_CLIPPING);
	glUniformMatrix4fv(shaderId.unifProjection, 1, GL_FALSE, &projection[0][0]);

	const glm::vec3 lightDirection(light.x, light.y, light.z);
	glUniform3fv(shaderId.unifLightDirection, 1, &lightDirection[0]);
	glUniform3fv(shaderId.unifDiffuseLightColour, 1, &LIGHT_DIFFUSE_COLOUR_DEFAULT[0]);
	glUniform3fv(shaderId.unifAmbientLightColour, 1, &LIGHT_AMBIENT_COLOUR_DEFAULT[0]);

	shaderId.isDirectionalLight = true;
}

void renderer::graphics_lib::setPointShaderUniforms(ShaderIds &shaderId, float screenRatio, const Light &light)
{
	const glm::mat4 projection = glm::perspective(PROJECTION_FOV, screenRatio, PROJECTION_NEAR_CLIPPING, PROJECTION_FAR_CLIPPING);
	glUniformMatrix4fv(shaderId.unifProjection, 1, GL_FALSE, &projection[0][0]);

	const glm::vec3 lightPosition(light.x, light.y, light.z);
	glUniform3fv(shaderId.unifLightPosition, 1, &lightPosition[0]);
}

void renderer::graphics_lib::setGlitterShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float materialAlphaX, float materialAlphaY)
{
	const glm::vec2 alpha(materialAlphaX, materialAlphaY);
	glUniform2fv(shaderId.unifMaterialAlpha, 1, &alpha[0]);
}

void renderer::graphics_lib::setFogShaderUniforms(ShaderIds &shaderId, const Fog &fog)
{
	glUniform1f(shaderId.unifFogDensity, FOG_DENSITY);
	glm::vec4 colour(fog.red / 255.f, fog.green / 255.f, fog.blue / 255.f, 1.f);
	glUniform4fv(shaderId.unifFogColour, 1, &colour[0]);
}

void renderer::graphics_lib::setSkyShaderUniforms(ShaderIds &shaderId, float screenRatio)
{
	const glm::mat4 projection = glm::perspective(PROJECTION_FOV, screenRatio, PROJECTION_NEAR_CLIPPING, PROJECTION_FAR_CLIPPING);
	glUniformMatrix4fv(shaderId.unifProjection, 1, GL_FALSE, &projection[0][0]);
}

void renderer::graphics_lib::setStencilPassShaderUniforms(ShaderIds &shaderId, float screenRatio)
{
	const glm::mat4 projection = glm::perspective(PROJECTION_FOV, screenRatio, PROJECTION_NEAR_CLIPPING, PROJECTION_FAR_CLIPPING);
	glUniformMatrix4fv(shaderId.unifProjection, 1, GL_FALSE, &projection[0][0]);
}

void renderer::graphics_lib::setDeferredDirectionalLightPassShaderUniforms(ShaderIds &shaderId, float screenWidth, float screenHeight, const Light &light, const Fog &fog)
{
	if(fog.enable)
		setFogShaderUniforms(shaderId, fog);

	const glm::vec3 lightCoordinates(light.x, light.y, light.z);
	glUniform3fv(shaderId.unifLightDirection, 1, &lightCoordinates[0]);
	glUniform3fv(shaderId.unifDiffuseLightColour, 1, &LIGHT_DIFFUSE_COLOUR_DEFAULT[0]);
	glUniform3fv(shaderId.unifAmbientLightColour, 1, &LIGHT_AMBIENT_COLOUR_DEFAULT[0]);

	shaderId.isDirectionalLight = true;
}

void renderer::graphics_lib::setDeferredPointLightPassShaderUniforms(ShaderIds &shaderId, float screenWidth, float screenHeight, const Light &light)
{
	const glm::mat4 projection = glm::perspective(PROJECTION_FOV, screenWidth / screenHeight, PROJECTION_NEAR_CLIPPING, PROJECTION_FAR_CLIPPING);
	glUniformMatrix4fv(shaderId.unifProjection, 1, GL_FALSE, &projection[0][0]);

	glm::vec2 screenSize(screenWidth, screenHeight);
	glUniform2fv(shaderId.unifScreenSize, 1, &screenSize[0]);

	float sphereRadius = computeBoundingSphereRadius(POINT_LIGHT_POWER);
	glm::vec2 lightParameters(POINT_LIGHT_POWER, sphereRadius);
	glUniform2fv(shaderId.unifLightParameters, 1, &lightParameters[0]);

	const glm::vec3 lightCoordinates(light.x, light.y, light.z);
	glUniform3fv(shaderId.unifLightPosition, 1, &lightCoordinates[0]);
	glUniform3fv(shaderId.unifDiffuseLightColour, 1, &LIGHT_DIFFUSE_COLOUR_DEFAULT[0]);
	glUniform3fv(shaderId.unifAmbientLightColour, 1, &LIGHT_AMBIENT_COLOUR_DEFAULT[0]);

	shaderId.isDirectionalLight = false;
}
