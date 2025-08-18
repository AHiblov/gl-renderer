/* postprocessing_shader_ids.h
 * Uniform IDs for shaders applied on framebuffers
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::graphics_lib::videocard_data
{

struct PostprocessingShaderIds
{
	//Common part

	unsigned int quadShaderId = -1u;

	unsigned int unifTextureId = -1u; //Framebuffer ID

	//Shader-specific
	unsigned int unifScreenRatio = -1u; //Vignette and Drops on Lens
	unsigned int unifScreenWidthHeight = -1u; //Emboss
};

}
