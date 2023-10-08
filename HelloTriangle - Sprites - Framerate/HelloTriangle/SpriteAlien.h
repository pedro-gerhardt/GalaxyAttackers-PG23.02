#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Classe para manipulação dos shaders
#include "Shader.h"
#include "Sprite.h"

class SpriteAlien : public Sprite {

public:
	void move();

	bool movendoAEsquerda;
	bool alterouDirecao;
	bool morreu;
protected:
};