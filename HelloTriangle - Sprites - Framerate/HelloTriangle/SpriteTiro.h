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

	void moveUp();

protected:
	int velTiro = 15.0;
	bool ativo = false;
};