#include "Figure.h"

void Figure::fillCircle(double radius, double x, double y){
  for (float th1 = 0.0;  th1 <= 360.0;  th1 = th1 + 1.0){
    float th2 = th1 + 10.0;
    float th1_rad = th1 / 180.0 * M_PI;
    float th2_rad = th2 / 180.0 * M_PI;

    float x1 = radius * cos(th1_rad);
    float y1 = radius * sin(th1_rad);
    float x2 = radius * cos(th2_rad);
    float y2 = radius * sin(th2_rad);

    glBegin(GL_TRIANGLES);
    glVertex2f( x, y );
    glVertex2f( x1+x, y1+y );
    glVertex2f( x2+x, y2+y );
    glEnd();
  }
}
