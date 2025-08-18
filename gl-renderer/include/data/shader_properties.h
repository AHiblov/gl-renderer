/* shader_properties.h
 * Keeps shader properties
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

namespace renderer::data
{

struct ShaderFlags
{
	static constexpr unsigned long long FEATURE_DIRECTIONAL_LIGHT = 0x1ull;
	static constexpr unsigned long long FEATURE_POINT_LIGHT = 0x2ull;
	//Reserved for spotlight 0x4ull
	static constexpr unsigned long long FEATURE_DEFERRED_GEOMETRY = 0x8ull;
	static constexpr unsigned long long FEATURE_DEFERRED_LIGHT = 0x10ull;
	static constexpr unsigned long long FEATURE_2D = 0x20ull;
	static constexpr unsigned long long FEATURE_SKY = 0x40ull;
	static constexpr unsigned long long FEATURE_SPECULAR = 0x80ull;
	static constexpr unsigned long long FEATURE_NORMALMAP = 0x100ull;
	static constexpr unsigned long long FEATURE_INSTANCING = 0x200ull;
	static constexpr unsigned long long FEATURE_FOG = 0x400ull;
	static constexpr unsigned long long FEATURE_TEXTURE_BOMBING_AND_TRIPLANAR_MAPPING = 0x800ull;
	static constexpr unsigned long long FEATURE_SMALL_WAVES = 0x1000ull;
	static constexpr unsigned long long FEATURE_GLITTER = 0x2000ull;
};

struct PostprocessingFlags
{
	static constexpr unsigned long long POSTPROCESSING_GRAYSCALE = 0x1ull;
	static constexpr unsigned long long POSTPROCESSING_VIGNETTE = 0x2ull;
	static constexpr unsigned long long POSTPROCESSING_DROPS_ON_LENS = 0x4ull;
	static constexpr unsigned long long POSTPROCESSING_EMBOSS = 0x8ull;
};

struct ShaderProperties
{
	ShaderProperties()
	{}

	ShaderProperties(const std::string &vertexShaderFile, const std::string &fragmentShaderFile, unsigned long long shaderProperties, unsigned long long postprocessingProperties):
		vertexShaderPath(vertexShaderFile), fragmentShaderPath(fragmentShaderFile), propertyFlags(shaderProperties), postprocessingFlags(postprocessingProperties)
	{
	}

	ShaderProperties(const ShaderProperties &other):
		vertexShaderPath(other.vertexShaderPath), fragmentShaderPath(other.fragmentShaderPath), propertyFlags(other.propertyFlags), postprocessingFlags(other.postprocessingFlags)
	{
	}



	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	unsigned long long propertyFlags = 0;
	unsigned long long postprocessingFlags = 0;
};

}
