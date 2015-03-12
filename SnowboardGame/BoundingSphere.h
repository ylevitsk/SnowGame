#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "Bounding.h"

class BoundingSphere: public Bounding {
   public:
      BoundingSphere(glm::vec3, float);
      ~BoundingSphere(void);
      bool overlap(Bounding *);
};

#endif
