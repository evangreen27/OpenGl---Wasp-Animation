//----------------------------------------------------------------------------
// Skeleton.h
//----------------------------------------------------------------------------

#pragma once

#include "Model.h"
#include "Joint.h"


//----------------------------------------------------------------------------

// SpinningCube is an example of a basic animating object. It can be used as a
// pattern for creating more complex objects.
class Skeleton {
public:
	Skeleton();
	Skeleton(char * path);

	void Update();
	void getJoints(Joint * Root);
	void Reset();
	void Draw(const glm::mat4 &viewProjMtx, uint shader);
	Joint* Root;
	static std::vector<Joint *> joints;
	static int jntnum;

private:
	// Constants
	std::vector<Model> skeleModels;
	glm::vec3 Position;
	glm::vec3 Axis;
	float SpinDelta;

	// Variables
	float Angle;
	glm::mat4x4 WorldMtx;
};

//----------------------------------------------------------------------------
