#include <iostream>

#include "third_party/GLSL_helper.h"
#include "Shader.h"

/** A straight-up global pointer. Points to the shader currently in use. */
Shader *g_shader;

/** List of pointers to all installed shaders. */
Shader *shaderList[MAX_SHADER_NUMBER + 1];

/**
 * Instantiates a file-static list of Shader objects. Each shader object
 * represents an installed GLSL shader program
 */
void initShaders(void) {
   /* TODO(evan) Free these Shader objects */

   shaderList[SHADER_NORM] = new Shader("vert.glsl", "frag.glsl");
   enableShader(SHADER_NORM);
   // Attributes
   g_shader->setAttribute("aPosition", A_POS);
   g_shader->setAttribute("aNormal", A_NORM);
   // Uniforms
   g_shader->setUniform("uProjMatrix", U_PROJ_MX);
   g_shader->setUniform("uViewMatrix", U_VIEW_MX);
   g_shader->setUniform("uModelMatrix", U_MODEL_MX);
   g_shader->setUniform("uLightDir", U_LIGHT_DIR);
   g_shader->setUniform("uLightColor", U_LIGHT_CLR);
   //setUniform("uCamPos", U_CAM_POS);
   // Uniform Material struct data
   g_shader->setUniform("uMat.aColor", U_MAT_A_CLR);
   g_shader->setUniform("uMat.dColor", U_MAT_D_CLR);
   g_shader->setUniform("uMat.sColor", U_MAT_S_CLR);
   g_shader->setUniform("uMat.shine", U_MAT_SHINE);

   shaderList[SHADER_TEX] = new Shader("vertTex.glsl", "fragTex.glsl");
   enableShader(SHADER_TEX);
   // Attributes
   g_shader->setAttribute("aPosition", A_POS);
   g_shader->setAttribute("aNormal", A_NORM);
   g_shader->setAttribute("aTexCoord", A_TEX_COORD);
   // Uniforms
   g_shader->setUniform("uProjMatrix", U_PROJ_MX);
   g_shader->setUniform("uViewMatrix", U_VIEW_MX);
   g_shader->setUniform("uModelMatrix", U_MODEL_MX);
   g_shader->setUniform("uLightDir", U_LIGHT_DIR);
   g_shader->setUniform("uLightColor", U_LIGHT_CLR);
   //setUniform("uCamPos", U_CAM_POS);
   // Uniform Material struct data
   g_shader->setUniform("uMat.aColor", U_MAT_A_CLR);
   g_shader->setUniform("uMat.dColor", U_MAT_D_CLR);
   g_shader->setUniform("uMat.sColor", U_MAT_S_CLR);
   g_shader->setUniform("uMat.shine", U_MAT_SHINE);
   g_shader->setUniform("uTexUnit", U_TEX_UNIT);

   shaderList[SHADER_FONT] = new Shader("vertFont.glsl", "fragFont.glsl");
   enableShader(SHADER_FONT);
   // Attributes
   g_shader->setAttribute("aTexCoord", A_TEX_COORD);
   // Uniforms
   g_shader->setUniform("uTexUnit", U_TEX_UNIT);
   g_shader->setUniform("uTextColor", U_TEXT_COLOR);
}

/**
 * Enables the shader associated with |shaderNum| and updates the global
 * shader pointer, |g_shader|, to match.
 */
void enableShader(int shaderNum) {
   g_shader = shaderList[shaderNum];
   g_shader->enable();
}

/**
 * Disables the shader associated with |shaderNum|.
 */
void disableShader(int shaderNum) {
   shaderList[shaderNum]->disable();
}

/**
 * Constructs a new Shader object, which represents a shader program consisting
 * of vertex shader |vShaderName| and fragment shader |fShaderName|.
 */
Shader::Shader(const char *vShaderName, const char *fShaderName)
{
   if (!this->install(vShaderName, fShaderName)) {
      std::cout << "Error installing shader!" << std::endl;
      exit(EXIT_FAILURE);
   }
}

void Shader::enable(void)
{
   glUseProgram(this->program);
}

void Shader::disable(void)
{
   glUseProgram(0);
}

void Shader::setAttribute(const char *handleName, int key)
{
   this->handles[key] = safe_glGetAttribLocation(this->program, handleName);
}

void Shader::setUniform(const char *handleName, int key)
{
   this->handles[key] = safe_glGetUniformLocation(this->program, handleName);
}

GLint Shader::get(int key)
{
   return this->handles[key];
}

bool Shader::install(const char *vShaderName, const char *fShaderName)
{
   GLuint VS; //handle to vertex shader object
   GLuint FS; //handle to fragment shader object
   GLint vCompiled, fCompiled, linked; //status of shader
   const char *vSource, *fSource;

   VS = glCreateShader(GL_VERTEX_SHADER);
   FS = glCreateShader(GL_FRAGMENT_SHADER);

   //load the source
   vSource = textFileRead(vShaderName);
   fSource = textFileRead(fShaderName);
   glShaderSource(VS, 1, &vSource, NULL);
   glShaderSource(FS, 1, &fSource, NULL);

   //compile shader and print log
   glCompileShader(VS);
   /* check shader status requires helper functions */
   printOpenGLError();
   glGetShaderiv(VS, GL_COMPILE_STATUS, &vCompiled);
   printShaderInfoLog(VS);

   //compile shader and print log
   glCompileShader(FS);
   /* check shader status requires helper functions */
   printOpenGLError();
   glGetShaderiv(FS, GL_COMPILE_STATUS, &fCompiled);
   printShaderInfoLog(FS);

   if (!vCompiled)
      std::cout << "Error compiling shader " << vShaderName << std::endl;
   if(!fCompiled)
      std::cout << "Error compiling shader " << fShaderName << std::endl;
   if (!vCompiled || !fCompiled)
      return false;

   //create a program object and attach the compiled shader
   this->program = glCreateProgram();
   glAttachShader(this->program, VS);
   glAttachShader(this->program, FS);

   glLinkProgram(this->program);
   /* check shader status requires helper functions */
   printOpenGLError();
   glGetProgramiv(this->program, GL_LINK_STATUS, &linked);
   printProgramInfoLog(this->program);

   return true;
}
