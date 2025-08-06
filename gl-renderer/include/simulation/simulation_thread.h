/* simulation_thread.h
 * Simulation code entry point
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "simulation/simulation_model.h"

namespace renderer::simulation
{

void performSimulation(SimulationModel &model, bool &needTerminate);

}
