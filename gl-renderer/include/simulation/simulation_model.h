/* simulation_model.h
 * Defines attributes for day and night simulation
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <mutex>

namespace renderer::simulation
{

struct SimulationModel
{
	std::mutex accessLock;

	bool wasUpdated;

	float time;

	bool isDay; //Between sunrise and sunset
	float sunRaysInterpolation;

	bool isMorningGoldenHour;
	bool isEveningGoldenHour;
	float goldenHourInterpolation;

	bool isDawnTwilight;
	bool isDuskTwilight;
	float twilightInterpolation;


	SimulationModel():
		wasUpdated(false), time(0), isDay(false), sunRaysInterpolation(0), isMorningGoldenHour(false), isEveningGoldenHour(false), goldenHourInterpolation(0), isDawnTwilight(false),
		isDuskTwilight(false), twilightInterpolation(0)
	{}
};

}
