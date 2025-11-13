/*---------------------------------------------------------*/
/* ----------------   Práctica  --------------------------*/
/*-----------------    2026-1   ---------------------------*/
/*------------- Alumno:                     ---------------*/
/*------------- No. Cuenta                  ---------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));
float MovementSpeed = 5.0f;
GLfloat lastX = SCR_WIDTH / 2.0f,
		lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la práctica 6
unsigned int generateTextures(char*, bool, bool);	// De la práctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_ladrillos;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// posiciones
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 90.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotRodIzqInc = 0.0f,
		giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	
	if(isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{

	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/sand.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}



void animate(void) 
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{
		movAuto_x += 3.0f;
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pratica X 2026-1", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);

	

	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 
	
	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	//Model piso("resources/objects/piso/piso.obj");
	//Model carro("resources/objects/lambo/carroceria.obj");
	//Model llanta("resources/objects/lambo/Wheel.obj");
	//Model casaVieja("resources/objects/casa/OldHouse.obj");
	//Model cubo("resources/objects/cubo/cube02.obj");
	//Model casaDoll("resources/objects/casa/DollHouse.obj");

	//ModelAnim animacionPersonaje("resources/objects/Personaje1/Arm.dae");
	//animacionPersonaje.initShaders(animShader.ID);
	Model tercerPiso("resources/General_Models/MuseoJumex.obj");
	Model segundoPiso("resources/General_Models/MuseoJumexSegundoPiso.obj");
	Model primerPiso("resources/General_Models/MuseoJumexTercerPiso.obj");
	Model vestibulo("resources/General_Models/vestibulo.obj");

	Model pintura("resources/f1/frame1.obj");
	Model pintura1("resources/f2/frame02.obj");
	Model pintura2("resources/f3/frame03.obj");
	Model pintura3("resources/f4/frame04.obj");
	Model pintura4("resources/f5/frame05.obj");

	Model sillon("resources/objects/sillon/sillon.obj");
	Model extintor("resources/objects/extintor/extintor.obj");

	//Modelos de planta baja

	Model easels("resources/General_Models/easels.obj");
	Model anunciacion("resources/planta_baja/anunciacion.obj");
	Model armino("resources/planta_baja/armino.obj");
	Model bautista("resources/planta_baja/bautista.obj");
	Model benci("resources/planta_baja/benci.obj");
	Model cristo("resources/planta_baja/cristo.obj");
	Model magos("resources/planta_baja/magos.obj");
	Model monalisa("resources/planta_baja/monalisa.obj");
	Model mundi("resources/planta_baja/mundi.obj");
	Model ultimacena("resources/planta_baja/ultimacena.obj");
	Model vitruvio("resources/planta_baja/vitruvio.obj");
			
			// Caballero de la planta baja

	Model CaballeroCuerpo("resources/caballero/cuerpo.obj");
	Model CaballeroBrazoIzquierdo("resources/caballero/brazoIzquierdo.obj");
	Model CaballeroBrazoDerecho("resources/caballero/brazoDerecho.obj");
	Model CaballeroPiernaIzquierda("resources/caballero/perinaIzquierda.obj");
	Model CaballeroPiernaDerecha("resources/caballero/perinaDerecha.obj");


	Model Colorful_Plant("resources/plants/Colorful_Plant/karafuruueki.obj");
	Model Tree("resources/plants/Tree/Basic_Tree_1.obj");
	Model Candle("resources/Candle/Model.obj");


	
	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection


	// Modelos Temporales para jerarquia
	mat4 tempJumex = mat4(1.0f);
	mat4 tempPrimero = mat4(1.0f);
	mat4 tempSegundo = mat4(1.0f);
	mat4 tempTercero = mat4(1.0f);
	mat4 tempEasels = mat4(1.0f);
	mat4 tempCaballero = mat4(1.0f);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		// ===================================================================
		// LUCES DE FOCO PARA LOS CANDLE'S
		// ===================================================================

		glm::vec3 purpleDiffuse = glm::vec3(0.6f, 0.1f, 0.8f); 
		glm::vec3 purpleAmbient = purpleDiffuse * glm::vec3(0.1f); 
		glm::vec3 purpleSpecular = glm::vec3(1.0f); 

		float lightHeight = 8.0f;
		glm::mat4 candleModelMatrix; 
		glm::vec3 candleWorldPos;  

		candleModelMatrix = translate(tempJumex, vec3(12.0f, lightHeight, -24.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[2].position", candleWorldPos);
		staticShader.setVec3("pointLight[2].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[2].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[2].specular", purpleSpecular);
		staticShader.setFloat("pointLight[2].constant", 1.0f);
		staticShader.setFloat("pointLight[2].linear", 0.14f);  
		staticShader.setFloat("pointLight[2].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(12.0f, lightHeight, -14.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[3].position", candleWorldPos);
		staticShader.setVec3("pointLight[3].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[3].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[3].specular", purpleSpecular);
		staticShader.setFloat("pointLight[3].constant", 1.0f);
		staticShader.setFloat("pointLight[3].linear", 0.14f);
		staticShader.setFloat("pointLight[3].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(12.0f, lightHeight, -4.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[4].position", candleWorldPos);
		staticShader.setVec3("pointLight[4].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[4].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[4].specular", purpleSpecular);
		staticShader.setFloat("pointLight[4].constant", 1.0f);
		staticShader.setFloat("pointLight[4].linear", 0.14f);
		staticShader.setFloat("pointLight[4].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(12.0f, lightHeight, 5.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[5].position", candleWorldPos);
		staticShader.setVec3("pointLight[5].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[5].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[5].specular", purpleSpecular);
		staticShader.setFloat("pointLight[5].constant", 1.0f);
		staticShader.setFloat("pointLight[5].linear", 0.14f);
		staticShader.setFloat("pointLight[5].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(12.0f, lightHeight, 15.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[6].position", candleWorldPos);
		staticShader.setVec3("pointLight[6].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[6].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[6].specular", purpleSpecular);
		staticShader.setFloat("pointLight[6].constant", 1.0f);
		staticShader.setFloat("pointLight[6].linear", 0.14f);
		staticShader.setFloat("pointLight[6].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(-12.0f, lightHeight, -24.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[7].position", candleWorldPos);
		staticShader.setVec3("pointLight[7].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[7].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[7].specular", purpleSpecular);
		staticShader.setFloat("pointLight[7].constant", 1.0f);
		staticShader.setFloat("pointLight[7].linear", 0.14f);
		staticShader.setFloat("pointLight[7].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(-12.0f, lightHeight, -14.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[8].position", candleWorldPos);
		staticShader.setVec3("pointLight[8].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[8].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[8].specular", purpleSpecular);
		staticShader.setFloat("pointLight[8].constant", 1.0f);
		staticShader.setFloat("pointLight[8].linear", 0.14f);
		staticShader.setFloat("pointLight[8].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(-12.0f, lightHeight, -4.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[9].position", candleWorldPos);
		staticShader.setVec3("pointLight[9].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[9].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[9].specular", purpleSpecular);
		staticShader.setFloat("pointLight[9].constant", 1.0f);
		staticShader.setFloat("pointLight[9].linear", 0.14f);
		staticShader.setFloat("pointLight[9].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(-12.0f, lightHeight, 5.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[10].position", candleWorldPos);
		staticShader.setVec3("pointLight[10].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[10].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[10].specular", purpleSpecular);
		staticShader.setFloat("pointLight[10].constant", 1.0f);
		staticShader.setFloat("pointLight[10].linear", 0.14f);
		staticShader.setFloat("pointLight[10].quadratic", 0.07f);

		candleModelMatrix = translate(tempJumex, vec3(-12.0f, lightHeight, 15.5f));
		candleWorldPos = glm::vec3(candleModelMatrix[3]);
		staticShader.setVec3("pointLight[11].position", candleWorldPos);
		staticShader.setVec3("pointLight[11].ambient", purpleAmbient);
		staticShader.setVec3("pointLight[11].diffuse", purpleDiffuse);
		staticShader.setVec3("pointLight[11].specular", purpleSpecular);
		staticShader.setFloat("pointLight[11].constant", 1.0f);
		staticShader.setFloat("pointLight[11].linear", 0.14f);
		staticShader.setFloat("pointLight[11].quadratic", 0.07f);

		// ===================================================================
		// LUCES DE FOCO ANARANJADAS DE ALTA INTENSIDAD PARA CADA CUADRO
		// ===================================================================

		staticShader.setVec3("spotLight[1].position", glm::vec3(-19.7f, 18.0f, 9.0f));   
		staticShader.setVec3("spotLight[1].direction", glm::vec3(0.0f, -1.0f, 0.0f));   
		staticShader.setVec3("spotLight[1].ambient", glm::vec3(0.8f, 0.4f, 0.1f));      
		staticShader.setVec3("spotLight[1].diffuse", glm::vec3(1.0f, 0.5f, 0.1f));      
		staticShader.setVec3("spotLight[1].specular", glm::vec3(1.0f, 0.7f, 0.3f));      
		staticShader.setFloat("spotLight[1].cutOff", glm::cos(glm::radians(20.0f)));    
		staticShader.setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(35.0f))); 
		staticShader.setFloat("spotLight[1].constant", 1.0f);
		staticShader.setFloat("spotLight[1].linear", 0.005f);   
		staticShader.setFloat("spotLight[1].quadratic", 0.0001f);

		staticShader.setVec3("spotLight[2].position", glm::vec3(-19.7f, 18.0f, 21.0f));
		staticShader.setVec3("spotLight[2].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[2].ambient", glm::vec3(0.8f, 0.4f, 0.1f));
		staticShader.setVec3("spotLight[2].diffuse", glm::vec3(1.0f, 0.5f, 0.1f));
		staticShader.setVec3("spotLight[2].specular", glm::vec3(1.0f, 0.7f, 0.3f));
		staticShader.setFloat("spotLight[2].cutOff", glm::cos(glm::radians(20.0f)));
		staticShader.setFloat("spotLight[2].outerCutOff", glm::cos(glm::radians(35.0f)));
		staticShader.setFloat("spotLight[2].constant", 1.0f);
		staticShader.setFloat("spotLight[2].linear", 0.005f);
		staticShader.setFloat("spotLight[2].quadratic", 0.0001f);

		staticShader.setVec3("spotLight[3].position", glm::vec3(-19.7f, 18.0f, 32.0f));
		staticShader.setVec3("spotLight[3].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[3].ambient", glm::vec3(0.8f, 0.4f, 0.1f));
		staticShader.setVec3("spotLight[3].diffuse", glm::vec3(1.0f, 0.5f, 0.1f));
		staticShader.setVec3("spotLight[3].specular", glm::vec3(1.0f, 0.7f, 0.3f));
		staticShader.setFloat("spotLight[3].cutOff", glm::cos(glm::radians(20.0f)));
		staticShader.setFloat("spotLight[3].outerCutOff", glm::cos(glm::radians(35.0f)));
		staticShader.setFloat("spotLight[3].constant", 1.0f);
		staticShader.setFloat("spotLight[3].linear", 0.005f);
		staticShader.setFloat("spotLight[3].quadratic", 0.0001f);

		staticShader.setVec3("spotLight[4].position", glm::vec3(19.7f, 18.0f, 9.0f));
		staticShader.setVec3("spotLight[4].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[4].ambient", glm::vec3(0.8f, 0.4f, 0.1f));
		staticShader.setVec3("spotLight[4].diffuse", glm::vec3(1.0f, 0.5f, 0.1f));
		staticShader.setVec3("spotLight[4].specular", glm::vec3(1.0f, 0.7f, 0.3f));
		staticShader.setFloat("spotLight[4].cutOff", glm::cos(glm::radians(20.0f)));
		staticShader.setFloat("spotLight[4].outerCutOff", glm::cos(glm::radians(35.0f)));
		staticShader.setFloat("spotLight[4].constant", 1.0f);
		staticShader.setFloat("spotLight[4].linear", 0.005f);
		staticShader.setFloat("spotLight[4].quadratic", 0.0001f);

		staticShader.setVec3("spotLight[5].position", glm::vec3(19.7f, 18.0f, 21.0f));
		staticShader.setVec3("spotLight[5].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[5].ambient", glm::vec3(0.8f, 0.4f, 0.1f));
		staticShader.setVec3("spotLight[5].diffuse", glm::vec3(1.0f, 0.5f, 0.1f));
		staticShader.setVec3("spotLight[5].specular", glm::vec3(1.0f, 0.7f, 0.3f));
		staticShader.setFloat("spotLight[5].cutOff", glm::cos(glm::radians(20.0f)));
		staticShader.setFloat("spotLight[5].outerCutOff", glm::cos(glm::radians(35.0f)));
		staticShader.setFloat("spotLight[5].constant", 1.0f);
		staticShader.setFloat("spotLight[5].linear", 0.005f);
		staticShader.setFloat("spotLight[5].quadratic", 0.0001f);


		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------




		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar código aquí
		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_ladrillos);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO[0]);
		//Colocar código aquí
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f, 5.0f, 1.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_unam);
		//glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		// -------------------------------------------------------------------------------------------------------------------------
		// Museo Jumex
		// -------------------------------------------------------------------------------------------------------------------------

		tempJumex = modelOp = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
		tmp = modelOp = rotate(modelOp, radians(giroMonito), vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		vestibulo.Draw(staticShader);

		tempPrimero = modelOp = translate(tempJumex, vec3(0.0f, 9.50f, 0.0f));
		tmp = modelOp = rotate(modelOp, radians(giroMonito), vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		primerPiso.Draw(staticShader);

		tempSegundo = modelOp = translate(tempPrimero, vec3(0.0f, 12.0f, 0.0f));
		tmp = modelOp = rotate(modelOp, radians(giroMonito), vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		segundoPiso.Draw(staticShader);

		tempTercero = modelOp = translate(tempSegundo, vec3(0.0f, 30.50f, -50.0f));
		tmp = modelOp = rotate(modelOp, radians(giroMonito), vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		tercerPiso.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Modelos de la planta baja
		// -------------------------------------------------------------------------------------------------------------------------
		
		for (int i = 0; i < 50; i = i + 10) {
			tempEasels = modelOp = translate(tempJumex, vec3(12.0f, 1.0f, -25.0f + i));
			modelOp = scale(modelOp, vec3(1.0f));
			modelOp = rotate(modelOp, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
			staticShader.setMat4("model", modelOp);
			easels.Draw(staticShader);

			tempEasels = modelOp = translate(tempJumex, vec3(-12.0f, 1.0f, -25.0f + i));
			modelOp = scale(modelOp, vec3(1.0f));
			modelOp = rotate(modelOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
			staticShader.setMat4("model", modelOp);
			easels.Draw(staticShader);
		}

		// Cuadros de planta baja

		mat4 cuadroOp = translate(tempJumex, vec3(12.0f, 5.3f, -24.5f));
		cuadroOp = rotate(cuadroOp, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		anunciacion.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(12.0f, 5.3f, -14.5f));
		cuadroOp = rotate(cuadroOp, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		armino.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(12.0f, 5.3f, -4.5f));
		cuadroOp = rotate(cuadroOp, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		bautista.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(12.0f, 5.3f, 5.5f));
		cuadroOp = rotate(cuadroOp, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		benci.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(12.0f, 5.3f, 15.5f));
		cuadroOp = rotate(cuadroOp, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		cristo.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(-12.0f, 5.3f, -24.5f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		magos.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(-12.0f, 5.3f, -14.5f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		monalisa.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(-12.0f, 5.3f, -4.5f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		mundi.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(-12.0f, 5.3f, 5.5f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		ultimacena.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(-12.0f, 5.3f, 15.5f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(0.8f * 0.17f * 0.67f));
		staticShader.setMat4("model", cuadroOp);
		vitruvio.Draw(staticShader);


			// Caballero de recepcion

		float tiempo = glfwGetTime();
		float angulo = sin(tiempo * 2.0f) * 10.0f;

		tempCaballero = cuadroOp = translate(tempJumex, vec3(-2.0f, 1.0f, -24.5f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3( 10 * 0.17f));
		staticShader.setMat4("model", cuadroOp);
		CaballeroPiernaIzquierda.Draw(staticShader);

		cuadroOp = translate(tempCaballero, vec3(0.0f, 0.0f, 0.0f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(10 * 0.17f));
		staticShader.setMat4("model", cuadroOp);
		CaballeroPiernaDerecha.Draw(staticShader);

		cuadroOp = translate(tempCaballero, vec3(0.0f, 0.0f, 0.0f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(10 * 0.17f));
		staticShader.setMat4("model", cuadroOp);
		CaballeroCuerpo.Draw(staticShader);

		cuadroOp = translate(tempCaballero, vec3(-0.40f, 3.3f, -0.2f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = rotate(cuadroOp, radians(-45.0f + angulo), vec3(1.0f, 0.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(10 * 0.17f));
		staticShader.setMat4("model", cuadroOp);
		CaballeroBrazoDerecho.Draw(staticShader);

		cuadroOp = translate(tempCaballero, vec3(0.40f, 3.3f, 0.2f));
		cuadroOp = rotate(cuadroOp, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
		cuadroOp = rotate(cuadroOp, radians(-45.0f + angulo), vec3(1.0f, 0.0f, 0.0f));
		cuadroOp = scale(cuadroOp, vec3(10 * 0.17f));
		staticShader.setMat4("model", cuadroOp);
		CaballeroBrazoIzquierdo.Draw(staticShader);

		// Arboles decoracion 

		for (int i = 0; i < 30; i++) {
			float ang = radians(137.5f * i);
			float r = 2.0f * sqrt(i);
			float x = r * cos(ang) * 10.0f;
			float z = r * sin(ang) * 10.0f;
			mat4 arbolOp = translate(mat4(1.0f), vec3(200.0f + x, -3.0f, z));
			arbolOp = rotate(arbolOp, radians(i * 12.0f), vec3(0.0f, 1.0f, 0.0f));
			staticShader.setMat4("model", arbolOp);
			Tree.Draw(staticShader);
		}

		for (int i = 0; i < 30; i++) {
			float ang = radians(137.5f * i);
			float r = 2.0f * sqrt(i);
			float x = r * cos(ang) * 10.0f;
			float z = r * sin(ang) * 10.0f;
			mat4 arbolOp = translate(mat4(1.0f), vec3(-200.0f + x, -3.0f, z));
			arbolOp = rotate(arbolOp, radians(i * 12.0f), vec3(0.0f, 1.0f, 0.0f));
			staticShader.setMat4("model", arbolOp);
			Tree.Draw(staticShader);
		}

		for (int i = 0; i < 30; i++) {
			float ang = radians(137.5f * i);
			float r = 2.0f * sqrt(i);
			float x = r * cos(ang) * 10.0f;
			float z = r * sin(ang) * 10.0f;
			mat4 arbolOp = translate(mat4(1.0f), vec3(x, -3.0f, 200.0f + z));
			arbolOp = rotate(arbolOp, radians(i * 12.0f), vec3(0.0f, 1.0f, 0.0f));
			staticShader.setMat4("model", arbolOp);
			Tree.Draw(staticShader);
		}

		for (int i = 0; i < 30; i++) {
			float ang = radians(137.5f * i);
			float r = 2.0f * sqrt(i);
			float x = r * cos(ang) * 10.0f;
			float z = r * sin(ang) * 10.0f;
			mat4 arbolOp = translate(mat4(1.0f), vec3(x, -3.0f, -200.0f + z));
			arbolOp = rotate(arbolOp, radians(i * 12.0f), vec3(0.0f, 1.0f, 0.0f));
			staticShader.setMat4("model", arbolOp);
			Tree.Draw(staticShader);
		}

			// Plantas de recepcion 

		float radio = 6.0f;
		int numPlantas = 5;

		for (int i = 0; i < numPlantas; i++) {
			float angulo = radians(360.0f / numPlantas * i);
			float x = sin(angulo) * radio;
			float z = cos(angulo) * radio;

			cuadroOp = translate(tempCaballero, vec3(x, -2.2f, z));
			cuadroOp = scale(cuadroOp, vec3(0.3f));
			staticShader.setMat4("model", cuadroOp);
			Colorful_Plant.Draw(staticShader);
		}

			// Candle's de la planta baja
		float PI = 3.141591;
		float radio2 = 4.0f;
		float freqX = 3.0f;   
		float freqZ = 4.0f;  
		float phase = PI / 2;  

		float t = (sin(tiempo) * 0.5f) + 0.5f;
		float xDer = mix(0.0f, 12.0f, t);
		float xIzq = mix(0.0f, -12.0f, t);

		float lissajousX = sin(tiempo * freqX) * radio2;
		float lissajousZ = sin(tiempo * freqZ + phase) * radio2;
		float factorCentro = (1.0f - abs(sin(tiempo))) * 1.0f;

		cuadroOp = translate(tempJumex, vec3(xDer + lissajousX, 6.3f * 1.25f, -24.5f + lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xDer + lissajousX, 6.3f * 1.25f, -14.5f + lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xDer + lissajousX, 6.3f * 1.25f, -4.5f + lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xDer + lissajousX, 6.3f * 1.25f, 5.5f + lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xDer + lissajousX, 6.3f * 1.25f, 15.5f + lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xIzq - lissajousX, 6.3f * 1.25f, -24.5f - lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xIzq - lissajousX, 6.3f * 1.25f, -14.5f - lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xIzq - lissajousX, 6.3f * 1.25f, -4.5f - lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xIzq - lissajousX, 6.3f * 1.25f, 5.5f - lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);

		cuadroOp = translate(tempJumex, vec3(xIzq - lissajousX, 6.3f * 1.25f, 15.5f - lissajousZ * factorCentro));
		staticShader.setMat4("model", cuadroOp);
		Candle.Draw(staticShader);


		// -------------------------------------------------------------------------------------------------------------------------
		// Modelos 1er piso
		// -------------------------------------------------------------------------------------------------------------------------
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-19.7f, 15.5f, 9.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.15f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		pintura.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-19.7f, 15.5f, 21.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.17f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		pintura1.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-19.7f, 15.5f, 32.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.17f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		pintura2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(19.7f, 15.5f, 9.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.17f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		pintura3.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(19.7f, 15.5f, 21.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.169f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		pintura4.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 12.8f, -8.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.009f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		sillon.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(19.7f, 11.8f, 40.0f));
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		extintor.Draw(staticShader);
		
		// -------------------------------------------------------------------------------------------------------------------------
		// Just in case
		// -------------------------------------------------------------------------------------------------------------------------
		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		torso.Draw(staticShader);

		//Pierna Der
		modelOp = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::rotate(modelOp, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaDer.Draw(staticShader);

		//Pie Der
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);

		//Pierna Izq
		modelOp = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaIzq.Draw(staticShader);

		//Pie Iz
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);	//Izq trase

		//Brazo derecho
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(-0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoDer.Draw(staticShader);

		//Brazo izquierdo
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoIzq.Draw(staticShader);

		//Cabeza
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", modelOp);
		cabeza.Draw(staticShader);*/

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	// Camaras fijas
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		camera.Position = glm::vec3(0.0f, 5.0f, -40.0f);
		camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		camera.Position = glm::vec3(0.0f, 15.0f, -40.0f);
		camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		camera.Position = glm::vec3(0.0f, 25.0f, -40.0f);
		camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		camera.Position = glm::vec3(0.0f, 45.0f, -40.0f);
		camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
		camera.Position = glm::vec3(200.0f, 150.0f, -200.0f);
		camera.Yaw = -125.0f;
		camera.Pitch = -35.264f;
		camera.Front = glm::normalize(glm::vec3(-1.0f, -0.4f, 1.0f));
	}
	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
