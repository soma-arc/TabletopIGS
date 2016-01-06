#ifndef INCLUDED_CIRCLE_H_
#define INCLUDED_CIRCLE_H_
#include <cmath>
#include "Complex.h"
#include "Figure.h"
#include "Point.h"

using namespace std;

class Circle : public Figure{
public:
  Circle();
  Circle(Point center, double r);
  Circle(Point a, Point b, Point c);
  Point getCenter();
  double getR();
  virtual void draw();
  void print(ostream *os);
  Point center;
  double r, z;
};

extern ostream &operator<<(ostream&, Circle&);
#endif
