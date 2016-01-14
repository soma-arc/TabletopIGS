#ifndef INCLUDED_LINE_H
#define INCLUDED_LINE_H

#include "Figure.h"
#include "Point.h"
#include <iostream>
#include <vector>

using namespace std;

class Line : public Figure{
 public:
  Line();
  Line(Point p1, Point p2);
  void setPoints(Point p1, Point p2);
  virtual void draw();
  virtual void fill();
  double calcX(double y);
  double calcY(double x);

  Point p1, p2;
  Complex a, b, c;
  double z;
  std::vector<float> color;
};

#endif
