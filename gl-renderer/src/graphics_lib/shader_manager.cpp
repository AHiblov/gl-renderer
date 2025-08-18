/* shader_manager.cpp
 * Creates and holds all shader IDs
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/shader_manager.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

#include "log.h"
#include "graphics_lib/operations/shader_operations.h"
#include "loaders/shader_loader.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::graphics_lib::operations;
using namespace renderer::loaders;

namespace
{
	//Regular properties
	const char *FEATURE_DIRECTIONAL_STRING = "directional";
	const char *FEATURE_POINT_STRING = "point";
	const char *FEATURE_SPECULAR_STRING = "specular";
	const char *FEATURE_NORMALMAP_STRING = "normalmap";
	const char *FEATURE_INSTANCING_STRING = "instancing";
	const char *FEATURE_FOG_STRING = "fog";
	const char *FEATURE_TEXTURE_BOMBING_AND_TRIPLANAR_MAPPING_STRING = "texture-bombing-and-triplanar-mapping";
	const char *FEATURE_SMALL_WAVES_STRING = "small-waves";
	const char *FEATURE_GLITTER = "glitter";

	//Deferred shading properties
	const char *FEATURE_DEFERRED_GEOMETRY_STRING = "deferred-geometry";
	const char *FEATURE_DEFERRED_LIGHT_STRING = "deferred-light";

	//Properties for special shaders
	const char *FEATURE_SKY_STRING = "sky";
	const char *FEATURE_2D_STRING = "2d";

	const char *FEATURE_BASIC_STRING = "--";

	//Postprocessing properties
	const char *POSTPROCESSING_GRAYSCALE_STRING = "postprocessing:grayscale";
	const char *POSTPROCESSING_VIGNETTE_STRING = "postprocessing:vignette";
	const char *POSTPROCESSING_DROPS_ON_LENS_STRING = "postprocessing:drops-on-lens";
	const char *POSTPROCESSING_EMBOSS_STRING = "postprocessing:emboss";



	/*
	@brief Makes bitwise flags from string representation
	*/
	void parseFeatures(const string &features, unsigned long long &flags, unsigned long long &postprocessingFlags);

	/*
	@brief Matches property name and its code
	@param[in] name - one of FEATURE_*_STRING
	@param[out] flag - corresponding flag
	*/
	bool stringPropertyToFlag(const string &name, unsigned long long &flag);

	/*
	@brief Matches property name and its postprocessing code
	@param[in] name - one of POSTPROCESSING_*_STRING
	@param[out] flag - corresponding flag
	*/
	bool stringPropertyToPostprocessingFlag(const string &name, unsigned long long &flag);
}

ShaderManager::ShaderManager(const string &descriptionPath):
	directionalLight(false)
{
	initShaderDescriptions(descriptionPath);

	shaderObjects.reserve(description.size() * 2);
}

ShaderManager::~ShaderManager()
{
	/*If a program object to be deleted has shader objects attached to it,
	those shader objects will be automatically detached but not deleted unless they have already been flagged for deletion by a previous call to glDeleteShader*/
	for(auto current: shaderObjects)
		glDeleteShader(current);
}

void ShaderManager::setLightType(bool directional)
{
	directionalLight = directional;
}

bool ShaderManager::getShaderId(const string &shaderName, ShaderIds &id)
{
	if(shaderName.empty())
	{
		Log::getInstance().error("No shader name is provided");
		return false;
	}

	auto iter = ids.find(shaderName);
	if(iter != ids.end())
	{
		id = iter->second;
		return true;
	}

	Log::getInstance().info(string("Creating shader \"") + shaderName + "\"");

	string vertexShader, fragmentShader;
	bool status = readShaders(shaderName, vertexShader, fragmentShader);
	if(!status)
	{
		Log::getInstance().error("Can't load shader");
		return false;
	}

	ShaderIds shaderId;
	unsigned int vertexShaderObject = -1u, fragmentShaderObject = -1u;
	status = makeShader(vertexShader, fragmentShader, shaderId.id, vertexShaderObject, fragmentShaderObject);
	if(!status)
	{
		Log::getInstance().error("Can't compile shader");
		return false;
	}

	//Keep shader object ID for deletion
	shaderObjects.push_back(vertexShaderObject);
	shaderObjects.push_back(fragmentShaderObject);

	ids[shaderName] = shaderId;
	id = ids[shaderName];

	return true;
}

bool ShaderManager::getShaderIndexByProperty(const string &property, bool enableFog, bool isDeferredRenderer, int &shaderIndex)
{
	unsigned long long flags = 0;
	if(!stringPropertyToFlag(property, flags))
	{
		Log::getInstance().error("Can't determine shader property");
		return false;
	}

	if(isDeferredRenderer)
		flags |= ShaderFlags::FEATURE_DEFERRED_GEOMETRY;

	if(directionalLight)
	{
		unsigned long long flagsWithLight = flags | ShaderFlags::FEATURE_DIRECTIONAL_LIGHT;
		unsigned long long flagsWithFog = flagsWithLight | ShaderFlags::FEATURE_FOG;

		auto iter = find_if(neededShaders.begin(), neededShaders.end(), [flagsWithLight](const pair<int, unsigned long long> &current)
		{
			return current.second == flagsWithLight;
		});

		if(enableFog && !isDeferredRenderer)
		{
			iter = find_if(neededShaders.begin(), neededShaders.end(), [flagsWithFog](const pair<int, unsigned long long> &current)
			{
				return current.second == flagsWithFog;
			});
		}

		if(iter == neededShaders.end())
		{
			int index = neededShaders.size();
			neededShaders[index] = enableFog && !isDeferredRenderer ? flagsWithFog: flagsWithLight;
			shaderIndex = index;
		}
		else shaderIndex = iter->first;
	}
	else
	{
		unsigned long long flagsWithLight = flags | ShaderFlags::FEATURE_POINT_LIGHT;
		unsigned long long flagsWithFog = flagsWithLight | ShaderFlags::FEATURE_FOG;

		auto iter = find_if(neededShaders.begin(), neededShaders.end(), [flagsWithLight](const pair<int, unsigned long long> &current)
		{
			return current.second == flagsWithLight;
		});

		if(enableFog && !isDeferredRenderer)
		{
			iter = find_if(neededShaders.begin(), neededShaders.end(), [flagsWithFog](const pair<int, unsigned long long> &current)
			{
				return current.second == flagsWithFog;
			});
		}

		if(iter == neededShaders.end())
		{
			int index = neededShaders.size();
			neededShaders[index] = enableFog && !isDeferredRenderer ? flagsWithFog: flagsWithLight;
			shaderIndex = index;
		}
		else shaderIndex = iter->first;
	}

	return true;
}

vector<ShaderIds>& ShaderManager::createNeededShaders(map<int, unsigned long long> &shaderFlags)
{
	orderedShaderIds.resize(neededShaders.size());

	for(auto &[index, flags]: neededShaders)
	{
		auto iter = find_if(description.begin(), description.end(), [flags](const pair<string, ShaderProperties> &current)
		{
			return current.second.propertyFlags == flags;
		});

		if(iter == description.end()) //Fallback in case shader with fog is not found
		{
			unsigned long long flagsWithoutFog = flags & ~ShaderFlags::FEATURE_FOG;

			stringstream ss;
			ss << flags;
			Log::getInstance().warning(string("Fog shader is not found for flags ") + ss.str() + ". Trying fallback");

			iter = find_if(description.begin(), description.end(), [flagsWithoutFog](const pair<string, ShaderProperties> &current)
			{
				return current.second.propertyFlags == flagsWithoutFog;
			});
		}

		if(iter == description.end())
		{
			stringstream ss;
			ss << flags;
			Log::getInstance().error(string("Can't find shader for flags ") + ss.str());
			continue;
		}

		string shaderName = iter->first;

		ShaderIds id;
		if(!getShaderId(shaderName, id))
		{
			Log::getInstance().error(string("Cannot create shader \"") + shaderName + "\"");
			continue;
		}

		orderedShaderIds[index] = id;
	}

	shaderFlags = neededShaders;
	return orderedShaderIds;
}

bool ShaderManager::getPostprocessingShaderId(const string &property, PostprocessingShaderIds **id)
{
	unsigned long long flags = 0;
	if(!stringPropertyToPostprocessingFlag(property, flags))
	{
		Log::getInstance().error("Can't determine postprocessing shader property");
		return false;
	}

	auto descriptionIter = find_if(description.begin(), description.end(), [flags](const pair<string, ShaderProperties> &current)
		{
			return current.second.postprocessingFlags == flags;
		});
	if(descriptionIter == description.end())
	{
		stringstream ss;
		ss << flags;
		Log::getInstance().error(string("Can't find postprocessing shader for flags ") + ss.str());
		return false;
	}

	string shaderName(descriptionIter->first);

	auto shaderIter = postprocessingIds.find(shaderName);
	if(shaderIter != postprocessingIds.end())
	{
		*id = &(shaderIter->second);
		return true;
	}

	Log::getInstance().info(string("Creating postprocessing shader \"") + shaderName + "\"");

	string vertexShader, fragmentShader;
	bool status = readShaders(shaderName, vertexShader, fragmentShader);
	if(!status)
	{
		Log::getInstance().error("Can't load postprocessing shader");
		return false;
	}

	ShaderIds quadShaderId; //Only id field is used
	unsigned int vertexShaderObject = -1u, fragmentShaderObject = -1u;
	status = makeShader(vertexShader, fragmentShader, quadShaderId.id, vertexShaderObject, fragmentShaderObject);
	if(!status)
	{
		Log::getInstance().error("Can't compile postprocessing shader");
		return false;
	}

	PostprocessingShaderIds postprocessingShaderId;
	postprocessingShaderId.quadShaderId = quadShaderId.id;
	postprocessingIds[shaderName] = postprocessingShaderId;
	*id = &postprocessingIds[shaderName];

	return true;
}

unsigned long long ShaderManager::getPostprocessingShaderFlags(const std::string &property) const
{
	unsigned long long flags = 0;
	if(!stringPropertyToPostprocessingFlag(property, flags))
	{
		return 0;
	}

	return flags;
}

void ShaderManager::getEditorShaderId(renderer::graphics_lib::videocard_data::ShaderIds &id)
{
	unsigned long long neededFlags = ShaderFlags::FEATURE_DIRECTIONAL_LIGHT;

	int neededIndex = 0;
	for(auto &[index, flags]: neededShaders)
	{
		if(neededFlags == flags)
		{
			neededIndex = index;
			break;
		}
	}

	id = orderedShaderIds[neededIndex];
}

const map<int, unsigned long long>& ShaderManager::getShaderFlags() const
{
	return neededShaders;
}

vector<ShaderIds>& ShaderManager::getOrderedShaderIds()
{
	return orderedShaderIds;
}

ShaderProperties ShaderManager::getProperties(const string &shaderName)
{
	auto iter = description.find(shaderName);
	if(iter == description.end())
	{
		Log::getInstance().error(string("Shader \"") + shaderName + "\" is not found");
		return ShaderProperties();
	}

	return iter->second;
}

void ShaderManager::initShaderDescriptions(const string &descriptionPath)
{
	if(descriptionPath.empty())
	{
		Log::getInstance().error("Shader description file path is not provided");
	}

	ifstream data(descriptionPath);
	if(!data.is_open())
	{
		Log::getInstance().error("Shader description file couldn't be opened");
	}

	int amount = 0;
	data >> amount;

	for(int i = 0; i < amount; i++)
	{
		string shaderName, vertexShaderPath, fragmentShaderPath, featuresString;
		data >> shaderName >> vertexShaderPath >> fragmentShaderPath;
		getline(data, featuresString); //Eat '\n' at ther end of fragment shader path line
		getline(data, featuresString);

		unsigned long long featureFlags = 0, postprocessingFlags = 0;
		parseFeatures(featuresString, featureFlags, postprocessingFlags);

		description[shaderName] = ShaderProperties(vertexShaderPath, fragmentShaderPath, featureFlags, postprocessingFlags);
	}
}

bool ShaderManager::readShaders(const string &shaderName, string &vertexShader, string &fragmentShader)
{
	if(shaderName.empty())
		return false;

	ShaderProperties properties = getProperties(shaderName);
	if(properties.vertexShaderPath.empty() || properties.fragmentShaderPath.empty())
	{
		Log::getInstance().error(string("File paths aren't specified for shader ") + shaderName);
		return false;
	}

	bool status = loadShader(properties.vertexShaderPath, vertexShader);
	if(!status)
	{
		Log::getInstance().error(string("Can't load shader from ") + properties.vertexShaderPath);
		return false;
	}
	status = loadShader(properties.fragmentShaderPath, fragmentShader);
	if(!status)
	{
		Log::getInstance().error(string("Can't load shader from ") + properties.fragmentShaderPath);
		return false;
	}

	return true;
}

namespace
{
	void parseFeatures(const string &features, unsigned long long &flags, unsigned long long &postprocessingFlags)
	{
		if(features.empty())
			return;

		istringstream iss(features);
		vector<string> splitedFeatures;
		copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(splitedFeatures));

		for(auto &cur: splitedFeatures)
		{
			unsigned long long currentFlag = 0;

			bool status = stringPropertyToFlag(cur, currentFlag);
			if(status)
			{
				flags |= currentFlag;
			}
			else
			{
				status = stringPropertyToPostprocessingFlag(cur, currentFlag);
				if(status)
					postprocessingFlags |= currentFlag;
			}
		}
	}

	bool stringPropertyToFlag(const string &name, unsigned long long &flag)
	{
		static const unordered_map<string, int> stringToFlag = {
			{FEATURE_DIRECTIONAL_STRING, ShaderFlags::FEATURE_DIRECTIONAL_LIGHT},
			{FEATURE_POINT_STRING, ShaderFlags::FEATURE_POINT_LIGHT},
			{FEATURE_DEFERRED_GEOMETRY_STRING, ShaderFlags::FEATURE_DEFERRED_GEOMETRY},
			{FEATURE_DEFERRED_LIGHT_STRING, ShaderFlags::FEATURE_DEFERRED_LIGHT},
			{FEATURE_SPECULAR_STRING, ShaderFlags::FEATURE_SPECULAR},
			{FEATURE_NORMALMAP_STRING, ShaderFlags::FEATURE_NORMALMAP},
			{FEATURE_INSTANCING_STRING, ShaderFlags::FEATURE_INSTANCING},
			{FEATURE_FOG_STRING, ShaderFlags::FEATURE_FOG},
			{FEATURE_TEXTURE_BOMBING_AND_TRIPLANAR_MAPPING_STRING, ShaderFlags::FEATURE_TEXTURE_BOMBING_AND_TRIPLANAR_MAPPING},
			{FEATURE_SMALL_WAVES_STRING, ShaderFlags::FEATURE_SMALL_WAVES},
			{FEATURE_GLITTER, ShaderFlags::FEATURE_GLITTER},
			{FEATURE_SKY_STRING, ShaderFlags::FEATURE_SKY},
			{FEATURE_2D_STRING, ShaderFlags::FEATURE_2D},
			{FEATURE_BASIC_STRING, 0} //No additional effects
		};

		auto iter = stringToFlag.find(name);
		if(iter == stringToFlag.end())
			return false;

		flag = iter->second;

		return true;
	}

	bool stringPropertyToPostprocessingFlag(const string &name, unsigned long long &flag)
	{
		static const unordered_map<string, int> stringToFlag = {
			{POSTPROCESSING_GRAYSCALE_STRING, PostprocessingFlags::POSTPROCESSING_GRAYSCALE},
			{POSTPROCESSING_VIGNETTE_STRING, PostprocessingFlags::POSTPROCESSING_VIGNETTE},
			{POSTPROCESSING_DROPS_ON_LENS_STRING, PostprocessingFlags::POSTPROCESSING_DROPS_ON_LENS},
			{POSTPROCESSING_EMBOSS_STRING, PostprocessingFlags::POSTPROCESSING_EMBOSS}
		};

		auto iter = stringToFlag.find(name);
		if(iter == stringToFlag.end())
			return false;

		flag = iter->second;

		return true;
	}
}
