#include "SpriteNave.h"

SpriteNave::SpriteNave()
{
	//Inicializar outros atributos
	vel = 5;
}

SpriteNave::~SpriteNave()
{
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
}

void SpriteNave::initialize(int nAnimations, int nFrames)
{
	this->nAnimations = nAnimations;
	this->nFrames = nFrames;
	ds = 1.0 / (float)nFrames;
	dt = 1.0 / (float)nAnimations;
	iFrame = 0;
	iAnimation = 0;

	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//Primeiro Tri�ngulo
		//x   y     z    s    t   
		-0.5, -0.5, 0.0, 0.0, 0.0, //v0
		 0.5,  0.5, 0.0, ds, dt, //v1
		-0.5,  0.5, 0.0, 0.0, dt, //v2

		//Segundo Tri�ngulo
		-0.5, -0.5, 0.0, 0.0, 0.0, //v0
		 0.5, -0.5, 0.0, ds, 0.0, //v3 
		 0.5,  0.5, 0.0, ds, dt, //v1
	};

	GLuint VBO;
	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posi��o
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo coordenada de textura 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	//Inicializa outros atributos
	state = MOVING;
}

void SpriteNave::update()
{
	//Conecta com o VAO
	glBindVertexArray(VAO);

	//Altera a matriz a transforma��o
	glm::mat4 model = glm::mat4(1); //matriz identidade
	//Aplicando as transforma��es
	model = glm::translate(model, position);

	if (state == MOVING) scaleFactor = dimensions;

	model = glm::scale(model, scaleFactor);
	//Enviando a matriz de modelo para o shader
	shader->setMat4("model", glm::value_ptr(model));
	shader->setVec2("offsets", iFrame * ds, iAnimation * dt);

	if (state != IDLE) //neste caso IDLE � ficar parado mesmo, n�o a anima��o de IDLE
		iFrame = (iFrame + 1) % nFrames; //para anima��es c�clicas, sempre incrementa o �ndice do Frame

	//Desconecta com o VAO
	glBindVertexArray(0);
}

void SpriteNave::draw()
{
	//Conecta com o VAO
	glBindVertexArray(VAO);

	//Conecta a textura do sprite
	glBindTexture(GL_TEXTURE_2D, texID);

	//Faz a chamada de desenho
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Desconecta com o VAO
	glBindVertexArray(0);

	//Desconecta a textura
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SpriteNave::moveLeft()
{
	position.x -= vel * 3.0;
}

void SpriteNave::moveRight()
{
	position.x += vel * 3.0;
}
