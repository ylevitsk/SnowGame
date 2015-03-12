#include <iostream>

#include "third_party/GLSL_helper.h"
#include "World.h"
#include "Camera.h"
#include "Player.h"
#include "Shader.h"
#include "Texture.h"
#include "Text.h"
#include <GLFW/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>

// Global game state hooray
int g_hitCount = 0;
int g_spawnCount = 0;
int g_maxNumModels = 10;
static float g_fpsRate = 0;
static double g_totalTime = 0;

// Prototypes -- so I can hide these functions at the bottom of the file.
float p2wx(int in_x);
float p2wy(int in_y);

// Global world width and height.
float g_height = 600;
float g_width = 800;

// Mouse information.
static glm::vec2 g_mouseDown(0);
static bool g_mouseDrag = false;

// Set the projection matrix in the shaders.
void setProjection(void)
{
   // TODO (evan) mess with perspective
   glm::mat4 proj = glm::perspective(80.0f, g_width / g_height, 0.1f, 1000.f);
   safe_glUniformMatrix4fv(g_shader->get(U_PROJ_MX), glm::value_ptr(proj));
}

// Set the view matrix in the shaders.
void setView(void)
{
   glm::vec3 position = g_camera->getPosition();

   safe_glUniformMatrix4fv(g_shader->get(U_VIEW_MX), glm::value_ptr(g_camera->viewMatrix()));
   safe_glUniform3f(g_shader->get(U_CAM_POS), position.x, position.y, position.z);
}

void setLight(void)
{
   // Light is up-back-and-left.
   safe_glUniform3f(g_shader->get(U_LIGHT_DIR), -1.f, 1.f, 1.f);
   // Simple white light.
   safe_glUniform3f(g_shader->get(U_LIGHT_CLR), 1.f, 1.f, 1.f);
}

void setMatrices(void)
{
   setProjection();
   setView();
   setLight();
}

// GLFW callback for when the window is resized.
void glfwWindowResize(GLFWwindow *window, int width, int height)
{
   g_width = (float)width;
   g_height = (float)height;
   glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));
}

// GLFW main drawing routine.
void glfwDraw(GLFWwindow *window)
{
   double elapsedTime;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // Enable the shader.
   enableShader(SHADER_NORM);

   // Send View and Projection matrix to the shader.
   //setProjection();
   //setView();
   //setLight();
   setMatrices();

   // Calculate elapsed time since the last iteration through the loop.
   elapsedTime = glfwGetTime() - g_totalTime;
   g_totalTime += elapsedTime;

   // Frame rate: 1 frame / elapsedTime sec.
   g_fpsRate = 1 / elapsedTime;

   // Draw the geometry.
   drawGeom(elapsedTime, g_totalTime, g_width, g_height);

   // Disable the shader.
   disableShader(SHADER_NORM);

   // Bring the back buffer to the front.
   glfwSwapBuffers(window);
}

// GLFW callback for whenever a mouse button is pressed/released.
void glfwClick(GLFWwindow *window, int button, int action, int mods)
{
   double screenX, screenY;
   if (action == GLFW_PRESS) {
      g_mouseDrag = true;
      glfwGetCursorPos(window, &screenX, &screenY);
      g_mouseDown.x = p2wx(screenX);
      g_mouseDown.y = p2wy(screenY);
   }
   else if (action == GLFW_RELEASE) {
      g_mouseDrag = false;
   }
}

// GLFW callback for whenever the mouse cursor is moved.
void glfwDrag(GLFWwindow *window, double x, double y)
{
   static glm::vec2 mousePos(0);
   // We set a vertical look lookConstraint of 80 degrees up, and down.
   static float lookConstraint = (80.0 / 90.0) * (M_PI / 2.0);
   float newAlpha;

   if (g_mouseDrag) {
      mousePos.x = p2wx(x);
      mousePos.y = p2wy(y);

      g_camera->mouseDrag(g_width, g_height, g_mouseDown.x, g_mouseDown.y, mousePos.x, mousePos.y);

      // Update the last mouse position to be the current position.
      g_mouseDown.x = mousePos.x;
      g_mouseDown.y = mousePos.y;
   }
}

// GLFW callback for whenever a key is pressed.
void glfwKeyPress(GLFWwindow *window, int key, int scan, int action, int mods)
{
   if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      g_camera->keyPress(key);
      worldKeyPress(window, key);

      switch (key) {
         // Quit the game.
         case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
      }
   }
}

// GLFW callback for any detected errors.
void glfwError(int error, const char *description)
{
   std::cerr << description << std::endl;
}

// Perform some initial GL setup.
void glInitialize(void)
{
   glClearColor(0.7f, 0.8f, 0.9f, 1); // Set the default pixel color.
   glClearDepth(1); // Depth buffer setup.
   glDepthFunc(GL_LEQUAL); // Type of depth testing.
   glEnable(GL_DEPTH_TEST); // Enable depth testing.
   glEnable(GL_CULL_FACE); // Turn on backface culling.

   g_mStack.useModelViewMatrix();
   g_mStack.loadIdentity();

   initShaders();

   initGeom();
   initTextures();
}

void endGame() {
   std::cout << "Game over, you monster." << std::endl;
   std::cout << "Total time: " << g_totalTime << " sec." << std::endl;
}

int main(int argc, char *argv[])
{
   /* GLFW Setup */
   GLFWwindow* window;

   glfwSetErrorCallback(glfwError);
   if (!glfwInit()) {
      exit(EXIT_FAILURE);
   }

   // Set up the main window.
   window = glfwCreateWindow(g_width, g_height, "Dwell on the future twice as much as the past.", NULL, NULL);
   if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
   }
   glfwMakeContextCurrent(window);
   glfwSetWindowPos(window, 400, 100);

   // Set up event listeners.
   glfwSetWindowSizeCallback(window, glfwWindowResize);
   glfwSetKeyCallback(window, glfwKeyPress);
   glfwSetMouseButtonCallback(window, glfwClick);
   glfwSetCursorPosCallback(window, glfwDrag);

#ifdef _WIN32
   GLenum err = glewInit();
   if (GLEW_OK != err){
      std::cerr << "Error initializing glew! " << glewGetErrorString(err) << std::endl;
      return 1;
   }
#endif

   // Other initializations.
   glInitialize();

   int ret = initTextResources();

   // Start the main execution loop.
   while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      glfwDraw(window);
   }

   // Clean up after GLFW.
   glfwDestroyWindow(window);
   glfwTerminate();
   exit(EXIT_SUCCESS);
}

// Convert pixel x-coordinate to world x-coordinate.
float p2wx(int in_x) {
   double left, right, scale, translate;

   right = g_width > g_height ? g_width / g_height : 1;
   left = -right;

   scale = (-g_width + 1) / (left - right);
   translate = scale * -left;

   return (in_x - translate) / scale;
}

// Convert pixel y-coordinate to world y-coordinate.
float p2wy(int in_y) {
   double bottom, top, scale, translate;

   // Flip  glfw y-coordinate.
   in_y = g_height - in_y;
   top = g_height >= g_width ? g_height / g_width : 1;
   bottom = -top;

   scale = (-g_height + 1) / (bottom - top);
   translate = scale * -bottom;

   return (in_y - translate) / scale;
}
