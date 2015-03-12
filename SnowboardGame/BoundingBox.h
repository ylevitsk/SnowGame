#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Bounding.h"

class BoundingBox: public Bounding {
   public:
      BoundingBox(glm::vec3, float, float);
      ~BoundingBox(void);
      bool overlap(Bounding *);
};

#endif
