#include "Player.h"
#include "Main.h"
#include "Camera.h"

#define STEP_DIST 0.2f

Player::Player(glm::vec3 position, glm::vec3 velocity, float radius): TangibleObject(position, velocity, radius)
{
   this->ball = new Model(SPHERE_MESH);

   this->gaze = glm::normalize(velocity);
}

void Player::update(Terrain *terrain)
{
   float height = terrain->getHeight(this->position.x, this->position.z);

   if (this->position.y < height - STEP_DIST)
      this->position.y += 0.1;
   else if (this->position.y > height + STEP_DIST)
      this->position.y -= 0.1;
}
float Player::getZ(){
	return this->position.z;
}
void Player::step(float elapsedSeconds)
{
   this->position += STEP_DIST * this->gaze;
   g_camera->followPlayer(this->position, this->gaze);
   this->bounding->update(position);
}

void Player::draw(void)
{
   g_mStack.pushMatrix();

      setMaterial(TUX_MAT);
      g_mStack.translate(this->position);
      g_mStack.scale(.5f, .5f, .5f);
      this->ball->draw();

   g_mStack.popMatrix();
}

void Player::onCollision(void) {

}

void Player::keyPress(GLFWwindow *window, int key)
{
   // Rotate by 2 degrees around the y-axis.
   const glm::mat4 rot_left = glm::rotate(glm::mat4(1), 10.0f, glm::vec3(0, 1, 0));
   const glm::mat4 rot_right = glm::rotate(glm::mat4(1), -10.0f, glm::vec3(0, 1, 0));
   glm::vec4 blah;

   // Camera controls.
   //if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
      //this->position += STEP_DIST * this->gaze;
   //}
   if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
      blah = rot_left * glm::vec4(this->gaze, 0);
      this->gaze = glm::vec3(blah);
   }
   if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
      blah = rot_right * glm::vec4(this->gaze, 0);
      this->gaze = glm::vec3(blah);
   }
}
