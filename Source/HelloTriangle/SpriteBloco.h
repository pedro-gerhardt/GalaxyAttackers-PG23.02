#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Classe para manipulação dos shaders
#include "Shader.h"
#include "Sprite.h"

class SpriteBloco: public Sprite {

public:
	inline bool isVivo() { return this->vida > 0; }
	inline int getVida() { return this->vida; }
	inline int setVida(int vida) { return this->vida = vida; }
	inline void diminuiVida() { this->vida = this->vida - 1; }

protected:
	int vida;
};