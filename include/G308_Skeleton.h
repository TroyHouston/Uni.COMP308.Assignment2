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

#ifndef SKELETONH
#define SKELETONH

#include <stdio.h>
#include <GL/glut.h>
#include <define.h>


using namespace std;

// Using bitmasking to denote different degrees of freedom for joint motion
typedef int DOF;

#define DOF_NONE 0
#define DOF_RX 1
#define DOF_RY 2
#define DOF_RZ 4
#define DOF_ROOT 8 // Root has 6, 3 translation and 3 rotation


typedef struct boneFrame {

	float tx, ty, tz;
	float rx, ry, rz;

} boneFrame;

//Type to represent a bone
typedef struct bone {
	char* name;
	float dirx, diry, dirz;
	float rotx, roty, rotz;
	DOF dof;
	float length;
	bone** children;
	int numChildren;

	boneFrame frames[600];

	//Challenge stuff
	float currentTranslatex, currentTranslatey, currentTranslatez;
	float currentRotationx, currentRotationy, currentRotationz;

} bone;

void trim(char**);

class Skeleton {

private:
	int buffSize, maxBones;
	bone* root;
	bool readASF(char*);
	void readHeading(char*, FILE*);
	void decomment(char*);
	void deleteBones(bone*);
	void readBone(char*, FILE*);
	void readHierarchy(char*, FILE*);
	void display(bone*, GLUquadric*);
	DOF dofFromString(char*);
	void drawAllFrom(bone* root, GLUquadric* q);
	void drawBoneSegment(bone* root, GLUquadric* q);
	void calculateCrossProduct(G308_Point v1, G308_Point v2, G308_Point* normal);
	float calculateAngleFromDotProduct(G308_Point v1, G308_Point v2);
	void drawJoints(bone* root);
	void drawJointAxis(GLUquadric* q);
	void displayFace(bone* root);
	void rotateBone(char* boneName, float ax, float ay, float az);
	void doCurrentMove(bone* bone);
	void translateBone(char* boneName,float ax, float ay, float az);
	bone* getBoneFromName(char* boneName);
	void drawCheckeredFloor();

public:
	int currentCharacter;
	int numBones;
	int numOfFrames;
	int floorMode;
	int floorCam;
	bool jointsOn;
	bool sceneAxisOn;
	bool jointAxisOn;
	bool floorOn;
	bool headOn;
	float angle;
	Skeleton(char*, char*);
	~Skeleton();
	void display();
	void clearChanges();
	void readAMC(FILE*);
	void readPoseFile(char* filename);
	void drawSceneAxis();
	void changePose();
	void loadFrame(int* frameNum);
};

#endif
