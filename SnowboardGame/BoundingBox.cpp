#include "BoundingBox.h"

BoundingBox::BoundingBox(glm::vec3 center, float width, float height): Bounding(center, width, height) {

}

bool BoundingBox::overlap(Bounding *that) {
   glm::vec3 thisBackLeftBottomCorner = glm::vec3(center.x - width / 2, center.y - height / 2, center.z - width / 2);
   glm::vec3 thisFrontRightTopCorner = glm::vec3(center.x + width / 2, center.y + height / 2, center.z + width / 2);

   glm::vec3 thatBackLeftBottomCorner = glm::vec3(that->getCenter().x - that->getWidth() / 2,
      that->getCenter().y - that->getHeight() / 2, that->getCenter().z - that->getWidth() / 2);
   glm::vec3 thatFrontRightTopCorner = glm::vec3(that->getCenter().x + that->getWidth() / 2,
      that->getCenter().y + that->getHeight() / 2, that->getCenter().z + that->getWidth() / 2);

   //if aMIN > bMAX OR bMIN > aMAX
      //disjoint
   //!(aMIN > bMAX OR bMIN > aMAX)

   bool c1 = thisBackLeftBottomCorner.x > thatFrontRightTopCorner.x &&
      thisBackLeftBottomCorner.y > thatFrontRightTopCorner.y &&
      thisBackLeftBottomCorner.z > thatFrontRightTopCorner.z;
   bool c2 = thatBackLeftBottomCorner.x > thisFrontRightTopCorner.x &&
      thatBackLeftBottomCorner.y > thisFrontRightTopCorner.y &&
      thatBackLeftBottomCorner.z > thisFrontRightTopCorner.z;

   /*if (c1)
      std::cout << "aMIN > bMAX: " << c1 << "\n";
   if (c2)
      std::cout << "bMIN > aMAX: " << c2 << "\n\n";*/

   return c1 || c2;
}
