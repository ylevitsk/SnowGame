#include "third_party/GLSL_helper.h"
#include "World.h"
#include "Shader.h"
#include "Terrain.h"
#include "Tree.h"

#include "glm/gtc/type_ptr.hpp"
#include "third_party/ImageLoader.h"

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain *loadTerrain(const char *heightmap, float height,
 const char *treemap, std::vector<Tree *> &trees)
{
   bool spawnTree = false;

   Image* image = loadBMP(heightmap);
   Terrain* t = new Terrain(image->width, image->height);
   for(int z = 0; z < image->height; z++) {
      for(int x = 0; x < image->width; x++) {
         unsigned char color =
            (unsigned char)image->pixels[3 * (z * image->width + x)]; // R-value
         float h = height * ((color / 255.0f));
         t->setHeight(x, z, h);
         t->setRandom(x, z);
      }
   }

   delete image;

   image = loadBMP(treemap);
   for (int z = 0; z < image->height; z++) {
      for (int x = 0; x < image->width; x++) {
         unsigned char g_color =
            (unsigned char)image->pixels[3 * (z * image->width + x) + 1];
         unsigned char b_color =
            (unsigned char)image->pixels[3 * (z * image->width + x) + 2];
         float g = (g_color / 255.0f);
         float b = (b_color / 255.0f);

         if (!b) {
            trees.push_back(new Tree(glm::vec3(x, t->getHeight(x, z), z), //position
                                     glm::vec3(0), // velocity
                                     1.f)); // collision radius
         }
         else if (!g) {
            t->setStartPos(glm::vec3(x, (int)t->getHeight(x, z), z));
         }
      }
   }

   glm::vec3 blah = t->getStartPos();

   delete image;
   t->computeNormals();
   t->initGeom();

   return t;
}

void Terrain::initGeom(void)
{
	std::vector<float> GrndPos, GrndNorm;
	std::vector<unsigned int> idx;
	Chunk *c = new Chunk(idx);
	int j = 0, z = 0;

	for (z = 0; z < this->length() - 1; z++) {
		for (int x = 0; x < this->width(); x++) {
			glm::vec3 normal = this->getNormal(x, z);
			glm::vec3 normal2 = this->getNormal(x, z + 1);

			GrndPos.push_back(x);
			GrndPos.push_back(this->getHeight(x, z));
			GrndPos.push_back(z);
			GrndPos.push_back(x);
			GrndPos.push_back(this->getHeight(x, z + 1));
			GrndPos.push_back(z + 1);

			GrndNorm.push_back(normal.x);
			GrndNorm.push_back(normal.y);
			GrndNorm.push_back(normal.z);
			GrndNorm.push_back(normal2.x);
			GrndNorm.push_back(normal2.y);
			GrndNorm.push_back(normal2.z);

			c->idx.push_back(j++);
			c->idx.push_back(j++);

		}

		// Create two degenerate triangles so we don't create strange artifacts
		// when moving to the next strip.
		c->idx.push_back(j - 1);
		c->idx.push_back(j);
		if (z % 100 == 0 && z){
			this->chunk.push_back(*c);
			c = new Chunk(idx);
		}
	}
	if (z % 100)
		chunk.push_back(*c);
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* GrndPos.size(), &GrndPos[0], GL_STATIC_DRAW);

	glGenBuffers(1, &this->nbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* GrndNorm.size(), &GrndNorm[0], GL_STATIC_DRAW);
}

void Terrain::draw(int i)
{
	g_mStack.pushMatrix();
	int j;
	int k = i / 100;
	g_mStack.scale(1.f, 1.f, 1.f);
	if (i - k * 100 >= 50){
		this->drawMesh(this->chunk[k].idx);
		this->drawMesh(this->chunk[k + 1].idx);
	}
	else if (k && (i - k * 100 < 10)){
		this->drawMesh(this->chunk[k - 1].idx);
		this->drawMesh(this->chunk[k].idx);
	}
	else
		this->drawMesh(this->chunk[k].idx);
	g_mStack.popMatrix();
}

void Terrain::drawMesh(std::vector<unsigned int> idx)
{
	glGenBuffers(1, &this->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* idx.size(), &idx[0], GL_STATIC_DRAW);

	// Bind model matrix uniform.
	safe_glUniformMatrix4fv(g_shader->get(U_MODEL_MX),
		glm::value_ptr(g_mStack.modelViewMatrix));

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	safe_glVertexAttribPointer(g_shader->get(A_POS), 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->nbo);
	safe_glVertexAttribPointer(g_shader->get(A_NORM), 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

	// Set material and draw the terrain.
	setMaterial(SNOW_MAT);
	glDrawElements(GL_TRIANGLE_STRIP, idx.size(), GL_UNSIGNED_INT, 0);
}
