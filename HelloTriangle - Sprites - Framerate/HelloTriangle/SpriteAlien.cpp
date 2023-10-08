#include "SpriteAlien.h"


void SpriteAlien::move()
{
	int padding = 100;
	if (movendoAEsquerda)
		position.x += vel * 2;
	else
		position.x -= vel * 2;

	if (position.x > (800 - padding)) {
		alterouDirecao = true;
		movendoAEsquerda = false;
	}
	if (position.x < (0 + padding)) {
		alterouDirecao = true;
		movendoAEsquerda = true;
	}
}