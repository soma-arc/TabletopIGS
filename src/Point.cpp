#include "Point.h"

Point::Point(){
  x = Complex::ZERO;
  y = Complex::ZERO;
  y = Complex::ONE;
}

Point::Point(double x, double y){
  this->x = Complex(x, 0.);
  this->y = Complex(y);
  this->z = Complex::ONE;
}

Point::Point(Complex x, Complex y){
  this->x = x;
  this->y = y;
  this->z = Complex::ONE;
}

void Point::draw(){
  fillCircle(DISPLAY_RADIUS, x.re(), y.re(), 0.);
}

void Point::setPosition(double x, double y){
  this->x = x;
  this->y = y;
}

void Point::setPosition(Complex x, Complex y){
  this->x = x;
  this->y = y;
}

double Point::dist(Point a, Point b){
  return std::sqrt((a.x.re() - b.x.re()) * (a.x.re() - b.x.re()) +
                   (a.y.re() - b.y.re()) * (a.y.re() - b.y.re()));
}

double Point::DISPLAY_RADIUS = 16;
