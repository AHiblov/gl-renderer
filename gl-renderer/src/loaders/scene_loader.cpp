/* scene_loader.cpp
 * Loads scene structure
 *
 * Author: Artem Hiblov
 */

#include "loaders/scene_loader.h"

#include <fstream>

#include "log.h"

using namespace std;
using namespace renderer::data;
using namespace renderer::loaders;

namespace
{
	const string STR_YES = "yes";
	const string STR_SCENE_SIGNATURE = "scene";

	/*
	@brief Loads camera data
	*/
	void readCamera(ifstream &data, Scene &scene);

	/*
	@brief Loads renderer type (forward/deferred)
	*/
	void readRendererType(ifstream &data, Scene &scene);

	void readFogData(ifstream &data, Scene &scene);

	/*
	@brief Loads post effect data
	*/
	void readPostEffect(ifstream &data, Scene &scene);

	/*
	@brief Loads light data
	*/
	void readLight(ifstream &data, Scene &scene);

	/*
	@brief Loads single terrain chunk
	@param[in] data - open file
	@param[in] index - 0-based chunk index
	@param[out] scene - structure for writing
	*/
	void readChunk(ifstream &data, int index, Scene &scene);

	/*
	@brief Loads single chunk terrain data
	*/
	void readTerrain(ifstream &data, Scene &scene);

	/*
	@brief Loads object data for single chunk
	@param[in] data - open file
	@param[in] index - 0-based chunk index
	@param[out] scene - structure for writing
	*/
	void readObjects(ifstream &data, int index, Scene &scene);

	/*
	@brief Loads particle data for single chunk
	@param[in] data - open file
	@param[in] index - 0-based chunk index
	@param[out] scene - structure for writing
	*/
	void readParticles(ifstream &data, int index, Scene &scene);
}

bool renderer::loaders::loadScene(const string &path, Scene &scene)
{
	if(path.empty())
	{
		Log::getInstance().error("Path for scene file is not provided");
		return false;
	}

	ifstream data(path);
	if(!data.is_open())
	{
		Log::getInstance().error(path + " can't be opened");
		return false;
	}

	string signature;
	data >> signature;
	if(signature != STR_SCENE_SIGNATURE)
	{
		Log::getInstance().error(path + " doesn't contain valid signature");
		return false;
	}

	readCamera(data, scene);

	readRendererType(data, scene);
	readFogData(data, scene);
	readPostEffect(data, scene);

	readLight(data, scene);

	/*
	chunks 1  chunkSignature, chunkNumber
	terrain-texturing: texture-bombing-and-triplanar-mapping  terrainTexturingSignature, terrainTexturing
	*/
	string chunkSignature;
	int chunkNumber = 0;

	data >> chunkSignature >> chunkNumber;
	scene.instances.resize(chunkNumber);
	scene.particles.resize(chunkNumber);

	string terrainTexturingSignature;

	data >> terrainTexturingSignature >> scene.terrainTexturing;

	for(int i = 0; i < chunkNumber; i++)
	{
		readChunk(data, i, scene);
	}

	data.close();

	return true;
}

namespace
{
	void readCamera(ifstream &data, Scene &scene)
	{
		/*
		camera  sectionName
		0.731092 5.44828 3.18898  x, y, z
		2.81259 -0.656  horizontalRotation, verticalRotation
		*/

		string sectionName;
		float x = 0.f, y = 0.f, z = 0.f;
		float horizontalRotation = 0.f, verticalRotation = 0.f;

		data >> sectionName >> x >> y >> z >> horizontalRotation >> verticalRotation;

		horizontalRotation = (horizontalRotation * 3.14159f) / 180.f;
		verticalRotation = (verticalRotation * 3.14159f) / 180.f;

		scene.camera = Camera(x, y, z, horizontalRotation, verticalRotation);
	}

	void readRendererType(ifstream &data, Scene &scene)
	{
		/*
		renderer-type: forward  rendererType
		*/

		string propertyName;
		data >> propertyName >> scene.rendererType;
	}

	void readFogData(ifstream &data, Scene &scene)
	{
		/*
		fog: no  enable
		fog-colour: 255 255 255  red green blue
		*/

		string fogPropertyName, enable, colourPropertyName;
		data >> fogPropertyName >> enable >> colourPropertyName >> scene.fog.red >> scene.fog.green >> scene.fog.blue;

		scene.fog.enable = (enable == STR_YES) ? true: false;
	}

	void readPostEffect(ifstream &data, Scene &scene)
	{
		/*
		post-effect: --  postprocessingEffect
		*/

		string propertyName;
		data >> propertyName >> scene.postprocessingEffect;
	}

	void readLight(ifstream &data, Scene &scene)
	{
		/*
		light  sectionName
		directional  lightType (other value: point)
		x y z
		*/

		string sectionName, lightType;
		float x = 0, y = 0, z = 0;

		data >> sectionName >> lightType >> x >> y >> z;
		scene.light = Light(lightType, x, y, z);
	}

	void readChunk(ifstream &data, int index, Scene &scene)
	{
		/*
		river-bank  chunkName
		0 0  x, z
		objects -- 1  objectsBlockSignature, shaderFeature, groupAmount
		wooden-house 4  objectName, objectAmount
		11 0 -9 180  [-11 0 -9 0 ...]  positions: x, y, z, rotation
		particles 1  particlesBlockSignature, groupAmount
	    grass instancing 5 10 3  name, shaderFeature, centerX, centerZ, radius
		*/
		readTerrain(data, scene);
		readObjects(data, index, scene);
		readParticles(data, index, scene);
	}

	void readTerrain(ifstream &data, Scene &scene)
	{
		/*
		river-bank  name
		0 0  x, z
		*/

		string chunkName;
		float x = 0, z = 0;
		data >> chunkName >> x >> z;

		scene.chunks.push_back(ChunkData(move(chunkName), x, z));
	}

	void readObjects(ifstream &data, int index, Scene &scene)
	{
		/*
		objects specular 1  objectsBlockSignature, shaderFeature, groupAmount
		wooden-house 4  name, amount
		11 0 -9 180  [-11 0 -9 0]  positions: x, y, z, rotation
		*/

		string objectsBlockSignature;
		int groupAmount = 0;
		data >> objectsBlockSignature >> groupAmount;

		for(int i = 0; i < groupAmount; i++)
		{
			string name, shaderFeature;
			int amount = 0;

			data >> name >> shaderFeature >> amount;

			vector<float> positions(amount * 4); //3 coordinates per vertex + rotation angle
			for(int j = 0; j < amount * 4; j += 4)
				data >> positions[j] >> positions[j+1] >> positions[j+2] >> positions[j+3];

			scene.instances[index].push_back(InstanceArray(move(name), move(shaderFeature), move(positions)));
		}
	}

	void readParticles(ifstream &data, int index, Scene &scene)
	{
	    /*
	    particles 1  particlesBlockSignature, groupAmount
	    grass instancing 5 10 3 0.75  name, shaderFeature, centerX, centerZ, radius, density
	    */

	    string particlesBlockSignature;
	    int groupAmount = 0;
	    data >> particlesBlockSignature >> groupAmount;

	    for(int i = 0; i < groupAmount; i++)
        {
            string name, shaderFeature;
            float centerX = 0, centerZ = 0;
            float radius = 0;
            float density = 0;
            data >> name >> shaderFeature >> centerX >> centerZ >> radius >> density;

            scene.particles[index].push_back(ParticleSet(name, shaderFeature, centerX, centerZ, radius, density));
        }
	}
}
