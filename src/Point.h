#include "Figure.h"
#include "Complex.h"
#include <cmath>
#include <GL/freeglut.h>

class Point : public Figure{
 public:
  Point();
  Point(double x, double y);
  Point(Complex x, Complex y);
  virtual void draw();
  void setPosition(double x, double y);
  void setPosition(Complex x, Complex y);

  static double dist(Point a, Point b);
  static double DISPLAY_RADIUS;

  Complex x, y, z;
};
