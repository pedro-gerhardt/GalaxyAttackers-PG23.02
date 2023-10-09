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

	inline int getVelAlien() { return this->velAlien; }

	inline bool getMorreu() { return this->morreu; }
	inline void setMorreu(bool morreu) { this->morreu = morreu; }

	inline bool getAlterouDirecao() { return this->alterouDirecao; }
	inline void setAlterouDirecao(bool alterouDirecao) { this->alterouDirecao = alterouDirecao; }

	inline bool getMovendoEsquerda() { return this->movendoAEsquerda; }
	inline void setMovendoEsquerda(bool movendo) { this->movendoAEsquerda = movendo; }
private:
	bool movendoAEsquerda;
	bool alterouDirecao;
	bool morreu;
	int velAlien = 5.0;;
};