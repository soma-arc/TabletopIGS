#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <Windows.h>
#include <gl/freeglut.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <AR/ar.h>
#include <AR/video.h>
#include <AR/gsub_lite.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "Complex.h"
#include "Point.h"
#include "ReferenceMarker.h"
#include "MarkedPoint.h"
#include "Line.h"
#include "Circle.h"
#include "Mobius.h"
#include <string>

ARParam arCameraParam;
ARGL_CONTEXT_SETTINGS_REF arSettings;
const int width = 800;
const int height = 600;
const int threshold = 100;
int crossMarkerId;
int pMarkerId;
ReferenceMarker origin("resources/origin.pat", 40.0);
unordered_map<int, MarkedPoint> idMarkedPointMap;


using namespace std;

bool setupCamera(char videoConfigFile[], char cameraParamFile[]) {

  if (arVideoOpen(videoConfigFile) < 0) {
    std::cout << "Camera not found \n";
    return false;
  }
  if (arVideoCapStart() != 0) {
    std::cout << "Video capture start failed \n";
    return false;
  }

  ARParam wparam;
  if (arParamLoad(cameraParamFile, 1, &wparam) < 0) {
    std::cout << "camera parameter file load failed \n";
    return false;
  }

  int xsize, ysize;
  if (arVideoInqSize(&xsize, &ysize) < 0) {
    std::cout << "get image size - error \n";
    return false;
  }

  arParamChangeSize(&wparam, xsize, ysize, &arCameraParam);
  arInitCparam(&arCameraParam);

  return true;
}

void drawString(string str, float x0, float y0, float z0){
  glRasterPos3f(x0, y0, z0);
  int size = (int)str.size();
  for(int i = 0; i < size; ++i){
    char ic = str[i];
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ic);
  }
}

void setGlColorHSVA(float h, float s, float v, float a){
	float c = v * s;
	h = fmod((h * 6.0), 6.0);
	float x = c * (1.0 - abs(fmod(h, 2.0) - 1.0));
	float r, g, b;

	if (0.0 <= h && h < 1.0) {
                r = c;
                g = x;
                b = 0.;
	} else if (1.0 <= h && h < 2.0) {
                r = x;
                g = c;
                b = 0.;
	} else if (2.0 <= h && h < 3.0) {
                r = 0.;
                g = c;
                b = x;
	} else if (3.0 <= h && h < 4.0) {
                r = 0.;
                g = x;
                b = c;
	} else if (4.0 <= h && h < 5.0) {
                r = x;
                g = 0.;
                b = c;
	} else if (5.0 <= h && h < 6.0) {
                r = c;
                g = 0.;
                b = x;
	} else {
                r = 0.;
                g = 0.;
                b = 0.;
	}

        glColor4f(r + v - c,
                  g + v - c,
                  c + v - c,
                  a);
}

void mySetLight() {
  GLfloat lightDiffuse[] = { 0.9, 0.9, 0.9, 1.0 };
  GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat lightAmbient[] = { 0.3, 0.3, 0.3, 0.1 };
  GLfloat lightPosition[] = { 0.0, 0.0, 0.0, 1.0 };

  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHT0);
}

void drawOrigin(){
  glBegin(GL_LINES);
  glVertex3d(-10., 0, 0.);
  glVertex3d(10., 0, 0.);
  glVertex3d(0, 10., 0.);
  glVertex3d(0, -10., 0.);
  glEnd();
}

vector<Line> getLines(vector<MarkedPoint> linePoints){
  vector<Line> lines;
  for_each(linePoints.begin(), linePoints.end(),
           [&](MarkedPoint p){
             vector<double> distance = p.distanceFromOrigin;
             vector<double> rotation = p.rotationFromOrigin;
             Point p1(distance[0], distance[1]);
             Line l(p1,
                    Point(p1.x.re() + 20 * cos(rotation[2] + M_PI_2),
                          p1.y.re() + 20 * sin(rotation[2] + M_PI_2)));
             setGlColorHSVA(0., 0., 0., 1.);
             l.z = distance[2];
             lines.push_back(l);
           });
  return lines;
}

vector<Circle> getCircles(vector<MarkedPoint> circlePoints){
  vector<Circle> circles;
  float hue = 0.0;
  for_each(circlePoints.begin(), circlePoints.end(),
           [&](MarkedPoint p){
             vector<double> distance = p.distanceFromOrigin;
             vector<double> rotation = p.rotationFromOrigin;
             Circle c(Point(distance[0], distance[1]), 30 + 15 * sin(rotation[2]));
             c.z = distance[2];
             c.color[0] = hue;
             circles.push_back(c);
             hue += 0.2;
           });
  return circles;
}

void drawInversedCircles(vector<Circle> circles){
  for_each(circles.begin(),
           circles.end(),
           [&](Circle c){
             for_each(circles.begin(),
                      circles.end(),
                      [&](Circle c2){
                        if(c.center.x.re() == c2.center.x.re() &&
                           c.center.y.re() == c2.center.y.re()) return;
                        Circle nc = Mobius::circleInverse(c, c2);
                        nc.z = c.z;
                        setGlColorHSVA(c2.color[0], 1., 1., 1.);
                        nc.fill();
                      });
           });
}

int circleId;
int lineId;

void display() {
  ARUint8* image = arVideoGetImage();
  if (image == NULL) return;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  arglDispImage(image, &arCameraParam, 1.0, arSettings);

  double P[16];
  arglCameraFrustumRH(&arCameraParam, 1, 1000, P);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(P);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  mySetLight();

  int markerNum;
  ARMarkerInfo* markerInfo;
  arDetectMarker(image, threshold, &markerInfo, &markerNum);
  vector<MarkedPoint> circlePoints;
  vector<MarkedPoint> linePoints;
  for (int j = 0; j < markerNum; j++) {
    glPushMatrix();
    if (markerInfo[j].id == origin.patternId) {
      double M[16];
      origin.getTransMat(&(markerInfo[j]));
    }else{
      if(markerInfo[j].id == -1) continue;
      if(markerInfo[j].id == circleId){
        MarkedPoint p = idMarkedPointMap[markerInfo[j].id];
        p.getTransMat(&(markerInfo[j]));
        p.setDistanceFromOrigin(origin);
        p.setRotationFromOrigin(origin);
        circlePoints.push_back(p);
      }else{
        MarkedPoint p = idMarkedPointMap[markerInfo[j].id];
        p.getTransMat(&(markerInfo[j]));
        p.setDistanceFromOrigin(origin);
        p.setRotationFromOrigin(origin);
        linePoints.push_back(p);
      }
    }
    glPopMatrix();
  }
  glPushMatrix();
  double M[16];
  arglCameraViewRH(origin.transMat, M, 1.0);
  glLoadMatrixd(M);
  setGlColorHSVA(0., 1., 1., 1.);
  drawOrigin();

  glLineWidth(5.f);
  vector<Line> lines = getLines(linePoints);

  vector<Circle> circles = getCircles(circlePoints);
  for_each(circles.begin(), circles.end(),
           [&](Circle c){
             setGlColorHSVA(c.color[0], 1., 1., 1.);
             c.fill();
             for_each(lines.begin(), lines.end(),
                      [&](Line l){
                        Circle invCircle = Mobius::circleInverse(c, l);
                        setGlColorHSVA(1., 0., 0., 1.);
                        invCircle.z = c.z;
                        invCircle.draw();
                      });
           });

  drawInversedCircles(circles);

  setGlColorHSVA(0., 0., 0., 1.);
  for_each(lines.begin(), lines.end(),
           [](Line l){ l.draw();});

  for_each(circles.begin(), circles.end(),
           [&](Circle c){
             c.drawCenter();
             glColor3f(0., 1., 0.);
             drawString("("+ to_string((int) c.center.x.re()) +", "+ to_string((int) c.center.y.re()) +")",
                        c.center.x.re() - 1., c.center.y.re() - 1., c.z);
           });
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
  if (key == 27) {
    exit(0);
  }
}

void quit() {
  arVideoCapStop();
  arVideoClose();
  arglCleanup(arSettings);
}

void loadPatterns(){
  MarkedPoint pointC("resources/C.pat", 40);
  MarkedPoint pointD("resources/D.pat", 40);
  MarkedPoint rect("resources/rect.pat", 40);
  MarkedPoint line("resources/line.pat", 40);
  MarkedPoint line2("resources/line2.pat", 40);
  idMarkedPointMap[pointC.patternId] = pointC;
  idMarkedPointMap[pointD.patternId] = pointD;
  idMarkedPointMap[rect.patternId] = rect;
  idMarkedPointMap[line.patternId] = line;
  idMarkedPointMap[line2.patternId] = line2;
  circleId = rect.patternId;
  lineId = line.patternId;
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitWindowSize(width, height);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("Inversive geometry");
  arSettings = arglSetupForCurrentContext();

  setupCamera("resources/WDM_camera_flipV.xml", "resources/camera_para.dat");

  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutKeyboardFunc(keyboard);
  atexit(quit);

  loadPatterns();

  glutMainLoop();
}
