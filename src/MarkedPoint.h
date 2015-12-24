#ifndef INCLUDED_MARKED_POINT_H
#define INCLUDED_MARKED_POINT_H

#include "Point.h"
#include "ReferenceMarker.h"

class MarkedPoint : public Point, public ReferenceMarker{
 public:
  MarkedPoint(const char* markerPath, double markerWidth);
  virtual void draw();
};

#endif
