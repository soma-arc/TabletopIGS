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
#include "Complex.h"
#include "Point.h"
#include "ReferenceMarker.h"

ARParam arCameraParam;
ARGL_CONTEXT_SETTINGS_REF arSettings;
int width = 800;
int height = 600;
double crossMarkerWidth = 80.0;
double pMarkerWidth = 40.0;
int crossMarkerId;
int pMarkerId;
ReferenceMarker origin("resources/cross.pat", 80.0);

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
    }else if(markerInfo[j].id == pMarkerId){
      glPushMatrix();
      double center[2] = { 0, 0 };
      double transMat[3][4];
      double M[16];
      if(first){
        arGetTransMat(&(markerInfo[j]), center, pMarkerWidth, transMat);
        first = false;
      }else{
        arGetTransMatCont(&(markerInfo[j]), pre, center, pMarkerWidth, transMat);
      }
      arglCameraViewRH(origin.transMat, M, 1.0);
      glLoadMatrixd(M);


      vector<double> distance = origin.getDistance(transMat);
      vector<double> rotation = origin.getRotation(transMat);
      std::cout << "distance ";
      for (auto n: distance) {
        std::cout << n << " ";
      }
      std::cout << "rotation ";
      for (auto n: rotation) {
        std::cout << n << " ";
      }
      std::cout << std::endl << std::endl;
      glLineWidth(10.);
      glBegin(GL_LINES);
      glVertex3f( 0., 0. , .5);
      glVertex3f( distance[0], distance[1], .5);
      glEnd();

      for(int x = 0 ; x < 3 ; x++){
        for(int y = 0 ; y < 4 ; y++){
          pre[x][y] = transMat[x][y];
        }
      }
      glPopMatrix();
    }
    glPopMatrix();
  }

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

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitWindowSize(width, height);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("ARToolKit Sample");
  arSettings = arglSetupForCurrentContext();

  setupCamera("resources/WDM_camera_flipV.xml", "resources/camera_para.dat");

  crossMarkerId = arLoadPatt("resources/cross.pat");
  pMarkerId = arLoadPatt("resources/p.pat");

  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutKeyboardFunc(keyboard);
  atexit(quit);

  glutMainLoop();
}
