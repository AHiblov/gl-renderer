/* editor_tools.cpp
 * Functions needed for editor
 *
 * Author: Artem Hiblov
 */

#include "utils/editor_tools.h"

#include <cmath>
#include <fstream>

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::utils;

namespace
{
	constexpr int EDITOR_FLOATS_PER_INSTANCE = 4;
}

void renderer::utils::findClosestObject(const glm::vec3 &cameraPosition, const vector<vector<InstanceArray>> &instanceGroups, int &chunkIndex, int &objectIndex, int &instanceIndex)
{
	int chunkWithMinIndex = -1;
	int groupWithMinIndex = -1;
	int minDistanceIndex = -1;
	float minDistance = 1'000'000.f;

	int instanceGroupAmount = instanceGroups.size();
	for(int i = 0; i < instanceGroupAmount; i++)
	{
		int groupAmount = instanceGroups[i].size();
		for(int j = 0; j < groupAmount; j++)
		{
			auto &curGroupArray = instanceGroups[i][j];

			int instanceAmount = curGroupArray.positions.size(); //3 position coordinates + rotation are kept for each instance
			for(int k = 0; k < instanceAmount; k += 4)
			{
				float xDifference = cameraPosition.x - curGroupArray.positions[k];
				float yDifference = cameraPosition.y - curGroupArray.positions[k+1];
				float zDifference = cameraPosition.z - curGroupArray.positions[k+2];
				float distance = sqrt(xDifference * xDifference + yDifference * yDifference + zDifference * zDifference);

				if(distance < minDistance)
				{
					minDistance = distance;
					minDistanceIndex = k;
					groupWithMinIndex = j;
					chunkWithMinIndex = i;
				}
			}
		}
	}

	chunkIndex = chunkWithMinIndex;
	objectIndex = groupWithMinIndex;
	instanceIndex = minDistanceIndex;
}

void renderer::utils::saveScene(const string &fileName, Scene &scene)
{
	ofstream data(fileName);

	data << "scene\n\n";

	float horizontalRotationDegrees = (scene.camera.horizontalRotationRadians * 180.f) / 3.14159f;
	float verticalRotationDegrees = (scene.camera.verticalRotationRadians * 180.f) / 3.14159f;
	data << "camera\n" << scene.camera.xPos << ' '<< scene.camera.yPos << ' ' << scene.camera.zPos << '\n' << horizontalRotationDegrees << ' ' << verticalRotationDegrees << "\n\n";

	data << "renderer-type: " << scene.rendererType << "\n";
	data << "fog: " << (scene.fog.enable ? "yes": "no") << "\n";
	data << "fog-colour: " << scene.fog.red << ' ' << scene.fog.green << ' ' << scene.fog.blue << "\n";
	data << "post-effect: " << scene.postprocessingEffect << "\n\n";

	data << "light\n" << scene.light.lightType << '\n' << scene.light.x << ' ' << scene.light.y << ' ' << scene.light.z << "\n\n";

	data << "chunks " << scene.chunks.size() << "\n";
	data << "terrain-texturing: " << scene.terrainTexturing << "\n";

	int chunkAmount = scene.chunks.size();
	for(int i = 0; i < chunkAmount; i++)
	{
		//Chunk
		{
			ChunkData &chunk = scene.chunks[i];
			data << chunk.name << '\n' << chunk.x << ' ' << chunk.z << '\n';
		}

		//Objects
		{
			data << "objects " << scene.instances[i].size() << '\n';

			for(auto &current: scene.instances[i])
			{
				data << current.name << ' ' << current.shaderFeature << ' ' << (current.positions.size() / EDITOR_FLOATS_PER_INSTANCE) << '\n';
				int counterTo4 = 0;
				for(auto &coordinate: current.positions)
				{
					data << coordinate << ' ';

					counterTo4++;
					if(counterTo4 % 4 == 0)
					{
						data << ' ';
						counterTo4 = 0;
					}
				}

				data << '\n';
			}
		}

		//Particles
		{
			data << "particles " << scene.particles[i].size() << '\n';

			for(auto &current: scene.particles[i])
			{
				data << current.name << ' ' << current.shaderFeature << ' ' << current.x << ' ' << current.z << ' ' << current.radius << ' ' << current.density << '\n';
			}
		}

		data << '\n'; //Visually separate data from distinct chunks
	}

	data.close();
}
