/* object_manager.cpp
 * Keeps data of objects
 *
 * Author: Artem Hiblov
 */

#include "managers/object_manager.h"

#include <algorithm>
#include <fstream>

#include "log.h"
#include "data/mesh.h"
#include "data/texture.h"
#include "graphics_lib/operations/mesh_operations.h"
#include "graphics_lib/operations/texture_operations.h"
#include "loaders/mesh_loader.h"
#include "loaders/texture_loader.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::loaders;
using namespace renderer::managers;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::operations;
using namespace renderer::graphics_lib::videocard_data;

namespace
{
	const char *ABSENT_NORMALMAP_STRING = "--";

	constexpr int TEXTURE_ATTRIBUTE_TRANSPARENCY = 0x1;
	constexpr int TEXTURE_ATTRIBUTE_NORMALMAP = 0x2;

	const char *PATH_LIBRARIES = "objects/libraries";
}

ObjectManager::ObjectManager(const string &descriptionPath):
	transferedBytes(0)
{
	bool status = initDescription(descriptionPath);
	if(!status)
		return;

	status = initLibraries(PATH_LIBRARIES);
}

ObjectManager::~ObjectManager()
{
	for(auto current: clonedVaos)
		deleteContainer(current);

	for(auto &current: meshIds)
	{
		deleteMesh(current.second);
		deleteTexture(current.second.textureId);

		if(current.second.normalTextureId != -1u)
			deleteTexture(current.second.normalTextureId);
	}
}

bool ObjectManager::getRenderingData(const string &name, ObjectRenderingData &data)
{
	auto iter = meshIds.find(name);

	if(iter == meshIds.end())
	{
		bool status = initObjectData(name);
		if(!status)
		{
			Log::getInstance().error(string("Can't get object \"") + name + "\"");
			return false;
		}

		iter = meshIds.find(name);
	}

	data = iter->second;

	return true;
}

bool ObjectManager::getRenderingDataWithClonedVbo(const string &name, ObjectRenderingData &data)
{
	auto iter = meshIds.find(name);

	if(iter == meshIds.end())
	{
		bool status = initObjectData(name);
		if(!status)
		{
			Log::getInstance().error(string("Can't get object \"") + name + "\"");
			return false;
		}

		iter = meshIds.find(name);
		data = iter->second;
	}
	else
	{
		ObjectRenderingData clonedMesh;
		makeMeshWithClonedVbo(iter->second, clonedMesh);

		clonedVaos.push_back(clonedMesh.vaoId);

		data = clonedMesh;
	}

	return true;
}

int ObjectManager::getTransferedBytesAmount()
{
	return transferedBytes;
}

bool ObjectManager::isTextureTransparent(const std::string &name)
{
	auto iter = textureFlags.find(name);
	if(iter == textureFlags.end())
		return false;

	return iter->second & TEXTURE_ATTRIBUTE_TRANSPARENCY;
}

bool ObjectManager::hasNormalmap(const std::string &name)
{
	auto iter = textureFlags.find(name);
	if(iter == textureFlags.end())
		return false;

	return iter->second & TEXTURE_ATTRIBUTE_NORMALMAP;
}

vector<string> ObjectManager::getObjectNames()
{
	vector<string> names;

	for(auto &[key, value]: description)
	{
		names.push_back(key);
	}

	sort(names.begin(), names.end());

	return names;
}

bool ObjectManager::initDescription(const string &descriptionPath)
{
	loadObjectDescription(descriptionPath, description);
	return true;
}

bool ObjectManager::initLibraries(const string &path)
{
	ifstream data(path);
	if(!data.is_open())
	{
		Log::getInstance().error("Can't read list of libraries");
		return false;
	}

	int libraryAmount = 0;
	data >> libraryAmount;

	for(int i = 0; i < libraryAmount; i++)
	{
		string libraryPath;
		data >> libraryPath;
		loadObjectDescription(libraryPath, description);
	}

	Log::getInstance().info(string("Loaded ") + to_string(libraryAmount) + " libraries");

	return true;
}

bool ObjectManager::initObjectData(const std::string &name)
{
	auto iter = description.find(name);
	if(iter == description.end())
	{
		Log::getInstance().error(string("Can't find description of \"") + name + "\" object");
		return false;
	}

	//Load data

	Mesh mesh;
	bool status = loaders::loadMesh(iter->second.meshPath, mesh);
	if(!status)
	{
		Log::getInstance().error("Can't load object mesh");
		return false;
	}

	Texture texture;
	status = loaders::loadTexture(iter->second.texturePath, texture);
	if(!status)
	{
		Log::getInstance().error("Can't load object texture");
		return false;
	}

	if(texture.bytesPerPixel == 4)
		textureFlags[name] |= TEXTURE_ATTRIBUTE_TRANSPARENCY;

	//Normalmap
	Texture normalTexture;
	if(iter->second.normalmapPath != ABSENT_NORMALMAP_STRING)
	{
		status = loaders::loadTexture(iter->second.normalmapPath, normalTexture);
		if(!status)
		{
			Log::getInstance().error("Can't load object normalmap texture");
			return false;
		}

		textureFlags[name] |= TEXTURE_ATTRIBUTE_NORMALMAP;
	}

	//Submit to video card

	ObjectRenderingData objectIds;
	status = graphics_lib::operations::makeMesh(mesh, objectIds);
	if(!status)
	{
		Log::getInstance().error("Can't create object mesh");
		return false;
	}

	status = graphics_lib::operations::makeTexture(texture, objectIds.textureId);
	if(!status)
	{
		Log::getInstance().error("Can't create object texture");
		return false;
	}

	//Normalmap
	if(textureFlags[name] & TEXTURE_ATTRIBUTE_NORMALMAP)
	{
		status = graphics_lib::operations::makeTexture(normalTexture, objectIds.normalTextureId);
		if(!status)
		{
			Log::getInstance().error("Can't create object normalmap texture");
			return false;
		}
	}

	meshIds[name] = objectIds;

	transferedBytes += mesh.vertices.size() * sizeof(float) + mesh.uvs.size() * sizeof(float) + mesh.normals.size() * sizeof(float);
	transferedBytes += texture.width * texture.height * texture.bytesPerPixel;
	if(textureFlags[name] & TEXTURE_ATTRIBUTE_NORMALMAP)
	{
		transferedBytes += mesh.tangent.size() * sizeof(float) + mesh.bitangent.size() * sizeof(float);
		transferedBytes += normalTexture.width * normalTexture.height * normalTexture.bytesPerPixel;
	}

	return true;
}
