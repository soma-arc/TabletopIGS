#include "MarkedPoint.h"

MarkedPoint::MarkedPoint(const char* markerPath, double markerWidth){
  this->patternId = arLoadPatt(markerPath);
  this->markerWidth = markerWidth;
  this->visible = false;
  this->x = Complex::ZERO;
  this->y = Complex::ZERO;
  this->z = Complex::ONE;
}

void MarkedPoint::draw(){
  if(visible == false) return;
  fillCircle(DISPLAY_RADIUS, x.re(), y.re());
}
