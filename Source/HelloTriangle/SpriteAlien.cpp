#include "SpriteAlien.h"


void SpriteAlien::move()
{
	int padding = 100;
	glm::vec3 pos = getPosition();
	if (movendoAEsquerda)
		setPosition(glm::vec3(pos.x + velAlien * 2, pos.y, pos.z));
	else
		setPosition(glm::vec3(pos.x - velAlien * 2, pos.y, pos.z));

	if (pos.x > (800 - padding)) {
		alterouDirecao = true;
		movendoAEsquerda = false;
	}
	if (pos.x < (0 + padding)) {
		alterouDirecao = true;
		movendoAEsquerda = true;
	}
}