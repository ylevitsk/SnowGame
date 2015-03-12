#include "GeometryCreator.h"
#include <iostream>

using namespace glm;

Mesh::Mesh(const std::vector<GLfloat> &Positions,
 const std::vector<GLfloat> &Normals, const std::vector<GLfloat> &TexCoords,
 const std::vector<GLuint> &Indices)
{
   this->PositionHandle = 0;
   this->NormalHandle = 0;
   this->IndexHandle = 0;
   this->TexCoordHandle = 0;
   this->IndexBufferLength = 0;
   IndexBufferLength = Indices.size();

   glGenBuffers(1, &this->PositionHandle);
   glGenBuffers(1, &this->NormalHandle);
   glGenBuffers(1, &this->IndexHandle);
   glGenBuffers(1, &this->TexCoordHandle);

   glBindBuffer(GL_ARRAY_BUFFER, this->PositionHandle);
   glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(GLfloat),
   &Positions.front(), GL_STATIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, this->NormalHandle);
   glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(GLfloat),
   &Normals.front(), GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->TexCoordHandle);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, TexCoords.size() * sizeof(GLfloat),
   &TexCoords.front(), GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexHandle);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint),
   &Indices.front(), GL_STATIC_DRAW);
}

void GeometryCreator::loadIndexBufferObjectFromMesh(
   Mesh *mesh,
   int *TriangleCount,
   GLuint *PositionBufferHandle,
   GLuint *NormalBufferHandle,
   GLuint *TexCoordBufferHandle,
   GLuint *IndexBufferHandle)
{
   // There should be (TriangleCount * 3) vertices rep'd in the index buffer.
   *TriangleCount = mesh->IndexBufferLength / 3;
   *PositionBufferHandle = mesh->PositionHandle;
   *NormalBufferHandle = mesh->NormalHandle;
   *TexCoordBufferHandle = mesh->TexCoordHandle;
   *IndexBufferHandle = mesh->IndexHandle;
}

Mesh * GeometryCreator::CreateCylinder(
   float const baseRadius,
   float const topRadius,
   float const height,
   unsigned int const slices,
   unsigned int const stacks)
{
   std::vector<GLfloat> Positions, Normals, TexCoords;
   std::vector<GLuint> Indices;

   // Make bottom disc
   Positions.push_back(0.f); Positions.push_back(0.f); Positions.push_back(0.f);
   Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);

   Positions.push_back(baseRadius); Positions.push_back(0.f); Positions.push_back(0.f);
   Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);
   TexCoords.push_back(0.f); TexCoords.push_back(0.f);

   for (unsigned int i = 1; i <= slices; ++ i)
   {
     float const Angle = (float) i * 2.f * 3.14159f / (slices);
     Positions.push_back(cos<float>(Angle)*baseRadius);
     Positions.push_back(sin<float>(Angle)*baseRadius);
     Positions.push_back(0.f);
     Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);
     Indices.push_back(0);
     Indices.push_back(i+1);
     Indices.push_back(i+0);
     TexCoords.push_back(0.f); TexCoords.push_back(0.f);
   }

   // Make top disc
   unsigned short const TopStart = Positions.size() / 3;
   Positions.push_back(0.f); Positions.push_back(0.f); Positions.push_back(height);
   Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);

   Positions.push_back(topRadius); Positions.push_back(0.f); Positions.push_back(height);
   Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);
   TexCoords.push_back(0.f); TexCoords.push_back(0.f);

   for (unsigned int i = 1; i <= slices; ++ i)
   {
     float const Angle = (float) i * 2.f * 3.14159f / slices;
     Positions.push_back(cos<float>(Angle)*topRadius);
     Positions.push_back(sin<float>(Angle)*topRadius);
     Positions.push_back(height);
     Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);
     Indices.push_back(TopStart);
     Indices.push_back(TopStart + i+0);
     Indices.push_back(TopStart + i+1);
     TexCoords.push_back(0.f); TexCoords.push_back(0.f);
   }

   // Make sides
   unsigned int SideStart1 = 0, SideStart2 = 0;
   for (unsigned int j = 0; j <= stacks; ++ j)
   {
     float Interpolation = (float) j / stacks;
     float Radius = Interpolation * topRadius + (1.f - Interpolation) * baseRadius;

     SideStart2 = Positions.size() / 3;
     for (unsigned int k = 0; k <= slices; ++ k)
     {
       float const Angle = (float) k * 2.f * 3.14159f / slices;
       Positions.push_back(cos<float>(Angle) * Radius);
       Positions.push_back(sin<float>(Angle) * Radius);
       Positions.push_back(Interpolation * height);
       Normals.push_back(cos<float>(Angle)); Normals.push_back(sin<float>(Angle)); Normals.push_back(0.f);
       TexCoords.push_back(Angle / (2.f * 3.14159f)); // tex x-coord
       TexCoords.push_back(Interpolation * height); // tex y-coord
     }

     if (j)
     {
       for (unsigned int k = 0; k < slices; ++ k)
       {
         Indices.push_back(SideStart1 + k);
         Indices.push_back(SideStart1 + k + 1);
         Indices.push_back(SideStart2 + k + 1);

         Indices.push_back(SideStart1 + k);
         Indices.push_back(SideStart2 + k + 1);
         Indices.push_back(SideStart2 + k);
       }
     }
     SideStart1 = SideStart2;
   }

   return new Mesh(Positions, Normals, TexCoords, Indices);
}

Mesh * GeometryCreator::CreatePlane(vec3 const & Size)
{
   std::vector<GLfloat> Positions, Normals, TexCoords;
   std::vector<GLuint> Indices;

   static float const PlanePositions[] =
   {
     -0.5,  0,  0.5,
     -0.5,  0, -0.5,
      0.5,  0, -0.5,
      0.5,  0,  0.5,
   };
   Positions = std::vector<GLfloat>(PlanePositions, PlanePositions + 4 * 3);
   int i = 0;
   for (std::vector<GLfloat>::iterator it = Positions.begin(); it != Positions.end(); ++ it, ++ i)
     *it *= Size[i %= 3];

   static float const PlaneNormals[] =
   {
     0, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0
   };
   Normals = std::vector<GLfloat>(PlaneNormals, PlaneNormals + 4 * 3);

   static float const PlaneTex[] =
   {
     0, 1,
     0, 0,
     1, 0,
     1, 1
   };
   TexCoords = std::vector<GLfloat>(PlaneTex, PlaneTex + 4 * 2);

   static float const PlaneIndices[] =
   {
     0, 1, 2,
     0, 2, 3
   };
   Indices = std::vector<GLuint>(PlaneIndices, PlaneIndices + 2 * 3);

   return new Mesh(Positions, Normals, TexCoords, Indices);
}

Mesh * GeometryCreator::CreateCube(vec3 const & Size)
{
   std::vector<GLfloat> Positions, Normals, TexCoords;
   std::vector<GLuint> Indices;

   static float const CubePositions[] =
   {
     -0.5, -0.5, -0.5, // back face verts [0-3]
     -0.5,  0.5, -0.5,
      0.5,  0.5, -0.5,
      0.5, -0.5, -0.5,
      
     -0.5, -0.5,  0.5, // front face verts [4-7]
     -0.5,  0.5,  0.5,
      0.5,  0.5,  0.5,
      0.5, -0.5,  0.5,
      
     -0.5, -0.5,  0.5, // left face verts [8-11]
     -0.5, -0.5, -0.5,
     -0.5,  0.5, -0.5,
     -0.5,  0.5,  0.5,
     
      0.5, -0.5,  0.5, // right face verts [12-15]
      0.5, -0.5, -0.5,
      0.5,  0.5, -0.5,
      0.5,  0.5,  0.5,
      
     -0.5,  0.5,  0.5, // top face verts [16-19]
     -0.5,  0.5, -0.5,
      0.5,  0.5, -0.5,
      0.5,  0.5,  0.5,
     
     -0.5, -0.5,  0.5, // bottom face verts [20-23]
     -0.5, -0.5, -0.5,
      0.5, -0.5, -0.5,
      0.5, -0.5,  0.5
   };
   Positions = std::vector<GLfloat>(CubePositions, CubePositions + 24 * 3);
   int i = 0;
   for (std::vector<GLfloat>::iterator it = Positions.begin(); it != Positions.end(); ++ it, ++ i)
     * it *= Size[i %= 3];

   static float const CubeNormals[] =
   {
      0,  0, -1, // back face verts [0-3]
      0,  0, -1,
      0,  0, -1,
      0,  0, -1,
      
      0,  0,  1, // front face verts [4-7]
      0,  0,  1,
      0,  0,  1,
      0,  0,  1,
      
     -1,  0,  0, // left face verts [8-11]
     -1,  0,  0,
     -1,  0,  0,
     -1,  0,  0,
     
      1,  0,  0, // right face verts [12-15]
      1,  0,  0,
      1,  0,  0,
      1,  0,  0,
     
      0,  1,  0, // top face verts [16-19]
      0,  1,  0,
      0,  1,  0,
      0,  1,  0,
     
      0, -1,  0, // bottom face verts [20-23]
      0, -1,  0,
      0, -1,  0,
      0, -1,  0
   };
   Normals = std::vector<GLfloat>(CubeNormals, CubeNormals + 24 * 3);

   static GLfloat CubeTex[] = {
     1, 0, //back
     1, 1,
     .75, 1,
     .75, 0,

     .25, 0, //front
     .25, 1,
     .5, 1,
     .5, 0,

     0, 0, // left
     0, 1,
     .25, 1,
     .25, 0,

     .5, 0, //right
     .5, 1,
     .75, 1,
     .75, 0,

     0, 0, // top
     0, 0,
     0, 0,
     0, 0,

     0, 0, // bottom
     0, 0,
     0, 0,
     0, 0
   };
   TexCoords = std::vector<GLfloat>(CubeTex, CubeTex + 24 * 2);

   static unsigned short const CubeIndices[] =
   {
      0,  1,  2, // back face verts [0-3]
      2,  3,  0,
      
      4,  7,  6, // front face verts [4-7]
      6,  5,  4,
      
      8, 11, 10, // left face verts [8-11]
     10,  9,  8,
      
     12, 13, 14, // right face verts [12-15]
     14, 15, 12,
      
     16, 19, 18, // top face verts [16-19]
     18, 17, 16,
      
     20, 21, 22, // bottom face verts [20-23]
     22, 23, 20
   };
   Indices = std::vector<GLuint>(CubeIndices, CubeIndices + 12 * 3);

   return new Mesh(Positions, Normals, TexCoords, Indices);
}

Mesh * GeometryCreator::CreateSphere(glm::vec3 const & Radii, unsigned int const Slices, unsigned int const Stacks)
{
   std::vector<float> Positions, Normals, TexCoords;
   std::vector<unsigned int> Indices;

   // Make top and bottom points
   Positions.push_back(0.f); Positions.push_back(Radii.y); Positions.push_back(0.f);
   Normals.push_back(0.f); Normals.push_back(1.f); Normals.push_back(0.f);
   TexCoords.push_back(0.f); TexCoords.push_back( 1.f);

   Positions.push_back(0.f); Positions.push_back(-Radii.y); Positions.push_back(0.f);
   Normals.push_back(0.f); Normals.push_back(-1.f); Normals.push_back(0.f);
   TexCoords.push_back(0.f); TexCoords.push_back(0.f);

   for (unsigned int i = 1; i <= Stacks; ++ i)
   {
      float const AngleV = (float) i * 3.14159f / Stacks;
      for (unsigned int j = 0; j <= Slices; ++ j)
      {
         float const AngleH = (float) j * 2.f * 3.14159f / Slices;
         vec3 Radial = vec3(
            cos<float>(AngleH)*sin<float>(AngleV),
            cos<float>(AngleV),
            sin<float>(AngleH)*sin<float>(AngleV));
         unsigned int const Start = Positions.size() / 3;
         Positions.push_back(Radial.x*Radii.x);
         Positions.push_back(Radial.y*Radii.y);
         Positions.push_back(Radial.z*Radii.z);
         Normals.push_back(Radial.x); Normals.push_back(Radial.y); Normals.push_back(Radial.z);
         TexCoords.push_back(Radial.x * .5f + .5f);
         TexCoords.push_back(Radial.y * .5f + .5f);

         if (j)
         {
            if (i == 1)
            {
               Indices.push_back(Start);
               Indices.push_back(Start - 1);
               Indices.push_back(0);
            }
            else
            {
               Indices.push_back(Start);
               Indices.push_back(Start - 1);
               Indices.push_back(Start - Slices - 2);

               Indices.push_back(Start);
               Indices.push_back(Start - Slices - 2);
               Indices.push_back(Start - Slices - 1);
            }

            if (i == Stacks)
            {
               Indices.push_back(Start);
               Indices.push_back(1);
               Indices.push_back(Start - 1);
            }
         }
      }
   }

   return new Mesh(Positions, Normals, TexCoords, Indices);
}
