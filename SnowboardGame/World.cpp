#include <math.h>
#include <iostream>
#include <vector>
#include "World.h"
#include "Player.h"
#include "Shader.h"
#include "Terrain.h"
#include "Tree.h"
#include "Texture.h"
#include "Text.h"

#include "Main.h"

#include "glm/gtc/type_ptr.hpp"
#include "third_party/GeometryCreator.h"

RenderingHelper g_mStack;

// Simple generated 3D meshes.
static Mesh *g_meshes[MAX_MESH_INDEX + 1];

static Player *player;
static Terrain *terrain;
static std::vector<Tree *> trees;
static Model *skyBox;

Model::Model(const int meshIndex)
{
   // Load the model from a generated primitive model.
   GeometryCreator::loadIndexBufferObjectFromMesh(g_meshes[meshIndex],
    &this->polyCount, &this->vbo, &this->nbo, &this->tbo, &this->ibo);
}

void Model::draw(void)
{
   // Bind model matrix uniform.
   safe_glUniformMatrix4fv(g_shader->get(U_MODEL_MX),
    glm::value_ptr(g_mStack.modelViewMatrix));

   // Bind vertex attributes.
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   safe_glVertexAttribPointer(g_shader->get(A_POS), 3, GL_FLOAT, GL_FALSE, 0, 0);
   glBindBuffer(GL_ARRAY_BUFFER, this->nbo);
   safe_glVertexAttribPointer(g_shader->get(A_NORM), 3, GL_FLOAT, GL_FALSE, 0, 0);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

   // Draw geometry
   glDrawElements(GL_TRIANGLES, this->polyCount * 3, GL_UNSIGNED_INT, 0);
}

void drawWorld(double elapsedTime) {
   // Enable the shader.
   enableShader(SHADER_NORM);

   // Enable uniforms and attributes.
   setMatrices();
   safe_glEnableVertexAttribArray(g_shader->get(A_POS));
   safe_glEnableVertexAttribArray(g_shader->get(A_NORM));

   terrain->draw(player->getZ());
   // TODO(evan) Move this call?
   player->update(terrain);
   player->step(elapsedTime);
   player->draw();
   // Iterate over and draw trees.
   std::vector<Tree *>::iterator tree = trees.begin();
   while (tree != trees.end()) {
      if ((*tree)->getPosition().z >= player->getZ() - 50 &&
          (*tree)->getPosition().z < player->getZ() + 50) {
         if ((*tree)->bounding->overlap(player->bounding)) {
            (*tree)->onCollision();
            player->onCollision();
         }
         (*tree)->draw();
      }
      tree++;
   }

   // Disable the shader.
   disableShader(SHADER_NORM);
}

void drawSkyBox() {
   enableShader(SHADER_TEX);

   // Enable uniforms and attributes.
   setMatrices();
   safe_glEnableVertexAttribArray(g_shader->get(A_POS));
   safe_glEnableVertexAttribArray(g_shader->get(A_NORM));

   // Disabling backface culling so the skybox shows up.
   glDisable(GL_CULL_FACE);

   // Set up the texture unit.
   glEnable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0);
   safe_glUniform1i(g_shader->get(U_TEX_UNIT), 0);
   safe_glEnableVertexAttribArray(g_shader->get(A_TEX_COORD));

   setMaterial(SKYBOX_MAT);
   glBindTexture(GL_TEXTURE_2D, BARK_TEX);

   g_mStack.pushMatrix();
      g_mStack.translate(glm::vec3(terrain->width() / 2, terrain->length() / 2, 0));
      g_mStack.scale(1000, 1000, 1000);
      skyBox->draw();
   g_mStack.popMatrix();

   // Reenable backface culling.
   glEnable(GL_CULL_FACE);

   disableShader(SHADER_TEX);
}

void worldKeyPress(GLFWwindow *window, int key)
{
   player->keyPress(window, key);
}

void drawGeom(double elapsedTime, double totalTime, float winWidth, float winHeight)
{
   drawWorld(elapsedTime);
   drawSkyBox();

   // Create score string to display to screen
   char timeStr[100];
   sprintf(timeStr, "Time: %.1f", totalTime);

   //TODO(Katie) make this easier for other people to use/understand
   float sx = 2.0 / winWidth;
   float sy = 2.0 / winHeight;
   renderText(timeStr, -1 + 8 * sx, 1 - 50 * sy, sx, sy);
}

void initGeom()
{
   // Generate some simple models.
   g_meshes[CYLINDER_MESH] = GeometryCreator::CreateCylinder(
    1.f, 1.f, 1.5f, 3, 3);
   g_meshes[CONE_MESH] = GeometryCreator::CreateCylinder(
    .8f, 0.f, 1.6f, 8, 16);
   g_meshes[GROUND_MESH] = GeometryCreator::CreateCylinder(
    1.f, 1.f, 1.5f, 8, 8);
   g_meshes[CUBE_MESH] = GeometryCreator::CreateCube(glm::vec3(1.f, 1.f, 1.f));
   g_meshes[PLANE_MESH] = GeometryCreator::CreatePlane(
    glm::vec3(1.f, 1.f, 1.f));
   g_meshes[SPHERE_MESH] = GeometryCreator::CreateSphere(glm::vec3(1.5f));
   g_meshes[SMOOTH_CYL_MESH] = GeometryCreator::CreateCylinder(
    1.f, 1.f, 1.5f, 4, 8);

   // TODO(evan) clean this up: loadTerrain("filename", <height>)
   terrain = loadTerrain("heightmap.bmp", 20, "treemap.bmp", trees);
   skyBox = new Model(CUBE_MESH);
   player = new Player(terrain->getStartPos(), glm::vec3(0, 0, 1), 0.5f);
}

void setMaterial(int matId)
{
   safe_glUniform3f(g_shader->get(U_MAT_A_CLR), 0.1, 0.1, 0.1);

   switch(matId) {
   case SNOW_MAT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.6, 0.6, 0.9);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.1, 0.1, 0.1);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 10.0);
      safe_glUniform3f(g_shader->get(U_MAT_A_CLR), 0.4, 0.4, 0.4);
      break;
   case PINE_MAT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.2, 0.2, 0.4);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.1, 0.1, 0.1);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 1.0);
      break;
   case PINE_MAT_LIGHT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.2, 0.5, 0.4);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.1, 0.3, 0.1);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 1.0);
      break;
   case BARK_MAT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.6, 0.2, 0.2);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.1, 0.1, 0.1);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 1.0);
      break;
   case TUX_MAT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.1, 0.1, 0.1);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.5, 0.5, 0.5);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 100.0);
      break;
   case ORANGE_MAT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.95, 0.4, 0.15);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.5, 0.35, 0.15);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 50.0);
      safe_glUniform3f(g_shader->get(U_MAT_A_CLR), 0.4, 0.2, 0.1);
      break;
   case BOARD_MAT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.1, 0.3, 0.1);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.1, 0.4, 0.1);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 5.0);
      break;
   case SKYBOX_MAT:
      safe_glUniform3f(g_shader->get(U_MAT_D_CLR), 0.125, 0.7, 0.67);
      safe_glUniform3f(g_shader->get(U_MAT_S_CLR), 0.1, 0.4, 0.1);
      safe_glUniform1f(g_shader->get(U_MAT_SHINE), 5.0);
      break;
   }
}
