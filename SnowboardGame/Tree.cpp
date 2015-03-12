#include "Tree.h"

Tree::Tree(glm::vec3 position, glm::vec3 velocity, float radius): TangibleObject(position, velocity, radius) {
   this->trunk = new Model(CYLINDER_MESH);
   this->tier = new Model(CONE_MESH);
}

void Tree::step(float elapsedSeconds) {

}
glm::vec3 Tree::getPosition(){
	return this->position;
}

void Tree::draw(void) {
   if (shaking) {
      shake(5);
   }

   g_mStack.pushMatrix();
      g_mStack.translate(position);
      g_mStack.scale(.5f, .5f, .5f);
      g_mStack.scale(.5f, 1.f, .5f);
      g_mStack.rotate(-90, iHat); // new basis: (+X, -Z, +Y)

      // Bind model material.
      setMaterial(BARK_MAT);
      this->trunk->draw();

      g_mStack.pushMatrix();
         // Bind model material.
         if (shaking) {
            setMaterial(PINE_MAT_LIGHT);
         }
         else {
            setMaterial(PINE_MAT);
         }

         g_mStack.scale(3.f, 3.f, 3.f);
         g_mStack.translate(glm::vec3(0, 0, .25f));
         this->tier->draw();
         g_mStack.translate(glm::vec3(0, 0, .25f));
         this->tier->draw();
         g_mStack.translate(glm::vec3(0, 0, .25f));
         this->tier->draw();
      g_mStack.popMatrix();

   g_mStack.popMatrix();
}

void Tree::onCollision(void) {
   shaking = true;
}
