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

ARParam arCameraParam;
ARGL_CONTEXT_SETTINGS_REF arSettings;
int width = 800;
int height = 600;
double marker_width = 80.0;
int marker_id;

bool setupCamera(char videoConfigFile[], char cameraParamFile[]) {

	if (arVideoOpen(videoConfigFile) < 0) {
		std::cout << "Camera not found \n";
		return false;
	}
	if (arVideoCapStart() != 0) {
		std::cout << "Video capture start failed \n";
		return false;
	}

	ARParam	wparam;
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
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
}

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
	int	marker_num;
	ARMarkerInfo* marker_info;
	arDetectMarker(image, threshold, &marker_info, &marker_num);

	int k = -1;
	for (int j = 0; j < marker_num; j++) {
		if (marker_info[j].id == marker_id) {
			if (k == -1) k = j;
			else if (marker_info[j].cf > marker_info[k].cf) k = j;
		}
	}

	if (k != -1) {
		double center[2] = { 0, 0 };
		double transMat[3][4];
		double M[16];
		arGetTransMat(&(marker_info[k]), center, marker_width, transMat);
		arglCameraViewRH(transMat, M, 1.0);

		glLoadMatrixd(M);
		glTranslatef(0, 0, 40);
		glutSolidCube(80);
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

	setupCamera("WDM_camera_flipV.xml", "camera_para.dat");

	marker_id = arLoadPatt("patt.hiro");

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	atexit(quit);

	glutMainLoop();
}
