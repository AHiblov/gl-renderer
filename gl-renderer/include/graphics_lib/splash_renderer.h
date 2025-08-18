/* splash_renderer.h
 * Splash screen component
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "graphics_lib/videocard_data/object_rendering_data.h"
#include "graphics_lib/videocard_data/shader_ids.h"

namespace renderer::graphics_lib
{

class SplashRenderer
{
public:
	SplashRenderer(const renderer::graphics_lib::videocard_data::ObjectRenderingData &data, const renderer::graphics_lib::videocard_data::ShaderIds &id);
	~SplashRenderer();

	void render();

private:
	renderer::graphics_lib::videocard_data::ObjectRenderingData objectData;
	renderer::graphics_lib::videocard_data::ShaderIds shaderId;
};

}
