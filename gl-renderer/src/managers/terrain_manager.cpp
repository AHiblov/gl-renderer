/* terrain_manager.cpp
 * Keeps data of all terrain chunks
 *
 * Author: Artem Hiblov
 */

#include "managers/terrain_manager.h"

#include <cmath>

#include "log.h"
#include "graphics_lib/operations/terrain_operations.h"
#include "graphics_lib/operations/texture_operations.h"
#include "loaders/terrain_loader.h"
#include "loaders/texture_loader.h"

#define SQR(x) ((x)*(x))

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::loaders;
using namespace renderer::managers;

namespace
{
	enum ECellPosition
	{
		position_bottomLeft = 0,
		position_bottomRight,
		position_topLeft,
		position_topRight,
		position_center
	};

	constexpr int ARRAY_DISTANCES_LENGTH = 5;

	/*
	@brief Finds two minimal values in array
	@param[in] distances - array of 5 elements
	@param[out] index1 - index of first min
	@param[out] index2 - index of second min
	*/
	void twoMins(const float *distances, int &index1, int &index2);

	/*
	@brief Converts ECellPosition to heightmap matrix indices deltas (to be added to rowIndex/columnIndex)
	@param[in] position - ECellPosition value in cell square matrix to convert
	@param[in] firstRowValue - rowDelta of the first call. Used in second call only
	@param[in] firstColumnValue - columnDelta of the first call. Used in second call only
	@param[out] rowDelta - to be added to rowIndex (0 or 1)
	@param[out] columnDelta - to be added to columnIndex (0 or 1)
	@return true if "position" is position_center
	*/
	bool minIndexToMatrixIndexDeltas(int position, int firstRowValue, int firstColumnValue, int &rowDelta, int &columnDelta);
}



TerrainManager::TerrainManager(const string &descriptionPath):
	transferedBytes(0)
{
	initDescription(descriptionPath);
}

TerrainManager::~TerrainManager()
{
}

float TerrainManager::getChunkDimensions(const string &chunkName)
{
	auto iter = dimensions.find(chunkName);

	if(iter != dimensions.end())
	{
		return iter->second;
	}

	auto descriptionIter = description.find(chunkName);

	float width = readChunkDimensions(descriptionIter->second.meshPath);
	dimensions[chunkName] = width;

	return width;
}

bool TerrainManager::getRenderingData(const string &name, ObjectRenderingData &data)
{
	auto iter = chunkIds.find(name);
	if(iter == chunkIds.end())
	{
		bool status = initTerrainData(name);
		if(!status)
		{
			Log::getInstance().error(string("Chunk \"") + name + "\" isn't found");
			return false;
		}

		iter = chunkIds.find(name);
	}

	data = iter->second;

	return true;
}

float TerrainManager::getHeight(float xOffset, float zOffset, const string &chunkName, float xCoord, float zCoord)
{
	Heightmap &hm = heightmap[chunkName];

	//Determine cell for (xCoord; zCoord) point

	int rowIndex = static_cast<int>((zCoord - zOffset) / hm.gridStep);
	int columnIndex = static_cast<int>((xCoord - xOffset) / hm.gridStep);

	if(rowIndex < 0) //Negative half of the axis
		rowIndex *= -1;
	if(rowIndex >= static_cast<int>(hm.heights.size()) - 1) //Don't allow overrun with respect to the next index
		rowIndex = hm.heights.size() - 2;

	if(columnIndex < 0) //Negative half of the axis
		columnIndex *= -1;
	if(columnIndex >= static_cast<int>(hm.heights.size()) - 1) //Don't allow overrun with respect to the next index
		columnIndex = hm.heights.size() - 2;

	//Coordinates inside single cell
	float xRemainder = fabs(xCoord - xOffset) - static_cast<float>(columnIndex) * hm.gridStep;
	float zRemainder = fabs(zCoord - zOffset) - static_cast<float>(rowIndex) * hm.gridStep;

	//Determine cell vertices (with known heights) to use

	//Coordinates of corners and center
	float distances[5] = {0};
	distances[position_bottomLeft] = sqrt(SQR(xRemainder) + SQR(zRemainder)); //Bottom left
	distances[position_bottomRight] = sqrt(SQR(xRemainder - hm.gridStep) + SQR(zRemainder)); //Bottom right
	distances[position_topLeft] = sqrt(SQR(xRemainder) + SQR(zRemainder - hm.gridStep)); //Top left
	distances[position_topRight] = sqrt(SQR(xRemainder - hm.gridStep) + SQR(zRemainder - hm.gridStep)); //Top right
	distances[position_center] = sqrt(SQR(xRemainder - hm.gridStep / 2.f) + SQR(zRemainder - hm.gridStep / 2.f)); //Center

	int min1Index = 0, min2Index = 0;
	twoMins(distances, min1Index, min2Index);

	int point1RowIndex = 0, point1ColumnIndex = 0;
	int point2RowIndex = 0, point2ColumnIndex = 0;

	bool isDiagonal = false;

	//Calculate indices for hm.heights
	if(min1Index == position_center)
	{
		minIndexToMatrixIndexDeltas(min2Index, 0, 0, point2RowIndex, point2ColumnIndex);
		isDiagonal = minIndexToMatrixIndexDeltas(min1Index, point2RowIndex, point2ColumnIndex, point1RowIndex, point1ColumnIndex);

		point2RowIndex += rowIndex;
		point2ColumnIndex += columnIndex;
		point1RowIndex += rowIndex;
		point1ColumnIndex += columnIndex;
	}
	else if(min2Index == position_center)
	{
		minIndexToMatrixIndexDeltas(min1Index, 0, 0, point1RowIndex, point1ColumnIndex);
		isDiagonal = minIndexToMatrixIndexDeltas(min2Index, point1RowIndex, point1ColumnIndex, point2RowIndex, point2ColumnIndex);

		point1RowIndex += rowIndex;
		point1ColumnIndex += columnIndex;
		point2RowIndex += rowIndex;
		point2ColumnIndex += columnIndex;
	}
	else
	{
		minIndexToMatrixIndexDeltas(min1Index, 0, 0, point1RowIndex, point1ColumnIndex);
		minIndexToMatrixIndexDeltas(min2Index, 0, 0, point2RowIndex, point2ColumnIndex);

		point1RowIndex += rowIndex;
		point1ColumnIndex += columnIndex;
		point2RowIndex += rowIndex;
		point2ColumnIndex += columnIndex;
	}

	//Calculate height for (xCoord; zCoord) point

	float point1Height = hm.heights[point1RowIndex][point1ColumnIndex];
	float point2Height = hm.heights[point2RowIndex][point2ColumnIndex];

	float distanceBetweenVertices = isDiagonal ? sqrt(SQR(hm.gridStep) + SQR(hm.gridStep)) : hm.gridStep;

	float distanceToPoint = 0;
	if(min1Index == position_center)
		distanceToPoint = distances[min2Index];
	else distanceToPoint = distances[min1Index];

	float ratio = distanceToPoint / distanceBetweenVertices;
	float targetHeight = min(point1Height, point2Height) + fabs(point2Height - point1Height) * ratio;

	return targetHeight;
}

int TerrainManager::getTransferedBytesAmount() const
{
	return transferedBytes;
}

void TerrainManager::initDescription(const string &descriptionPath)
{
	loadTerrainDescription(descriptionPath, description);
}

bool TerrainManager::initTerrainData(const string &chunkName)
{
	auto iter = description.find(chunkName);
	if(iter == description.end())
	{
		Log::getInstance().error(string("Can't find description of \"") + chunkName + "\" chunk");
		return false;
	}

	//Load data

	Mesh chunk;
	Heightmap currentHeightmap;
	float sideLength = 0;
	bool status = loadTerrain(iter->second.meshPath, chunk, currentHeightmap, sideLength);
	if(!status)
	{
		Log::getInstance().error("Can't load terrain mesh");
		return false;
	}

	Texture texture;
	status = loaders::loadTexture(iter->second.texturePath, texture);
	if(!status)
	{
		Log::getInstance().error("Can't load terrain texture");
		return false;
	}

	heightmap[chunkName] = move(currentHeightmap);

	//Pass to video card

	ObjectRenderingData terrainIds;
	status = graphics_lib::operations::makeTerrain(chunk, terrainIds);
	if(!status)
	{
		Log::getInstance().error("Can't create terrain mesh");
		return false;
	}

	status = graphics_lib::operations::makeTexture(texture, terrainIds.textureId);
	if(!status)
	{
		Log::getInstance().error("Can't create terrain texture");
		return false;
	}
	chunkIds[chunkName] = terrainIds;

	transferedBytes += chunk.vertices.size() * sizeof(float) + chunk.uvs.size() * sizeof(float) + chunk.normals.size() * sizeof(float);
	transferedBytes += texture.width * texture.height * texture.bytesPerPixel;

	return true;
}



namespace
{
	void twoMins(const float *distances, int &index1, int &index2)
	{
		if(!distances)
			return;

		float smallest1 = numeric_limits<float>::infinity();
		int pos1 = 0;
		for(int i = 0; i < ARRAY_DISTANCES_LENGTH; i++)
		{
			if(distances[i] < smallest1)
			{
				smallest1 = distances[i];
				pos1 = i;
			}
		}

		float smallest2 = numeric_limits<float>::infinity();
		int pos2 = 0;
		for(int i = 0; i < ARRAY_DISTANCES_LENGTH; i++)
		{
			if((distances[i] < smallest2) && (smallest1 < distances[i]))
			{
				smallest2 = distances[i];
				pos2 = i;
			}
		}

		index1 = pos1;
		index2 = pos2;
	}

	bool minIndexToMatrixIndexDeltas(int position, int firstRowValue, int firstColumnValue, int &rowDelta, int &columnDelta)
	{
		switch(position)
		{
		case position_bottomLeft:
			rowDelta = 0;
			columnDelta = 0;
			break;
		case position_bottomRight:
			rowDelta = 1;
			columnDelta = 0;
			break;
		case position_topLeft:
			rowDelta = 0;
			columnDelta = 1;
			break;
		case position_topRight:
			rowDelta = 1;
			columnDelta = 1;
			break;
		case position_center: //Must be second call
			rowDelta = 1 - firstRowValue;
			columnDelta = 1 - firstColumnValue;
			break;
		}

		return position == position_center ? true: false;
	}
}
