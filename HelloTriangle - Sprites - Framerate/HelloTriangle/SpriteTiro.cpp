#include "SpriteTiro.h"


void SpriteTiro::move()
{
	if (alien)
		position.y -= velTiro;
	else 
		position.y += velTiro;
}