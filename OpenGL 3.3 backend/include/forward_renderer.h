/* forward_renderer.h
 * Forward renderer implementation
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "graphics_lib/base_3d_renderer.h"

namespace renderer::graphics_lib
{

class ForwardRenderer: public Base3DRenderer
{
public:
	ForwardRenderer(const std::vector<renderer::graphics_lib::videocard_data::ShaderIds> &shaderIds, const renderer::graphics_lib::videocard_data::ShaderIds &sky,
		const std::map<int, unsigned long long> &shaderFlags, bool isDirectional);
	virtual ~ForwardRenderer();

	virtual void render();

	virtual void setTargetFramebuffer();
};

}
