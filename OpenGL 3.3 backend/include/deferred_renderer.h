/* deferred_renderer.h
 * Deferred renderer implementation
 * OpenGL 3.3
 *
 * Author: Artem hiblov
 */

#pragma once

#include "graphics_lib/base_3d_renderer.h"

namespace renderer::graphics_lib
{

class DeferredRenderer: public Base3DRenderer
{
	static constexpr int TOTAL_TEXTURES = 4;

	static constexpr int TEXTURE_INDEX_POSITION = 0;
	static constexpr int TEXTURE_INDEX_UV = 1;
	static constexpr int TEXTURE_INDEX_NORMAL = 2;
	static constexpr int TEXTURE_INDEX_DIFFUSE = 3;

public:
	DeferredRenderer(const std::vector<renderer::graphics_lib::videocard_data::ShaderIds> &shaderIds, const renderer::graphics_lib::videocard_data::ShaderIds &sky,
		const std::map<int, unsigned long long> &shaderFlags, const renderer::graphics_lib::videocard_data::ObjectRenderingData &sphere, bool isDirectional, bool isDeferredDirectional, bool isFog,
		const renderer::graphics_lib::videocard_data::ShaderIds &directionalLightPass, const renderer::graphics_lib::videocard_data::ShaderIds &stencilPass,
		const renderer::graphics_lib::videocard_data::ShaderIds &pointLightPass, const glm::vec3 &lightPosition, int width, int height);
	virtual ~DeferredRenderer();

	virtual void render();

	virtual void setTargetFramebuffer();

	void setWriteFramebufferId(unsigned int writeBuffer);

	//Simulation staff
	virtual void setLightDirection(const glm::vec3 &direction) override;
	virtual void setDiffuseLightColour(const glm::vec3 &colour) override;
	virtual void setAmbientLightColour(const glm::vec3 &colour) override;

private:
	void initializeBuffer();

	void performStencilPass();
	void performDirectionalLightPass();
	void performPointLightPass();
	void drawSkyAsForwardRendering();



	int writeFramebufferId; //Rendering to screen or to postprocessing framebuffer

	const int framebufferWidth;
	const int framebufferHeight;

	unsigned int framebufferId = -1u;
	unsigned int textureIds[TOTAL_TEXTURES];
	unsigned int depthBufferId = -1u; //Now stored for deletion only

	unsigned int quadBufferId = -1u;

	bool useFog;

	renderer::graphics_lib::videocard_data::ShaderIds directionalLightPassShaderId;

	renderer::graphics_lib::videocard_data::ShaderIds stencilPassId;
	renderer::graphics_lib::videocard_data::ShaderIds pointLightPassShaderId;
	renderer::graphics_lib::videocard_data::ObjectRenderingData lightSphere;
	glm::vec3 pointLightPosition;

	bool isDeferredLightDirectional;
};

}
