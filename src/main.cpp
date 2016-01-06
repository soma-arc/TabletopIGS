#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <Windows.h>
#include <gl/freeglut.h>
#endif

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
#include "LineEvaluator.h"
#include "Circle.h"
#include "Mobius.h"

ARParam arCameraParam;
ARGL_CONTEXT_SETTINGS_REF arSettings;
int width = 800;
int height = 600;
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

void cylinder(float radius, float height, int sides)
{
  double pi = 3.1415;
  glNormal3d(0.0, 1.0, 0.0);
  glBegin(GL_POLYGON);
  for(double i = 0; i < sides; i++) {
    double t = pi*2/sides * (double)i;
    glVertex3d(radius * cos(t), radius * sin(t), height);
  }
  glEnd();

  glBegin(GL_QUAD_STRIP);
  for(double i=0;i<=sides;i=i+1){
    double t = i*2*pi/sides;
    glNormal3f((GLfloat)cos(t), (GLfloat)sin(t), 0.0);
    glVertex3f((GLfloat)(radius*cos(t)),(GLfloat)(radius*sin(t)), -height);
    glVertex3f((GLfloat)(radius*cos(t)),(GLfloat)(radius*sin(t)), height);
  }
  glEnd();
  //下面
  glNormal3d(0.0, -1.0, 0.0);
  glBegin(GL_POLYGON);
  for(double i = sides; i >= 0; --i) {
    double t = pi*2/sides * (double)i;
    glVertex3d(radius * cos(t), radius * sin(t), -height);
  }
  glEnd();
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
  // glEnable(GL_LIGHTING);
  // glEnable(GL_DEPTH_TEST);
  // glEnable(GL_COLOR_MATERIAL);

  int threshold = 100;
  int markerNum;
  ARMarkerInfo* markerInfo;
  arDetectMarker(image, threshold, &markerInfo, &markerNum);
  vector<MarkedPoint> points;
  vector<MarkedPoint> linePoints;
  for (int j = 0; j < markerNum; j++) {
    glPushMatrix();
    if (markerInfo[j].id == origin.patternId) {
      double M[16];
      origin.getTransMat(&(markerInfo[j]));
      arglCameraViewRH(origin.transMat, M, 1.0);
      glLoadMatrixd(M);
      glTranslatef(0, 0, 20);
      glutSolidCube(40);
    }else{
      glPushMatrix();
      if(markerInfo[j].id == -1) continue;
      if(markerInfo[j].id == circleId){
        MarkedPoint p = idMarkedPointMap[markerInfo[j].id];
        p.getTransMat(&(markerInfo[j]));
        p.setDistanceFromOrigin(origin);
        p.setRotationFromOrigin(origin);
        points.push_back(p);
      }else{
        cout <<"detect line" << endl;
        MarkedPoint p = idMarkedPointMap[markerInfo[j].id];
        p.getTransMat(&(markerInfo[j]));
        p.setDistanceFromOrigin(origin);
        p.setRotationFromOrigin(origin);
        linePoints.push_back(p);
      }
      glPopMatrix();
    }
    glPopMatrix();
  }
  glPushMatrix();
  double M[16];
  arglCameraViewRH(origin.transMat, M, 1.0);
  glLoadMatrixd(M);
  vector<Circle> circles;
  for_each(points.begin(), points.end(),
           [&](MarkedPoint p){
             vector<double> distance = p.distanceFromOrigin;
             vector<double> rotation = p.rotationFromOrigin;
             Circle c(Point(distance[0], distance[1]), 30);
             cout << "circle " << c << endl;
             c.z = distance[2];
             circles.push_back(c);
             setGlColorHSVA(0., 1., 1., 1.);
             c.draw();
           });

  vector<Line> lines;
  glLineWidth(5.f);
  for_each(linePoints.begin(), linePoints.end(),
           [&](MarkedPoint p){
             vector<double> distance = p.distanceFromOrigin;
             vector<double> rotation = p.rotationFromOrigin;
             Point pp(distance[0], distance[1]);
             cout << pp  <<" , " << Point(pp.x.re() + 50 * cos(rotation[2]), pp.y.re() + 50 * sin(rotation[2])) << endl;
             Line l(pp,
                    Point(pp.x.re() + 50 * cos(rotation[2]),
                          pp.y.re() + 50 * sin(rotation[2])));
             lines.push_back(l);
             setGlColorHSVA(0., 0., 0., 1.);
             l.z = distance[2] * 10;
             l.draw();
           });

  const int maxLevel = 2;
  vector<vector<Circle>> inversedCircles;
  inversedCircles.push_back(circles);
  cout << "search " << inversedCircles.size() <<endl;
  for(int i = 0; i < maxLevel ; i++){
    vector<Circle> v;
    inversedCircles.push_back(v);
    for_each(inversedCircles[i].begin(),
             inversedCircles[i].end(),
             [&](Circle c){
               for_each(inversedCircles[i].begin(),
                        inversedCircles[i].end(),
                        [&](Circle c2){
                          if(c.center.x.re() == c2.center.x.re() &&
                             c.center.y.re() == c2.center.y.re()) return;
                          Circle in = Mobius::circleInverse(c, c2);
                          in.z = c.z;
                          setGlColorHSVA(0.15 * (i + 1), 1., 1., 1.);
                          in.draw();
                          inversedCircles[i + 1].push_back(in);
                        });
             });
  }
  // for_each(circles.begin(),
  //          circles.end(),
  //          [&](Circle c){
  //            for_each(circles.begin(),
  //                     circles.end(),
  //                     [&](Circle c2){
  //                       if(c.center.x.re() == c2.center.x.re() &&
  //                          c.center.y.re() == c2.center.y.re()) return;
  //                       Circle in = Mobius::circleInverse(c, c2);
  //                       in.z = c.z;
  //                       setGlColorHSVA(0.1, 1., 1., 1.);
  //                       in.draw();
  //                       // glPushMatrix();
  //                       // glTranslatef(in.center.x.re(), in.center.y.re(), in.z);
  //                       // cylinder(in.r, 2, 100);
  //                       // glPopMatrix();
  //                     });
  //              });
  glPopMatrix();
  cout << "end " << endl;
  // glDisable(GL_LIGHTING);
  // glDisable(GL_DEPTH_TEST);
  // glDisable(GL_COLOR_MATERIAL);
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
  MarkedPoint pointA("resources/A.pat", 40);
  MarkedPoint pointB("resources/B.pat", 40);
  MarkedPoint pointC("resources/C.pat", 40);
  MarkedPoint pointD("resources/D.pat", 40);
  MarkedPoint pointE("resources/E.pat", 40);
  MarkedPoint pointF("resources/F.pat", 40);
  MarkedPoint pointP("resources/P.pat", 40);
  MarkedPoint rect("resources/rect.pat", 40);
  MarkedPoint line("resources/line.pat", 40);
  idMarkedPointMap[pointA.patternId] = pointA;
  idMarkedPointMap[pointB.patternId] = pointB;
  idMarkedPointMap[pointC.patternId] = pointC;
  idMarkedPointMap[pointD.patternId] = pointD;
  idMarkedPointMap[pointE.patternId] = pointE;
  idMarkedPointMap[pointF.patternId] = pointF;
  idMarkedPointMap[pointP.patternId] = pointP;
  idMarkedPointMap[rect.patternId] = rect;
  idMarkedPointMap[line.patternId] = line;
  circleId = rect.patternId;
  lineId = line.patternId;
  cout << circleId << endl;
    cout << lineId << endl;
    cout << origin.patternId << endl;
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitWindowSize(width, height);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("ARToolKit Sample");
  arSettings = arglSetupForCurrentContext();

  setupCamera("resources/WDM_camera_flipV.xml", "resources/camera_para.dat");

  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutKeyboardFunc(keyboard);
  atexit(quit);

  loadPatterns();

  glutMainLoop();
}
