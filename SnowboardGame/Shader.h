#ifndef SHADER_H
#define SHADER_H

#include <GLFW/glfw3.h>
#include <map>

#define A_POS 2
#define A_NORM 3
#define A_TEX_COORD 4

#define U_PROJ_MX 11
#define U_VIEW_MX 12
#define U_MODEL_MX 13
#define U_CAM_POS 14

#define U_LIGHT_DIR 15
#define U_LIGHT_CLR 16

#define U_MAT_A_CLR 20
#define U_MAT_D_CLR 21
#define U_MAT_S_CLR 22
#define U_MAT_SHINE 23
#define U_TEX_UNIT 24
#define U_TEXT_COLOR 25

#define SHADER_NORM 0
#define SHADER_TEX 1
#define SHADER_FONT 2
#define MAX_SHADER_NUMBER 2

void initShaders(void);

void enableShader(int shaderNum);

void disableShader(int shaderNum);

class Shader
{
   private:
      std::map<int, GLint> handles; 
      GLint program;

      bool install(const char *vShaderName, const char *fShaderName);

   public:
      Shader(const char *vShaderName, const char *fShaderName);
      ~Shader(void) {}

      void enable(void);
      void disable(void);
      void setAttribute(const char *handleName, int key);
      void setUniform(const char *handleName, int key);
      GLint get(int key);
};

extern Shader *g_shader;


#endif
