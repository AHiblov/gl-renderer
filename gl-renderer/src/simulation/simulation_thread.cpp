/* simulation_thread.cpp
 * Simulation code entry point
 *
 * Author: Artem Hiblov
 */

#include "simulation/simulation_thread.h"

#include <chrono>
#include <unistd.h>

using namespace std;
using namespace std::chrono;
using namespace renderer::simulation;

namespace
{
	constexpr float TIME_MILLISECONDS_IN_SECOND = 1000.f;
	constexpr int TIME_MICROSECONDS_IN_MILLISECOND = 1000;

	constexpr int SIMULATION_MAX_UPDATES_PER_SECONS = 5;
	constexpr int SIMULATION_MILLISECONDS_PER_UPDATE = TIME_MILLISECONDS_IN_SECOND / SIMULATION_MAX_UPDATES_PER_SECONS;

	constexpr float SIMULATION_DAY_AND_NIGHT_PERIOD = 120.f; //Seconds

	constexpr float SIMULATION_SUNRISE_TIME = 0.075f;
	constexpr float SIMULATION_SUNSET_TIME = 0.6f;
	constexpr float SIMULATION_DAY_PERIOD = SIMULATION_SUNSET_TIME - SIMULATION_SUNRISE_TIME;

	constexpr float SIMULATION_GOLDEN_HOUR_PERIOD = 0.17f;
	constexpr float SIMULATION_MORNING_GOLDEN_HOUR_TIME = SIMULATION_SUNRISE_TIME + SIMULATION_GOLDEN_HOUR_PERIOD;
	constexpr float SIMULATION_EVENING_GOLDEN_HOUR_TIME = SIMULATION_SUNSET_TIME - SIMULATION_GOLDEN_HOUR_PERIOD;

	constexpr float SIMULATION_TWILIGHT_PERIOD = 0.074f;
	constexpr float SIMULATION_DAWN_TWILIGHT_TIME = SIMULATION_SUNRISE_TIME - SIMULATION_TWILIGHT_PERIOD;
	constexpr float SIMULATION_DUSK_TWILIGHT_TIME = SIMULATION_SUNSET_TIME + SIMULATION_TWILIGHT_PERIOD;
}

void renderer::simulation::performSimulation(SimulationModel &model, bool &needTerminate)
{
	steady_clock::time_point startTime = steady_clock::now();

	while(true)
	{
		steady_clock::time_point currentTime = steady_clock::now();
		steady_clock::time_point iterationStartTime = currentTime;

		double currentT = (duration_cast<milliseconds>(currentTime - startTime).count() / TIME_MILLISECONDS_IN_SECOND) / SIMULATION_DAY_AND_NIGHT_PERIOD;

		if(currentT >= 1.f)
		{
			startTime = steady_clock::now();
			currentT = 0;
		}

		if(needTerminate)
			break;

		bool isDay = false;
		float sunRaysInterpoation = 0;

		bool isMorningGoldenHour = false, isEveningGoldenHour = false;
		float goldenHourInterpolation = 0;

		bool isDawnTwilight = false, isDuskTwilight = false;
		float twilightInterpolation = 0;

		if((SIMULATION_SUNRISE_TIME < currentT) && (currentT <= SIMULATION_SUNSET_TIME)) //Day
		{
			isDay = true;
			sunRaysInterpoation = (currentT - SIMULATION_SUNRISE_TIME) / SIMULATION_DAY_PERIOD;

			if((SIMULATION_SUNRISE_TIME < currentT) && (currentT <= SIMULATION_MORNING_GOLDEN_HOUR_TIME)) //Morning golden hour
			{
				isMorningGoldenHour = true;
				goldenHourInterpolation = (currentT - SIMULATION_SUNRISE_TIME) / SIMULATION_GOLDEN_HOUR_PERIOD;
			}
			else if((SIMULATION_EVENING_GOLDEN_HOUR_TIME < currentT) && (currentT <= SIMULATION_SUNSET_TIME)) //Evening golden hour
			{
				isEveningGoldenHour = true;
				goldenHourInterpolation = (currentT - SIMULATION_EVENING_GOLDEN_HOUR_TIME) / SIMULATION_GOLDEN_HOUR_PERIOD;
			}
		}

		if((SIMULATION_DAWN_TWILIGHT_TIME < currentT) && (currentT <= SIMULATION_SUNRISE_TIME)) //Morning twilight
		{
			isDawnTwilight = true;
			twilightInterpolation = (currentT - SIMULATION_DAWN_TWILIGHT_TIME) / SIMULATION_TWILIGHT_PERIOD;
		}
		if((SIMULATION_SUNSET_TIME < currentT) && (currentT <= SIMULATION_DUSK_TWILIGHT_TIME)) //Evening twilight
		{
			isDuskTwilight = true;
			twilightInterpolation = (currentT - SIMULATION_SUNSET_TIME) / SIMULATION_TWILIGHT_PERIOD;
		}

		{
			lock_guard<mutex> lockGuard(model.accessLock);

			model.time = currentT;
			model.isDay = isDay;
			model.sunRaysInterpolation = sunRaysInterpoation;

			model.isMorningGoldenHour = isMorningGoldenHour;
			model.isEveningGoldenHour = isEveningGoldenHour;
			model.goldenHourInterpolation = goldenHourInterpolation;

			model.isDawnTwilight = isDawnTwilight;
			model.isDuskTwilight = isDuskTwilight;
			model.twilightInterpolation = twilightInterpolation;

			model.wasUpdated = true;
		}

		currentTime = steady_clock::now();
		usleep((duration_cast<milliseconds>(currentTime - iterationStartTime).count() + SIMULATION_MILLISECONDS_PER_UPDATE) * TIME_MICROSECONDS_IN_MILLISECOND);
	}

}
