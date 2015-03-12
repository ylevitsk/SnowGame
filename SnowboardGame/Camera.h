#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define CAM_STEP 2.0f

class Camera
{
   private:
      float yaw;
      float pitch;
      float zoomAmt;
      glm::vec3 lookAt;
      glm::vec3 position;

   public:
      ~Camera(void) {}
      Camera(void);
      glm::mat4 viewMatrix(void);
      glm::vec3 getPosition(void);
      void keyPress(int key);
      void mouseDrag(float winWidth, float winHeight, float fromX, float fromY, float toX, float toY);
      void followPlayer(glm::vec3 playerPosition, glm::vec3 playerGaze);
};

extern Camera *g_camera;

#endif
