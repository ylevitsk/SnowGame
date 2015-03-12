#ifndef TANGIBLE_OBJECT_H
#define TANGIBLE_OBJECT_H

#include "BoundingSphere.h"
#include <time.h>

class TangibleObject {
   private:
      bool shakeLeft;
      int stepSize, directionSwitchCount;
      float shakeAccumulator;

      bool incrementAccumulator(float *accumulator, float upperBound, float lowerBound, int stepSize, bool *directionMarker) {
         bool directionSwitch = false;

         if (*accumulator > upperBound) {
            *directionMarker = false;
            directionSwitch = true;
         }
         else if (*accumulator < lowerBound) {
            *directionMarker = true;
            directionSwitch = true;
         }

         if (*directionMarker) {
            *accumulator += stepSize * 0.001f;
         }
         else {
            *accumulator -= stepSize * 0.001f;
         }

         return directionSwitch;
      }

      void initializeMemberVariables(glm::vec3 position, glm::vec3 velocity, glm::vec3 boundingSpherePosition, float boundingSphereRadius) {
         this->position = position;
         this->velocity = velocity;
         this->bounding = new BoundingSphere(position, boundingSphereRadius);

         //defaults for shake animation
         this->shaking = false;
         this->shakeLeft = true;
         this->stepSize = 30;
         this->directionSwitchCount = 0;
         this->shakeAccumulator = 0.f;

         //TODO: replace with call to global counter
         srand(time(NULL));
         id = rand();
      }

   protected:
      glm::vec3 position, velocity;
      bool shaking;
      int id;

      bool shake(int occulations) {
         if (shaking) {
            if (incrementAccumulator(&shakeAccumulator, 0.1, -0.1, stepSize, &shakeLeft)) {
               directionSwitchCount++;
            }

            if (directionSwitchCount >= occulations) {
               shaking = false;
               directionSwitchCount = 0;
            }
            position.x += shakeAccumulator;
         }
		 return true;
      }

   public:
      Bounding *bounding;

      //velocity is meters per second
      TangibleObject(glm::vec3 position, glm::vec3 velocity, float boundingSphereRadius) {
         initializeMemberVariables(position, velocity, position, boundingSphereRadius);
      }

      TangibleObject(glm::vec3 position, glm::vec3 velocity, glm::vec3 boundingSpherePosition, float boundingSphereRadius) {
         initializeMemberVariables(position, velocity, boundingSpherePosition, boundingSphereRadius);
      }

      virtual void step(float) = 0;
      virtual void draw(void) = 0;
      virtual void onCollision(void) = 0;

      int getId(void);
};

#endif
