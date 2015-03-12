/*
 * OpenGL 3.0 Core-friendly replacement for glu primitives
 *
 *  Created on: 9/26/2012
 *     Author: Mostly Ian Dunn
 */

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif

#ifdef __unix__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/glut.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freeglut.lib")
#endif

#include "glm/glm.hpp"
#include <vector>

struct Mesh
{
   GLuint PositionHandle;
   GLuint NormalHandle;
   GLuint TexCoordHandle;
   GLuint IndexHandle;

   unsigned int IndexBufferLength;

   Mesh(const std::vector<GLfloat> &Positions,
        const std::vector<GLfloat> &Normals,
        const std::vector<GLfloat> &TexCoords,
        const std::vector<GLuint> &Indices);
};

class GeometryCreator
{

public:
   static void loadIndexBufferObjectFromMesh(
     Mesh *mesh,
     int *TriangleCount,
     GLuint *PositionBufferHandle,
     GLuint *NormalBufferHandle,
     GLuint *TexCoordHandle,
     GLuint *IndexBufferHandle);

   static Mesh *CreatePlane(glm::vec3 const &Size);
   static Mesh *CreateCube(glm::vec3 const &Size);

   static Mesh *CreateCylinder(
      float const baseRadius, 
      float const topRadius, 
      float const height, 
      unsigned int const slices, 
      unsigned int const stacks);

   static Mesh *CreateSphere(
      glm::vec3 const & Radii = glm::vec3(1.f), 
      unsigned int const Slices = 32, 
      unsigned int const Stacks = 16);
};
