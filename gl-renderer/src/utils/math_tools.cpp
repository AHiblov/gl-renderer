/* math_tools.cpp
 * Math computations absent in the standard library
 *
 * Author: Artem Hiblov
 */

#include "utils/math_tools.h"

#include <cmath>

#include <glm/glm.hpp>

using namespace renderer::utils;

glm::vec3 renderer::utils::slerp(const glm::vec3 &vector1, const glm::vec3 &vector2, float t)
{
	float dotProduct = glm::dot(glm::normalize(vector1), glm::normalize(vector2));
	float theta = acosf(dotProduct);

	return glm::vec3((vector1 * sinf((1 - t) * theta)) + (vector2 * sinf(t * theta))) / sinf(theta);
}

float renderer::utils::computeBoundingSphereRadius(float lightPower)
{
	float a = 0.01f, b = 0.01f, c = 0.1f;
	float attenuationCoefficient = 256.f * lightPower;

	float D = b * b - 4. * a * (c - attenuationCoefficient);
	float radius = -b + sqrt(D);

	return radius;
}
