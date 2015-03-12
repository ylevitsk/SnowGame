#ifndef _FRUSTUMG_
#define _FRUSTUMG_

#ifndef _PLANE_
#include "Plane.h"
#endif

class Plane;

class FrustumG
{
private:

	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};


public:

	static enum { OUTSIDE, INTERSECT, INSIDE };

	Plane pl[6];

	glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;



	FrustumG();
	~FrustumG();

	void setCamInternals(float angle, float ratio, float nearD, float farD);
	void setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u);
	int pointInFrustum(glm::vec3 &p);
	int sphereInFrustum(glm::vec3 &p, float raio);
	//int FrustumG::boxInFrustum(AABox &b);

	void drawPoints();
	void drawLines();
	void drawPlanes();
	void drawNormals();
};


#endif