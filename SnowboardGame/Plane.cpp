#include "Plane.h"
#include <stdio.h>


Plane::Plane(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3) {

	set3Points(v1, v2, v3);
}


Plane::Plane() {}

Plane::~Plane() {}


void Plane::set3Points(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3) {


	glm::vec3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal =glm::cross( aux2, aux1);

	normal = glm::normalize(normal);// .normalize();
	point = (v2);
	glm::vec3 temp = normal *point;
	d = -(temp.x + temp.y + temp.z);
}

void Plane::setNormalAndPoint(glm::vec3 &normal, glm::vec3 &point) {

	this->normal = (normal);
	glm::normalize(this->normal);//.normalize();
	glm::vec3 temp = this->normal *point;
	d = -(temp.x + temp.y + temp.z);
}

void Plane::setCoefficients(float a, float b, float c, float d) {

	// set the normal vector
	normal = glm::vec3(a, b, c);
	//compute the lenght of the vector
	float l = normal.length();
	// normalize the vector
	normal = glm::vec3(a / l, b / l, c / l);
	// and divide d by th length as well
	this->d = d / l;
}




float Plane::distance(glm::vec3 &p) {
	glm::vec3 temp = normal *p;
	return (d + temp.x + temp.y + temp.z);
}

void Plane::print() {

	//printf("Plane("); normal.print(); printf("# %f)", d);
}
