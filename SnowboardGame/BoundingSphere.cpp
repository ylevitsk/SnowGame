#include "BoundingSphere.h"

BoundingSphere::BoundingSphere(glm::vec3 center, float radius): Bounding(center, radius, radius) {

}

BoundingSphere::~BoundingSphere(void) {
   //TODO: fill in destructor
}

bool BoundingSphere::overlap(Bounding *that) {
   //find distance between centers
   float xd = that->getCenter().x - this->getCenter().x;
   float yd = that->getCenter().y - this->getCenter().y;
   float zd = that->getCenter().z - this->getCenter().z;
   float distance = sqrt(pow(xd, 2) + pow(yd, 2) + pow(zd, 2));

   //add up radii
   float radiiSum = this->width + that->getWidth();

   //if the distance between the centers is less than the radii sum, they intersect
   return distance < radiiSum;
}
