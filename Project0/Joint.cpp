////////////////////////////////////////
// Joint.cpp
////////////////////////////////////////

#include "Joint.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

Joint::Joint() {
	glGenBuffers(1, &VertexBuffer);
	glGenBuffers(1, &IndexBuffer);

	Count = 0;
}

////////////////////////////////////////////////////////////////////////////////

Joint::~Joint() {
	glDeleteBuffers(1, &IndexBuffer);
	glDeleteBuffers(1, &VertexBuffer);
}


////////////////////////////////////////////////////////////////////////////////

bool Joint::Load(Tokenizer &token) {
	token.FindToken("{");
	while (1) {
		char temp[256];
		token.GetToken(temp);
		if (strcmp(temp, "offset") == 0) {
			offset.x = token.GetFloat();
			offset.y = token.GetFloat();
			offset.z = token.GetFloat();
		}
		else if (strcmp(temp, "balljoint") == 0) {
			Joint *jnt = new Joint;
			jnt->Load(token);
			addChild(jnt);
		}
		else if (strcmp(temp, "boxmin") == 0) {
			boxmin.x = token.GetFloat();
			boxmin.y = token.GetFloat();
			boxmin.z = token.GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0) {
			boxmax.x = token.GetFloat();
			boxmax.y = token.GetFloat();
			boxmax.z = token.GetFloat();
		}
		else if (strcmp(temp, "pose") == 0) {
			pose.x = token.GetFloat();
			pose.y = token.GetFloat();
			pose.z = token.GetFloat();
		}
		else if (strcmp(temp, "rotxlimit") == 0) {
			rotxlimit.x = token.GetFloat();
			rotxlimit.y = token.GetFloat();
		}
		else if (strcmp(temp, "rotylimit") == 0) {
			rotylimit.x = token.GetFloat();
			rotylimit.y = token.GetFloat();
		}
		else if (strcmp(temp, "rotzlimit") == 0) {
			rotzlimit.x = token.GetFloat();
			rotzlimit.y = token.GetFloat();
		}
		else if (strcmp(temp, "}") == 0) return true;
		else token.SkipLine(); // Unrecognized token
			}
}

bool Joint::addChild(Joint * jnt) {
	children.push_back(jnt);
	jnt->parent=this;
	return true;
}

void Joint::Draw(const glm::mat4 &modelMtx, const glm::mat4 &viewProjMtx, uint shader) {
	// Set up shader
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMtx"), 1, false, (float*)&WorldMat);

	glm::mat4 mvpMtx = viewProjMtx * WorldMat;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjMtx"), 1, false, (float*)&mvpMtx);

	// Set up state
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

	uint posLoc = 0;
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), 0);

	uint normLoc = 1;
	glEnableVertexAttribArray(normLoc);
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)12);

	// Draw geometry
	glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, 0);

	// Clean up state
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(posLoc);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

void Joint::MakeBox() {
	// Specify vertices
	//Skeleton::joints.push_back(this);
	std::vector<ModelVertex> vtx = {
		// Front
		{ { boxmin.x,boxmin.y,boxmax.z },{ 0,0,1 } },
		{ { boxmax.x,boxmin.y,boxmax.z },{ 0,0,1 } },
		{ { boxmax.x,boxmax.y,boxmax.z },{ 0,0,1 } },
		{ { boxmin.x,boxmax.y,boxmax.z },{ 0,0,1 } },

		// Back
		{ { boxmax.x,boxmin.y,boxmin.z },{ 0,0,-1 } },
		{ { boxmin.x,boxmin.y,boxmin.z },{ 0,0,-1 } },
		{ { boxmin.x,boxmax.y,boxmin.z },{ 0,0,-1 } },
		{ { boxmax.x,boxmax.y,boxmin.z },{ 0,0,-1 } },

		// Top
		{ { boxmin.x,boxmax.y,boxmax.z },{ 0,1,0 } },
		{ { boxmax.x,boxmax.y,boxmax.z },{ 0,1,0 } },
		{ { boxmax.x,boxmax.y,boxmin.z },{ 0,1,0 } },
		{ { boxmin.x,boxmax.y,boxmin.z },{ 0,1,0 } },

		// Bottom
		{ { boxmin.x,boxmin.y,boxmin.z },{ 0,-1,0 } },
		{ { boxmax.x,boxmin.y,boxmin.z },{ 0,-1,0 } },
		{ { boxmax.x,boxmin.y,boxmax.z },{ 0,-1,0 } },
		{ { boxmin.x,boxmin.y,boxmax.z },{ 0,-1,0 } },

		// Left
		{ { boxmin.x,boxmin.y,boxmin.z },{ -1,0,0 } },
		{ { boxmin.x,boxmin.y,boxmax.z },{ -1,0,0 } },
		{ { boxmin.x,boxmax.y,boxmax.z },{ -1,0,0 } },
		{ { boxmin.x,boxmax.y,boxmin.z },{ -1,0,0 } },

		// Right
		{ { boxmax.x,boxmin.y,boxmax.z },{ 1,0,0 } },
		{ { boxmax.x,boxmin.y,boxmin.z },{ 1,0,0 } },
		{ { boxmax.x,boxmax.y,boxmin.z },{ 1,0,0 } },
		{ { boxmax.x,boxmax.y,boxmax.z },{ 1,0,0 } },
	};

	// Specify indices
	std::vector<uint> idx = {
		0,1,2,		0,2,3,			// Front
		4,5,6,		4,6,7,			// Back
		8,9,10,		8,10,11,		// Top
		12,13,14,	12,14,15,		// Bottom
		16,17,18,	16,18,19,		// Left
		20,21,22,	20,22,23,		// Right
	};

	// Create vertex & index buffers
	SetBuffers(vtx, idx);

	if (pose.x < rotxlimit[0]) {
		pose.x = rotxlimit[0];
	}
	if (pose.x > rotxlimit[1]) {
		pose.x = rotxlimit[1];
	}
	if (pose.y < rotylimit[0]) {
		pose.y = rotylimit[0];
	}
	if (pose.y > rotylimit[1]) {
		pose.y = rotylimit[1];
	}
	if (pose.z < rotzlimit[0]) {
		pose.z = rotzlimit[0];
	}
	if (pose.z > rotzlimit[1]) {
		pose.z = rotzlimit[1];
	}

}

void Joint::SetBuffers(const std::vector<ModelVertex> &vtx, const std::vector<uint> &idx) {
	Count = (int)idx.size();

	// Store vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(ModelVertex), &vtx[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Store index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(uint), &idx[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Joint::MakeAll() {
	for (auto i : children) {
		i->MakeAll();
	}
	MakeBox();
}

void Joint::DrawAll(const glm::mat4 & modelMtx, const glm::mat4 & viewProjMtx, uint shader) {
	Draw(modelMtx, viewProjMtx, shader);
	for (auto i : children) {
		i->DrawAll(modelMtx, viewProjMtx, shader);
	}
}

void Joint::update(){
	if (parent != NULL) {
		if(root == false)
			WorldMat = parent->WorldMat;
		else
			WorldMat = glm::mat4(1.0f);
	}

//DOF limits

//rotate and translate in correct order  (z,y,x)
WorldMat = glm::translate(WorldMat,{ offset.x,offset.y,offset.z });
LocalMat = glm::translate(LocalMat, { offset.x,offset.y,offset.z });
WorldMat = glm::rotate(WorldMat, pose.z, glm::vec3(0.0f, 0.0f, 1.0f));
WorldMat = glm::rotate(WorldMat, pose.y, glm::vec3(0.0f, 1.0f, 0.0f));
WorldMat = glm::rotate(WorldMat, pose.x, glm::vec3(1.0f, 0.0f, 0.0f));

	for (auto i : children) {
		i->update();
	}
}