#include "LineEvaluator.h"

LineEvaluator::LineEvaluator(){}

LineEvaluator::LineEvaluator(Point& p1, Point& p2){
  this->p1 = p1;
  this->p2 = p2;
  this->line = Line(p1, p2);
}

void LineEvaluator::eval(){
  this->line.setPoints(p1, p2);
}

void LineEvaluator::draw(){
  line.draw();
}
