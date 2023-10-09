/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 14/08/2023
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       
using namespace std;

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STB_IMAGE
#include <stb_image.h>

//Classe para manipulação dos shaders
#include "Shader.h"

//Classe Sprite 
#include "Sprite.h"
#include "SpriteAlien.h"
#include "SpriteTiro.h"
#include "SpriteBloco.h"

//Classe Timer
#include "Timer.h"


// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 800;
const int qtdEtsColuna = 6;
const int qtdEtsLinha = 4;
const int qtdBlocos = 3;

int explodindo = 0;
int timerAlienTiro = 10;
int vidaContagem = 5;

GameState gameState = STARTING;

Sprite naveUsuario;
SpriteAlien naveEt[qtdEtsLinha][qtdEtsColuna];
SpriteTiro tiro;
SpriteTiro tiroAlien;
Sprite explosao;
Sprite vida;
SpriteBloco blocos[qtdBlocos];
Sprite gameOverScreen;
Sprite victoryScreen;
Sprite startScreen;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupGeometry();
int setupTexture(string filePath, int& width, int& height);
int setupSprite();

bool testaColisaoTiroEmObjeto(glm::vec3 tiroPos, glm::vec3 objetoPos, int offset);
bool testaCondicaoVitoria();
bool testaCondicaoDerrota();
void atualizaEDesenha();
void diparaTiroAlien();
void testaAliensLateral();
void testaTiroEmBloco();
void testaTiroEmAlien();
void testaTiroEmNave();
void inicializaSprites(Shader *shader);
// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Galaxy Attackers", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	srand(time(NULL));

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;

	// Compilando e buildando o programa de shader
	Shader shader("../shaders/helloTriangle.vs", "../shaders/helloTriangle.fs");
	shader.Use();

	inicializaSprites(&shader);

	//Cria a matriz de projeção paralela ortogáfica
	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::ortho(0.0, 800.0, 0.0, 800.0, -1.0, 1.0);
	
	shader.setMat4("projection", glm::value_ptr(projection));
	shader.setInt("texbuffer", 0);
	
	glActiveTexture(GL_TEXTURE0);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	Timer timer;
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		timer.start();

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		switch (gameState) {
			case STARTING:
				startScreen.update();
				startScreen.draw();
				glfwSwapBuffers(window);
				continue;

			case GAME_OVER:
				gameOverScreen.update();
				gameOverScreen.draw();
				glfwSwapBuffers(window);
				std::this_thread::sleep_for(std::chrono::milliseconds((int)3000));
				gameState = STARTING;
				inicializaSprites(&shader);
				break;

			case VICTORY:
				victoryScreen.update();
				victoryScreen.draw();
				glfwSwapBuffers(window);
				std::this_thread::sleep_for(std::chrono::milliseconds((int)3000));
				gameState = STARTING;
				inicializaSprites(&shader);
				break;

			default:
				break;
		}

		atualizaEDesenha();

		testaTiroEmAlien();

		testaTiroEmNave();

		testaTiroEmBloco();
		
		testaAliensLateral();
		
		diparaTiroAlien();

		if (testaCondicaoVitoria())
			gameState = VICTORY;
		
		if (testaCondicaoDerrota()) 
			gameState = GAME_OVER;

		/*if (gameState != PLAYING)
			break;*/

		timer.finish();
		double waitingTime = timer.calcWaitingTime(12, timer.getElapsedTimeMs());
		if (waitingTime)
			std::this_thread::sleep_for(std::chrono::milliseconds((int)waitingTime));

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}


	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

void inicializaSprites(Shader *shader) {

	//Fazendo a leitura da textura do personagem
	int sprWidth, sprHeight;
	int texID = setupTexture("../../Textures/characters/PNG/Nave/nave.png", sprWidth, sprHeight);

	int sprWidthAlien1, sprHeightAlien1, sprWidthAlien2, sprHeightAlien2, sprWidthAlien3, sprHeightAlien3, sprWidthAlien4, sprHeightAlien4;
	int texID1 = setupTexture("../../Textures/characters/PNG/Nave/alien1.png", sprWidthAlien1, sprHeightAlien1);
	int texID2 = setupTexture("../../Textures/characters/PNG/Nave/alien2.png", sprWidthAlien2, sprHeightAlien2);
	int texID3 = setupTexture("../../Textures/characters/PNG/Nave/alien3.png", sprWidthAlien3, sprHeightAlien3);
	int texID4 = setupTexture("../../Textures/characters/PNG/Nave/alien4.png", sprWidthAlien4, sprHeightAlien4);

	// Criando a instância de nosso objeto sprite do Personagem
	naveUsuario.initialize(1, 1);
	naveUsuario.setPosition(glm::vec3(400.0, 150.0, 0.0));
	naveUsuario.setDimensions(glm::vec3(sprWidth / 1 * 4.0, sprHeight * 4.0, 1.0));
	naveUsuario.setShader(shader);
	naveUsuario.setTexID(texID);

	for (int i = 0; i < qtdEtsColuna; i++) {
		naveEt[0][i].initialize(1, 2);
		naveEt[0][i].setPosition(glm::vec3(300.0 + (i * 50.0), 500.0, 0.0));
		naveEt[0][i].setDimensions(glm::vec3(sprWidthAlien1 / 2 * 4.0, sprHeightAlien1 * 4.0, 1.0));
		naveEt[0][i].setShader(shader);
		naveEt[0][i].setTexID(texID1);
		naveEt[0][i].setState(0);
		naveEt[0][i].setMorreu(false);
	}
	for (int i = 0; i < qtdEtsColuna; i++) {
		naveEt[1][i].initialize(1, 2);
		naveEt[1][i].setPosition(glm::vec3(300.0 + (i * 50.0), 550.0, 0.0));
		naveEt[1][i].setDimensions(glm::vec3(sprWidthAlien2 / 2 * 4.0, sprHeightAlien2 * 4.0, 1.0));
		naveEt[1][i].setShader(shader);
		naveEt[1][i].setTexID(texID2);
		naveEt[1][i].setState(0);
		naveEt[1][i].setMorreu(false);
	}
	for (int i = 0; i < qtdEtsColuna; i++) {
		naveEt[2][i].initialize(1, 2);
		naveEt[2][i].setPosition(glm::vec3(300.0 + (i * 50.0), 600.0, 0.0));
		naveEt[2][i].setDimensions(glm::vec3(sprWidthAlien3 / 2 * 4.0, sprHeightAlien3 * 4.0, 1.0));
		naveEt[2][i].setShader(shader);
		naveEt[2][i].setTexID(texID3);
		naveEt[2][i].setState(0);
		naveEt[2][i].setMorreu(false);
	}
	for (int i = 0; i < qtdEtsColuna; i++) {
		naveEt[3][i].initialize(1, 2);
		naveEt[3][i].setPosition(glm::vec3(300.0 + (i * 50.0), 650.0, 0.0));
		naveEt[3][i].setDimensions(glm::vec3(sprWidthAlien4 / 2 * 4.0, sprHeightAlien4 * 4.0, 1.0));
		naveEt[3][i].setShader(shader);
		naveEt[3][i].setTexID(texID4);
		naveEt[3][i].setState(0);
		naveEt[3][i].setMorreu(false);
	}

	int sprWidthTiroA, sprHeightTiroA;
	int texIDTiroA = setupTexture("../../Textures/characters/PNG/Nave/tiroAlien.png", sprWidthTiroA, sprHeightTiroA);

	int sprWidthTiro, sprHeightTiro;
	int texIDTiro = setupTexture("../../Textures/characters/PNG/Nave/tiroNave.png", sprWidthTiro, sprHeightTiro);
	tiro.initialize(1, 1);
	tiro.setPosition(glm::vec3(-50.0, -50.0, 0.0));
	tiro.setDimensions(glm::vec3(sprWidthTiro / 1 * 4.0, sprHeightTiro * 4.0, 1.0));
	tiro.setShader(shader);
	tiro.setTexID(texIDTiro);
	tiro.setAlien(false);

	tiroAlien.initialize(1, 2);
	tiroAlien.setPosition(glm::vec3(-50.0, -50.0, 0.0));
	tiroAlien.setDimensions(glm::vec3(sprWidthTiroA / 2 * 4.0, sprHeightTiroA * 4.0, 1.0));
	tiroAlien.setShader(shader);
	tiroAlien.setTexID(texIDTiroA);
	tiroAlien.setAlien(true);

	int sprHeightExp, sprWidthExp;
	int texIDExplosao = setupTexture("../../Textures/characters/PNG/Nave/explosao.png", sprWidthExp, sprHeightExp);
	explosao.initialize(1, 4);
	explosao.setPosition(glm::vec3(-50.0, -50.0, 0.0));
	explosao.setDimensions(glm::vec3(sprWidthExp / 4 * 4.0, sprHeightExp * 4.0, 1.0));
	explosao.setShader(shader);
	explosao.setTexID(texIDExplosao);

	int sprWidthVida, sprHeightVida;
	int texIDVida = setupTexture("../../Textures/characters/PNG/Nave/vida.png", sprWidthVida, sprHeightVida);
	vida.initialize(1, 6);
	vida.setPosition(glm::vec3(100.0, 700.0, 0.0));
	vida.setDimensions(glm::vec3(sprWidthVida / 6 * 2.0, sprHeightVida * 2.0, 1.0));
	vida.setShader(shader);
	vida.setTexID(texIDVida);
	vida.setState(IDLE);

	int sprWidthBloco, sprHeightBloco;
	int texIDBloco = setupTexture("../../Textures/characters/PNG/Nave/bloco.png", sprWidthBloco, sprHeightBloco);
	for (int i = 0; i < qtdBlocos; i++) {
		blocos[i].initialize(1, 4);
		blocos[i].setPosition(glm::vec3(200.0 + (i * 200.0), 300.0, 0.0));
		blocos[i].setDimensions(glm::vec3(sprWidthBloco / 4 * 4.0, sprHeightBloco * 4.0, 1.0));
		blocos[i].setShader(shader);
		blocos[i].setTexID(texIDBloco);
		blocos[i].setState(IDLE);
		blocos[i].setVida(4);
	}

	int sprWidthGameOverScreen, sprHeightGameOverScreen;
	int texIDGameOverScreen = setupTexture("../../Textures/characters/PNG/Nave/gameover.png", sprWidthGameOverScreen, sprHeightGameOverScreen);
	gameOverScreen.initialize(1, 1);
	gameOverScreen.setPosition(glm::vec3(400.0, 400.0, 0.0));
	gameOverScreen.setDimensions(glm::vec3(sprWidthGameOverScreen * 0.8, sprHeightGameOverScreen * 0.8, 1.0));
	gameOverScreen.setShader(shader);
	gameOverScreen.setTexID(texIDGameOverScreen);

	int sprWidthVictoryScreen, sprHeightVictoryScreen;
	int texIDVictoryScreen = setupTexture("../../Textures/characters/PNG/Nave/victory.png", sprWidthVictoryScreen, sprHeightVictoryScreen);
	victoryScreen.initialize(1, 1);
	victoryScreen.setPosition(glm::vec3(400.0, 400.0, 0.0));
	victoryScreen.setDimensions(glm::vec3(sprWidthVictoryScreen * 0.8, sprHeightVictoryScreen * 0.8, 1.0));
	victoryScreen.setShader(shader);
	victoryScreen.setTexID(texIDVictoryScreen);

	int sprWidthStartScreen, sprHeightStartScreen;
	int texIDStartScreen = setupTexture("../../Textures/characters/PNG/Nave/start.png", sprWidthStartScreen, sprHeightStartScreen);
	startScreen.initialize(1, 1);
	startScreen.setPosition(glm::vec3(400.0, 400.0, 0.0));
	startScreen.setDimensions(glm::vec3(sprWidthStartScreen * 0.8, sprHeightStartScreen * 0.8, 1.0));
	startScreen.setShader(shader);
	startScreen.setTexID(texIDStartScreen);
}


bool testaCondicaoVitoria() {
	for (int j = 0; j < qtdEtsLinha; j++) {
		for (int i = 0; i < qtdEtsColuna; i++) {
			if (!naveEt[j][i].getMorreu()) {
				return false;
			}
		}
	}

	return true;
}

bool testaCondicaoDerrota() {
	return vidaContagem == 0;
}

void testaTiroEmAlien() {
	for (int j = 0; j < qtdEtsLinha; j++) {
		for (int i = 0; i < qtdEtsColuna; i++) {
			if (!naveEt[j][i].getMorreu() && tiro.getAtivo() && testaColisaoTiroEmObjeto(tiro.getPosition(), naveEt[j][i].getPosition(), 16)) {
				naveEt[j][i].setMorreu(true);
				explosao.setPosition(naveEt[j][i].getPosition());
				explodindo = 4;
				tiro.setAtivo(false);
				break;
			}
		}
		if (!tiro.getAtivo()) break;
	}
}

void testaTiroEmNave() {
	if (tiroAlien.getAtivo() && testaColisaoTiroEmObjeto(tiroAlien.getPosition(), naveUsuario.getPosition(), 16)) {
		vida.setState(MOVING);
		tiroAlien.setAtivo(false);
		vidaContagem--;
	}
}

void testaAliensLateral() {
	for (int j = 0; j < qtdEtsLinha; j++) {
		if (naveEt[j][0].getAlterouDirecao()) {
			for (int i = 0; i < qtdEtsColuna; i++) {
				naveEt[j][i].setMovendoEsquerda(naveEt[j][0].getMovendoEsquerda());
			}
			naveEt[j][0].setAlterouDirecao(false);
		}
		else if (naveEt[j][qtdEtsColuna - 1].getAlterouDirecao()) {
			for (int i = 0; i < qtdEtsColuna; i++) {
				naveEt[j][i].setMovendoEsquerda(naveEt[j][qtdEtsColuna - 1].getMovendoEsquerda());
			}
			naveEt[j][qtdEtsColuna - 1].setAlterouDirecao(false);
		}
	}
}

void testaTiroEmBloco() {
	for (int i = 0; i < qtdBlocos; i++) {
		if (blocos[i].isVivo() && tiroAlien.getAtivo() && testaColisaoTiroEmObjeto(tiroAlien.getPosition(), blocos[i].getPosition(), 64)) {
			blocos[i].setState(MOVING);
			tiroAlien.setAtivo(false);
			blocos[i].diminuiVida();
		}
		if (blocos[i].isVivo() && tiro.getAtivo() && testaColisaoTiroEmObjeto(tiro.getPosition(), blocos[i].getPosition(), 48)) {
			blocos[i].setState(MOVING);
			tiro.setAtivo(false);
			blocos[i].diminuiVida();
		}
	}
}

void diparaTiroAlien() {
	timerAlienTiro--;
	if (timerAlienTiro <= 0 && !tiroAlien.getAtivo()) {
		while (1) {
			int yEt = rand() % 4;
			int xEt = rand() % 6;
			if (!naveEt[yEt][xEt].getMorreu()) {
				glm::vec3 et = naveEt[yEt][xEt].getPosition();
				tiroAlien.setPosition(glm::vec3(et.x, et.y, et.z));
				tiroAlien.move();
				tiroAlien.setAtivo(true);
				break;
			}
		}
		timerAlienTiro = 10;
	}
}

void atualizaEDesenha() {
	naveUsuario.update();
	naveUsuario.draw();

	vida.update();
	vida.draw();
	vida.setState(IDLE);
	for (int i = 0; i < qtdBlocos; i++) {
		if (blocos[i].isVivo()) {
			blocos[i].update();
			blocos[i].draw();
			blocos[i].setState(IDLE);
		}
	}
	if (explodindo > 0) {
		explosao.update();
		explosao.draw();
		explodindo--;
		if (explodindo == 0)
			Sprite explosao;
	}

	if (tiro.getAtivo()) {
		tiro.update();
		tiro.draw();
		tiro.move();
		if (tiro.getPosition().y > 800)
			tiro.setAtivo(false);
	}
	if (tiroAlien.getAtivo()) {
		tiroAlien.update();
		tiroAlien.draw();
		tiroAlien.move();
		if (tiroAlien.getPosition().y < 0)
			tiroAlien.setAtivo(false);
	}

	for (int j = 0; j < qtdEtsLinha; j++) {
		for (int i = 0; i < qtdEtsColuna; i++) {
			naveEt[j][i].update();
			naveEt[j][i].move();
			if (!naveEt[j][i].getMorreu())
				naveEt[j][i].draw();
		}
	}
}

bool testaColisaoTiroEmObjeto(glm::vec3 tiroPos, glm::vec3 objetoPos, int offset) {
	return (abs(abs(objetoPos.x) - abs(tiroPos.x)) < offset && abs(abs(objetoPos.y) - abs(tiroPos.y)) < offset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (gameState == PLAYING) {
		if ( key == GLFW_KEY_A || key == GLFW_KEY_LEFT )
		{
			naveUsuario.setState(MOVING);
			naveUsuario.moveLeft();
		}
		else if ( key == GLFW_KEY_D || key == GLFW_KEY_RIGHT )
		{
			naveUsuario.setState(MOVING);
			naveUsuario.moveRight();
		}
		if (action == GLFW_RELEASE) //soltou a tecla
		{
			naveUsuario.setState(IDLE);
		}
		if (key == GLFW_KEY_G) {
			gameState = VICTORY;
		}
		if (key == GLFW_KEY_P) {
			gameState = GAME_OVER;
		}
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (gameState == STARTING) {
			gameState = PLAYING;
			return;
		}
		glm::vec3 tiroPosicao = tiro.getPosition();
		if (!tiro.getAtivo() || tiroPosicao.x < 0 || tiroPosicao.x > 800 || tiroPosicao.y < 0 || tiroPosicao.y > 800) {
			glm::vec3 navePosicao = naveUsuario.getPosition();
			tiro.setPosition(glm::vec3(navePosicao.x, navePosicao.y + 4.0 * (8.0), navePosicao.z));
			tiro.move();
			tiro.setAtivo(true);
		}
	}
}

int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		//x   y     z    s    t   
		-0.5, -0.5, 0.0, 0.0, 0.0,//v0
		 0.5, -0.5, 0.0, 1.0, 0.0,//v1
 		 0.0,  0.5, 0.0, 0.5, 1.0 //v2 
	};

	GLuint VBO, VAO;
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo coordenada de textura 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0); 

	return VAO;
}

int setupTexture(string filePath, int &width, int &height)
{
	GLuint texID;
	// Geração do identificador do buffer
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Configurando parâmetro de wrapping da textura em s e t
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	//Configurando o parâmetro de filtering de magnificação e minificação da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	
	int nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 4);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}
	else
	{
		cout << "Erro ao ler a textura!" << endl;
		return -1;
	}
	
	return texID;
}

int setupSprite()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		//Primeiro Triângulo
		//x   y     z    s    t   
		-0.5, -0.5, 0.0, 0.0, 0.0, //v0
		 0.5,  0.5, 0.0, 1.0, 1.0, //v1
		-0.5,  0.5, 0.0, 0.0, 1.0, //v2

		//Segundo Triângulo
		-0.5, -0.5, 0.0, 0.0, 0.0, //v0
		 0.5, -0.5, 0.0, 1.0, 0.0, //v3 
		 0.5,  0.5, 0.0, 1.0, 1.0, //v1
	};

	GLuint VBO, VAO;
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo coordenada de textura 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}


