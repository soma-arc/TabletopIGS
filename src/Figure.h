#ifndef INCLUDED_FIGURE_H
#define INCLUDED_FIGURE_H

#define _USE_MATH_DEFINES
#include <GL/freeglut.h>
#include <cmath>

class Figure{
 public:
  virtual void draw() = 0;
  void drawCircle(double radius, double x, double y, double z);
  void fillCircle(double radius, double x, double y, double z);
};

#endif
