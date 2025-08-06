/* frame_renderer.h
 * Dispatches single frame drawing
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "data/visibility_flags.h"
#include "graphics_lib/forward_renderer.h"
#include "graphics_lib/postprocessing_renderer.h"

namespace renderer::graphics_lib
{

class FrameRenderer
{
	static constexpr int UI_STR_MAX_LENGTH = 40;

public:
	FrameRenderer(renderer::graphics_lib::Base3DRenderer *mainRend, renderer::graphics_lib::PostprocessingRenderer *postprocessingRend, bool isDeferred);
	~FrameRenderer();

	//----- Frame rendering -----

	/*
	@brief Renders single frame
	*/
	void renderFrame();

	/*
	@brief Renders UI for single frame
	*/
	void renderUi();

	//----- Per-frame data manipulation -----

	void updateCamera(const glm::mat4 &newViewMatrix);

	//---- Simulation-related changes -----

	//Pass-through
	void setLightDirection(const glm::vec3 &direction);

	//Pass-through
	void setDiffuseLightColour(const glm::vec3 &colour);

	//Pass-through
	void setAmbientLightColour(const glm::vec3 &colour);

	//----- Setters -----

	/*
	@brief Sets vilibility flags for chunks. Pass-through
	*/
	void setVisibilityFlags(std::vector<renderer::data::VisibilityFlags> *visibility);

	/*
	@brief Sets pointer to camera position (for sky shader). Pass-through
	*/
	void setCameraPosition(glm::vec3 *viewPosition);

	/*
	@brief Sets OpenGL properties
	*/
	void setRendererLibraryProperties();


	int getDrawnTriangleCount() const;

	/*
	@brief Sets FPS count and drawn triangle amount info
	*/
	void setStatisticsLine(const std::string &str);

	void setSimulationLine(const std::string &str);

protected:
	renderer::graphics_lib::Base3DRenderer *mainRenderer;
	renderer::graphics_lib::PostprocessingRenderer *postprocessingRenderer;

	bool isDeferredRenderer;

	char statisticsString[UI_STR_MAX_LENGTH];
	char simulationString[UI_STR_MAX_LENGTH];
};

}
