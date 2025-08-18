/* base_3d_renderer.h
 * Common drawing stuff for 3D objects
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "data/visibility_flags.h"
#include "graphics_lib/abstract_renderer.h"
#include "graphics_lib/videocard_data/rendering_scene.h"
#include "graphics_lib/videocard_data/shader_ids.h"

namespace renderer::graphics_lib
{

class Base3DRenderer: public AbstractRenderer
{
	static constexpr int RENDER_METHODS = 10;

public:
	Base3DRenderer(const std::vector<renderer::graphics_lib::videocard_data::ShaderIds> &shaderIds, const renderer::graphics_lib::videocard_data::ShaderIds &sky,
		const std::map<int, unsigned long long> &shaderFlags, bool isDirectional);
	virtual ~Base3DRenderer();

	/*
	@brief Draws all opaque objects, particles, and terrain
	*/
	void renderOpaqueMeshes();

	void renderTransparentMeshes();

	//----- Forward rendering or geometry pass of deferred rendering -----

	/*
	@brief Draws terrain chunks; directional light
	@param[in] index - chunk index in array
	*/
	void renderTerrainDirectional(int index);

	/*
	@brief Draws terrain chunks; point light
	@param[in] index - chunk index in array
	*/
	void renderTerrainPoint(int index);

	/*
	@brief Draws 3D objects; directional light
	*/
	void renderObjectsDirectional(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; directional light, normalmap
	*/
	void renderObjectsDirectionalNormalmap(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; directional light, waves as normals
	*/
	void renderObjectsDirectionalWaves(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; directional light, glitter
	*/
	void renderObjectsDirectionalGlitter(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; directional light, geometry instancing
	*/
	void renderObjectsDirectionalInstancing(const renderer::graphics_lib::videocard_data::ParticleQuadSubdivision::ParticleNode &instanceGroup);

	/*
	@brief Draws 3D objects; point light
	*/
	void renderObjectsPoint(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; point light, normalmap
	*/
	void renderObjectsPointNormalmap(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; point light, waves as normals
	*/
	void renderObjectsPointWaves(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; point light, glitter
	*/
	void renderObjectsPointGlitter(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances);

	/*
	@brief Draws 3D objects; point light, geometry instancing
	*/
	void renderObjectsPointInstancing(const renderer::graphics_lib::videocard_data::ParticleQuadSubdivision::ParticleNode &instanceGroup);

	/*
	@brief Draws object with center equal to camera position. Always forward rendering
	*/
	void renderSky();

	//----- Simulation-related changes -----

	/*
	@brief Sets light direction to all direclional light shaders
	*/
	virtual void setLightDirection(const glm::vec3 &direction);

	/*
	@brief Sets diffuse light colour to all direclional light shaders
	*/
	virtual void setDiffuseLightColour(const glm::vec3 &colour);

	/*
	@brief Sets ambient light colour to all direclional light shaders
	*/
	virtual void setAmbientLightColour(const glm::vec3 &colour);


	//Update data
	void updateCamera(const glm::mat4 &newViewMatrix);
	void setVisibilityFlags(std::vector<renderer::data::VisibilityFlags> *visibility);
	void setCameraPosition(glm::vec3 *viewPosition);
	void setRenderingScene(renderer::graphics_lib::videocard_data::RenderingScene *scene);
	renderer::graphics_lib::videocard_data::RenderingScene* getRenderingScene(); //Editor-specific

	int getDrawnTriangleCount() const;

protected:
	void initialize(const std::map<int, unsigned long long> &shaderFlags, bool isDirectional);
	void copyShaderArray(const std::vector<renderer::graphics_lib::videocard_data::ShaderIds> &shaderIds);



	/*
	@brief Pointer to terrain rendering method. Concrete method depends on properties required by terrain
	*/
	void (Base3DRenderer:: *renderTerrain)(int index);

	/*
	@brief Pointer to object rendering methods. Concrete method depends on properties required by object
	*/
	void (Base3DRenderer:: *renderObjects[RENDER_METHODS])(const renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode &objectInstances); //Order of shaders must match with "shaders" array

	/*
	@brief Pointer to particle rendering method. Concrete method depends on properties required by object
	*/
	void (Base3DRenderer:: *renderParticles)(const renderer::graphics_lib::videocard_data::ParticleQuadSubdivision::ParticleNode &instanceGroup);



	int previousShader;
	renderer::graphics_lib::videocard_data::ShaderIds *shaders;
	int shaderAmount;

	renderer::graphics_lib::videocard_data::RenderingScene *renderingScene;
	std::vector<renderer::data::VisibilityFlags> *visibilityFlagsPtr; //Non-owning pointer

	glm::mat4 viewMatrix; //Common for both objects and terrain

	renderer::graphics_lib::videocard_data::ShaderIds skyShader;
	glm::vec3 *cameraPositionPtr; //Non-owning pointer

	int triangleCount;
};

}
