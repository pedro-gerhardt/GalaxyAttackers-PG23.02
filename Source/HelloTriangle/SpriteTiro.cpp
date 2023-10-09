#include "SpriteTiro.h"


void SpriteTiro::move()
{
	glm::vec3 pos = getPosition();
	if (alien)
		setPosition(glm::vec3(pos.x, pos.y - velTiro, pos.z));
	else
		setPosition(glm::vec3(pos.x, pos.y + velTiro, pos.z));
}