////////////////////////////////////////
// Skeleton.cpp
////////////////////////////////////////

#include "Skeleton.h"

////////////////////////////////////////////////////////////////////////////////
int Skeleton::jntnum = 0;
std::vector<Joint *>Skeleton::joints;
Skeleton::Skeleton() {
	// Initialize constant data
	Position = glm::vec3(0);
	Axis = glm::vec3(0, 1, 0);		// Y-Axis
	SpinDelta = 0.01f;
	// Resets variable data
	Reset();
}

Skeleton::Skeleton(char * path) {
	// Initialize constant data
	Tokenizer token;
	token.Open(path);
	token.FindToken("balljoint");
	// Parse tree
	//numjnt = 0;
	Root = new Joint;
	Root->Load(token);
	Root->root = true;
	getJoints(Root);
	// Finish
	token.Close();

	//for each joint, set position to the offset

	Position = glm::vec3(0);
	Axis = glm::vec3(0, 1, 0);		// Y-Axis
	Root->MakeAll();
	SpinDelta = 0.01f;

	// Resets variable data
	Reset();
}

////////////////////////////////////////////////////////////////////////////////

void Skeleton::Update() {
	// Update (animate) any variable properties
	Root->update();
}

void Skeleton::getJoints(Joint * Root) {
	Skeleton::joints.push_back(Root);
	for (auto i : Root->children) {
		getJoints(i);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Skeleton::Reset() {
	// Reset dynamic variables to a default state
	//Angle = 0;
	//WorldMtx = glm::mat4(1);		// Reset to identity matrix
}

////////////////////////////////////////////////////////////////////////////////

void Skeleton::Draw(const glm::mat4 &viewProjMtx, uint shader) {
		Root->DrawAll(WorldMtx, viewProjMtx, shader);
}

////////////////////////////////////////////////////////////////////////////////
