////////////////////////////////////////
// Joint.h
////////////////////////////////////////

#pragma once

#include "Tokenizer.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
class Joint {

	struct ModelVertex {
		glm::vec3 Position;
		glm::vec3 Normal;
	};

public:
	Joint();
	~Joint();

	//void Draw(const glm::mat4 &modelMtx, const glm::mat4 &viewProjMtx, uint shader);

	bool Load(Tokenizer & t);
	bool addChild(Joint * jnt);

	void Draw(const glm::mat4 & modelMtx, const glm::mat4 & viewProjMtx, uint shader);
	void DrawAll(const glm::mat4 & modelMtx, const glm::mat4 & viewProjMtx, uint shader);

	void update();

	void MakeBox();

	void MakeAll();

	void SetBuffers(const std::vector<ModelVertex>& vtx, const std::vector<uint>& idx);

	// Access functions

	int Count;
	Joint * parent;
	std::vector<Joint *> children;
	glm::vec3 offset = { 0.0f,0.0f,0.0f };
	glm::vec3 boxmin = { -0.1,-0.1,-0.1 };
	glm::vec3 boxmax = { 0.1,0.1,0.1 };
	glm::vec3 pose = { 0,0,0 };
	glm::vec2 rotxlimit = { -100000,100000 };
	glm::vec2 rotylimit = { -100000,100000 };
	glm::vec2 rotzlimit = { -100000,100000 };

	glm::mat4 LocalMat = glm::mat4(1.0f);
	glm::mat4 WorldMat = glm::mat4(1.0f);
	glm::mat4 SkinningMat = glm::mat4(1.0f);
	bool root = false;
	int num = 0;

private:
	uint VertexBuffer;
	uint IndexBuffer;
};

////////////////////////////////////////////////////////////////////////////////
