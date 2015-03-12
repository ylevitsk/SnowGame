#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>

#include "Camera.h"

#define MIN_ZOOM 2
#define START_ZOOM 5
#define MAX_ZOOM 30

/** Global pointer to the camera object. */
Camera *g_camera = new Camera();

Camera::Camera(void)
{
   this->yaw = 0;
   this->pitch = -M_PI / 2.0;
   this->lookAt = glm::vec3(0, 0, -1); // Look down the -z-axis.
   this->position = glm::vec3(0, 0, 0);
   this->zoomAmt = START_ZOOM;
}

/**
 * Returns a view matrix, which transforms coordinates from world space to
 * camera space.
 */
glm::mat4 Camera::viewMatrix(void)
{
   glm::mat4 gaze = glm::lookAt(glm::vec3(0), this->lookAt, glm::vec3(0, 1, 0));
   // Translate the world to the camera's basis.
   glm::mat4 trans = glm::translate(glm::mat4(1), -this->position);

   return gaze * trans;
}

/** Returns a vec3 representing the camera's position in world space. */
glm::vec3 Camera::getPosition(void)
{
   return this->position;
}

/** Camera's specialized handler for keyboard input; called from main loop. */
void Camera::keyPress(int key)
{
   const glm::mat4 rot = glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 1, 0));
   glm::vec4 strafe;

   switch(key) {
      // Camera controls.
      case GLFW_KEY_I:
         // Zoom in.
         if (this->zoomAmt > MIN_ZOOM) {
            this->zoomAmt -= 1;
         }
         break;
      /*case GLFW_KEY_J:
         strafe = rot * glm::vec4(this->lookAt, 0);
         this->translation.x += CAM_STEP * strafe.x;
         this->translation.z += CAM_STEP * strafe.z;
         break;*/
      case GLFW_KEY_K:
         // Zoom out.
         if (this->zoomAmt < MAX_ZOOM) {
            this->zoomAmt += 1;
         }
         break;
      /*case GLFW_KEY_L:
         strafe = rot * glm::vec4(this->lookAt, 0);
         this->translation.x -= CAM_STEP * strafe.x;
         this->translation.z -= CAM_STEP * strafe.z;
         break;*/
   }
}

/** TODO(evan) this isn't used right now. Offer a 'free look' mode? */
void Camera::mouseDrag(float winWidth, float winHeight, float fromX,
 float fromY, float toX, float toY)
{
   // We set a vertical look lookConstraint of 80 degrees up, and down.
   static float lookConstraint = (80.0 / 90.0) * (M_PI / 2.0);
   float newYaw;

   // Map difference in y to alpha (in radians)
   newYaw = winHeight / 2.0 * ((toY - fromY) / winHeight) * M_PI + this->yaw;

   if (newYaw > -lookConstraint && newYaw < lookConstraint) {
      this->yaw = newYaw;
   }
   // Map difference in x to beta (in radians)
   this->pitch += winWidth / 2.0 * ((toX - fromX) / winWidth) * M_PI;


   this->lookAt.x = cos(this->yaw) * cos(this->pitch);
   this->lookAt.y = sin(this->yaw);
   this->lookAt.z = cos(this->yaw) * cos(M_PI / 2 - this->pitch);
}

/**
 * Updates the camera's world space position based on the player's position
 * and gaze vectors.
 */
void Camera::followPlayer(glm::vec3 playerPosition, glm::vec3 playerGaze)
{
   // Initially set camera's position as the player's position.
   this->position = playerPosition;

   // Set camera behind (x, z) and above (y) the player.
   this->position.x -= this->zoomAmt * playerGaze.x;
   this->position.z -= this->zoomAmt * playerGaze.z;
   this->position.y += this->zoomAmt / 2.0;

   // Update camera gaze to look at player.
   this->lookAt = playerPosition - this->position;
}
