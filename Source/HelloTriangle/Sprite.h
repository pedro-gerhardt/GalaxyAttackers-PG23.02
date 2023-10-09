#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Classe para manipulação dos shaders
#include "Shader.h"
#include "Enum.h"

class Sprite
{
public:
	Sprite();
	~Sprite();
	void initialize(int nAnimations, int nFrames);
	inline void setTexID(GLuint texID) { this->texID = texID; }

	inline void setPosition(glm::vec3 position) { this->position = position; }
	inline void setDimensions(glm::vec3 dimensions) { this->dimensions = dimensions; scaleFactor = dimensions; }
	inline void setShader(Shader* shader) { this->shader = shader;  }
	inline void setState(int state) { this->state = state;  }

	inline glm::vec3 getPosition() { return this->position; }

	void update();
	void draw();

	void moveLeft();
	void moveRight();

private:
	GLuint VAO; //identificador do buffer de geometria
	GLuint texID; //identificador do buffer de textura
	
	//Atributos relacionados às transformações
	glm::vec3 position, dimensions, rotationAxis;
	float angle;
	float vel;

	Shader* shader; //armazena o endereço do shader

	int nAnimations, nFrames, iAnimation, iFrame;
	float ds, dt;
	int state;
	glm::vec3 scaleFactor;

};

