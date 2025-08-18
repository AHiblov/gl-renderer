/* shader_manager.h
 * Creates and holds all shader IDs
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "data/shader_properties.h"
#include "graphics_lib/videocard_data/postprocessing_shader_ids.h"
#include "graphics_lib/videocard_data/shader_ids.h"

namespace renderer::graphics_lib
{

class ShaderManager
{
public:
	ShaderManager(const std::string &descriptionPath);
	~ShaderManager();

	void setLightType(bool directional);

	/*
	@brief Creates shader if neccessary and returns its ID
	*/
	bool getShaderId(const std::string &shaderName, renderer::graphics_lib::videocard_data::ShaderIds &id);

	/*
	@brief Collects indices in property flags container. Does not create shader
	*/
	bool getShaderIndexByProperty(const std::string &property, bool enableFog, bool isDeferredRenderer, int &shaderIndex);

	/*
	@brief Creates all the shaders needed for scene
	*/
	std::vector<renderer::graphics_lib::videocard_data::ShaderIds>& createNeededShaders(std::map<int, unsigned long long> &shaderFlags);

	/*
	@brief Creates shader for posteffect if neccessary and returns its ID
	*/
	bool getPostprocessingShaderId(const std::string &property, renderer::graphics_lib::videocard_data::PostprocessingShaderIds **id);

	//----- Getters. External usage -----

	unsigned long long getPostprocessingShaderFlags(const std::string &property) const;

	//Editor-specific. Must be called after createNeededShaders
	void getEditorShaderId(renderer::graphics_lib::videocard_data::ShaderIds &id);

	const std::map<int, unsigned long long>& getShaderFlags() const;
	std::vector<renderer::graphics_lib::videocard_data::ShaderIds>& getOrderedShaderIds();

private:
	//----- Properties-related stuff -----

	/*
	@brief Finds properties by shader name
	*/
	renderer::data::ShaderProperties getProperties(const std::string &shaderName);

	//----- Read from files -----

	/*
	@brief Fills in the description container
	*/
	void initShaderDescriptions(const std::string &descriptionPath);

	/*
	@brief Gets shader source code
	*/
	bool readShaders(const std::string &shaderName, std::string &vertexShader, std::string &fragmentShader);



	std::map<std::string, renderer::data::ShaderProperties> description;

	std::map<std::string, renderer::graphics_lib::videocard_data::ShaderIds> ids; //All created shaders
	std::map<std::string, renderer::graphics_lib::videocard_data::PostprocessingShaderIds> postprocessingIds;

	std::vector<renderer::graphics_lib::videocard_data::ShaderIds> orderedShaderIds; //The order matches neededShaders indices. Only shaders required by getShaderIndexByProperty
	std::map<int, unsigned long long> neededShaders;
	bool directionalLight;

	std::vector<unsigned int> shaderObjects; //For shader deletion
};

}
