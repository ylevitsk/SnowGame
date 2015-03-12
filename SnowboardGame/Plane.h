#ifndef _PLANE_
#define _PLANE_

#include "glm/gtc/type_ptr.hpp"

class Plane
{

public:

	glm::vec3 normal, point;
	float d;


	Plane(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3);
	Plane(void);
	~Plane();

	void set3Points(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3);
	void setNormalAndPoint(glm::vec3 &normal, glm::vec3 &point);
	void setCoefficients(float a, float b, float c, float d);
	float distance(glm::vec3 &p);

	void print();

};


#endif