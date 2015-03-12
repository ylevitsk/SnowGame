#ifndef PLAYER_H
#define PLAYER_H

#include "TangibleObject.h"
#include "Terrain.h"
#include "World.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <GLFW/glfw3.h>

class Player : public TangibleObject
{
   private:
      Model *ball;
      glm::vec3 gaze;

   public:
      ~Player(void) {}
      Player(glm::vec3 position, glm::vec3 velocity, float radius);
      void update(Terrain *);
      void step(float);
      void draw(void);
      void onCollision(void);
      void keyPress(GLFWwindow *window, int key);
	  float getZ();
};

#endif
