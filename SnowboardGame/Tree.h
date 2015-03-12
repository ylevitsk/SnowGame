#ifndef TREE_H
#define TREE_H

#include "World.h"
#include "TangibleObject.h"

class Tree : public TangibleObject {
   private:
      Model *trunk;
      Model *tier;

   public:
      Tree(glm::vec3 position, glm::vec3 velocity, float radius);
      ~Tree(void) {}
      void step(float);
      void draw(void);
      void onCollision(void);
	  glm::vec3 getPosition();
};

#endif
