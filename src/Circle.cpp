#ifndef INCLUDED_Circle_CPP_
#define INCLUDED_Circle_CPP_

#include "Circle.h"

Circle::Circle(){
}

Circle::Circle(Point center, double r){
  this->center = center;
  this->r = r;
  this->z = 0.;
  color = {1.f, 1.f, 1.f, 1.f};
}

Circle::Circle(Point a, Point b, Point c){
  double lA = Point::dist(b, c);
  double lB = Point::dist(a, c);
  double lC = Point::dist(a, b);
  double coefA = lA * lA * (lB * lB + lC * lC - lA * lA);
  double coefB = lB * lB * (lA * lA + lC * lC - lB * lB);
  double coefC = lC * lC * (lA * lA + lB * lB - lC * lC);
  double denom = coefA + coefB + coefC;
  this->center = Point((coefA * a.x.re() + coefB * b.x.re() + coefC * c.x.re()) / denom,
                       (coefA * a.y.re() + coefB * b.y.re() + coefC * c.y.re()) / denom);
  this->r = Point::dist(center, a);
  this->z = 0.;
color = {1.f, 1.f, 1.f, 1.f};
}

void Circle::draw(){
  drawCircle(r, center.x.re(), center.y.re(), z);
}

void Circle::fill(){
  fillCircle(r, center.x.re(), center.y.re(), z);
}

void Circle::print(ostream *os){
  *os << "r = " << r << " center = " << center << " }";
}

ostream &operator<<(ostream &os,  Circle &c){
  c.print(&os);
  return os;
}

#endif
