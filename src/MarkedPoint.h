#ifndef INCLUDED_MARKED_POINT_H
#define INCLUDED_MARKED_POINT_H

#include "Point.h"
#include "ReferenceMarker.h"
#include <vector>

using namespace std;

class MarkedPoint : public Point, public ReferenceMarker{
 public:
  MarkedPoint();
  MarkedPoint(const char* markerPath, double markerWidth);
  virtual void draw();
  void setDistanceFromOrigin(ReferenceMarker origin);
  void setRotationFromOrigin(ReferenceMarker origin);

  vector<double> distanceFromOrigin;
  vector<double> rotationFromOrigin;
};

#endif
