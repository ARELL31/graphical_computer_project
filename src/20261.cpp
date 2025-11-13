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
#include <al.h>
#include <alc.h>

#ifdef _WIN32
#pragma comment(lib, "OpenAL32.lib")
#endif


using namespace std;
using namespace glm;

struct AudioZone {
	glm::vec3 center;
	float radius;
	ALuint source;
	ALuint buffer;
	bool isPlaying;
	string audioFile;
};

// Variables para gestión de audio por zonas
std::vector<AudioZone> audioZones;
ALCdevice* audioDevice = nullptr;
ALCcontext* audioContext = nullptr;

// Prototipos de funciones de audio
void initializeAudioSystem();
void createAudioZone(const glm::vec3& center, float radius, const std::string& audioFile);
void updateAudioZones();
void cleanupAudioSystem();

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
Camera camera(glm::vec3(0.0f, 5.0f, 3.0f));
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
float	movAuto_y1 = 0.0f,
		movAuto_x2 = 1.5f,
		movAuto_z2 = 1.5f,
		movAuto_x3 = 0.0f,
		movAuto_z3 = 0.0f,
		orienta_x1 = 0.0f,
		orienta_y1 = 180.0f,
		orienta_z1 = 0.0f,
		giroCarro2 = 0.0f,
		angleCarro2 = 45.0f,
		giroCarro3 = 0.0f,
		angleCarro3 = 45.0f,
		giroRuedas = 0.0f;
int		etapa_giro = 0,
		etapa_track1 = 0,
		etapa_track2 = 0;
bool	animacion1 = false,
		animacion2 = false;

float paraBaseX = 10.0f;
float paraBaseY = 25.0f;  // inicia arriba del segundo piso
float paraBaseZ = -20.0f;

float paraOffsetY = 0.0f;
float paraSpeed = 0.05f;   // velocidad mayor para ciclo visible
bool paraGoingDown = true;

//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f,
//pinturas key frame
//pinturas key frame - POSICIONES EN PAREDES
		posXp1 = -19.7f,
		posYp1 = 15.5f,
		posZp1 = 7.0f,
		rotp1 = 180.0f,
		scalep1 = 0.15f,

		posXp2 = -19.7f,
		posYp2 = 15.5f,
		posZp2 = 17.0f,
		rotp2 = 180.0f,
		scalep2 = 0.17f,

		posXp3 = -19.7f,
		posYp3 = 15.5f,
		posZp3 = 27.0f,
		rotp3 = 180.0f,
		scalep3 = 0.17f,

		posXp4 = -19.7f,
		posYp4 = 15.5f,
		posZp4 = 37.0f,
		rotp4 = 180.0f,
		scalep4 = 0.169f,

		posXp5 = 19.7f,
		posYp5 = 15.5f,
		posZp5 = 7.0f,
		rotp5 = 0.0f,
		scalep5 = 0.17f,

		posXp6 = 19.7f,
		posYp6 = 15.5f,
		posZp6 = 17.0f,
		rotp6 = 0.0f,
		scalep6 = 0.169f,

		posXp7 = 19.7f,
		posYp7 = 15.5f,
		posZp7 = 27.0f,
		rotp7 = 0.0f,
		scalep7 = 0.169f,

		posXp8 = 19.7f,
		posYp8 = 15.5f,
		posZp8 = 37.0f,
		rotp8 = 0.0f,
		scalep8 = 0.169f,

		posXp9 = 15.7f,
		posYp9 = 15.5f,
		posZp9 = -42.2f,
		rotp9 = 90.0f,
		scalep9 = 0.169f,

		posXp10 = -15.7f,
		posYp10 = 15.5f,
		posZp10 = -42.2f,
		rotp10 = 90.0f,
		scalep10 = 0.169f,

		posXp11 = -15.7f,
		posYp11 = 15.5f,
		posZp11 = 42.2f,
		rotp11 = -90.0f,
		scalep11 = 0.169f,

		posXp12 = 15.7f,
		posYp12 = 15.5f,
		posZp12 = 42.2f,
		rotp12 = -90.0f,
		scalep12 = 0.169f;

float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotRodIzqInc = 0.0f,
		giroMonitoInc = 0.0f,
		//pinturas
		incposXp1 = 0.0f,
		incposYp1 = 0.0f,
		incposZp1 = 0.0f,
		incrotp1 = 0.0f,
		incscalep1 = 0.0f,

		incposXp2 = 0.0f,
		incposYp2 = 0.0f,
		incposZp2 = 0.0f,
		incrotp2 = 0.0f,
		incscalep2 = 0.0f,

		incposXp3 = 0.0f,
		incposYp3 = 0.0f,
		incposZp3 = 0.0f,
		incrotp3 = 0.0f,
		incscalep3 = 0.0f,

		incposXp4 = 0.0f,
		incposYp4 = 0.0f,
		incposZp4 = 0.0f,
		incrotp4 = 0.0f,
		incscalep4 = 0.0f,

		incposXp5 = 0.0f,
		incposYp5 = 0.0f,
		incposZp5 = 0.0f,
		incrotp5 = 0.0f,
		incscalep5 = 0.0f,

		incposXp6 = 0.0f,
		incposYp6 = 0.0f,
		incposZp6 = 0.0f,
		incrotp6 = 0.0f,
		incscalep6 = 0.0f,

		incposXp7 = 0.0f,
		incposYp7 = 0.0f,
		incposZp7 = 0.0f,
		incrotp7 = 0.0f,
		incscalep7 = 0.0f,

		incposXp8 = 0.0f,
		incposYp8 = 0.0f,
		incposZp8 = 0.0f,
		incrotp8 = 0.0f,
		incscalep8 = 0.0f,

		incposXp9 = 0.0f,
		incposYp9 = 0.0f,
		incposZp9 = 0.0f,
		incrotp9 = 0.0f,
		incscalep9 = 0.0f,

		incposXp10 = 0.0f,
		incposYp10 = 0.0f,
		incposZp10 = 0.0f,
		incrotp10 = 0.0f,
		incscalep10 = 0.0f,

		incposXp11 = 0.0f,
		incposYp11 = 0.0f,
		incposZp11 = 0.0f,
		incrotp11 = 0.0f,
		incscalep11 = 0.0f,

		incposXp12 = 0.0f,
		incposYp12 = 0.0f,
		incposZp12 = 0.0f,
		incrotp12 = 0.0f,
		incscalep12 = 0.0f;

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
	//Variables para pinturas KeyFrames
	float posXp1 = 0.0f;
	float posYp1 = 0.0f;
	float posZp1 = 0.0f;
	float rotp1 = 0.0f;
	float scalep1 = 0.0f;
	float posXp2 = 0.0f;
	float posYp2 = 0.0f;
	float scalep2 = 0.0f;
	float posZp2 = 0.0f;
	float rotp2 = 0.0f;
	float posXp3 = 0.0f;
	float posYp3 = 0.0f;
	float posZp3 = 0.0f;
	float rotp3 = 0.0f;
	float scalep3 = 0.0f;
	float posXp4 = 0.0f;
	float posYp4 = 0.0f;
	float posZp4 = 0.0f;
	float rotp4 = 0.0f;
	float scalep4 = 0.0f;
	float posXp5 = 0.0f;
	float posYp5 = 0.0f;
	float posZp5 = 0.0f;
	float rotp5 = 0.0f;
	float scalep5 = 0.0f;
	float posXp6 = 0.0f;
	float posYp6 = 0.0f;
	float posZp6 = 0.0f;
	float rotp6 = 0.0f;
	float scalep6 = 0.0f;
	float posXp7 = 0.0f;
	float posYp7 = 0.0f;
	float posZp7 = 0.0f;
	float rotp7 = 0.0f;
	float scalep7 = 0.0f;
	float posXp8 = 0.0f;
	float posYp8 = 0.0f;
	float posZp8 = 0.0f;
	float rotp8 = 0.0f;
	float scalep8 = 0.0f;
	float posXp9 = 0.0f;
	float posYp9 = 0.0f;
	float posZp9 = 0.0f;
	float rotp9 = 0.0f;
	float scalep9 = 0.0f;
	float posXp10 = 0.0f;
	float posYp10 = 0.0f;
	float posZp10 = 0.0f;
	float rotp10 = 0.0f;
	float scalep10 = 0.0f;
	float posXp11 = 0.0f;
	float posYp11 = 0.0f;
	float posZp11 = 0.0f;
	float rotp11 = 0.0f;
	float scalep11 = 0.0f;
	float posXp12 = 0.0f;
	float posYp12 = 0.0f;
	float posZp12 = 0.0f;
	float rotp12 = 0.0f;
	float scalep12 = 0.0f;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 7;			//introducir número en caso de tener Key guardados
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

	KeyFrame[FrameIndex].posXp1 = posXp1;
	KeyFrame[FrameIndex].posYp1 = posYp1;
	KeyFrame[FrameIndex].posZp1 = posZp1;
	KeyFrame[FrameIndex].rotp1 = rotp1;
	KeyFrame[FrameIndex].scalep1 = scalep1;

	KeyFrame[FrameIndex].posXp2 = posXp2;
	KeyFrame[FrameIndex].posYp2 = posYp2;
	KeyFrame[FrameIndex].posZp2 = posZp2;
	KeyFrame[FrameIndex].rotp2 = rotp2;
	KeyFrame[FrameIndex].scalep2 = scalep2;

	KeyFrame[FrameIndex].posXp3 = posXp3;
	KeyFrame[FrameIndex].posYp3 = posYp3;
	KeyFrame[FrameIndex].posZp3 = posZp3;
	KeyFrame[FrameIndex].rotp3 = rotp3;
	KeyFrame[FrameIndex].scalep3 = scalep3;

	KeyFrame[FrameIndex].posXp4 = posXp4;
	KeyFrame[FrameIndex].posYp4 = posYp4;
	KeyFrame[FrameIndex].posZp4 = posZp4;
	KeyFrame[FrameIndex].rotp4 = rotp4;
	KeyFrame[FrameIndex].scalep4 = scalep4;

	KeyFrame[FrameIndex].posXp5 = posXp5;
	KeyFrame[FrameIndex].posYp5 = posYp5;
	KeyFrame[FrameIndex].posZp5 = posZp5;
	KeyFrame[FrameIndex].rotp5 = rotp5;
	KeyFrame[FrameIndex].scalep5 = scalep5;

	KeyFrame[FrameIndex].posXp6 = posXp6;
	KeyFrame[FrameIndex].posYp6 = posYp6;
	KeyFrame[FrameIndex].posZp6 = posZp6;
	KeyFrame[FrameIndex].rotp6 = rotp6;
	KeyFrame[FrameIndex].scalep6 = scalep6;

	KeyFrame[FrameIndex].posXp7 = posXp7;
	KeyFrame[FrameIndex].posYp7 = posYp7;
	KeyFrame[FrameIndex].posZp7 = posZp7;
	KeyFrame[FrameIndex].rotp7 = rotp7;
	KeyFrame[FrameIndex].scalep7 = scalep7;

	KeyFrame[FrameIndex].posXp8 = posXp8;
	KeyFrame[FrameIndex].posYp8 = posYp8;
	KeyFrame[FrameIndex].posZp8 = posZp8;
	KeyFrame[FrameIndex].rotp8 = rotp8;
	KeyFrame[FrameIndex].scalep8 = scalep8;

	KeyFrame[FrameIndex].posXp9 = posXp9;
	KeyFrame[FrameIndex].posYp9 = posYp9;
	KeyFrame[FrameIndex].posZp9 = posZp9;
	KeyFrame[FrameIndex].rotp9 = rotp9;
	KeyFrame[FrameIndex].scalep9 = scalep9;

	KeyFrame[FrameIndex].posXp10 = posXp10;
	KeyFrame[FrameIndex].posYp10 = posYp10;
	KeyFrame[FrameIndex].posZp10 = posZp10;
	KeyFrame[FrameIndex].rotp10 = rotp10;
	KeyFrame[FrameIndex].scalep10 = scalep10;

	KeyFrame[FrameIndex].posXp11 = posXp11;
	KeyFrame[FrameIndex].posYp11 = posYp11;
	KeyFrame[FrameIndex].posZp11 = posZp11;
	KeyFrame[FrameIndex].rotp11 = rotp11;
	KeyFrame[FrameIndex].scalep11 = scalep11;

	KeyFrame[FrameIndex].posXp12 = posXp12;
	KeyFrame[FrameIndex].posYp12 = posYp12;
	KeyFrame[FrameIndex].posZp12 = posZp12;
	KeyFrame[FrameIndex].rotp12 = rotp12;
	KeyFrame[FrameIndex].scalep12 = scalep12;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;

	posXp1 = KeyFrame[0].posXp1;
	posYp1 = KeyFrame[0].posYp1;
	posZp1 = KeyFrame[0].posZp1;
	rotp1 = KeyFrame[0].rotp1;
	scalep1 = KeyFrame[0].scalep1;

	posXp2 = KeyFrame[0].posXp2;
	posYp2 = KeyFrame[0].posYp2;
	posZp2 = KeyFrame[0].posZp2;
	rotp2 = KeyFrame[0].rotp2;
	scalep2 = KeyFrame[0].scalep2;

	posXp3 = KeyFrame[0].posXp3;
	posYp3 = KeyFrame[0].posYp3;
	posZp3 = KeyFrame[0].posZp3;
	rotp3 = KeyFrame[0].rotp3;
	scalep3 = KeyFrame[0].scalep3;

	posXp4 = KeyFrame[0].posXp4;
	posYp4 = KeyFrame[0].posYp4;
	posZp4 = KeyFrame[0].posZp4;
	rotp4 = KeyFrame[0].rotp4;
	scalep4 = KeyFrame[0].scalep4;

	posXp5 = KeyFrame[0].posXp5;
	posYp5 = KeyFrame[0].posYp5;
	posZp5 = KeyFrame[0].posZp5;
	rotp5 = KeyFrame[0].rotp5;
	scalep5 = KeyFrame[0].scalep5;
	
	posXp6 = KeyFrame[0].posXp6;
	posYp6 = KeyFrame[0].posYp6;
	posZp6 = KeyFrame[0].posZp6;
	rotp6 = KeyFrame[0].rotp6;
	scalep6 = KeyFrame[0].scalep6;
	
	posXp7 = KeyFrame[0].posXp7;
	posYp7 = KeyFrame[0].posYp7;
	posZp7 = KeyFrame[0].posZp7;
	rotp7 = KeyFrame[0].rotp7;
	scalep7 = KeyFrame[0].scalep7;
	
	posXp8 = KeyFrame[0].posXp8;
	posYp8 = KeyFrame[0].posYp8;
	posZp8 = KeyFrame[0].posZp8;
	rotp8 = KeyFrame[0].rotp8;
	scalep8 = KeyFrame[0].scalep8;
	
	posXp9 = KeyFrame[0].posXp9;
	posYp9 = KeyFrame[0].posYp9;
	posZp9 = KeyFrame[0].posZp9;
	rotp9 = KeyFrame[0].rotp9;
	scalep9 = KeyFrame[0].scalep9;
	
	posXp10 = KeyFrame[0].posXp10;
	posYp10 = KeyFrame[0].posYp10;
	posZp10 = KeyFrame[0].posZp10;
	rotp10 = KeyFrame[0].rotp10;
	scalep10 = KeyFrame[0].scalep10;
	
	posXp11 = KeyFrame[0].posXp11;
	posYp11 = KeyFrame[0].posYp11;
	posZp11 = KeyFrame[0].posZp11;
	rotp11 = KeyFrame[0].rotp11;
	scalep11 = KeyFrame[0].scalep11;
	
	posXp12 = KeyFrame[0].posXp12;
	posYp12 = KeyFrame[0].posYp12;
	posZp12 = KeyFrame[0].posZp12;
	rotp12 = KeyFrame[0].rotp12;
	scalep12 = KeyFrame[0].scalep12;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

	incposXp1 = (KeyFrame[playIndex + 1].posXp1 - KeyFrame[playIndex].posXp1) / i_max_steps;
	incposYp1 = (KeyFrame[playIndex + 1].posYp1 - KeyFrame[playIndex].posYp1) / i_max_steps;
	incposZp1 = (KeyFrame[playIndex + 1].posZp1 - KeyFrame[playIndex].posZp1) / i_max_steps;
	incrotp1 = (KeyFrame[playIndex + 1].rotp1 - KeyFrame[playIndex].rotp1) / i_max_steps;
	incscalep1 = (KeyFrame[playIndex + 1].scalep1 - KeyFrame[playIndex].scalep1) / i_max_steps;
	
	incposXp2 = (KeyFrame[playIndex + 1].posXp2 - KeyFrame[playIndex].posXp2) / i_max_steps;
	incposYp2 = (KeyFrame[playIndex + 1].posYp2 - KeyFrame[playIndex].posYp2) / i_max_steps;
	incposZp2 = (KeyFrame[playIndex + 1].posZp2 - KeyFrame[playIndex].posZp2) / i_max_steps;
	incrotp2 = (KeyFrame[playIndex + 1].rotp2 - KeyFrame[playIndex].rotp2) / i_max_steps;
	incscalep2 = (KeyFrame[playIndex + 1].scalep2 - KeyFrame[playIndex].scalep2) / i_max_steps;
	
	incposXp3 = (KeyFrame[playIndex + 1].posXp3 - KeyFrame[playIndex].posXp3) / i_max_steps;
	incposYp3 = (KeyFrame[playIndex + 1].posYp3 - KeyFrame[playIndex].posYp3) / i_max_steps;
	incposZp3 = (KeyFrame[playIndex + 1].posZp3 - KeyFrame[playIndex].posZp3) / i_max_steps;
	incrotp3 = (KeyFrame[playIndex + 1].rotp3 - KeyFrame[playIndex].rotp3) / i_max_steps;
	incscalep3 = (KeyFrame[playIndex + 1].scalep3 - KeyFrame[playIndex].scalep3) / i_max_steps;
	
	incposXp4 = (KeyFrame[playIndex + 1].posXp4 - KeyFrame[playIndex].posXp4) / i_max_steps;
	incposYp4 = (KeyFrame[playIndex + 1].posYp4 - KeyFrame[playIndex].posYp4) / i_max_steps;
	incposZp4 = (KeyFrame[playIndex + 1].posZp4 - KeyFrame[playIndex].posZp4) / i_max_steps;
	incrotp4 = (KeyFrame[playIndex + 1].rotp4 - KeyFrame[playIndex].rotp4) / i_max_steps;
	incscalep4 = (KeyFrame[playIndex + 1].scalep4 - KeyFrame[playIndex].scalep4) / i_max_steps;
	
	incposXp5 = (KeyFrame[playIndex + 1].posXp5 - KeyFrame[playIndex].posXp5) / i_max_steps;
	incposYp5 = (KeyFrame[playIndex + 1].posYp5 - KeyFrame[playIndex].posYp5) / i_max_steps;
	incposZp5 = (KeyFrame[playIndex + 1].posZp5 - KeyFrame[playIndex].posZp5) / i_max_steps;
	incrotp5 = (KeyFrame[playIndex + 1].rotp5 - KeyFrame[playIndex].rotp5) / i_max_steps;
	incscalep5 = (KeyFrame[playIndex + 1].scalep5 - KeyFrame[playIndex].scalep5) / i_max_steps;
	
	incposXp6 = (KeyFrame[playIndex + 1].posXp6 - KeyFrame[playIndex].posXp6) / i_max_steps;
	incposYp6 = (KeyFrame[playIndex + 1].posYp6 - KeyFrame[playIndex].posYp6) / i_max_steps;
	incposZp6 = (KeyFrame[playIndex + 1].posZp6 - KeyFrame[playIndex].posZp6) / i_max_steps;
	incrotp6 = (KeyFrame[playIndex + 1].rotp6 - KeyFrame[playIndex].rotp6) / i_max_steps;
	incscalep6 = (KeyFrame[playIndex + 1].scalep6 - KeyFrame[playIndex].scalep6) / i_max_steps;
	
	incposXp7 = (KeyFrame[playIndex + 1].posXp7 - KeyFrame[playIndex].posXp7) / i_max_steps;
	incposYp7 = (KeyFrame[playIndex + 1].posYp7 - KeyFrame[playIndex].posYp7) / i_max_steps;
	incposZp7 = (KeyFrame[playIndex + 1].posZp7 - KeyFrame[playIndex].posZp7) / i_max_steps;
	incrotp7 = (KeyFrame[playIndex + 1].rotp7 - KeyFrame[playIndex].rotp7) / i_max_steps;
	incscalep7 = (KeyFrame[playIndex + 1].scalep7 - KeyFrame[playIndex].scalep7) / i_max_steps;
	
	incposXp8 = (KeyFrame[playIndex + 1].posXp8 - KeyFrame[playIndex].posXp8) / i_max_steps;
	incposYp8 = (KeyFrame[playIndex + 1].posYp8 - KeyFrame[playIndex].posYp8) / i_max_steps;
	incposZp8 = (KeyFrame[playIndex + 1].posZp8 - KeyFrame[playIndex].posZp8) / i_max_steps;
	incrotp8 = (KeyFrame[playIndex + 1].rotp8 - KeyFrame[playIndex].rotp8) / i_max_steps;
	incscalep8 = (KeyFrame[playIndex + 1].scalep8 - KeyFrame[playIndex].scalep8) / i_max_steps;
	
	incposXp9 = (KeyFrame[playIndex + 1].posXp9 - KeyFrame[playIndex].posXp9) / i_max_steps;
	incposYp9 = (KeyFrame[playIndex + 1].posYp9 - KeyFrame[playIndex].posYp9) / i_max_steps;
	incposZp9 = (KeyFrame[playIndex + 1].posZp9 - KeyFrame[playIndex].posZp9) / i_max_steps;
	incrotp9 = (KeyFrame[playIndex + 1].rotp9 - KeyFrame[playIndex].rotp9) / i_max_steps;
	incscalep9 = (KeyFrame[playIndex + 1].scalep9 - KeyFrame[playIndex].scalep9) / i_max_steps;
	
	incposXp10 = (KeyFrame[playIndex + 1].posXp10 - KeyFrame[playIndex].posXp10) / i_max_steps;
	incposYp10 = (KeyFrame[playIndex + 1].posYp10 - KeyFrame[playIndex].posYp10) / i_max_steps;
	incposZp10 = (KeyFrame[playIndex + 1].posZp10 - KeyFrame[playIndex].posZp10) / i_max_steps;
	incrotp10 = (KeyFrame[playIndex + 1].rotp10 - KeyFrame[playIndex].rotp10) / i_max_steps;
	incscalep10 = (KeyFrame[playIndex + 1].scalep10 - KeyFrame[playIndex].scalep10) / i_max_steps;
	
	incposXp11 = (KeyFrame[playIndex + 1].posXp11 - KeyFrame[playIndex].posXp11) / i_max_steps;
	incposYp11 = (KeyFrame[playIndex + 1].posYp11 - KeyFrame[playIndex].posYp11) / i_max_steps;
	incposZp11 = (KeyFrame[playIndex + 1].posZp11 - KeyFrame[playIndex].posZp11) / i_max_steps;
	incrotp11 = (KeyFrame[playIndex + 1].rotp11 - KeyFrame[playIndex].rotp11) / i_max_steps;
	incscalep11 = (KeyFrame[playIndex + 1].scalep11 - KeyFrame[playIndex].scalep11) / i_max_steps;
	
	incposXp12 = (KeyFrame[playIndex + 1].posXp12 - KeyFrame[playIndex].posXp12) / i_max_steps;
	incposYp12 = (KeyFrame[playIndex + 1].posYp12 - KeyFrame[playIndex].posYp12) / i_max_steps;
	incposZp12 = (KeyFrame[playIndex + 1].posZp12 - KeyFrame[playIndex].posZp12) / i_max_steps;
	incrotp12 = (KeyFrame[playIndex + 1].rotp12 - KeyFrame[playIndex].rotp12) / i_max_steps;
	incscalep12 = (KeyFrame[playIndex + 1].scalep12 - KeyFrame[playIndex].scalep12) / i_max_steps;
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

			posXp1 += incposXp1;
			posYp1 += incposYp1;
			posZp1 += incposZp1;
			rotp1 += incrotp1;
			scalep1 += incscalep1;

			posXp2 += incposXp2;
			posYp2 += incposYp2;
			posZp2 += incposZp2;
			rotp2 += incrotp2;
			scalep2 += incscalep2;

			posXp3 += incposXp3;
			posYp3 += incposYp3;
			posZp3 += incposZp3;
			rotp3 += incrotp3;
			scalep3 += incscalep3;

			posXp4 += incposXp4;
			posYp4 += incposYp4;
			posZp4 += incposZp4;
			rotp4 += incrotp4;
			scalep4 += incscalep4;

			posXp5 += incposXp5;
			posYp5 += incposYp5;
			posZp5 += incposZp5;
			rotp5 += incrotp5;
			scalep5 += incscalep5;

			posXp6 += incposXp6;
			posYp6 += incposYp6;
			posZp6 += incposZp6;
			rotp6 += incrotp6;
			scalep6 += incscalep6;

			posXp7 += incposXp7;
			posYp7 += incposYp7;
			posZp7 += incposZp7;
			rotp7 += incrotp7;
			scalep7 += incscalep7;

			posXp8 += incposXp8;
			posYp8 += incposYp8;
			posZp8 += incposZp8;
			rotp8 += incrotp8;
			scalep8 += incscalep8;

			posXp9 += incposXp9;
			posYp9 += incposYp9;
			posZp9 += incposZp9;
			rotp9 += incrotp9;
			scalep9 += incscalep9;

			posXp10 += incposXp10;
			posYp10 += incposYp10;
			posZp10 += incposZp10;
			rotp10 += incrotp10;
			scalep10 += incscalep10;

			posXp11 += incposXp11;
			posYp11 += incposYp11;
			posZp11 += incposZp11;
			rotp11 += incrotp11;
			scalep11 += incscalep11;

			posXp12 += incposXp12;
			posYp12 += incposYp12;
			posZp12 += incposZp12;
			rotp12 += incrotp12;
			scalep12 += incscalep12;

			i_curr_steps++;
		}
	}

	//Vehículos
	if (animacion1){
		orienta_y1 += 0.5f;
		switch (etapa_giro) {
		case 0:
			if (orienta_y1 >= 360.0f) {
				orienta_x1 += 0.3f;
				movAuto_y1 = (orienta_x1 / 45.0f) * 7.0f;
				if (orienta_x1 >= 45.0f || movAuto_y1 >= 7.0f) {
					etapa_giro = 1;
				}
			}
			break;
		case 1:
			if (orienta_y1 >= 720.0f) {
				etapa_giro = 2;
			}
			break;
		case 2:
			orienta_x1 -= 0.3f;
			movAuto_y1 = (abs(orienta_x1) / 45.0f) * 7.0f;
			if (orienta_x1 <= -45.0f) {
				etapa_giro = 3;
			}
			break;
		case 3:
			if (orienta_y1 >= 1080.0f) {
				orienta_x1 += 0.3f;
				movAuto_y1 = (orienta_x1 / -45.0f) * 7.0f;
				if (orienta_x1 >= 0.0f || movAuto_y1 <= 0.0f) {
					etapa_giro = 4;
				}
			}
			break;
		case 4:
			if (orienta_y1 >= 1440.0f) {
				orienta_y1 = 0.0f;
				etapa_giro = 0;
			}
			break;
		}
	}
	if (animacion2) {
		giroRuedas += 20.0f;
		if (giroRuedas >= 360.0f) {
			giroRuedas = 0.0f;
		}
		switch (etapa_track1) {
		case 0:
			movAuto_x2 += 0.06f;
			movAuto_z2 = movAuto_x2;
			if (movAuto_z2 >= 1.7) {
				etapa_track1 = 1;
			}
			break;
		case 1:
			if (giroCarro2 < 135.0f) {
				giroCarro2 += 3.0f;
			}
			else {
				giroCarro2 += 2.0f;
			}
			angleCarro2 += 2.0f;
			movAuto_x2 = 0.0f + 2.3f * sin(glm::radians(angleCarro2));
			movAuto_z2 = 3.25f - 2.3f * cos(glm::radians(angleCarro2));
			if (angleCarro2 >= 315.0f) {
				etapa_track1 = 2;
			}
			break;
		case 2:
			if (giroCarro2 > 270.0f) {
				giroCarro2 -= 1.0f;
			}
			movAuto_x2 += 0.04f;
			movAuto_z2 = movAuto_x2 * -1.0f;
			if (movAuto_x2 >= 1.7f) {
				etapa_track1 = 3;
			}
			break;
		case 3:
			giroCarro2 -= 1.0f;
			angleCarro2 -= 1.0f;
			movAuto_x2 = 0.0f - 2.3f * sin(glm::radians(angleCarro2));
			movAuto_z2 = - 3.25f + 2.3f * cos(glm::radians(angleCarro2));
			if (angleCarro2 <= 45.0f) {
				etapa_track1 = 0;
			}
			break;
		}
		switch (etapa_track2) {
		case 0:
			if (giroCarro3 > 0.0f) {
				giroCarro3 -= 1.0f;
			}
			movAuto_x3 += 0.04f;
			movAuto_z3 = movAuto_x3;
			if(movAuto_z3 >= 1.7) {
				etapa_track2 = 1;
			}
			break;
		case 1:
			giroCarro3 -= 1.0f;
			angleCarro3 += 1.0f;
			movAuto_x3 = 0.0f + 2.3f * sin(glm::radians(angleCarro3));
			movAuto_z3 = 3.25f - 2.3f * cos(glm::radians(angleCarro3));
			if (angleCarro3 >= 315.0f) {
				etapa_track2 = 2;
			}
			break;
		case 2:
			movAuto_x3 += 0.06f;
			movAuto_z3 = movAuto_x3 * -1.0f;
			if (movAuto_x3 >= 1.7f) {
				etapa_track2 = 3;
			}
			break;
		case 3:
			if (giroCarro3 <= -180.0f) {
				giroCarro3 += 3.0f;
			}
			else {
				giroCarro3 += 2.0f;
			}
			angleCarro3 -= 2.0f;
			movAuto_x3 = 0.0f - 2.3f * sin(glm::radians(angleCarro3));
			movAuto_z3 = -3.25f + 2.3f * cos(glm::radians(angleCarro3));
			if (angleCarro3 <= 45.0f) {
				etapa_track2 = 0;
			}
			break;
		}
	}

	// Movimiento del paracaídas
	if (paraGoingDown) {
		paraOffsetY -= paraSpeed;
		if (paraBaseY + paraOffsetY <= 0.0f)  // llegó al piso
			paraGoingDown = false;
	}
	else {
		paraOffsetY += paraSpeed;
		if (paraOffsetY >= 0.0f)  // llegó a la altura inicial
			paraGoingDown = true;
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

	initializeAudioSystem();

	createAudioZone(glm::vec3(0.0f, 5.0f, -40.0f), 8.0f, "audio/recepcion.wav");
	createAudioZone(glm::vec3(0.0f, 15.0f, -40.0f), 10.0f, "audio/primer_piso.wav");
	createAudioZone(glm::vec3(0.0f, 25.0f, -40.0f), 12.0f, "audio/segundo_piso.wav");
	createAudioZone(glm::vec3(0.0f, 45.0f, -40.0f), 15.0f, "audio/tercer_piso.wav");



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

	Model tercerPiso("resources/General_Models/MuseoJumex.obj");
	Model segundoPiso("resources/General_Models/MuseoJumexSegundoPiso.obj");
	Model primerPiso("resources/General_Models/MuseoJumexTercerPiso.obj");
	Model vestibulo("resources/General_Models/vestibulo.obj");

	Model pintura("resources/f1/frame1.obj");
	Model pintura1("resources/f2/frame02.obj");
	Model pintura2("resources/f3/frame03.obj");
	Model pintura3("resources/f4/frame04.obj");
	Model pintura4("resources/f5/frame05.obj");
	Model pintura5("resources/f6/frame06.obj");
	Model pintura6("resources/f7/frame07.obj");
	Model pintura7("resources/f8/frame08.obj");
	Model pintura8("resources/f9/frame09.obj");
	Model pintura9("resources/f10/frame10.obj");
	Model pintura10("resources/f11/frame11.obj");
	Model pintura11("resources/f12/frame12.obj");

	ModelAnim personajeGal("resources/modelo/modelo.dae");
	personajeGal.initShaders(animShader.ID);


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
	
	//Segundo piso
	Model paracaidas("resources/Paracaidas/paracaidas.obj");

	
	
	//Tercer piso
	Model carro1("resources/objects/Carro1/Carro1.obj");
	Model carro2("resources/objects/Carro2/Carro2.obj");
	Model carro3("resources/objects/Carro3/Carro3.obj");
	Model carro4("resources/objects/Carro4/Cybertruck.obj");
	Model carro5("resources/objects/Carro5/Suv_4x4.obj");
	Model carro6("resources/objects/Carro6/Carro6.obj");
	Model carro7("resources/objects/Carro7/mclaren_mp45.obj");
	Model carro8("resources/objects/Carro8/Carro8.obj");
	Model carro9("resources/objects/Carro9/Carro9.obj");
	Model carro10("resources/objects/Carro10/Gta-spano-2010 obj.obj");
	Model carro11("resources/objects/Carro11/Carro11.obj");
	Model llanta11("resources/objects/Carro11/llanta2.obj");
	Model carro12("resources/objects/Carro12/Carro.obj");
	Model llanta12("resources/objects/Carro12/llanta.obj");
	Model cubo("resources/objects/Cubo/Cubo.obj");
	Model pista("resources/objects/Pista/track.obj");
	Model base("resources/objects/Base/Base.obj");
	
	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;

		KeyFrame[i].posXp1 = 0.0f;
		KeyFrame[i].posYp1 = 0.0f;
		KeyFrame[i].posZp1 = 0.0f;
		KeyFrame[i].rotp1 = 0.0f;
		KeyFrame[i].scalep1 = 0.0f;

		KeyFrame[i].posXp2 = 0.0f;
		KeyFrame[i].posYp2 = 0.0f;
		KeyFrame[i].posZp2 = 0.0f;
		KeyFrame[i].rotp2 = 0.0f;
		KeyFrame[i].scalep2 = 0.0f;
		
		KeyFrame[i].posXp3 = 0.0f;
		KeyFrame[i].posYp3 = 0.0f;
		KeyFrame[i].posZp3 = 0.0f;
		KeyFrame[i].rotp3 = 0.0f;
		KeyFrame[i].scalep3 = 0.0f;

		KeyFrame[i].posXp4 = 0.0f;
		KeyFrame[i].posYp4 = 0.0f;
		KeyFrame[i].posZp4 = 0.0f;
		KeyFrame[i].rotp4 = 0.0f;
		KeyFrame[i].scalep4 = 0.0f;

		KeyFrame[i].posXp5 = 0.0f;
		KeyFrame[i].posYp5 = 0.0f;
		KeyFrame[i].posZp5 = 0.0f;
		KeyFrame[i].rotp5 = 0.0f;
		KeyFrame[i].scalep5 = 0.0f;

		KeyFrame[i].posXp6 = 0.0f;
		KeyFrame[i].posYp6 = 0.0f;
		KeyFrame[i].posZp6 = 0.0f;
		KeyFrame[i].rotp6 = 0.0f;
		KeyFrame[i].scalep6 = 0.0f;


		KeyFrame[i].posXp7 = 0.0f;
		KeyFrame[i].posYp7 = 0.0f;
		KeyFrame[i].posZp7 = 0.0f;
		KeyFrame[i].rotp7 = 0.0f;
		KeyFrame[i].scalep7 = 0.0f;

		KeyFrame[i].posXp8 = 0.0f;
		KeyFrame[i].posYp8 = 0.0f;
		KeyFrame[i].posZp8 = 0.0f;
		KeyFrame[i].rotp8 = 0.0f;
		KeyFrame[i].scalep8 = 0.0f;
		
		KeyFrame[i].posXp9 = 0.0f;
		KeyFrame[i].posYp9 = 0.0f;
		KeyFrame[i].posZp9 = 0.0f;
		KeyFrame[i].rotp9 = 0.0f;
		KeyFrame[i].scalep9 = 0.0f;
		
		KeyFrame[i].posXp10 = 0.0f;
		KeyFrame[i].posYp10 = 0.0f;
		KeyFrame[i].posZp10 = 0.0f;
		KeyFrame[i].rotp10 = 0.0f;
		KeyFrame[i].scalep10 = 0.0f;
		
		KeyFrame[i].posXp11 = 0.0f;
		KeyFrame[i].posYp11 = 0.0f;
		KeyFrame[i].posZp11 = 0.0f;
		KeyFrame[i].rotp11 = 0.0f;
		KeyFrame[i].scalep11 = 0.0f;
		
		KeyFrame[i].posXp12 = 0.0f;
		KeyFrame[i].posYp12 = 0.0f;
		KeyFrame[i].posZp12 = 0.0f;
		KeyFrame[i].rotp12 = 0.0f;
		KeyFrame[i].scalep12 = 0.0f;
	}

	KeyFrame[0].posXp1 = -19.7f; //-19.7
	KeyFrame[0].posYp1 = 15.5f; 
	KeyFrame[0].posZp1 = 7.0f; 
	KeyFrame[0].rotp1 = 180.0f;
	KeyFrame[0].scalep1 = 0.15f,

	KeyFrame[0].posXp2 = -19.7f; 
	KeyFrame[0].posYp2 = 15.5f; 
	KeyFrame[0].posZp2 = 17.0f; 
	KeyFrame[0].rotp2 = 180.0f;
	KeyFrame[0].scalep2 = 0.17f,

	KeyFrame[0].posXp3 = -19.7f; 
	KeyFrame[0].posYp3 = 15.5f; 
	KeyFrame[0].posZp3 = 27.0f; 
	KeyFrame[0].rotp3 = 180.0f;
	KeyFrame[0].scalep3 = 0.17f,

	KeyFrame[0].posXp4 = -19.7f; 
	KeyFrame[0].posYp4 = 15.5f; 
	KeyFrame[0].posZp4 = 37.0f; 
	KeyFrame[0].rotp4 = 180.0f;
	KeyFrame[0].scalep4 = 0.169f,

	KeyFrame[0].posXp5 = 19.7f; 
	KeyFrame[0].posYp5 = 15.5f; 
	KeyFrame[0].posZp5 = 7.0f; 
	KeyFrame[0].rotp5 = 0.0f;
	KeyFrame[0].scalep5 = 0.17f,
	
	KeyFrame[0].posXp6 = 19.7f; 
	KeyFrame[0].posYp6 = 15.5f; 
	KeyFrame[0].posZp6 = 17.0f; 
	KeyFrame[0].rotp6 = 0.0f;
	KeyFrame[0].scalep6 = 0.169f,
	
	KeyFrame[0].posXp7 = 19.7f; 
	KeyFrame[0].posYp7 = 15.5f; 
	KeyFrame[0].posZp7 = 27.0f; 
	KeyFrame[0].rotp7 = 0.0f;
	KeyFrame[0].scalep7 = 0.169f,
	
	KeyFrame[0].posXp8 = 19.7f; 
	KeyFrame[0].posYp8 = 15.5f; 
	KeyFrame[0].posZp8 = 37.0f; 
	KeyFrame[0].rotp8 = 0.0f;
	KeyFrame[0].scalep8 = 0.169f,
	
	KeyFrame[0].posXp9 = 15.7f; 
	KeyFrame[0].posYp9 = 15.5f; 
	KeyFrame[0].posZp9 = -42.2f; 
	KeyFrame[0].rotp9 = 90.0f;
	KeyFrame[0].scalep9 = 0.169f,

	KeyFrame[0].posXp10 = -15.7f; 
	KeyFrame[0].posYp10 = 15.5f; 
	KeyFrame[0].posZp10 = -42.2f; 
	KeyFrame[0].rotp10 = 90.0f;
	KeyFrame[0].scalep10 = 0.169f,
	
	KeyFrame[0].posXp11 = -15.7f; 
	KeyFrame[0].posYp11 = 15.5f; 
	KeyFrame[0].posZp11 = 42.2f; 
	KeyFrame[0].rotp11 = -90.0f;
	KeyFrame[0].scalep11 = 0.169f,
	
	KeyFrame[0].posXp12 = 15.7f; 
	KeyFrame[0].posYp12 = 15.5f; 
	KeyFrame[0].posZp12 = 42.2f; 
	KeyFrame[0].rotp12 = -90.0f;
	KeyFrame[0].scalep12 = 0.169f;


	KeyFrame[1].posXp1 = -10.0f; 
	KeyFrame[1].posYp1 = 16.7f; 
	KeyFrame[1].posZp1 = 8.0f; 
	KeyFrame[1].rotp1 = 270.0f;
	KeyFrame[1].scalep1 = 0.18f;

	KeyFrame[1].posXp2 = -10.0f; 
	KeyFrame[1].posYp2 = 16.7f; 
	KeyFrame[1].posZp2 = 18.0f; 
	KeyFrame[1].rotp2 = 270.0f;
	KeyFrame[1].scalep2 = 0.2f;

	KeyFrame[1].posXp3 = -10.0f; 
	KeyFrame[1].posYp3 = 16.7f; 
	KeyFrame[1].posZp3 = 28.0f; 
	KeyFrame[1].rotp3 = 270.0f;
	KeyFrame[1].scalep3 = 0.2f;

	KeyFrame[1].posXp4 = -10.0f; 
	KeyFrame[1].posYp4 = 16.7f; 
	KeyFrame[1].posZp4 = 38.0f; 
	KeyFrame[1].rotp4 = 270.0f;
	KeyFrame[1].scalep4 = 0.2f;

	KeyFrame[1].posXp5 = 10.0f; 
	KeyFrame[1].posYp5 = 16.7f; 
	KeyFrame[1].posZp5 = 8.0f; 
	KeyFrame[1].rotp5 = 270.0f;
	KeyFrame[1].scalep5 = 0.2f;

	KeyFrame[1].posXp6 = 10.0f; 
	KeyFrame[1].posYp6 = 16.7f; 
	KeyFrame[1].posZp6 = 18.0f; 
	KeyFrame[1].rotp6 = 270.0f;
	KeyFrame[1].scalep6 = 0.2f;

	KeyFrame[1].posXp7 = 10.0f; 
	KeyFrame[1].posYp7 = 16.7f; 
	KeyFrame[1].posZp7 = 28.0f; 
	KeyFrame[1].rotp7 = 270.0f;
	KeyFrame[1].scalep7 = 0.2f;

	KeyFrame[1].posXp8 = 10.0f; 
	KeyFrame[1].posYp8 = 16.7f; 
	KeyFrame[1].posZp8 = 38.0f; 
	KeyFrame[1].rotp8 = 270.0f;
	KeyFrame[1].scalep8 = 0.2f;

	KeyFrame[1].posXp9 = 15.7f; 
	KeyFrame[1].posYp9 = 14.0f; 
	KeyFrame[1].posZp9 = -42.2f; 
	KeyFrame[1].rotp9 = 45.0f;
	KeyFrame[1].scalep9 = 0.8f;
	KeyFrame[1].scalep9 = 0.12f;

	KeyFrame[1].posXp10 = -20.0f; 
	KeyFrame[1].posYp10 = 16.7f; 
	KeyFrame[1].posZp10 = -47.0f; 
	KeyFrame[1].rotp10 = 90.0f;
	KeyFrame[1].scalep10 = 0.8f;

	KeyFrame[1].posXp11 = -20.0f; 
	KeyFrame[1].posYp11 = 16.7f; 
	KeyFrame[1].posZp11 = 47.0f; 
	KeyFrame[1].rotp11 = -90.0f;
	KeyFrame[1].scalep11 = 0.8f;

	KeyFrame[1].posXp12 = 20.0f; 
	KeyFrame[1].posYp12 = 16.7f; 
	KeyFrame[1].posZp12 = 47.0f; 
	KeyFrame[1].rotp12 = -90.0f;
	KeyFrame[1].scalep12 = 0.8f;


	KeyFrame[2].posXp1 = -12.0f; 
	KeyFrame[2].posYp1 = 14.9f; 
	KeyFrame[2].posZp1 = 7.0f; 
	KeyFrame[2].rotp1 = 360.0f;
	KeyFrame[2].scalep1 = 0.11f;

	KeyFrame[2].posXp2 = -12.0f; 
	KeyFrame[2].posYp2 = 13.7f; 
	KeyFrame[2].posZp2 = 17.0f; 
	KeyFrame[2].rotp2 = 360.0f;
	KeyFrame[2].scalep2 = 0.13f;

	KeyFrame[2].posXp3 = -12.0f; 
	KeyFrame[2].posYp3 = 13.7f; 
	KeyFrame[2].posZp3 = 27.0f; 
	KeyFrame[2].rotp3 = 360.0f;
	KeyFrame[2].scalep3 = 0.13f;


	KeyFrame[2].posXp4 = -12.0f; 
	KeyFrame[2].posYp4 = 13.7f; 
	KeyFrame[2].posZp4 = 37.0f; 
	KeyFrame[2].rotp4 = 360.0f;
	KeyFrame[2].scalep4 = 0.13f;


	KeyFrame[2].posXp5 = 12.0f; 
	KeyFrame[2].posYp5 = 13.7f; 
	KeyFrame[2].posZp5 = 7.0f; 
	KeyFrame[2].rotp5 = 360.0f;
	KeyFrame[2].scalep5 = 0.13f;

	KeyFrame[2].posXp6 = 12.0f; 
	KeyFrame[2].posYp6 = 13.7f; 
	KeyFrame[2].posZp6 = 17.0f; 
	KeyFrame[2].rotp6 = 360.0f;
	KeyFrame[2].scalep6 = 0.13f;


	KeyFrame[2].posXp7 = 12.0f; 
	KeyFrame[2].posYp7 = 13.7f; 
	KeyFrame[2].posZp7 = 27.0f; 
	KeyFrame[2].rotp7 = 360.0f;
	KeyFrame[2].scalep7 = 0.13f;


	KeyFrame[2].posXp8 = 12.0f; 
	KeyFrame[2].posYp8 = 13.7f; 
	KeyFrame[2].posZp8 = 37.0f; 
	KeyFrame[2].rotp8 = 360.0f;
	KeyFrame[2].scalep8 = 0.13f;


	KeyFrame[2].posXp9 = 8.0f; 
	KeyFrame[2].posYp9 = 13.5f; 
	KeyFrame[2].posZp9 = -35.0f; 
	KeyFrame[2].rotp9 = 270.0f;
	KeyFrame[2].scalep9 = 0.15f;


	KeyFrame[2].posXp10 = -20.0f; 
	KeyFrame[2].posYp10 = 13.7f; 
	KeyFrame[2].posZp10 = -47.0f; 
	KeyFrame[2].rotp10 = 45.0f;


	KeyFrame[2].posXp11 = -20.0f; 
	KeyFrame[2].posYp11 = 13.7f; 
	KeyFrame[2].posZp11 = 47.0f; 
	KeyFrame[2].rotp11 = -135.0f;


	KeyFrame[2].posXp12 = 20.0f; 
	KeyFrame[2].posYp12 = 13.7f; 
	KeyFrame[2].posZp12 = 47.0f; 
	KeyFrame[2].rotp12 = -135.0f;

	//KeyFrame[3]
	KeyFrame[3].posXp1 = -13.5f;
	KeyFrame[3].posYp1 = 14.4f;
	KeyFrame[3].posZp1 = 7.0f;
	KeyFrame[3].rotp1 = 450.0f;
	KeyFrame[3].scalep1 = 0.13f;

	KeyFrame[3].posXp2 = -13.5f;
	KeyFrame[3].posYp2 = 14.0f;
	KeyFrame[3].posZp2 = 17.0f;
	KeyFrame[3].rotp2 = 450.0f;
	KeyFrame[3].scalep2 = 0.14f;

	KeyFrame[3].posXp3 = -13.5f;
	KeyFrame[3].posYp3 = 13.7f;
	KeyFrame[3].posZp3 = 27.0f;
	KeyFrame[3].rotp3 = 450.0f;
	KeyFrame[3].scalep3 = 0.14f;

	KeyFrame[3].posXp4 = -13.5f;
	KeyFrame[3].posYp4 = 13.7f;
	KeyFrame[3].posZp4 = 37.0f;
	KeyFrame[3].rotp4 = 450.0f;
	KeyFrame[3].scalep4 = 0.14f;

	KeyFrame[3].posXp5 = 13.5f;
	KeyFrame[3].posYp5 = 13.7f;
	KeyFrame[3].posZp5 = 7.0f;
	KeyFrame[3].rotp5 = 450.0f;
	KeyFrame[3].scalep5 = 0.14f;

	KeyFrame[3].posXp6 = 13.5f;
	KeyFrame[3].posYp6 = 13.7f;
	KeyFrame[3].posZp6 = 17.0f;
	KeyFrame[3].rotp6 = 450.0f;
	KeyFrame[3].scalep6 = 0.14f;

	KeyFrame[3].posXp7 = 13.5f;
	KeyFrame[3].posYp7 = 13.7f;
	KeyFrame[3].posZp7 = 27.0f;
	KeyFrame[3].rotp7 = 450.0f;
	KeyFrame[3].scalep7 = 0.14f;

	KeyFrame[3].posXp8 = 13.5f;
	KeyFrame[3].posYp8 = 13.7f;
	KeyFrame[3].posZp8 = 37.0f;
	KeyFrame[3].rotp8 = 450.0f;
	KeyFrame[3].scalep8 = 0.14f;

	KeyFrame[3].posXp9 = 5.0f;
	KeyFrame[3].posYp9 = 13.0f;
	KeyFrame[3].posZp9 = -20.0f;
	KeyFrame[3].rotp9 = 45.0f;
	KeyFrame[3].scalep9 = 0.17f;

	KeyFrame[3].posXp10 = -20.0f;
	KeyFrame[3].posYp10 = 13.7f;
	KeyFrame[3].posZp10 = -47.0f;
	KeyFrame[3].rotp10 = 45.0f;

	KeyFrame[3].posXp11 = -20.0f;
	KeyFrame[3].posYp11 = 13.7f;
	KeyFrame[3].posZp11 = 47.0f;
	KeyFrame[3].rotp11 = -135.0f;

	KeyFrame[3].posXp12 = 20.0f;
	KeyFrame[3].posYp12 = 13.7f;
	KeyFrame[3].posZp12 = 47.0f;
	KeyFrame[3].rotp12 = -135.0f;

	//keyframe 4
	KeyFrame[4].posXp1 = -15.5f;
	KeyFrame[4].posYp1 = 14.7f;
	KeyFrame[4].posZp1 = 7.0f;
	KeyFrame[4].rotp1 = 540.0f;
	KeyFrame[4].scalep1 = 0.09f;

	KeyFrame[4].posXp2 = -15.5f;
	KeyFrame[4].posYp2 = 13.7f;
	KeyFrame[4].posZp2 = 17.0f;
	KeyFrame[4].rotp2 = 540.0f;
	KeyFrame[4].scalep2 = 0.11f;

	KeyFrame[4].posXp3 = -15.5f;
	KeyFrame[4].posYp3 = 13.7f;
	KeyFrame[4].posZp3 = 27.0f;
	KeyFrame[4].rotp3 = 540.0f;
	KeyFrame[4].scalep3 = 0.11f;

	KeyFrame[4].posXp4 = -15.5f;
	KeyFrame[4].posYp4 = 13.7f;
	KeyFrame[4].posZp4 = 37.0f;
	KeyFrame[4].rotp4 = 540.0f;
	KeyFrame[4].scalep4 = 0.11f;

	KeyFrame[4].posXp5 = 15.5f;
	KeyFrame[4].posYp5 = 13.7f;
	KeyFrame[4].posZp5 = 7.0f;
	KeyFrame[4].rotp5 = 540.0f;
	KeyFrame[4].scalep5 = 0.11f;

	KeyFrame[4].posXp6 = 15.5f;
	KeyFrame[4].posYp6 = 13.7f;
	KeyFrame[4].posZp6 = 17.0f;
	KeyFrame[4].rotp6 = 540.0f;
	KeyFrame[4].scalep6 = 0.11f;

	KeyFrame[4].posXp7 = 15.5f;
	KeyFrame[4].posYp7 = 13.7f;
	KeyFrame[4].posZp7 = 27.0f;
	KeyFrame[4].rotp7 = 540.0f;
	KeyFrame[4].scalep7 = 0.11f;

	KeyFrame[4].posXp8 = 15.5f;
	KeyFrame[4].posYp8 = 13.7f;
	KeyFrame[4].posZp8 = 37.0f;
	KeyFrame[4].rotp8 = 540.0f;
	KeyFrame[4].scalep8 = 0.11f;

	KeyFrame[4].posXp9 = -5.0f;
	KeyFrame[4].posYp9 = 14.0f;
	KeyFrame[4].posZp9 = -15.0f;
	KeyFrame[4].rotp9 = 135.0f;
	KeyFrame[4].scalep9 = 0.13f;

	KeyFrame[4].posXp10 = -20.0f;
	KeyFrame[4].posYp10 = 13.7f;
	KeyFrame[4].posZp10 = -47.0f;
	KeyFrame[4].rotp10 = 45.0f;

	KeyFrame[4].posXp11 = -20.0f;
	KeyFrame[4].posYp11 = 13.7f;
	KeyFrame[4].posZp11 = 47.0f;
	KeyFrame[4].rotp11 = -135.0f;

	KeyFrame[4].posXp12 = 20.0f;
	KeyFrame[4].posYp12 = 13.7f;
	KeyFrame[4].posZp12 = 47.0f;
	KeyFrame[4].rotp12 = -135.0f;

	//keyframe 5
	KeyFrame[5].posXp1 = -16.0f;
	KeyFrame[5].posYp1 = 15.2f;
	KeyFrame[5].posZp1 = 7.0f;
	KeyFrame[5].rotp1 = 630.0f;
	KeyFrame[5].scalep1 = 0.12f;

	KeyFrame[5].posXp2 = -16.0f;
	KeyFrame[5].posYp2 = 14.7f;
	KeyFrame[5].posZp2 = 17.0f;
	KeyFrame[5].rotp2 = 630.0f;
	KeyFrame[5].scalep2 = 0.14f;

	KeyFrame[5].posXp3 = -16.0f;
	KeyFrame[5].posYp3 = 13.7f;
	KeyFrame[5].posZp3 = 27.0f;
	KeyFrame[5].rotp3 = 630.0f;
	KeyFrame[5].scalep3 = 0.14f;

	KeyFrame[5].posXp4 = -16.0f;
	KeyFrame[5].posYp4 = 13.7f;
	KeyFrame[5].posZp4 = 37.0f;
	KeyFrame[5].rotp4 = 630.0f;
	KeyFrame[5].scalep4 = 0.14f;

	KeyFrame[5].posXp5 = 16.0f;
	KeyFrame[5].posYp5 = 13.7f;
	KeyFrame[5].posZp5 = 7.0f;
	KeyFrame[5].rotp5 = 630.0f;
	KeyFrame[5].scalep5 = 0.14f;

	KeyFrame[5].posXp6 = 16.0f;
	KeyFrame[5].posYp6 = 13.7f;
	KeyFrame[5].posZp6 = 17.0f;
	KeyFrame[5].rotp6 = 630.0f;
	KeyFrame[5].scalep6 = 0.14f;

	KeyFrame[5].posXp7 = 16.0f;
	KeyFrame[5].posYp7 = 13.7f;
	KeyFrame[5].posZp7 = 27.0f;
	KeyFrame[5].rotp7 = 630.0f;
	KeyFrame[5].scalep7 = 0.14f;

	KeyFrame[5].posXp8 = 16.0f;
	KeyFrame[5].posYp8 = 13.7f;
	KeyFrame[5].posZp8 = 37.0f;
	KeyFrame[5].rotp8 = 630.0f;
	KeyFrame[5].scalep8 = 0.14f;

	KeyFrame[5].posXp9 = 12.0f;
	KeyFrame[5].posYp9 = 14.9f;
	KeyFrame[5].posZp9 = -30.0f;
	KeyFrame[5].rotp9 = 225.0f;
	KeyFrame[5].scalep9 = 0.16f;

	KeyFrame[5].posXp10 = -20.0f;
	KeyFrame[5].posYp10 = 13.7f;
	KeyFrame[5].posZp10 = -47.0f;
	KeyFrame[5].rotp10 = 45.0f;

	KeyFrame[5].posXp11 = -20.0f;
	KeyFrame[5].posYp11 = 13.7f;
	KeyFrame[5].posZp11 = 47.0f;
	KeyFrame[5].rotp11 = -135.0f;

	KeyFrame[5].posXp12 = 20.0f;
	KeyFrame[5].posYp12 = 13.7f;
	KeyFrame[5].posZp12 = 47.0f;
	KeyFrame[5].rotp12 = -135.0f;

	//keyframe 6
	KeyFrame[6].posXp1 = -19.7f; //-19.7
	KeyFrame[6].posYp1 = 15.5f;
	KeyFrame[6].posZp1 = 7.0f;
	KeyFrame[6].rotp1 = 180.0f;
	KeyFrame[6].scalep1 = 0.15f;

	KeyFrame[6].posXp2 = -19.7f;
	KeyFrame[6].posYp2 = 15.5f;
	KeyFrame[6].posZp2 = 17.0f;
	KeyFrame[6].rotp2 = 180.0f;
	KeyFrame[6].scalep2 = 0.17f;

	KeyFrame[6].posXp3 = -19.7f;
	KeyFrame[6].posYp3 = 15.5f;
	KeyFrame[6].posZp3 = 27.0f;
	KeyFrame[6].rotp3 = 180.0f;
	KeyFrame[6].scalep3 = 0.17f;

	KeyFrame[6].posXp4 = -19.7f;
	KeyFrame[6].posYp4 = 15.5f;
	KeyFrame[6].posZp4 = 37.0f;
	KeyFrame[6].rotp4 = 180.0f;
	KeyFrame[6].scalep4 = 0.169f;

	KeyFrame[6].posXp5 = 19.7f;
	KeyFrame[6].posYp5 = 15.5f;
	KeyFrame[6].posZp5 = 7.0f;
	KeyFrame[6].rotp5 = 0.0f;
	KeyFrame[6].scalep5 = 0.17f;

	KeyFrame[6].posXp6 = 19.7f;
	KeyFrame[6].posYp6 = 15.5f;
	KeyFrame[6].posZp6 = 17.0f;
	KeyFrame[6].rotp6 = 0.0f;
	KeyFrame[6].scalep6 = 0.169f;

	KeyFrame[6].posXp7 = 19.7f;
	KeyFrame[6].posYp7 = 15.5f;
	KeyFrame[6].posZp7 = 27.0f;
	KeyFrame[6].rotp7 = 0.0f;
	KeyFrame[6].scalep7 = 0.169f;

	KeyFrame[6].posXp8 = 19.7f;
	KeyFrame[6].posYp8 = 15.5f;
	KeyFrame[6].posZp8 = 37.0f;
	KeyFrame[6].rotp8 = 0.0f;
	KeyFrame[6].scalep8 = 0.169f;

	KeyFrame[6].posXp9 = 15.7f;
	KeyFrame[6].posYp9 = 15.5f;
	KeyFrame[6].posZp9 = -42.2f;
	KeyFrame[6].rotp9 = 315.0f;
	KeyFrame[6].scalep9 = 0.169f;

	KeyFrame[6].posXp10 = -15.7f;
	KeyFrame[6].posYp10 = 15.5f;
	KeyFrame[6].posZp10 = -42.2f;
	KeyFrame[6].rotp10 = 90.0f;

	KeyFrame[6].posXp11 = -15.7f;
	KeyFrame[6].posYp11 = 15.5f;
	KeyFrame[6].posZp11 = 42.2f;
	KeyFrame[6].rotp11 = -90.0f;

	KeyFrame[6].posXp12 = 15.7f;
	KeyFrame[6].posYp12 = 15.5f;
	KeyFrame[6].posZp12 = 42.2f;
	KeyFrame[6].rotp12 = -90.0f;

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
	mat4 tmp11 = mat4(1.0f);
	mat4 tmp12 = mat4(1.0f);
	mat4 tmpTrack = mat4(1.0f);
	mat4 tmpMini = mat4(1.0f);

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

		updateAudioZones();

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
		// Personaje Animado 1er Piso
		// -------------------------------------------------------------------------------------------------------------------------
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 9.8f, 15.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", modelOp);
		personajeGal.Draw(animShader);

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
			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp1, posYp1, posZp1));
			modelOp = glm::rotate(modelOp, glm::radians(rotp1), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep1));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura.Draw(staticShader); //-19.7, 15.5, 7.0, r=180  scale 0.15

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp2, posYp2, posZp2));
			modelOp = glm::rotate(modelOp, glm::radians(rotp2), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep2));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura1.Draw(staticShader); //-19.7, 15.5, 17.0, r=180 scale 0.17
	
			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp3, posYp3, posZp3));
			modelOp = glm::rotate(modelOp, glm::radians(rotp3), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep3));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura2.Draw(staticShader); //-19.7, 15.5, 27.0, r=180 scale 0.17

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp4, posYp4, posZp4));
			modelOp = glm::rotate(modelOp, glm::radians(rotp4), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep4));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura7.Draw(staticShader); //-19.7, 15.5, 37.0, r=180 scale 0.169

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp5, posYp5, posZp5));
			modelOp = glm::rotate(modelOp, glm::radians(rotp5), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep5));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura3.Draw(staticShader); //19.7, 15.5, 7.0, r=0 scale 0.17

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp6, posYp6, posZp6));
			modelOp = glm::rotate(modelOp, glm::radians(rotp6), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep6));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura4.Draw(staticShader); //19.7, 15.5, 17.0, r=0 s 0.169

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp7, posYp7, posZp7));
			modelOp = glm::rotate(modelOp, glm::radians(rotp7), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep7));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura5.Draw(staticShader); //19.7, 15.5, 27.0, r=0 s 0.169

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posXp8, posYp8, posZp8));
			modelOp = glm::rotate(modelOp, glm::radians(rotp8), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(scalep8));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura6.Draw(staticShader); //19.7, 15.5, 37.0, r=0 s 0.169 s 0.169

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(15.7, 15.5, -42.2));
			modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(0.169));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura8.Draw(staticShader); //15.7, 15.5, -42.2, r=90 s 0.169

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.7f, 15.5f, -42.2f));
			modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(0.169f));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura9.Draw(staticShader); //-15.7, 15.5, -42.2, r=90

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.7f, 15.5f, 42.2f));
			modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(0.169f));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura10.Draw(staticShader); //-15.7, 15.5, 42.2, r=-90

			modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(15.7f, 15.5f, 42.2f));
			modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(0.169f));
			staticShader.setMat4("model", modelOp);
			staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			pintura11.Draw(staticShader); //15.7, 15.5, 42.2, r=-90

		// -------------------------------------------------------------------------------------------------------------------------
		// Modelos 2do piso
		// -------------------------------------------------------------------------------------------------------------------------
		
			glm::mat4 modelPara = glm::mat4(1.0f);
			modelPara = glm::translate(modelPara, glm::vec3(paraBaseX, paraBaseY + paraOffsetY, paraBaseZ));

			modelPara = glm::scale(modelPara, glm::vec3(0.02f)); // tamaño pequeño visible
			staticShader.setMat4("model", modelPara);

			paracaidas.Draw(staticShader);



		// -------------------------------------------------------------------------------------------------------------------------
		// Modelos 3er piso
		// -------------------------------------------------------------------------------------------------------------------------
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 42.0f, -20.0f));	//-2.0f, 42.0f, 33.0f
		modelOp = glm::scale(modelOp, glm::vec3(0.012f));
		modelOp = glm::rotate(modelOp, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro1.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(19.0f, 39.51f, 0.0f));	//-21.0f, 39.51f, 23.0f
		modelOp = glm::scale(modelOp, glm::vec3(0.03f));
		modelOp = glm::rotate(modelOp, glm::radians(205.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro2.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(21.0f, 39.62f, 20.0f));	//-2.0f, 39.62f, 19.0f
		modelOp = glm::scale(modelOp, glm::vec3(0.03f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro3.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 39.65f, 45.0f));	//0.0f, 39.65f, -45.0f
		modelOp = glm::scale(modelOp, glm::vec3(0.08f));
		modelOp = glm::rotate(modelOp, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro4.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 39.1f, 37.0f));	//0.96
		modelOp = glm::scale(modelOp, glm::vec3(0.008f));
		modelOp = glm::rotate(modelOp, glm::radians(-160.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro5.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-17.0f, 39.62f, 40.0f));	//0.96
		modelOp = glm::scale(modelOp, glm::vec3(2.4f));
		modelOp = glm::rotate(modelOp, glm::radians(-20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro6.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 40.0f + movAuto_y1, 18.0f));
		modelOp = glm::rotate(modelOp, glm::radians(orienta_y1), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(orienta_z1), glm::vec3(0.0f, 0.0f, 1.0f));
		modelOp = glm::rotate(modelOp, glm::radians(orienta_x1), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(4.5f));
		staticShader.setMat4("model", modelOp);
		carro7.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 39.7f, 18.0f));
		tmpMini = modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f, 1.0f, 5.0f));
		staticShader.setMat4("model", modelOp);
		base.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-21.0f, 39.7f, 20.0f));	//-2.0f, 39.62f, 6.0f
		modelOp = glm::scale(modelOp, glm::vec3(0.14f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro8.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-18.0f, 39.65f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.12f));
		modelOp = glm::rotate(modelOp, glm::radians(155.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro9.Draw(staticShader);
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-18.0f, 39.65f, -20.0f));//-21.0f, 39.65f, -17.0f
		modelOp = glm::scale(modelOp, glm::vec3(0.07f));
		modelOp = glm::rotate(modelOp, glm::radians(155.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro10.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 39.55f, -25.0f));
		tmpMini = modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.5f, 0.5f, 2.0f));
		staticShader.setMat4("model", modelOp);
		cubo.Draw(staticShader);

		modelOp = glm::translate(tmpMini, glm::vec3(1.0f, 1.88f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.0016f));
		modelOp = glm::rotate(modelOp, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro2.Draw(staticShader);

		modelOp = glm::translate(tmpMini, glm::vec3(-1.0f, 1.88f, -2.0f));	
		modelOp = glm::scale(modelOp, glm::vec3(0.0016f));
		modelOp = glm::rotate(modelOp, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro3.Draw(staticShader);

		modelOp = glm::translate(tmpMini, glm::vec3(-1.0f, 1.89f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.004f));
		modelOp = glm::rotate(modelOp, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro4.Draw(staticShader);

		modelOp = glm::translate(tmpMini, glm::vec3(1.0f, 1.89f, 2.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.004f));
		modelOp = glm::rotate(modelOp, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carro10.Draw(staticShader);
		
		modelOp = glm::translate(tmpMini, glm::vec3(-1.0, 1.935f, 2.0));	//0.0f, 1.96f, 0.0f		//-1.7f, 1.96f, 4.8f
		tmp11 = modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		carro11.Draw(staticShader);

		modelOp = glm::translate(tmp11, glm::vec3(0.272f, 0.015f, -0.159f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);

		modelOp = glm::translate(tmp11, glm::vec3(-0.236f, 0.015f, -0.159f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);

		modelOp = glm::translate(tmp11, glm::vec3(0.272f, 0.015f, 0.159f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);

		modelOp = glm::translate(tmp11, glm::vec3(-0.236f, 0.015f, 0.159f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);

		modelOp = glm::translate(tmpMini, glm::vec3(1.0f, 1.925f, -2.0f));
		tmp12 = modelOp = glm::rotate(modelOp, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		carro12.Draw(staticShader);

		modelOp = glm::translate(tmp12, glm::vec3(-11.91f, 2.375f, -20.575f) * 0.013f);
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);

		modelOp = glm::translate(tmp12, glm::vec3(-11.91f, 2.375f, 19.762f) * 0.013f);
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);

		modelOp = glm::translate(tmp12, glm::vec3(11.91f, 2.375f, 19.762f) * 0.013f);
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);

		modelOp = glm::translate(tmp12, glm::vec3(11.91f, 2.375f, -20.575f) * 0.013f);
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 39.55f, -5.0f));
		tmpTrack = modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.5f, 0.5f, 3.0f));
		staticShader.setMat4("model", modelOp);
		cubo.Draw(staticShader);

		modelOp = glm::translate(tmpTrack, glm::vec3(0.0f, 1.9f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.07f));
		staticShader.setMat4("model", modelOp);
		pista.Draw(staticShader);

		modelOp = glm::translate(tmpTrack, glm::vec3(movAuto_x2, 1.96f, -movAuto_z2));	//0.0f, 1.96f, 0.0f		//-1.7f, 1.96f, 4.8f
		modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tmp11 = modelOp = glm::rotate(modelOp, glm::radians(giroCarro2), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		carro11.Draw(staticShader);

		modelOp = glm::translate(tmp11, glm::vec3(0.272f, 0.015f, -0.159f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(0.0f, 0.0f, 1.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);
		
		modelOp = glm::translate(tmp11, glm::vec3(-0.236f, 0.015f, -0.159f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(0.0f, 0.0f, 1.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);

		modelOp = glm::translate(tmp11, glm::vec3(0.272f, 0.015f, 0.159f));
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(0.0f, 0.0f, -1.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);

		modelOp = glm::translate(tmp11, glm::vec3(-0.236f, 0.015f, 0.159f));
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(0.0f, 0.0f, -1.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.0f));
		staticShader.setMat4("model", modelOp);
		llanta11.Draw(staticShader);

		modelOp = glm::translate(tmpTrack, glm::vec3(movAuto_x3, 1.945f, movAuto_z3));
		modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tmp12 = modelOp = glm::rotate(modelOp, glm::radians(giroCarro3), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		carro12.Draw(staticShader);
		
		modelOp = glm::translate(tmp12, glm::vec3(-11.91f, 2.375f, -20.575f) * 0.013f);
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);

		modelOp = glm::translate(tmp12, glm::vec3(-11.91f, 2.375f, 19.762f) * 0.013f);
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);

		modelOp = glm::translate(tmp12, glm::vec3(11.91f, 2.375f, 19.762f) * 0.013f);
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(-1.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);

		modelOp = glm::translate(tmp12, glm::vec3(11.91f, 2.375f, -20.575f) * 0.013f);
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(giroRuedas), glm::vec3(-1.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.013f));
		staticShader.setMat4("model", modelOp);
		llanta12.Draw(staticShader);
		
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
	cleanupAudioSystem();
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
		animacion1 ^= true;
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		animacion2 ^= true;
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		orienta_x1 = 0.0f;
		orienta_y1 = 180.0f;
		orienta_z1 = 0.0f;
		movAuto_y1 = 0.0f;
		movAuto_x2 = 1.5f;
		movAuto_z2 = 1.5f;
		movAuto_x3 = 0.0f;
		movAuto_z3 = 0.0f;
		animacion1 = animacion2 = false;
		angleCarro2 = angleCarro3 = 45.0f;
		giroCarro2 = giroCarro3 = 0.0f;
		giroRuedas = 0.0f;
		etapa_giro = 0;
		etapa_track1 = etapa_track2 = 0;
	}

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


void initializeAudioSystem() {
	// Inicializar OpenAL
	audioDevice = alcOpenDevice(nullptr);
	if (!audioDevice) {
		std::cerr << "No se pudo abrir el dispositivo de audio" << std::endl;
		return;
	}

	audioContext = alcCreateContext(audioDevice, nullptr);
	if (!alcMakeContextCurrent(audioContext)) {
		std::cerr << "No se pudo crear el contexto de audio" << std::endl;
		return;
	}

	// Configurar propiedades del listener (oyente)
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);

	// Configurar orientación del listener (forward, up)
	ALfloat listenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, listenerOri);
}

bool loadWavFile(const char* filename, ALuint* buffer) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "No se pudo abrir el archivo WAV: " << filename << std::endl;
		return false;
	}

	char riff[4]; file.read(riff, 4);
	file.ignore(4);
	char wave[4]; file.read(wave, 4);

	char fmt[4]; file.read(fmt, 4);
	int subchunk1Size; file.read((char*)&subchunk1Size, 4);
	short audioFormat; file.read((char*)&audioFormat, 2);
	short numChannels; file.read((char*)&numChannels, 2);
	int sampleRate; file.read((char*)&sampleRate, 4);
	file.ignore(6);
	short bitsPerSample; file.read((char*)&bitsPerSample, 2);

	char dataHeader[4];
	int dataSize;
	while (true) {
		file.read(dataHeader, 4);
		file.read((char*)&dataSize, 4);
		if (std::string(dataHeader, 4) == "data") break;
		file.ignore(dataSize);
	}

	std::vector<char> data(dataSize);
	file.read(data.data(), dataSize);

	ALenum format;
	if (numChannels == 1 && bitsPerSample == 8) format = AL_FORMAT_MONO8;
	else if (numChannels == 1 && bitsPerSample == 16) format = AL_FORMAT_MONO16;
	else if (numChannels == 2 && bitsPerSample == 8) format = AL_FORMAT_STEREO8;
	else if (numChannels == 2 && bitsPerSample == 16) format = AL_FORMAT_STEREO16;
	else {
		std::cerr << "Formato WAV no soportado: " << filename << std::endl;
		return false;
	}

	alGenBuffers(1, buffer);
	alBufferData(*buffer, format, data.data(), dataSize, sampleRate);

	std::cout << "Audio cargado: " << filename << std::endl;
	return true;
}

void createAudioZone(const glm::vec3& center, float radius, const std::string& audioFile) {
	AudioZone zone;
	zone.center = center;
	zone.radius = radius;
	zone.audioFile = audioFile;
	zone.isPlaying = false;

	// Cargar el buffer de audio
	if (loadWavFile(audioFile.c_str(), &zone.buffer)) {
		// Crear la fuente de audio
		alGenSources(1, &zone.source);
		alSourcei(zone.source, AL_BUFFER, zone.buffer);

		// Configurar propiedades 3D de la fuente
		alSourcef(zone.source, AL_PITCH, 1.0f);
		alSourcef(zone.source, AL_GAIN, 0.0f); // Iniciar en silencio
		alSource3f(zone.source, AL_POSITION, center.x, center.y, center.z);
		alSource3f(zone.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		alSourcei(zone.source, AL_LOOPING, AL_TRUE);

		// Configurar atenuación por distancia
		alSourcef(zone.source, AL_REFERENCE_DISTANCE, radius * 0.3f);
		alSourcef(zone.source, AL_MAX_DISTANCE, radius);
		alSourcef(zone.source, AL_ROLLOFF_FACTOR, 1.0f);

		audioZones.push_back(zone);
		std::cout << "Zona de audio creada en: (" << center.x << ", " << center.y << ", " << center.z
			<< ") con radio: " << radius << std::endl;
	}
}

void updateAudioZones() {
	// Actualizar posición del listener con la cámara
	alListener3f(AL_POSITION, camera.Position.x, camera.Position.y, camera.Position.z);

	// Actualizar orientación del listener
	glm::vec3 forward = camera.Front;
	glm::vec3 up = camera.Up;
	ALfloat listenerOri[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
	alListenerfv(AL_ORIENTATION, listenerOri);

	// Actualizar cada zona de audio
	for (auto& zone : audioZones) {
		float distance = glm::distance(camera.Position, zone.center);

		if (distance <= zone.radius) {
			// Dentro de la zona - calcular volumen basado en distancia
			float volume = 1.0f - (distance / zone.radius);
			volume = glm::clamp(volume, 0.0f, 1.0f);

			alSourcef(zone.source, AL_GAIN, volume);

			if (!zone.isPlaying) {
				alSourcePlay(zone.source);
				zone.isPlaying = true;
				std::cout << "Reproduciendo audio: " << zone.audioFile << std::endl;
			}
		}
		else {
			// Fuera de la zona - silenciar
			alSourcef(zone.source, AL_GAIN, 0.0f);

			if (zone.isPlaying) {
				alSourceStop(zone.source);
				zone.isPlaying = false;
				std::cout << "Deteniendo audio: " << zone.audioFile << std::endl;
			}
		}
	}
}

void cleanupAudioSystem() {
	// Detener y limpiar todas las zonas de audio
	for (auto& zone : audioZones) {
		alSourceStop(zone.source);
		alDeleteSources(1, &zone.source);
		alDeleteBuffers(1, &zone.buffer);
	}
	audioZones.clear();

	// Limpiar contexto y dispositivo
	alcMakeContextCurrent(nullptr);
	if (audioContext) {
		alcDestroyContext(audioContext);
	}
	if (audioDevice) {
		alcCloseDevice(audioDevice);
	}
}