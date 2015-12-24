#ifndef INCLUDED_EVALUATOR_H
#define INCLUDED_EVALUATOR_H

#include "Figure.h"

class Evaluator{
 public:
  virtual void eval() = 0;
  virtual void draw() = 0;
};

#endif
