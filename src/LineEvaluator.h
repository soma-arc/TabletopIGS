#ifndef INCLUDED_LINE_EVALUATOR_H
#define INCLUDED_LINE_EVALUATOR_H

#include "Evaluator.h"
#include "Point.h"
#include "Line.h"
#include "Figure.h"

class LineEvaluator : public Evaluator{
 public:
  LineEvaluator();
  LineEvaluator(Point& p1, Point& p2);
  virtual void eval();
  virtual void draw();
  Point& p1 = Point();
  Point& p2 = Point();
  Line line;
};

#endif
