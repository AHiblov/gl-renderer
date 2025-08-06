/* postprocessing_renderer.h
 * Applies 2D fullscreen effects on framebuffer
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "graphics_lib/abstract_renderer.h"
#include "graphics_lib/videocard_data/postprocessing_shader_ids.h"

namespace renderer::graphics_lib
{

class PostprocessingRenderer: public AbstractRenderer
{
public:
	PostprocessingRenderer(renderer::graphics_lib::videocard_data::PostprocessingShaderIds &ids, int width, int height);
	virtual ~PostprocessingRenderer();

	virtual void render();

	virtual void setTargetFramebuffer();

	unsigned int getFramebufferId() const;

private:
	void initializeFramebuffer();



	renderer::graphics_lib::videocard_data::PostprocessingShaderIds postprocessingIds;

	unsigned int framebufferId = -1u;
	unsigned int textureId = -1u;
	unsigned int depthTextureId = -1u; //Now stored for deletion only

	unsigned int quadVaoId = -1u;

	int framebufferWidth;
	int framebufferHeight;
};

}
