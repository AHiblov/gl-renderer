/* abstract_renderer.h
 * Interface for all scene renderer types
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::graphics_lib
{

class AbstractRenderer
{
public:
	virtual ~AbstractRenderer() {}

	/*
	@brief Draws part that the renderer is responsible for
	*/
	virtual void render() = 0;

	/*
	@brief Sets contained framebuffer(s) as target for writing
	*/
	virtual void setTargetFramebuffer() = 0;
};

}
