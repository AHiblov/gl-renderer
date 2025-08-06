/* object_manager.h
 * Keeps data of all objects
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "data/object_file_paths.h"
#include "graphics_lib/videocard_data/object_rendering_data.h"

namespace renderer::managers
{

class ObjectManager
{
public:
	ObjectManager(const std::string &descriptionPath);
	~ObjectManager();

	/*
	@brief Fills structure needed for object rendering
	@param[in] name - object name
	@param[out] data - structure to be filled
	*/
	bool getRenderingData(const std::string &name, renderer::graphics_lib::videocard_data::ObjectRenderingData &data);

	/*
	@brief Creates copy of ObjectRenderingData with new VAO and same VBOs
	@param[in] name - object name
	@param[out] data - structure to be filled
	*/
	bool getRenderingDataWithClonedVbo(const std::string &name, renderer::graphics_lib::videocard_data::ObjectRenderingData &data);

	int getTransferedBytesAmount();

	bool isTextureTransparent(const std::string &name);

	bool hasNormalmap(const std::string &name);

	//---- Editor-specific ----

	/*
	@brief Returns sorted list of object names
	*/
	std::vector<std::string> getObjectNames();

private:
	/*
	@brief Loads descriptions from file
	*/
	bool initDescription(const std::string &descriptionPath);

	bool initLibraries(const std::string &path);

	/*
	@brief Initializes data needed for object rendering
	@param[in] name - object name
	*/
	bool initObjectData(const std::string &name);



	std::map<std::string, renderer::graphics_lib::videocard_data::ObjectRenderingData> meshIds;
	std::map<std::string, int> textureFlags;
	std::vector<unsigned int> clonedVaos;
	std::map<std::string, renderer::data::ObjectFilePaths> description;

	int transferedBytes; //How many bytes transfered to videocard
};

}
