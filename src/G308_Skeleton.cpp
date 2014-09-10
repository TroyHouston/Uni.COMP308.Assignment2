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

#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <ctype.h>
#include "G308_Skeleton.h"
#include "define.h"

Skeleton::Skeleton(char* filenameASF, char* filenameAMC) {
	jointsOn = true;
	sceneAxisOn = true;
	jointAxisOn = true;
	floorOn = false;
	headOn = true;
	numBones = 1;
	numOfFrames = 0;
	buffSize = 200;
	maxBones = 60;
	angle = 0;
	floorMode = 2;
	currentCharacter = 0;

	root = (bone*) malloc(sizeof(bone) * maxBones);

	for (int i = 0; i < 60; i++) {
		root[i].numChildren = 0;
		root[i].dirx = 0;
		root[i].diry = 0;
		root[i].dirz = 0;
		root[i].rotx = 0;
		root[i].roty = 0;
		root[i].rotz = 0;
		root[i].dof = DOF_NONE;
		root[i].length = 0;
		root[i].name = NULL;
		root[i].children = (bone**) malloc(sizeof(bone*) * 5);

		//Challenge stuff
		root[i].currentTranslatex = 0;
		root[i].currentTranslatey = 0;
		root[i].currentTranslatez = 0;
		root[i].currentRotationx = 0;
		root[i].currentRotationy = 0;
		root[i].currentRotationz = 0;

	}
	char*name = (char*) malloc(sizeof(char) * 5);
	name[0] = 'r';
	name[1] = name[2] = 'o';
	name[3] = 't';
	name[4] = '\0';
	root[0].name = name;
	root[0].dof = DOF_ROOT;
	readASF(filenameASF);
	// Read in the AMC file, store as frames.
	FILE* amc = fopen(filenameAMC, "r");
	readAMC(amc);
}

Skeleton::~Skeleton() {
	deleteBones(root);
}

void Skeleton::deleteBones(bone* root) {
	for (int i = 0; i < maxBones; i++) {
		if (root[i].name != NULL) {
			free(root[i].name);
		}
		if (root[i].children != NULL) {
			free(root[i].children);
		}
	}
	free(root);
}

// Draw 'infinite' lines to represent the global axis (0,0,0).
void Skeleton::drawSceneAxis() {

	glEnable(GL_LINE_SMOOTH);

	// X-Axis
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(-1000, 0, 0);
	glVertex3f(1000, 0, 0);
	glEnd();

	// Y-Axis
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0, -1000, 0);
	glVertex3f(0, 1000, 0);
	glEnd();

	// Z-Axis
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0, 0, -1000);
	glVertex3f(0, 0, 1000);
	glEnd();
}

// Draws a checkered floor. Allows 2 versions. Depending on toggle.
void Skeleton::drawCheckeredFloor() {

	// Sets depth based on animated or pose
	float depth = 0;
	if (floorCam == 0)
		depth = -18.0f;
	else
		depth = 0;

	int GridSizeX = 16;
	int GridSizeY = 16;
	int SizeX = 8;
	int SizeY = 8;

	// Draws floor as a quads.
	glBegin(GL_QUADS);
	for (int x = -16; x < GridSizeX;++x)
		for (int y = -16; y < GridSizeY;++y)
		{
			// Random color mode.
			if (floorMode == 1) {
				int a = rand() % 2;
				int b = rand() % 2;
				int c = rand() % 2;

				if ((x+y) % 2 == 0)
					glColor3f(a,b,c); // Colored Square
				else
					glColor3f(0.0f,0.0f,0.0f); // Black Square

			}
			// Yellow color mode.
			else if(floorMode == 0){
				if ((x+y) % 2 == 0)
					glColor3f(1,1,0); // Colored Square
				else
					glColor3f(0.0f,0.0f,0.0f); // Black Square
			}

			glVertex3f(    x*SizeX,depth,    y*SizeY);
			glVertex3f((x+1)*SizeX,depth,    y*SizeY);
			glVertex3f((x+1)*SizeX,depth, (y+1)*SizeY);
			glVertex3f(    x*SizeX,depth, (y+1)*SizeY);

		}
	glEnd();

}

// Main display method.
void Skeleton::display() {
	if (root == NULL) {
		return;
	}
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(0.05, 0.05, 0.05);

	GLUquadric* quad = gluNewQuadric(); //Create a new quadric to allow you to draw cylinders
	if (quad == 0) {
		printf("Not enough memory to allocate space to draw\n");
		exit(EXIT_FAILURE);
	}


	// Draw the scene axis
	if (sceneAxisOn == true){
		drawSceneAxis();
	}
	//Actually draw the skeleton
	display(root, quad);
	// Draw the floor
	if (floorOn == true) {
		drawCheckeredFloor();
	}

	gluDeleteQuadric(quad);
	glPopMatrix();
}

// Second display method.
void Skeleton::display(bone* root, GLUquadric* q) {
	if (root == NULL) {
		return;
	}

	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	drawAllFrom(root, q);

}

// Recursive draw bone method
void Skeleton::drawAllFrom(bone* root, GLUquadric* q) {
	if (root == NULL) {
		return;
	}

	glPushMatrix();
	// Draw this bone
	drawBoneSegment(root, q);
	// Draw all of the bone children, and their children.
	for(int i = 0; i < root->numChildren; i++){
		drawAllFrom(root->children[i], q);
	}
	glPopMatrix();


}

// Draw the joints (spheres) of the skeleton
void Skeleton::drawJoints(bone* root) {
	glPushMatrix();
	{
		glColor3f(0, 1, 1);
		glutSolidSphere(0.3, 10, 10);
	}
	glPopMatrix();
}

// Draw the joint axis of rotation.
void Skeleton::drawJointAxis(GLUquadric* q) {
	glPushMatrix();
	{
		// Z-axis
		glColor3f(0, 0, 1);
		gluCylinder(q, 0.1, 0.1, 1, 10, 10);
		glTranslatef(0, 0, 1);
		glutSolidCone(0.2, 0.2, 10, 10);
	}
	glPopMatrix();
	glPushMatrix();
	{
		// Y-axis
		glColor3f(0, 1, 0);
		glRotatef(-90, 1, 0, 0);
		gluCylinder(q, 0.1, 0.1, 1, 10, 10);
		glTranslatef(0, 0, 1);
		glutSolidCone(0.2, 0.2, 10, 10);
	}
	glPopMatrix();
	glPushMatrix();
	{
		// X-axis
		glColor3f(1, 0, 0);
		glRotatef(90, 0, 1, 0);
		gluCylinder(q, 0.1, 0.1, 1, 10, 10);
		glTranslatef(0, 0, 1);
		glutSolidCone(0.2, 0.2, 10, 10);
	}
	glPopMatrix();
}

// Draw th face (head and eyes)
void Skeleton::displayFace(bone* root) {
	// Head
	glPushMatrix();
	// Choose random colors
	int a = rand() % 2;
	int b = rand() % 2;
	int c = rand() % 2;
	glColor3f(a,b,c);
	glScalef(0.6, 0.6, 0.7);
	glTranslatef(0, 0, root->length);
	glutSolidSphere(2, 100, 100);
	glPopMatrix();
	// Eye 1
	glPushMatrix();
	glColor3f(0, 1, 1);
	glTranslatef(0.4, -2 * 0.6 + 0.2, root->length);
	glutSolidSphere(0.25, 100, 100);
	glPopMatrix();
	// Eye 2
	glPushMatrix();
	glColor3f(0, 1, 1);
	glTranslatef(-0.4, -2 * 0.6 + 0.2, root->length);
	glutSolidSphere(0.25, 100, 100);
	glPopMatrix();
}

// Draw an individual bone
void Skeleton::drawBoneSegment(bone* root, GLUquadric* q) {
	if (root == NULL) {
		return;
	}

	GLfloat a = 0.0;

	// Find rotation vector normal.
	G308_Point v1 = { 0, 0, 1 };
	G308_Point v2 = { root->dirx, root->diry, root->dirz };
	G308_Point normal = { 0, 0, 0 };

	calculateCrossProduct(v1, v2, &normal);

	// Calculate rotation angle from bone's direction vector (degrees).
	a = calculateAngleFromDotProduct(v1, v2);

	// Draw spheres to represent joints
	if (jointsOn == true) {
		drawJoints(root);
	}

	// Rotate local coordinates
	glRotatef(root->rotz, 0, 0, 1);
	glRotatef(root->roty, 0, 1, 0);
	glRotatef(root->rotx, 1, 0, 0);

	// Rotate current frame/pose
	doCurrentMove(root);

	// Rotate local coordinates
	glRotatef(-root->rotx, 1, 0, 0);
	glRotatef(-root->roty, 0, 1, 0);
	glRotatef(-root->rotz, 0, 0, 1);

	// Draw axis of each individual joint
	if (jointAxisOn == true) {
		glPushMatrix();
		glRotatef(a, normal.x, normal.y, normal.z);
		drawJointAxis(q);
		glPopMatrix();
	}

	glPushMatrix();

	glColor3f(1, 1, 1);
	// Rotate bone.
	glRotatef(a, normal.x, normal.y, normal.z);
	// Draw face on the 'head' bone
	if (headOn == true && strcmp(root->name, "head") == 0) {
		displayFace(root);
	}
	// Draw the actual bone
	gluCylinder(q, 0.3, 0.3, root->length, 3, 3);
	// If using second character
	// Draw cuboid limbs
	if (currentCharacter == 1) {
		glScalef(0.2, 0.2, 1);
		glTranslatef(0,-2,0);

		glTranslatef(0, 0, root->length /2);
		// Draw chest
		if (strcmp(root->name,"lowerback") == 0 || strcmp(root->name,"upperback") == 0 || strcmp(root->name,"thorax") == 0) {
			glPushMatrix();
			glScalef(12, 2, 1);
			glutSolidCube(root->length);
			glPopMatrix();
		}
		else
			glutSolidCube(root->length);

		// Draw lower(root) chest
		if (strcmp(root->name,"lowerback") == 0) {
			glPushMatrix();
			glTranslatef(0,0,-root->length);
			glScalef(12, 3, 1);
			glutSolidCube(root->length);
			glPopMatrix();
		}
	}
	glPopMatrix();

	// Move bone to correct location
	glTranslatef(root->length * root->dirx, root->length * root->diry,
			root->length * root->dirz);

}

// Translate a bone a given amount.
void Skeleton::translateBone(char* boneName, float ax, float ay, float az) {
	bone* b = getBoneFromName(boneName);

	if (b == NULL) {
		printf("No bone named %s was found.", boneName);
		return;
	}

	b->currentTranslatex = ax;
	b->currentTranslatey = ay;
	b->currentTranslatez = az;
}

// Get a bone from a given bone name.
bone* Skeleton::getBoneFromName(char* boneName) {
	bone* b = NULL;
	for (int i = 0; i < numBones; i++) {
		if (strcmp(root[i].name, boneName) == 0) {
			b = &root[i];
		}
	}
	return b;
}

//  Rotate a bone a given amount.
void Skeleton::rotateBone(char* boneName, float ax, float ay, float az) {
	bone* b = getBoneFromName(boneName);
	if (b == NULL) {
		printf("No bone named %s was found.", boneName);
		return;
	}

	b->currentRotationx = ax;
	b->currentRotationy = ay;
	b->currentRotationz = az;
}

// Rotate and translate according to the current frame/pose
void Skeleton::doCurrentMove(bone* b){

	glTranslatef(b->currentTranslatex, b->currentTranslatey, b->currentTranslatez);

	glRotatef(b->currentRotationz, 0.0, 0.0, 1.0);
	glRotatef(b->currentRotationy, 0.0, 1.0, 0.0);
	glRotatef(b->currentRotationx, 1.0, 0.0, 0.0);
}

// Calculate cross product (obviously)
void Skeleton::calculateCrossProduct(G308_Point v1, G308_Point v2, G308_Point* normal){
	normal->x = v1.y * v2.z - v1.z * v2.y;
	normal->y = v1.z * v2.x - v1.x * v2.z;
	normal->z = v1.x * v2.y - v1.y * v2.x;

	GLfloat result = sqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
	if (result == 0.0) {
		return;
	}

	// Set final value
	normal->x /= result;
	normal->y /= result;
	normal->z /= result;
}

// Get the angle from using the dot product on two vectors
float Skeleton::calculateAngleFromDotProduct(G308_Point v1, G308_Point v2){
	// Equation to use
	// a . b = |a|*|b|*cosA

	// a.b Dot product
	float dotProduct = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	// |a| Magnitude of a
	float magA = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
	// |b| Magnitude of b
	float magB = sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);

	float angle = 0.0;
	angle = acos(dotProduct / (magA * magB));
	// Convert to degrees from radians
	return angle * (180.0 / 3.1416);
}

// Reads an ASF File
bool Skeleton::readASF(char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Failed to open file %s\n", filename);
		exit(EXIT_FAILURE);
	}

	char *buff = new char[buffSize];
	char *p;

	printf("Starting reading skeleton file\n");
	while ((p = fgets(buff, buffSize, file)) != NULL) {
		//Reading actually happened!

		char* start = buff;
		trim(&start);

		//Check if it is a comment or just empty
		if (buff[0] == '#' || buff[0] == '\0') {
			continue;
		}

		start = strchr(buff, ':');
		if (start != NULL) {
			//This actually is a heading
			//Reading may actually consume the next heading
			//so headings need to be a recursive construct?
			readHeading(buff, file);
		}
	}

	delete[] buff;
	fclose(file);
	printf("Completed reading skeleton file\n");
	return true;
}

/**
 * Trim the current string, by adding a null character into where the comments start
 */
void Skeleton::decomment(char * buff) {
	char* comStart = strchr(buff, '#');
	if (comStart != NULL) {
		//remove irrelevant part of string
		*comStart = '\0';
	}
}

void Skeleton::readHeading(char* buff, FILE* file) {
	char* temp = buff;
	decomment(buff);
	trim(&temp);

	char head[50];
	char rest[200];
	int num = sscanf(temp, ":%s %s", head, rest);
	if (num == 0) {
		printf("Could not get heading name from %s, all is lost\n", temp);
		exit(EXIT_FAILURE);
	}
	if (strcmp(head, "version") == 0) {
		//version string - must be 1.10
		char* version = rest;
		if (num != 2) {
			char *p = { '\0' };
			while (strlen(p) == 0) {
				char* p = fgets(buff, buffSize, file);
				decomment(p);
				trim(&p);
				version = p;
			}
		}
		if (strcmp(version, "1.10") != 0) {
			printf("Invalid version: %s, must be 1.10\n", version);
			exit(EXIT_FAILURE);
		}
		//Finished reading version so read the next thing?
	} else if (strcmp(head, "name") == 0) {
		//This allows the skeleton to be called something
		//other than the file name
		//We don't actually care what the name is, so can
		//ignore this whole section

	} else if (strcmp(head, "documentation") == 0) {
		//Documentation section has no meaningful information
		//only of use if you want to copy the file. So we skip it
	} else if (strcmp(head, "units") == 0) {
		//Has factors for the units
		//To be able to model the real person,
		//these must be parsed correctly
		//Only really need to check if deg or rad, but even
		//that is probably not needed for the core or extension
	} else if (strcmp(head, "root") == 0) {
		//Read in information about root
		//Or be lazy and just assume it is going to be the normal CMU thing!
	} else if (strcmp(head, "bonedata") == 0) {
		//Description of each bone
		//This does need to actually be read :(
		char *p;
		while ((p = fgets(buff, buffSize, file)) != NULL) {
			decomment(p);
			trim(&p);
			if (strlen(p) > 0) {
				if (p[0] == ':') {
					return readHeading(buff, file);
				} else if (strcmp(p, "begin") != 0) {
					printf("Expected begin for bone data %d, found \"%s\"", numBones, p);
					exit(EXIT_FAILURE);
				} else {
					readBone(buff, file);
					numBones++;
				}

			}
		}
	} else if (strcmp(head, "hierarchy") == 0) {
		//Description of how the bones fit together
		char *p;
		while ((p = fgets(buff, buffSize, file)) != NULL) {
			trim(&p);
			decomment(p);
			if (strlen(p) > 0) {
				if (p[0] == ':') {
					return readHeading(buff, file);
				} else if (strcmp(p, "begin") != 0) {
					printf("Expected begin in hierarchy, found %s", p);
					exit(EXIT_FAILURE);
				} else {
					readHierarchy(buff, file);
				}

			}
		}
	} else {
		printf("Unknown heading %s\n", head);
	}

}

void Skeleton::readHierarchy(char* buff, FILE* file) {
	char *p;
	char t1[200];
	while ((p = fgets(buff, buffSize, file)) != NULL) {
		trim(&p);
		decomment(p);
		if (strlen(p) > 0) {
			if (strcmp(p, "end") == 0) {
				//end of hierarchy
				return;
			} else {
				//Read the root node
				sscanf(p, "%s ", t1);
				bone* rootBone = NULL;
				for (int i = 0; i < numBones; i++) {
					if (strcmp(root[i].name, t1) == 0) {
						rootBone = root + i;
						break;
					}
				}
				//Read the connections
				p += strlen(t1);
				bone* other = NULL;
				while (*p != '\0') {
					sscanf(p, "%s ", t1);

					for (int i = 0; i < numBones; i++) {
						if (strcmp(root[i].name, t1) == 0) {
							other = root + i;
							break;
						}
					}
					if (other == NULL) {
						printf("Unknown bone %s found in hierarchy. Failure", t1);
						exit(EXIT_FAILURE);
					}
					rootBone->children[rootBone->numChildren] = other;
					rootBone->numChildren++;
					p += strlen(t1) + 1;

				}
			}
		}

	}
}

void Skeleton::readBone(char* buff, FILE* file) {
	char *p;
	char t1[50];
	while ((p = fgets(buff, buffSize, file)) != NULL) {
		trim(&p);
		decomment(p);
		if (strlen(p) > 0) {
			if (strcmp(p, "end") == 0) {
				//end of this bone
				return;
			} else {
				sscanf(p, "%s ", t1);
				if (strcmp(t1, "name") == 0) {
					//Read the name and actually remember it
					char* name = (char*) malloc(sizeof(char) * 10);
					sscanf(p, "name %s", name);
					root[numBones].name = name;
				} else if (strcmp(t1, "direction") == 0) {
					//Also actually important
					float x, y, z;
					sscanf(p, "direction %f %f %f", &x, &y, &z);
					root[numBones].dirx = x;
					root[numBones].diry = y;
					root[numBones].dirz = z;
				} else if (strcmp(t1, "length") == 0) {
					//Also actually important
					float len;
					sscanf(p, "length %f", &len);
					root[numBones].length = len;
				} else if (strcmp(t1, "dof") == 0) {
					//Read the degrees of freedom!
					char d1[5];
					char d2[5];
					char d3[5];
					int num = sscanf(p, "dof %s %s %s", d1, d2, d3);
					switch (num) {
					DOF dof;
					case 3:
						dof = dofFromString(d3);
						root[numBones].dof = root[numBones].dof | dof;
						//fallthrough!!
						/* no break */
					case 2:
						dof = dofFromString(d2);
						root[numBones].dof = root[numBones].dof | dof;
						//fallthrough!!
						/* no break */
					case 1:
						dof = dofFromString(d1);
						root[numBones].dof = root[numBones].dof | dof;
						break;
					}
				} else if (strcmp(t1, "axis") == 0) {
					//Read the rotation axis
					float x, y, z;
					int num = sscanf(p, "axis %f %f %f XYZ", &x, &y, &z);
					if (num != 3) {
						printf("axis format doesn't match expected\n");
						printf("Expected: axis %%f %%f %%f XYZ\n");
						printf("Got: %s", p);
						exit(EXIT_FAILURE);
					}
					root[numBones].rotx = x;
					root[numBones].roty = y;
					root[numBones].rotz = z;
				}
				//There are more things but they are not needed for the core
			}

		}
	}
}

/**
 * Helper function to turn a DOF from the AMC file into the correct DOF value
 */
DOF Skeleton::dofFromString(char* s) {
	if (strcmp(s, "rx") == 0)
		return DOF_RX;
	if (strcmp(s, "ry") == 0)
		return DOF_RY;
	if (strcmp(s, "rz") == 0)
		return DOF_RZ;
	printf("Unknown DOF found: %s", s);
	return DOF_NONE;
}

/*
 * Remove leading and trailing whitespace. Increments the
 * pointer until it points to a non whitespace char
 * and then adds nulls to the end until it has no
 * whitespace on the end
 */
void trim(char **p) {
	if (p == NULL) {
		printf("File terminated without version number!\n");
		exit(EXIT_FAILURE);
	}

	//Remove leading whitespace
	while (**p == ' ' || **p == '\t') {
		(*p)++;
	}

	int len = strlen(*p);
	while (len > 0) {
		char last = (*p)[len - 1];
		if (last == '\r' || last == '\n' || last == ' ' || last == '\t') {
			(*p)[--len] = '\0';
		} else {
			return;
		}
	}
}

// Clears the current stored rotations and translations (current frame/pose)
void Skeleton::clearChanges() {
	for (int i = 0; i < numBones; i++) {
		root[i].currentRotationx = 0;
		root[i].currentRotationy = 0;
		root[i].currentRotationz = 0;
		root[i].currentTranslatex= 0;
		root[i].currentTranslatey = 0;
		root[i].currentTranslatez= 0;
	}

	glutPostRedisplay();
}

// Reads an AMC file in each bone's frames array
void Skeleton::readAMC(FILE* amcFile) {
	clearChanges();

	char buffer[100];
	for (int i = 0; i < 3; i++) {
		fgets(buffer, 100, amcFile);}


	// Initialise variables
	char name[20] = {0};
	float rx = 0;
	float ry = 0;
	float rz = 0;
	float tx = 0;
	float ty = 0;
	float tz = 0;

	int count = 0;

	printf("Loading AMC File... \n");
	while (fgets(buffer, 100, amcFile)) {

		sscanf(buffer, "%s %f %f %f\n", name, &rx, &ry, &rz);
		// Use this scan format if scanning a root.
		if (strcmp(name, "root")== 0)
			sscanf(buffer, "%s %f %f %f %f %f %f\n", name, &tx, &ty, &tz, &rx, &ry, &rz);

		// Check if new frame is starting
		if (isdigit(name[0])) {
			count++;
			continue;
		}

		bone* b = getBoneFromName(name);

		// Store new frame
		b->frames[count-1].rx = rx;
		b->frames[count-1].ry = ry;
		b->frames[count-1].rz = rz;

		b->frames[count-1].tx = tx;
		b->frames[count-1].ty = ty;
		b->frames[count-1].tz = tz;

		// Reset varaibles
		memset(&name[0], 0, sizeof(name));
		rx = 0;
		ry = 0;
		rz = 0;
		tx = 0;
		ty = 0;
		tz = 0;
	}

	numOfFrames = count;
}

// Load a specific frame into the current memory.
void Skeleton::loadFrame(int* frameNum) {
	int frame = *frameNum -1;

	for (int i = 0; i < numBones; i++) {
		boneFrame bf = root[i].frames[frame];

		root[i].currentRotationx = bf.rx;
		root[i].currentRotationy = bf.ry;
		root[i].currentRotationz = bf.rz;

		root[i].currentTranslatex = bf.tx;
		root[i].currentTranslatey = bf.ty;
		root[i].currentTranslatez = bf.tz;

	}

}

// Read a (my custom) Pose file
void Skeleton::readPoseFile(char* filename) {
	FILE* poseFile;
	poseFile = fopen(filename, "r");

	char name[20] = {0};
	float x = 0;
	float y = 0;
	float z = 0;

	while (fscanf(poseFile, "%s %f %f %f", name, &x, &y, &z) == 4) {
		rotateBone(name, x,y,z);
	}

}


