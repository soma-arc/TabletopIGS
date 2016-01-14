#ifndef INCLUDED_MOBIUS_H
#define INCLUDED_MOBIUS_H

#include "Point.h"
#include "Circle.h"
#include "Line.h"

class Mobius{
 public:
  Mobius();
  static Point circleInverse(Circle c, Point p);
  static Circle circleInverse(Circle inverseCircle, Circle otherCircle);
  static Circle circleInverse(Circle inverseCircle, Line line);
};

#endif
