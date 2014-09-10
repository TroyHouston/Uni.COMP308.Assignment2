//---------------------------------------------------------------------------
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
// Copyright (c) 2012 by Taehyun Rhee
//
// Edited by Roma Klapaukh, Daniel Atkins, and Taehyun Rhee
//
//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <GL/glut.h>
#include "define.h"
#include "G308_Skeleton.h"

GLuint g_mainWnd;
GLuint g_nWinWidth = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

void G308_keyboardListener(unsigned char, int, int);
void G308_Reshape(int w, int h);
void G308_display();
void G308_init();
void G308_SetCamera();
void G308_SetLight();
void printControlsToScreen();
void toggleBool(bool* toggle, char* name);
void createMenu(void);
void menu(int num);
void tick (int t);

Skeleton* skeleton;
char controlsInfo[6][80];
static int menu_id;
int mode = 0;
int currentFrame = 0;
bool reverse = false;
int mediaMode = 2;
int msPerTick = 25;

int main(int argc, char** argv) {
	if (argc != 3) {
		//Usage instructions for core and challenge
		printf("Usage\n");
		printf("./Ass2 priman.asf priman.amc\n");
		exit(EXIT_FAILURE);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(g_nWinWidth, g_nWinHeight);
	g_mainWnd = glutCreateWindow("COMP308 Assignment2");

	glutKeyboardFunc(G308_keyboardListener);
	createMenu();
	glutDisplayFunc(G308_display);
	glutReshapeFunc(G308_Reshape);

	G308_init();

	// [Assignment2] : Read ASF file
	skeleton = new Skeleton(argv[1], argv[2]);

	glutMainLoop();


	return EXIT_SUCCESS;
}

// Init Light and Camera
void G308_init() {

	//glutKeyboardFunc(G308_keyboardListener);
	G308_SetLight();
	G308_SetCamera();
}

void printString(std::string s, int i) {
	glRasterPos2i(10, G308_WIN_HEIGHT- 20 -(i *20));
	void * font = GLUT_BITMAP_9_BY_15;
	for (std::string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 1.0, 1.0);
		glutBitmapCharacter(font, c);
	}
}

void printControlsToScreen() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, G308_WIN_WIDTH, 0.0, G308_WIN_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	int numOfLines = 10;
	std::string strings[numOfLines];
	glColor3d(1.0, 1.0, 1.0);
	strings[0] = "Controls:";
	strings[1] = "Use: W, A, S, D, to control the View";
	strings[2] = "Use: Z, X, to control the Zoom";
	strings[3] = "Use: P, to Toggle Pose";
	strings[4] = "Use: 1, to Toggle Floor Shown";
	strings[5] = "Use: 2, to Toggle Joints Shown";
	strings[6] = "Use: 3, to Toggle Joint Axis Shown";
	strings[7] = "Use: 4, to Toggle Scene Axis Shown";
	strings[8] = "Use: 5, to Toggle Head Shown";
	strings[9] = "Use: 6, to Toggle Characters";


	for (int i = 0; i < numOfLines; i++) {
		printString(strings[i], i);
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// Display call back
void G308_display() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%s\n", gluErrorString(err));
	}

	// [Assignmet2] : render skeleton
	if (skeleton != NULL) {
		skeleton->display();
	}

	printControlsToScreen();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

void toggleBool(bool* toggle, char* name) {
	if (*toggle == true) {
		*toggle = false;
		printf("%s - Off\n", name);
	}
	else {
		*toggle = true;
		printf("%s - On\n", name);
	}
}

void toggleBetweenPoses() {
	mode++;
	if (mode == 4)
		mode = 0;
	printf("Toggled Pose\n");

	switch(mode){
	case 0:
		skeleton->clearChanges(); break;
	case 1:
		skeleton->clearChanges();
		skeleton->readPoseFile("assets/sitting.txt"); break;
	case 2:
		skeleton->clearChanges();
		skeleton->readPoseFile("assets/walking.txt"); break;	// Walking Pose
	case 3:
		skeleton->clearChanges();
		skeleton->readPoseFile("assets/custom.txt");break;	// Other Pose
	}
}

void G308_keyboardListener(unsigned char key, int x, int y) {
	//Code to respond to key events

	// Camera Controls
	switch (key) {
	case 'w':
		glRotatef(-5, 1,0,0); break;
	case 'a':
		glRotatef(-5, 0,1,0); break;
	case 's':
		glRotatef(5, 1,0,0); break;
	case 'd':
		glRotatef(5, 0,1,0); break;
	case 'z':
		glScalef(0.8, 0.8, 0.8); break;
	case 'x':
		glScalef(1.25, 1.25, 1.25); break;

		// Misc
	case 'p':	// Switch the 4. Norm -> Sit -> Walking -> Special
		toggleBetweenPoses(); break;

		// Rendering Options
	case '1':
		if (skeleton->floorMode == 0){
			skeleton->floorMode = 1;
		}
		else if (skeleton->floorMode == 1){
			skeleton->floorMode = 2;
			toggleBool(&skeleton->floorOn, "Toggled Floor");
		}
		else if (skeleton->floorMode == 2){
			skeleton->floorMode = 0;
			toggleBool(&skeleton->floorOn, "Toggled Floor");
		}
		break;
	case '2':
		toggleBool(&skeleton->jointsOn, "Toggled Joints");
		break;
	case '3':
		toggleBool(&skeleton->jointAxisOn, "Toggled Joint Axis");
		break;
	case '4':
		toggleBool(&skeleton->sceneAxisOn, "Toggled Scene Axis");
		break;
	case '5':
		toggleBool(&skeleton->headOn, "Toggled Head Shown");
		break;
	case '6':
		skeleton->currentCharacter++;
		if (skeleton->currentCharacter == 2)
			skeleton->currentCharacter = 0;
		break;
	}

	glutPostRedisplay();

}

// Reshape functionsp
void G308_Reshape(int w, int h) {
	if (h == 0)
		h = 1;

	g_nWinWidth = w;
	g_nWinHeight = h;

	glViewport(0, 0, g_nWinWidth, g_nWinHeight);
}

// Set Camera Position
void G308_SetCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (mediaMode != 2)
		gluLookAt(7.0, 7.0, 7, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	else
		gluLookAt(0.0, 0.0, 7, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Set View Position
void G308_SetLight() {
	float direction[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	float diffintensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}

// Menu listener
void menu(int num){
	switch (num) {
	case 0:
		skeleton->floorCam = 1;
		mediaMode = 0;
		G308_SetCamera();
		msPerTick = 50;
		tick(mediaMode);break;
	case 1:
		if (mediaMode == 2)
			return;
		mediaMode = 1;
		msPerTick = 25;
		break;
	case 2:
		skeleton->clearChanges();
		skeleton->floorCam = 0;
		mediaMode = 2;
		currentFrame = 0;
		msPerTick = 5;
		mode = 0;
		G308_SetCamera();
		break;
	case 3:
		toggleBool(&reverse, "null");
		msPerTick = 50;
		break;
	case 4:
		if(msPerTick == 50)
			msPerTick = 2;
		else if(msPerTick == 2)
			msPerTick = 1;
		else if(msPerTick == 1)
			msPerTick = 0;
		break;

	}
	glutPostRedisplay();
}

// Creates the below menu. Attachs it to the right click button
void createMenu(){
	menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Play", 0);
	glutAddMenuEntry("Pause", 1);
	glutAddMenuEntry("Stop", 2);
	glutAddMenuEntry("Rewind", 3);
	glutAddMenuEntry("Fast Forward", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Tick method for the animation.
void tick (int t) {
	if (currentFrame == skeleton->numOfFrames)
		reverse = true;
	// If at end of frames -> reverse
	else if (currentFrame <= 1)
		reverse = false;
	// If paused
	if (t == 1)
		return;
	// If stopped
	else if (t == 2) {
		skeleton->clearChanges();
		return;
	}
	// Callback the tick method after specified number of milliseconds
	glutTimerFunc(msPerTick, tick, mediaMode);

	// Change direction of
	if (reverse == false)
		currentFrame++;
	else
		currentFrame--;

	skeleton->loadFrame(&currentFrame);

	glutPostRedisplay();
}





