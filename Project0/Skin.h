////////////////////////////////////////
// Skin.h
////////////////////////////////////////

#pragma once

#include "Joint.h"
#include "Vertex.h"
#include "Triangle.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class Skin {

	struct ModelVertex {
		glm::vec3 Position;
		glm::vec3 Normal;
	};

public:
	Skin(char* path, Joint * Root);
	~Skin();

	void Draw(const glm::mat4 &viewProjMtx, uint shader);
	//void drawAll(const glm::mat4 &viewProjMtx, uint shader);
	void SetBuffers();
	void Update(Joint *);

	void UpdateVerts();

	bool LoadPos(Tokenizer &token);
	bool LoadNorm(Tokenizer &token);
	bool LoadSW(Tokenizer &token);
	bool LoadTri(Tokenizer &token);
	bool LoadBind(Tokenizer &token);
	void Update();
	void Draw();
	void Skin::mapJoints(Joint * Root);

	
	std::vector<GLuint> triangles;
	std::vector<Triangle *> triangleptrs;
	std::vector<std::vector<float>> skinweights;
	std::vector<glm::mat4> bindings;
	std::vector<Vertex *> Vertices;
	int count;
	int cnt;
	int jntnum = 0;
	int n;
	int tris;
	std::vector<Joint *> joints;
	std::vector<ModelVertex> vtx;

private:
	uint VertexBuffer;
	uint IndexBuffer;
};

////////////////////////////////////////////////////////////////////////////////

/*
positions[num]{
	[x] [y] [z]
}
normals[num]{
	[x] [y] [z]
}
skinweights[num]{
	[numbinds] [joint0
	] [weight0
	] [j1
	] [w1
	] …[jN - 1
	][wN - 1
	]
}
triangles[num]{
	[index0
	] [index1
	] [index2
	]
}
bindings[num]{
	matrix{
	[ax] [ay] [az] [bx] [by] [bz] [cx] [cy] [cz] [dx] [dy] [dz]
	*/