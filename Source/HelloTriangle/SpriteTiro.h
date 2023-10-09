#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Classe para manipulação dos shaders
#include "Shader.h"
#include "Sprite.h"

class SpriteTiro: public Sprite {

public:
	inline bool getAtivo() { return this->ativo; }
	inline void setAtivo(bool ativo) { this->ativo = ativo; }

	inline bool getAlien() { return this->alien; }
	inline void setAlien(bool alien) { this->alien = alien; }

	void move();

protected:
	int velTiro = 30.0;
	bool ativo = false;
	bool alien = false;
};