#ifndef BOUNDING_H
#define BOUNDING_H

#include "glm/gtc/type_ptr.hpp"


class Bounding {
   protected:
      glm::vec3 center;
      float width, height;

   public:
      Bounding(glm::vec3 center, float width, float height) {
         this->center = center;
         this->width = width;
         this->height = height;
      }

      glm::vec3 getCenter(void) {
         return center;
      }

      float getWidth(void) {
         return width;
      }

      float getHeight(void) {
         return height;
      }

      void update(glm::vec3 center) {
         this->center = center;
      }

      virtual bool overlap(Bounding *) = 0;
};

#endif
