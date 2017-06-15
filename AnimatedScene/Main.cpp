#define GLEW_STATIC
#include <GL/glew.h>
#pragma comment(lib, "glew32s.lib")
#include <GL/freeglut.h>

#include <iostream>
#include <memory>

#include "Camera.h"
#include "Utils.h"
#include "Scene.h"

namespace {
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const float DELTA_TIME = 1000.f / 30;
	const float MAX_ROTATION_DIST = 30.f;

	int glutWindow;
	int prevMouseX = -1;
	int prevMouseY = -1;
	
	bool cameraRotationEnabled = false;
	Camera camera(1.f * WINDOW_WIDTH, 1.f * WINDOW_HEIGHT);

	std::unique_ptr<Scene> scene;

	void Initialize()
	{
		srand(static_cast<unsigned int>(time(nullptr)));

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);

		try {
			scene = std::make_unique<Scene>();
		}
		catch (const std::exception& ex) {
			std::cout << "Exception catch: " << ex.what() << std::endl;
			std::cout << "Press enter to exit\n";
			std::cin.get();
			exit(EXIT_FAILURE);
		}
	}


	void Destroy()
	{
		scene.reset();
		glutDestroyWindow(glutWindow);
	}

	void Update()
	{
		scene->Update(1.f / DELTA_TIME);
	}

	void Display()
	{
		Update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scene->Draw(camera);
		glutSwapBuffers();
	}

	void MouseButton(int button, int state, int x, int y)
	{
		if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) {
			cameraRotationEnabled = (state == GLUT_DOWN);
		}
	}

	void MouseMotion(int x, int y)
	{
		// Initialization
		if (prevMouseX < 0 && prevMouseY < 0) {
			prevMouseX = x;
			prevMouseY = y;
			return;
		}

		auto deltaX = static_cast<float>(x - prevMouseX);
		auto deltaY = static_cast<float>(y - prevMouseY);

		if (cameraRotationEnabled) {
			if (fabsf(deltaX) < MAX_ROTATION_DIST && fabsf(deltaY) < MAX_ROTATION_DIST) {
				camera.Rotate(deltaX, deltaY);
			}
		}

		prevMouseX = x;
		prevMouseY = y;
	}

	void WheelMotion(int wheel, int direction, int mx, int my)
	{
		if (direction < 0) {
			camera.ZoomIn();
		}
		else {
			camera.ZoomOut();
		}
	}

	void KeyboardDown(unsigned char key, int mx, int my)
	{
	}

	void KeyboardUp(unsigned char key, int mx, int my)
	{
	}

	void Timer(int value)
	{
		glutTimerFunc(static_cast<unsigned int>(DELTA_TIME), Timer, 0);
		glutPostRedisplay();
	}

	void GLAPIENTRY OpenGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
		const char* message, const void* userParam)
	{
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << message << std::endl;
			return;
		default:
			return;
		}
	}

	void SetupOpenGLCallback()
	{
		auto debugExtAddr = wglGetProcAddress("glDebugMessageCallbackARB");
		auto debugExtCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKARBPROC>(debugExtAddr);

		if (debugExtCallback) // callback function exists
		{
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			debugExtCallback(reinterpret_cast<GLDEBUGPROCARB>(OpenGLCallback), nullptr);
		}
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutWindow = glutCreateWindow("Animated Scene");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		return -1;
	}

	SetupOpenGLCallback();
	Utils::InitTextureLoader();
	Initialize();

	glutDisplayFunc(Display);
	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(WheelMotion);
	glutTimerFunc(static_cast<unsigned int>(DELTA_TIME), Timer, 0);

	glutMainLoop();

	Destroy();

	return 0;
}