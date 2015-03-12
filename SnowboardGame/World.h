#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <GLFW/glfw3.h>

#include "third_party/GLSL_helper.h"
#include "third_party/MStackHelp.h"

#define RAD_TO_DEG(rad) rad * 180 / M_PI
#define DEG_TO_RAD(deg) deg * M_PI / 180

#define CYLINDER_MESH 0
#define CONE_MESH 1
#define CUBE_MESH 2
#define PLANE_MESH 3
#define GROUND_MESH 4
#define SPHERE_MESH 5
#define SMOOTH_CYL_MESH 6
#define MAX_MESH_INDEX 6
#define SNOW_MAT 0
#define PINE_MAT 1
#define BARK_MAT 2
#define TUX_MAT 3
#define ORANGE_MAT 4
#define BOARD_MAT 5
#define SKYBOX_MAT 6
#define PINE_MAT_LIGHT 7

static const glm::vec3 iHat(1, 0, 0);
static const glm::vec3 jHat(0, 1, 0);
static const glm::vec3 kHat(0, 0, 1);

// Matrix stack utility.
extern RenderingHelper g_mStack;

class Model
{
   private:
      GLuint vbo, ibo, nbo, tbo;
      int polyCount;

   public:
      ~Model(void) {}
      Model(const int meshIndex);
      void draw(void);
};

void initGeom();
void drawGeom(double elapsedTime, double totalTime, float winWidth, float winHeight);
void setMaterial(int matId);
void worldKeyPress(GLFWwindow *window, int key);

#endif
