#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include "imageloader.h"
#include "vec3f.h"
#include "GLSL_helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
//#include <glfw3.h>
#include "mesh.h"
#include "irrKlang.h"

using namespace std;
using namespace irrklang;

//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		int ** rd; //probability
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;
			
			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}

			rd = new int*[l];
			for (int i = 0; i < l; i++) {
				rd[i] = new int[w];
			}
			
			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}
			
			computedNormals = false;
		}
		
		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;

			for (int i = 0; i < l; i++) {
				delete[] rd[i];
			}
			delete[] rd;
			
			for(int i = 0; i < l; i++) {
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
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}
			
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);
					
					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < w - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}
					
					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}
					
					normals2[z][x] = sum;
				}
			}
			
			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];
					
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
					
					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}
			
			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;
			
			computedNormals = true;
		}
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};


//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
			t->setRandom(x, y);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}

float _angle = 60.0f;
Terrain* _terrain, *_treeTerrain;

//flag and ID to toggle on and off the shader
int shade = 1;
int ShadeProg;

//Handles to the shader data
GLint h_aPosition, h_aNormal;
GLint h_uColor;
GLint h_uModelMatrix;
GLint h_uViewMatrix;
GLint h_uProjMatrix;
static float g_width, g_height;

GLint h_uLightPos;
GLint h_uLightColor;
GLint h_uMatAmb, h_uMatDif, h_uMatSpec, h_uMatShine;

GLuint GrndBuffObj, GIndxBuffObj, GIndxBuffObj2, GNBuffObj;
GLuint vBallibo = 0, vBallpositions = 0, vBalluvs, vBallnormals;
int vBallvertexCount;

float alpha = 0, beta = atan(1) * 2;
//float alpha = -1.32, beta = 1.44;
float startX, startY, lookX = 0, lookY = 0, lookZ = 0;
//float startX, startY, lookX = -1.384, lookY = 70.5, lookZ = -10.233;
glm::vec3 lookAt = lookAt = glm::vec3(cosf(alpha) * cosf(beta), sinf(alpha), cosf(alpha) * cosf(atan(1) * 2 - beta)), eye = glm::vec3(lookX, lookY, lookZ), up = glm::vec3(0, 1, 0);
float g_MtransX = 02, g_MtransY = 0, g_MtransZ = 0, angle = 0;
glm::vec3 axis = glm::vec3(1.0f, 0.0f, 0);
float newScaleY = 1.0, newScaleX = 1.0, newScaleZ = 1.0;

int TiboLen;

/* projection matrix  - do not change */
void SetProjectionMatrix() {
	glm::mat4 Projection = glm::perspective(80.0f, (float)g_width / g_height, 0.1f, 100.f);
	safe_glUniformMatrix4fv(h_uProjMatrix, glm::value_ptr(Projection));
}
/* camera controls - do not change */
void SetView() {
	/*glm::mat4 Trans = glm::translate( glm::mat4(1.0f), glm::vec3(0.0f, 0, g_trans));
	glm::mat4 RotateX = glm::rotate( Trans, g_angle, glm::vec3(0.0f, 1, 0));
	safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(RotateX));*/
	//lookAt = glm::vec3(cosf(alpha) * cosf(beta), sinf(alpha), cosf(alpha) * cosf(atan(1) * 2 - beta));
	glm::mat4 view = glm::lookAt(eye, lookAt, up);
	cout << "eye " << eye.r << " " << eye.g << " " << eye.b << endl;
	safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(view));
}
/* set model transforms to the identity*/
void SetModelI() {
	glm::mat4 tmp = glm::mat4(1.0f);
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(tmp));
}
/* model transforms - change these to create a shape with boxes*/
void SetModel() {
	glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(g_MtransX, g_MtransY, g_MtransZ));
	glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), angle, axis);
	glm::mat4 ScaleY = glm::scale(glm::mat4(1.0f), glm::vec3(newScaleX, newScaleY, newScaleZ));
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(Trans*ScaleY * RotateY));
}
int InstallShader(const GLchar *vShaderName, const GLchar *fShaderName) {
		GLuint VS; //handles to shader object
		GLuint FS; //handles to frag shader object
		GLint vCompiled, fCompiled, linked; //status of shader

		VS = glCreateShader(GL_VERTEX_SHADER);
		FS = glCreateShader(GL_FRAGMENT_SHADER);

		//load the source
		glShaderSource(VS, 1, &vShaderName, NULL);
		glShaderSource(FS, 1, &fShaderName, NULL);

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

		if (!vCompiled || !fCompiled) {
			printf("Error compiling either shader %s or %s", vShaderName, fShaderName);
			return 0;
		}

		//create a program object and attach the compiled shader
		ShadeProg = glCreateProgram();
		glAttachShader(ShadeProg, VS);
		glAttachShader(ShadeProg, FS);

		glLinkProgram(ShadeProg);
		/* check shader status requires helper functions */
		printOpenGLError();
		glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
		printProgramInfoLog(ShadeProg);

		glUseProgram(ShadeProg);

		/* get handles to attribute and uniform data in shader */
		h_aPosition = safe_glGetAttribLocation(ShadeProg, "aPosition");
		h_uColor = safe_glGetUniformLocation(ShadeProg, "uColor");
		h_uProjMatrix = safe_glGetUniformLocation(ShadeProg, "uProjMatrix");
		h_uViewMatrix = safe_glGetUniformLocation(ShadeProg, "uViewMatrix");
		h_uModelMatrix = safe_glGetUniformLocation(ShadeProg, "uModelMatrix");
		h_uLightPos = safe_glGetUniformLocation(ShadeProg, "uLightPos");
		h_uLightColor = safe_glGetUniformLocation(ShadeProg, "uLightColor");
		h_uMatAmb = safe_glGetUniformLocation(ShadeProg, "uMat.aColor");
		h_uMatDif = safe_glGetUniformLocation(ShadeProg, "uMat.dColor");
		h_uMatSpec = safe_glGetUniformLocation(ShadeProg, "uMat.sColor");
		h_uMatShine = safe_glGetUniformLocation(ShadeProg, "uMat.shine");
		h_aNormal = safe_glGetAttribLocation(ShadeProg, "aNormal");
		//h_uFlag = safe_glGetUniformLocation(ShadeProg, "uFlag");

		printf("sucessfully installed shader %d\n", ShadeProg);
		return 1;
	}

void cleanup() {
	delete _terrain;
}

//static void Keyboard(GLFWwindow * window, int key, int scancode, int action, int mods) {
void Keyboard(unsigned char key, int x, int y) {
	glm::vec3 w = glm::normalize(lookAt - eye) * -1.0f;
	glm::vec3 p = glm::cross(up, w);
	glm::vec3 u = glm::normalize(p);
	switch (key) {
		case 'w':
			eye -= .2f * w;
			lookAt -= .2f * w;
			break;
		case 's':
			eye += .2f * w;
			lookAt += .2f * w;
			break;
		case  'a':
			eye -= .2f * u;
			lookAt -= .2f * u;
			break;
		case 'd':
			eye += .2f * u;
			lookAt += .2f * u;
			break;
		//case GLFW_KEY_ESCAPE:
			//glfwSetWindowShouldClose(window, GL_TRUE);
			//break;
	}
	glutPostRedisplay();
}

void initialize() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	Model model;
	std::ifstream modelFile("PR10m.obj");
	//std::ifstream modelFile("Volleyball.obj");
	model.load(modelFile);
	vBallvertexCount = model.meshes()[0].makeVBO(&vBallibo, &vBallpositions, &vBalluvs, &vBallnormals);

}

void Resize(int w, int h) {
//void glfwWindowResize(GLFWwindow *window, int w, int h){
	g_width = (float)w;
	g_height = (float)h;
	glViewport(0, 0, (GLsizei)(w), (GLsizei)(h));
	
	glMatrixMode(GL_PROJECTION);
	//gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}
void SetMaterial(int i) {
	glUseProgram(ShadeProg);
	switch (i) {
	case 0: //ground plane
		glUniform3f(h_uMatAmb, 0.4f, 0.4f, 0.4f);
		glUniform3f(h_uMatDif, 0.0, 0.8, 0.05);
		glUniform3f(h_uMatSpec, 0.0, 0.0, 0.0);
		glUniform1f(h_uMatShine, 100.0);
		break;
	case 1: //nothing right now
		glUniform3f(h_uMatAmb, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDif, 0.91, 0.782, 0.82);
		glUniform3f(h_uMatSpec, 1.0, 0.913, 0.8);
		glUniform1f(h_uMatShine, 200.0);
		break;
	case 2: //active dinosaur
		glUniform3f(h_uMatAmb, 0.2, 0.2, 0.2);
		glUniform3f(h_uMatDif, 0.6, 0.2, 1.0);
		glUniform3f(h_uMatSpec, 0.1, 0.1, 0.1);
		glUniform1f(h_uMatShine, 2.0);
		break;
	case 3: //inactive dinosaur
		glUniform3f(h_uMatAmb, 0.13, 0.13, 0.14);
		glUniform3f(h_uMatDif, 0.3, 0.3, 0.4);
		glUniform3f(h_uMatSpec, 0.3, 0.3, 0.4);
		glUniform1f(h_uMatShine, 4.0);
		break;
	}
}
static void initGround() {
	std::vector<float> GrndPos, GrndNorm;
	std::vector<unsigned short> idx, idx2;
	int j = 0;
	for (int z = 0; z < _terrain->length()-1; z++) {
		for (int x = 0; x < _terrain->width(); x++) {
			//if (z == 99 && x == 2)
				//break;
			Vec3f normal = _terrain->getNormal(x, z);
			Vec3f normal2 = _terrain->getNormal(x, z + 1);

			GrndPos.push_back(x);
			GrndPos.push_back(_terrain->getHeight(x, z));
			GrndPos.push_back(z);
			GrndPos.push_back(x);
			GrndPos.push_back(_terrain->getHeight(x, z + 1));
			GrndPos.push_back(z + 1);

			GrndNorm.push_back(normal[0]);
			GrndNorm.push_back(normal[1]);
			GrndNorm.push_back(normal[2]);
			GrndNorm.push_back(normal2[0]);
			GrndNorm.push_back(normal2[1]);
			GrndNorm.push_back(normal2[2]);

			if (x == 0 || x == 20 || x == 40 )
				idx.push_back(65000);
			
			idx.push_back(j++);
			idx.push_back(j++);
			idx.push_back(j++);
			idx.push_back(j++);
			idx.push_back(j++);
			idx.push_back(j++);
		}
	}

	TiboLen = idx.size();
	glGenBuffers(1, &GrndBuffObj);
	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* GrndPos.size(), &GrndPos[0], GL_STATIC_DRAW);

	glGenBuffers(1, &GIndxBuffObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)* idx.size(), &idx[0], GL_STATIC_DRAW);

	glGenBuffers(1, &GNBuffObj);
	glBindBuffer(GL_ARRAY_BUFFER, GNBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* GrndNorm.size(), &GrndNorm[0], GL_STATIC_DRAW);
	
}
/* Initialize the geometry */
void InitGeom() {
	initGround();
}

void Draw(){
//void Draw(GLFWwindow * window) {	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Start our shader
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(65000);
	glUseProgram(ShadeProg);

	// set up the projection and camera - do not change 
	SetProjectionMatrix();
	SetView();

	// TO DO set up the light's position and color
	glUniform3f(h_uLightPos, -0.5f, 0.8f, 0.1f);
	glUniform3f(h_uLightColor, 0.6f, 0.6f, 0.6f);

	//actually draw the ground plane data
	SetModelI();
	SetMaterial(0);

	//actually draw the grond plane 
	safe_glEnableVertexAttribArray(h_aPosition);

	// bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
	safe_glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	safe_glEnableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, GNBuffObj);
	safe_glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// bind ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);

	// set the current color  - change this 
	glUniform3f(h_uColor, 0.3f, 0.9f, 0.0f);

	g_MtransX = 0;// (-(float)(_terrain->width() - 1) / 2);
	g_MtransY = 4;
	g_MtransZ = 0;// -(float)(_terrain->length() - 1) / 2;
	//angle = 30;
	//axis = glm::vec3(1.0f, 0.0f, 0.0f);
	
	//float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	newScaleX = newScaleZ =2;
	newScaleY = 1;
	angle = 0;
	SetModel();
	// draw - note the difference for using an ibo
	glDrawElements(GL_TRIANGLE_STRIP, TiboLen, GL_UNSIGNED_SHORT, 0);

	// Disable the attributes used by our shader
	safe_glDisableVertexAttribArray(h_aPosition);
	safe_glDisableVertexAttribArray(h_aNormal);

	safe_glEnableVertexAttribArray(h_aPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vBallpositions);
	glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	safe_glEnableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vBallnormals);
	glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// If you used the IBO (generally a good idea):
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vBallibo);
	glUniform3f(h_uColor, 0.5, 0.0, 0.5);
	
	for (int z = 0; z < _terrain->length(); z++){
		for (int x = 0; x < _terrain->width(); x++){
			if (_terrain->getRand(x, z) == 1){
				newScaleX = .1;
				newScaleY = .1;
				newScaleZ = .1;
				angle = -90;
				axis = glm::vec3(1, 0, 0);
				g_MtransX = x * 2;
				g_MtransY = _terrain->getHeight(x, z) + 6;
				g_MtransZ = z *2;
				//angle = Accumulator * 35.f;
				SetMaterial(3);
				SetModel();
				glDrawElements(GL_TRIANGLES, vBallvertexCount, GL_UNSIGNED_INT, 0);

			}
		}
	}

	safe_glDisableVertexAttribArray(h_aPosition);
	safe_glDisableVertexAttribArray(h_aNormal);


	//Disable the shader
	glUseProgram(0);
	//glfwSwapBuffers(window);
	glutSwapBuffers();
}
float p2wx(int in_x) {
	//fill in with the correct return value
	float l = -1, r = 1;

	if (g_width > g_height) {
		l = -g_width / g_height;
		r = g_width / g_height;
	}

	float c = (-g_width + 1) / (l - r);
	float d = c * (-l);

	return ((float)in_x - d) / c;
}

float p2wy(int in_y) {
	//flip glut y
	in_y = g_height - in_y;

	//fill in with the correct return value
	float l = -1, r = 1, t = 1, b = -1;

	if (g_width < g_height) {
		b = -g_height / g_width;
		t = g_height = g_width;
	}

	float e = (-g_height + 1) / (b - t);
	float f = e * (-b);

	return ((float)in_y - e) / e;
}
void Mouse(int button, int state, int x, int y) {
//void Mouse(GLFWwindow *window, int button, int action, int mods){
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN){
			startX = p2wx(x);
			startY = p2wy(y);
		}
	}
	/*double screenX, screenY;
	if (action == GLFW_PRESS){
		glfwGetCursorPos(window, &screenX, &screenY);
		startX = p2wx(screenX);
		startY = p2wy(screenY);
	}*/
}

void Motion(int x, int y){
//void Motion(GLFWwindow *window, double x, double y){
	beta -= (p2wx(x) - startX) * p2wx(g_width) / atan(1) * 4;
	alpha -= (p2wy(y) - startY)*  p2wy(g_height) / atan(1) * 4;
	if (alpha >= 1.4)
		alpha = 1.4;
	if (alpha <= -1.4)
		alpha = -1.4;
	cout << "beta " << beta << endl;
	cout << "alpha " << alpha << endl;
	startY = p2wy(y);
	startX = p2wx(x);
	lookAt = glm::vec3(eye.x + cosf(alpha) * cosf(beta), eye.y + sinf(alpha), eye.z + cosf(alpha) * cosf(atan(1) * 2 - beta));
	glutPostRedisplay();
}

void update(int value) {
	_angle += 1.0f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}
// GLFW callback for any detected errors.
void glfwError(int error, const char *description)
{
	std::cerr << description << std::endl;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(20, 20);
	glutInitWindowSize(600, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Game");
#ifdef _WIN32
	GLenum err = glewInit();
	if (GLEW_OK != err){
		std::cerr << "Error initializing glew! " << glewGetErrorString(err) << std::endl;
		return 1;
	}
#endif
	getGLversion();
	//install the shader
	if (!InstallShader(textFileRead((char *)"Lab4_vert.glsl"), textFileRead((char *)"Lab4_frag.glsl"))) {
		printf("Error installing shader!\n");
		return 0;
	}
	ISoundEngine * engine = createIrrKlangDevice();
	if (!engine)
		return 0;
	initialize();
	_terrain = loadTerrain("heightmap.bmp", 20);
	engine->play2D("music.mp3", true);
	//_treeTerrain = loadTerrain("heightmap2.bmp", 20);
	InitGeom();
	glutDisplayFunc(Draw);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Resize);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(25, update, 0);
	glutMainLoop();
	return 0;

	/*
	GLFWwindow * window;
	glfwSetErrorCallback(glfwError);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(600, 600, "Game", NULL, NULL);
	if (!window){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowPos(window, 80, 80);

	//glfwSetWindowSizeCallback(window, glfwWindowResize);
	//glfwSetKeyCallback(window, Keyboard);
	//glfwSetMouseButtonCallback(window, Mouse);
	//glfwSetCursorPosCallback(window, Motion);
#ifdef _WIN32
	GLenum err = glewInit();
	if (GLEW_OK != err){
		std::cerr << "Error initializing glew! " << glewGetErrorString(err) << std::endl;
		return 1;
	}
#endif
	getGLversion();
	//install the shader
	if (!InstallShader(textFileRead((char *)"Lab4_vert.glsl"), textFileRead((char *)"Lab4_frag.glsl"))) {
		printf("Error installing shader!\n");
		return 0;
	}
	initialize();
	_terrain = loadTerrain("heightmap2.bmp", 20);
	InitGeom();
	while (!glfwWindowShouldClose(window))	{
		Draw(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);	*/
}









