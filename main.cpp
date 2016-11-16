#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>

#define GLM_FORCE_RADIANS 1
#include <SDL2/SDL.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

#include "ui.h"
#include "RenderMesh.h"
#include "RenderUI.h"
#include "Model.h"
#include "Camera.h"

static const bool SCREEN_FULLSCREEN = false;
static const int SCREEN_WIDTH  = 960;
static const int SCREEN_HEIGHT = 540;
static SDL_Window *window = NULL;
static SDL_GLContext maincontext;

namespace ViewConstants
{
    namespace TopViewWithAngle {
        glm::vec3 Eye(-20.0f, 20.0f, -20.0f); // Origin of camera
        glm::vec3 Center(0.0f, 0.0f, 0.0f); // Point you are looking at
        glm::vec3 Up(0.0f, 1.0f, 0.0f); // Up vector of camera
    }
    namespace TopView {
        glm::vec3 Eye(0.0f, 20.0f, 0.0f); // Origin of camera
        glm::vec3 Center(0.0f, 0.0f, 0.0f); // Point you are looking at
        glm::vec3 Up(0.0f, 0.0f, 1.0f); // Up vector of camera
    }
    namespace FrontView {
        glm::vec3 Eye(0.0f, 0.0f, 30.0f); // Origin of camera
        glm::vec3 Center(0.0f, 0.0f, 0.0f); // Point you are looking at
        glm::vec3 Up(0.0f, 1.0f, 0.0f); // Up vector of camera
    }
    namespace SideView {
        glm::vec3 Eye(30.0f, 0.0f, 0.0f); // Origin of camera
        glm::vec3 Center(0.0f, 0.0f, 0.0f); // Point you are looking at
        glm::vec3 Up(0.0f, 1.0f, 0.0f); // Up vector of camera
    }
}

struct Keys
{
    bool w = false, a = false, s = false, d = false; // camera movement
    bool leftctrl = false;
};

void HandleKeysForCamera(Keys& keys, Camera& camera)
{
    float movespeed = 0.5f;
    if(keys.w)
    {
        camera.Move(movespeed, 0.0f);
    }
    if(keys.s)
    {
        camera.Move(-movespeed, 0.0f);
    }
    if(keys.a)
    {
        camera.Move(0.0f, movespeed);
    }
    if(keys.d)
    {
        camera.Move(0.0f, -movespeed);
    }
}

int main()
{
	SDL_Event event;
	bool quit = false;
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Could not initialize SDL: %s\n",  SDL_GetError());
		exit(2);
	}
	atexit(SDL_Quit);
	SDL_GL_LoadLibrary(NULL);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if(SCREEN_FULLSCREEN)
	{
		window = SDL_CreateWindow("demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
	}
	else
	{
		window = SDL_CreateWindow("demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	}

	maincontext = SDL_GL_CreateContext(window);
	if(maincontext == NULL)
	{
		fprintf(stderr, "Failed to create OpenGL context: %s\n",  SDL_GetError());
		exit(2);
	}
	// Check OpenGL properties
	printf("OpenGL loaded\n");
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	SDL_GL_SetSwapInterval(1);

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);

    glm::mat4 projectionM = glm::perspective(45.0f, width / (float)height, 0.1f, 100.0f);

    Render::Mesh::Source grid;
    const ::Mesh::TStaticMesh gridData = Model::Grid();
    grid.BufferData((::Mesh::Vertex*)&gridData[0], gridData.size());

    Render::Mesh::Source tank;
    const ::Mesh::TStaticMesh tankData = Model::SimpleTank();
    tank.BufferData((::Mesh::Vertex*)&tankData[0], tankData.size());

    Render::UI::Source ui;

    Camera camera;
    Keys keys;

    Render::Mesh::Renderer meshRenderer;
    Render::UI::Renderer uiRenderer;
    UIState uistate(width / 2, height / 2);
	while(!quit)
	{
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3D rendering
        meshRenderer.Draw(grid, glm::mat4(1.0f), camera.GetView(), projectionM);
        meshRenderer.Draw(tank, glm::mat4(1.0f), camera.GetView(), projectionM);

        // UI Rendering
        std::vector<UI::Vertex> uibuffer;
        generateSquare(uibuffer, uistate.mousex, uistate.mousey, 10, 10, glm::vec3(0.0f, 0.0f, 0.0f));
        ui.BufferData(&uibuffer[0], uibuffer.size());
        glm::mat4 projectionui = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
        glDisable(GL_DEPTH_TEST);
        uiRenderer.Draw(ui, projectionui);
        
		SDL_GL_SwapWindow(window);
        SDL_WarpMouseInWindow(window, width / 2.0f, height / 2.0f);
        SDL_SetRelativeMouseMode(SDL_TRUE);
        HandleKeysForCamera(keys, camera);
		while(SDL_PollEvent(&event))
		{
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    quit = true;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                        {
                            quit = true;
                            break;
                        }
                        case SDLK_w:
                        {
                            keys.w = true;
                            break;
                        }
                        case SDLK_a:
                        {
                            keys.a = true;
                            break;
                        }
                        case SDLK_s:
                        {
                            keys.s = true;
                            break;
                        }
                        case SDLK_d:
                        {
                            keys.d = true;
                            break;
                        }
                        case SDLK_LCTRL:
                        {
                            keys.leftctrl = true;
                            break;
                        }
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_w:
                        {
                            keys.w = false;
                            break;
                        }
                        case SDLK_a:
                        {
                            keys.a = false;
                            break;
                        }
                        case SDLK_s:
                        {
                            keys.s = false;
                            break;
                        }
                        case SDLK_d:
                        {
                            keys.d = false;
                            break;
                        }
                        case SDLK_LCTRL:
                        {
                            keys.leftctrl = false;
                            break;
                        }
                    }
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    if(!keys.leftctrl) {
                        uistate.mousex = std::max(0, std::min(width, uistate.mousex + event.motion.xrel));
                        uistate.mousey = std::max(0, std::min(height, uistate.mousey + event.motion.yrel));
                    }
                    else {
                        camera.MouseMove(event.motion.xrel / 100.0f, event.motion.yrel / 400.0f);
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    if (event.button.button == 1) uistate.mousedown = true;
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    if (event.button.button == 1) uistate.mousedown = false;
                    break;
                }
            }
		}
	}
	return 0;
}
