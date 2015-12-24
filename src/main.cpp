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

ARParam arCameraParam;
ARGL_CONTEXT_SETTINGS_REF arSettings;
int width = 800;
int height = 600;
double crossMarkerWidth = 80.0;
double pMarkerWidth = 40.0;
int crossMarkerId;
int pMarkerId;
ReferenceMarker origin("resources/cross.pat", 80.0);
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

double pre[3][4];
bool first = true;
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
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

  int threshold = 100;
  int markerNum;
  ARMarkerInfo* markerInfo;
  arDetectMarker(image, threshold, &markerInfo, &markerNum);
  for (int j = 0; j < markerNum; j++) {
    glPushMatrix();
    if (markerInfo[j].id == origin.patternId) {
      double M[16];
      origin.getTransMat(&(markerInfo[j]));
      arglCameraViewRH(origin.transMat, M, 1.0);
      glLoadMatrixd(M);
      glTranslatef(0, 0, 40);
      glutSolidCube(80);
    }else{
      glPushMatrix();
      if(markerInfo[j].id == -1) continue;
      MarkedPoint& p = idMarkedPointMap[markerInfo[j].id];
      p.getTransMat(&(markerInfo[j]));
      p.setDistanceFromOrigin(origin);
      p.setRotationFromOrigin(origin);
      glPopMatrix();
    }
    glPopMatrix();
  }
  glPushMatrix();
  double M[16];
  arglCameraViewRH(origin.transMat, M, 1.0);
  glLoadMatrixd(M);
  for(auto itr = idMarkedPointMap.begin(); itr != idMarkedPointMap.end(); ++itr) {
    cout << "id"  << itr->first << endl;
    if(itr->second.visible == false || itr->first == -1) continue;
    vector<double> distance = itr->second.distanceFromOrigin;
    cout << distance[0] << " " << distance[1] << " " << distance[2] << endl;
    glBegin(GL_LINES);
    glVertex3f(0., 0., 0.);
    glVertex3f(distance[0], distance[1], distance[2]);
    itr->second.draw();
    glEnd();
  }
  glPopMatrix();

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

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
  idMarkedPointMap[pointA.patternId] = pointA;
  idMarkedPointMap[pointB.patternId] = pointB;
  idMarkedPointMap[pointC.patternId] = pointC;
  idMarkedPointMap[pointD.patternId] = pointD;
  idMarkedPointMap[pointE.patternId] = pointE;
  idMarkedPointMap[pointF.patternId] = pointF;
  for(auto itr = idMarkedPointMap.begin(); itr != idMarkedPointMap.end(); ++itr) {
    cout << "id"  << itr->first << endl;
  }
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
