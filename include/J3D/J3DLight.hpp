#pragma once

#include <glm/glm.hpp>

struct J3DLight {
	// Position of the light in space. Set W to 0 for view-space, and 1 for world-space.
	// Shaders will automatically apply the View matrix if W is set to 1.
	glm::vec4 Position = glm::vec4(0, 0, 0, 0);
	// Direction that the light is shining in. World-space if Position.W is 1, view-space if 0.
	glm::vec4 Direction = glm::vec4(0, 0, 0, 1);
	// Color of the light.
	glm::vec4 Color = glm::vec4(0, 0, 0, 0);
	// Attenuation of the light's power depending on the angle of view.
	glm::vec4 AngleAtten = glm::vec4(1, 0, 0, 1);
	// Attenuation of the light's power depending on distance.
	glm::vec4 DistAtten = glm::vec4(1, 0, 0, 1);
};
