#ifndef INCLUDED_LINE_CPP
#define INCLUDED_LINE_CPP

#include "Line.h"

Line::Line(){}

Line::Line(Point p1, Point p2){
  this->p1 = p1;
  this->p2 = p2;
  a = (p2.y - p1.y) * -1;
  b = p2.x - p1.x;
  c = (a * p1.x + b * p1.y) * -1 / p1.z;
  color = {1.f, 1.f, 1.f, 1.f};
}

void Line::setPoints(Point p1, Point p2){
  this->p1 = p1;
  this->p2 = p2;
  a = (p2.y - p1.y) * -1;
  b = p2.x - p1.x;
  c = (a * p1.x + b * p1.y) * -1 / p1.z;
}

void Line::draw(){
  int width = glutGet( GLUT_WINDOW_WIDTH );
  glBegin(GL_LINES);
  glVertex3f(width, calcY(width), z);
  glVertex3f(-width, calcY(-width), z);
  glEnd();
}

void Line::fill(){
  int width = glutGet( GLUT_WINDOW_WIDTH );
  glBegin(GL_LINES);
  glVertex3f(width, calcY(width), z);
  glVertex3f(-width, calcY(-width), z);
  glEnd();
}

double Line::calcX(double y){
  return -1 * (b.re() * y + c.re()) / a.re();
}

double Line::calcY(double x){
  return -1 * (a.re() * x + c.re()) / b.re();
}

#endif
