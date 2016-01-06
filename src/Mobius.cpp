#ifndef INCLUDED_MOBIUS_CPP
#define INCLUDED_MOBIUS_CPP

#include "Mobius.h"

Point Mobius::circleInverse(Circle c, Point p){
  double len = Point::dist(c.center, p);
  return Point(((p.x.re() - c.center.x.re()) * c.r * c.r)/(len * len ) + c.center.x.re(),
               ((p.y.re() - c.center.y.re()) * c.r * c.r)/(len * len ) + c.center.y.re());
}

Circle Mobius::circleInverse(Circle inverseCircle, Circle otherCircle){
  Point a = Mobius::circleInverse(inverseCircle,
                                  Point(otherCircle.center.x + otherCircle.r,
                                        otherCircle.center.y));
  Point b = Mobius::circleInverse(inverseCircle,
                                  Point(otherCircle.center.x - otherCircle.r,
                                        otherCircle.center.y));
  Point c = Mobius::circleInverse(inverseCircle,
                                  Point(otherCircle.center.x,
                                        otherCircle.center.y + otherCircle.r));
  return Circle(a, b, c);
}

Line Mobius::circleInverse(Circle inverseCircle, Line line){
  Point a = Mobius::circleInverse(inverseCircle,
                                  Point(line.calcX(1.0),
                                        1.0));
  Point b = Mobius::circleInverse(inverseCircle,
                                  Point(line.calcX(1.5),
                                        1.5));
  return Line(a, b);
}

#endif
