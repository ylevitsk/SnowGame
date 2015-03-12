#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>
#include <stdlib.h>
#include <vector>
// TODO(evan) Switch to GLM.
#include "Tree.h"
//#include "third_party/vec3f.h"
#include <GLFW/glfw3.h>
#include "Chunk.h"

const float FALLOUT_RATIO = 0.5f;
//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
private:
	int w; //Width
	int l; //Length
	float** hs; //Heights
	int ** rd; //probability
	glm::vec3 ** normals;

	bool computedNormals; //Whether normals is up-to-date

	std::vector<Chunk> chunk;
	GLuint vbo, ibo, nbo;
	int polyCount;
	glm::vec3 startPos;

	void drawMesh(std::vector<unsigned int>);

public:
	void initGeom();
	void draw(int);

	Terrain(int w2, int l2) {
		w = w2;
		l = l2;

		hs = new float*[l];
		for (int i = 0; i < l; i++) {
			hs[i] = new float[w];
		}

		rd = new int*[l];
		for (int i = 0; i < l; i++) {
			rd[i] = new int[w];
		}

		normals = new glm::vec3*[l];
		for (int i = 0; i < l; i++) {
			normals[i] = new glm::vec3[w];
		}

		computedNormals = false;
	}

	~Terrain() {
		for (int i = 0; i < l; i++) {
			delete[] hs[i];
		}
		delete[] hs;

		for (int i = 0; i < l; i++) {
			delete[] rd[i];
		}
		delete[] rd;

		for (int i = 0; i < l; i++) {
			delete[] normals[i];
		}
		delete[] normals;
	}

	int width() {
		return w;
	}

	int length() {
		return l;
	}

	void setStartPos(glm::vec3 startPos) {
		this->startPos = startPos;
	}

	glm::vec3 getStartPos() {
		return this->startPos;
	}


	void setRandom(int x, int z){
		float height = hs[z][x];
		if (height < -9.3)
			rd[z][x] = 1;
		else if (height < -6)
			rd[z][x] = rand() % 2;
		else
			rd[z][x] = 0;
	}

	//Sets the height at (x, z) to y
	void setHeight(int x, int z, float y) {
		hs[z][x] = y;
		computedNormals = false;
	}

	//Returns the height at (x, z)
	float getHeight(int x, int z) {
		return hs[z][x];
	}
	//Returns the rand at (x, z)
	float getRand(int x, int z) {
		return rd[z][x];
	}

	//Computes the normals, if they haven't been computed yet
	void computeNormals() {
		if (computedNormals) {
			return;
		}

		//Compute the rough version of the normals
		glm::vec3** normals2 = new glm::vec3*[l];
		for (int i = 0; i < l; i++) {
			normals2[i] = new glm::vec3[w];
		}

		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				glm::vec3 sum(0.0f, 0.0f, 0.0f);

				glm::vec3 out;
				if (z > 0) {
					out = glm::vec3(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
				}
				glm::vec3 in;
				if (z < l - 1) {
					in = glm::vec3(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
				}
				glm::vec3 left;
				if (x > 0) {
					left = glm::vec3(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
				}
				glm::vec3 right;
				if (x < w - 1) {
					right = glm::vec3(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
				}

				if (x > 0 && z > 0) {
					sum += glm::normalize(glm::cross(out, left));// out.cross(left).normalize();
				}
				if (x > 0 && z < l - 1) {
					sum += glm::normalize(glm::cross(left, in));// left.cross(in).normalize();
				}
				if (x < w - 1 && z < l - 1) {
					sum += glm::normalize(glm::cross(in, right));// in.cross(right).normalize();
				}
				if (x < w - 1 && z > 0) {
					sum += glm::normalize(glm::cross(right, out));// right.cross(out).normalize();
				}

				normals2[z][x] = sum;
			}
		}

		//Smooth out the normals const float FALLOUT_RATIO = 0.5f;
		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				glm::vec3 sum = normals2[z][x];

				if (x > 0) {
					sum += normals2[z][x - 1] * FALLOUT_RATIO;
				}
				if (x < w - 1) {
					sum += normals2[z][x + 1] * FALLOUT_RATIO;
				}
				if (z > 0) {
					sum += normals2[z - 1][x] * FALLOUT_RATIO;
				}
				if (z < l - 1) {
					sum += normals2[z + 1][x] * FALLOUT_RATIO;
				}

				if (sqrtf(pow(sum.x, 2) + pow(sum.y, 2) + pow(sum.z, 2)) == 0) {
					sum = glm::vec3(0.0f, 1.0f, 0.0f);
				}
				normals[z][x] = sum;
			}
		}

		for (int i = 0; i < l; i++) {
			delete[] normals2[i];
		}
		delete[] normals2;

		computedNormals = true;
	}

	//Returns the normal at (x, z)
	glm::vec3 getNormal(int x, int z) {
		if (!computedNormals) {
			computeNormals();
		}
		return normals[z][x];
	}
};

Terrain *loadTerrain(const char *heightmap, float height,
	const char *treemap, std::vector<Tree *> &trees);

#endif