/* scene.h
 * Keeps all data of a scene
 *
 * Author: Artem Hoblov
 */

#pragma once

#include <string>
#include <vector>

namespace renderer::data
{

struct Camera
{
	Camera():
		xPos(0.f), yPos(0.f), zPos(0.f), horizontalRotationRadians(0.f), verticalRotationRadians(0.f)
	{}

	Camera(float x, float y, float z, float horizontalRotation, float verticalRotation):
		xPos(x), yPos(y), zPos(z), horizontalRotationRadians(horizontalRotation), verticalRotationRadians(verticalRotation)
	{}

	float xPos, yPos, zPos;
	float horizontalRotationRadians, verticalRotationRadians;
};

struct Light
{
	Light():
		x(0.f), y(0.f), z(0.f)
	{}

	Light(const std::string &type, float xPos, float yPos, float zPos):
		lightType(type), x(xPos), y(yPos), z(zPos)
	{}

	std::string lightType;
	float x, y, z;
};

struct Fog
{
	bool enable;
	int red, green, blue;

	Fog():
		enable(false), red(0), green(0), blue(0)
	{}
};

struct ChunkData
{
	ChunkData():
		x(0.f), z(0.f), size(0.f)
	{}

	ChunkData(const std::string &chunkName, float xPos, float zPos):
		name(chunkName), x(xPos), z(zPos), size(0.f)
	{}

	std::string name;
	float x, z;
	float size;
};

struct InstanceArray
{
	InstanceArray()
	{}

	InstanceArray(const std::string &objectName, const std::string &feature, std::vector<float> &&coordinates):
		name(objectName), shaderFeature(feature), positions(std::move(coordinates))
	{}

	std::string name;
	std::string shaderFeature;
	std::vector<float> positions;
};

struct ParticleSet
{
    ParticleSet():
    	x(0.f), z(0.f), radius(0.f), density(0.f)
    {}

    ParticleSet(const std::string &objectName, const std::string &feature, float centerX, float centerZ, float areaRadius, float particleDensity):
        name(objectName), shaderFeature(feature), x(centerX), z(centerZ), radius(areaRadius), density(particleDensity)
    {}

    std::string name;
	std::string shaderFeature;
	float x, z;
	float radius;
	float density;
};

struct Scene
{
	Camera camera;
	Light light;
	std::string rendererType;
	Fog fog;
	std::string postprocessingEffect;
	std::string terrainTexturing;
	std::vector<ChunkData> chunks;
	std::vector<std::vector<InstanceArray>> instances; //All instances for all chunks
	std::vector<std::vector<ParticleSet>> particles; //All particles for all chunks
};

}
