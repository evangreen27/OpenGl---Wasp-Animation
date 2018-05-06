////////////////////////////////////////
// Skin.cpp
////////////////////////////////////////

#include "Skin.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

Skin::Skin(char * path, Joint* Root) {
	glGenBuffers(1, &VertexBuffer);
	glGenBuffers(1, &IndexBuffer);

	cnt = 0;

	Tokenizer token;
	token.Open(path);
	token.FindToken("positions");
	LoadPos(token);
	token.FindToken("normals");
	LoadNorm(token);
	token.FindToken("skinweights");
	LoadSW(token);
	token.FindToken("triangles");
	LoadTri(token);
	token.FindToken("bindings");
	LoadBind(token);
	// Finish
	token.Close();

	// Resets variable data
	//Reset();

	mapJoints(Root);

}

////////////////////////////////////////////////////////////////////////////////

Skin::~Skin() {
	glDeleteBuffers(1, &IndexBuffer);
	glDeleteBuffers(1, &VertexBuffer);
}


////////////////////////////////////////////////////////////////////////////////

bool Skin::LoadPos(Tokenizer &token) {
	int num = token.GetInt();
	n = num;
	token.FindToken("{");
	count = 0;
	for(count = 0; count < num; count++) {
		token.SkipLine();
		Vertex* v = new Vertex;
		v->position.x = token.GetFloat();
		v->position.y = token.GetFloat();
		v->position.z = token.GetFloat();
		//std::cout << v->position.x << std::endl;
		Vertices.push_back(v);
	}
	return true;
}

void Skin::mapJoints(Joint * Root) {
	joints.push_back(Root);
	for (auto i : Root->children)
		mapJoints(i);
}

bool Skin::LoadNorm(Tokenizer &token) {
	int num = token.GetInt();
	token.FindToken("{");
	for (count = 0; count < num; count++) {
		token.SkipLine();
		Vertices[count]->normal.x = token.GetFloat();
		Vertices[count]->normal.y = token.GetFloat();
		Vertices[count]->normal.z = token.GetFloat();
		//std::cout << Vertices[count].normal.x << std::endl;
	}
	return true;
}

bool Skin::LoadSW(Tokenizer &token) {
	int num = token.GetInt();
	skinweights = std::vector<std::vector<float>>(num);
	token.FindToken("{");
	for (count = 0; count < num; count++) {
		//token.SkipLine();
		float amt = token.GetFloat() * 2;
		for (int i=0; i < amt; i++) {
		skinweights[count].push_back(token.GetFloat());
		//std::cout << "SW: " << skinweights[count][i] << std::endl;
		}
	}
	return true;

}

bool Skin::LoadTri(Tokenizer &token) {
	int num = token.GetInt();
	tris = num;
	token.FindToken("{");
	for (count = 0; count < num; count++) {
		token.SkipLine();
		uint x = token.GetInt();
		uint y = token.GetInt();
		uint z = token.GetInt();
		//std::cout << triangles[count].x << std::endl;
		//std::cout << triangles[count].y << std::endl;
		//std::cout << triangles[count].z << std::endl;
		triangles.push_back(x);
		triangles.push_back(y);
		triangles.push_back(z);
	}
	return true;
}

bool Skin::LoadBind(Tokenizer &token) {
	int num = token.GetInt();
	for (count = 0; count < num; count++) {
		token.FindToken("matrix");
		token.FindToken("{");

		glm::mat4 bMat;
		bMat[0][0] = token.GetFloat();
		bMat[0][1] = token.GetFloat();
		bMat[0][2] = token.GetFloat();

		bMat[1][0] = token.GetFloat();
		bMat[1][1] = token.GetFloat();
		bMat[1][2] = token.GetFloat();

		bMat[2][0] = token.GetFloat();
		bMat[2][1] = token.GetFloat();
		bMat[2][2] = token.GetFloat();

		bMat[3][0] = token.GetFloat();
		bMat[3][1] = token.GetFloat();
		bMat[3][2] = token.GetFloat();

		bMat[0][3] = 0.0f;
		bMat[1][3] = 0.0f;
		bMat[2][3] = 0.0f;

		bMat[3][3] = 1.0f;

		bMat = glm::inverse(bMat);
		bindings.push_back(bMat);
	}
	return true;
}

void Skin::Update(Joint * Root) {
	/*Compute skinning matrix for each joint: M=W·B-1
	(you can precompute and store B-1 instead of B)
	 Loop through vertices and compute blended position & normal*/
	Root->SkinningMat = Root->WorldMat * bindings[jntnum];
	jntnum++;
	
	for (auto i : Root->children) {
		Update(i);
	}
} 

void Skin::UpdateVerts() {
	jntnum = 0;
	vtx.clear();
	int i = 0;
	while(i < n){
		if (skinweights[i].size() == 1) {
			glm::vec4 blendPos = joints[skinweights[i][0]]->SkinningMat * Vertices[i]->position;
			glm::vec4 blendNorm = joints[skinweights[i][0]]->SkinningMat * Vertices[i]->normal;
			Vertices[i]->blendedPos = blendPos;
			Vertices[i]->blendedNorm = blendNorm;
		}
		else if (skinweights[i].size() == 2) {
			glm::vec4 blendPos = joints[skinweights[i][0]]->SkinningMat * Vertices[i]->position;
			glm::vec4 blendNorm = joints[skinweights[i][0]]->SkinningMat * Vertices[i]->normal;
			Vertices[i]->blendedPos = blendPos;
			Vertices[i]->blendedNorm = blendNorm;
		}
		else if (skinweights[i].size() == 4) {
			glm::vec4 w1v1 = joints[skinweights[i][0]]->SkinningMat * Vertices[i]->position;
			glm::vec4 w2v2 = joints[skinweights[i][2]]->SkinningMat * Vertices[i]->position;
			//std::cout << "w1: " << skinweights[i][1] << "w2: " << skinweights[i][3] << std::endl;
			glm::vec4 wmv1 = w1v1 * skinweights[i][1];
			glm::vec4 wmv2 = w2v2 * skinweights[i][3];
			glm::vec4 blendPos = wmv1 + wmv2;
			Vertices[i]->blendedPos = blendPos;

			glm::vec4 nw1v1 = joints[skinweights[i][0]]->SkinningMat * Vertices[i]->normal;
			glm::vec4 nw2v2 = joints[skinweights[i][2]]->SkinningMat * Vertices[i]->normal;
			glm::vec4 nwmv1 = nw1v1 * skinweights[i][1];
			glm::vec4 nwmv2 = nw2v2 * skinweights[i][3];
			glm::vec4 blendNorm = nwmv1 + nwmv2;
			Vertices[i]->blendedNorm = blendNorm;
		}
		else if (skinweights[i].size() == 6) {
			glm::mat4 w1v1 = joints[skinweights[i][0]]->SkinningMat * skinweights[i][1];
			glm::mat4 w2v2 = joints[skinweights[i][2]]->SkinningMat * skinweights[i][3];
			glm::mat4 w3v3 = joints[skinweights[i][4]]->SkinningMat * skinweights[i][5];
			glm::vec4 wmv1 = w1v1 * Vertices[i]->position;
			glm::vec4 wmv2 = w2v2 * Vertices[i]->position;
			glm::vec4 wmv3 = w3v3 * Vertices[i]->position;
			glm::vec4 Nwmv1 = w1v1 * Vertices[i]->normal;
			glm::vec4 Nwmv2 = w2v2 * Vertices[i]->normal;
			glm::vec4 Nwmv3 = w3v3 * Vertices[i]->normal;
			glm::vec4 blendPos = wmv1 + wmv2 + wmv3;
			Vertices[i]->blendedPos = blendPos;
			glm::vec4 blendNorm = Nwmv1 + Nwmv2 + Nwmv3;
			Vertices[i]->blendedNorm = blendNorm;
		}
		else if (skinweights[i].size() == 8) {
			glm::mat4 w1v1 = joints[skinweights[i][0]]->SkinningMat * skinweights[i][1];
			glm::mat4 w2v2 = joints[skinweights[i][2]]->SkinningMat * skinweights[i][3];
			glm::mat4 w3v3 = joints[skinweights[i][4]]->SkinningMat * skinweights[i][5];
			glm::mat4 w4v4 = joints[skinweights[i][6]]->SkinningMat * skinweights[i][7];
			glm::vec4 wmv1 = w1v1 * Vertices[i]->position;
			glm::vec4 wmv2 = w2v2 * Vertices[i]->position;
			glm::vec4 wmv3 = w3v3 * Vertices[i]->position;
			glm::vec4 wmv4 = w4v4 * Vertices[i]->position;
			glm::vec4 Nwmv1 = w1v1 * Vertices[i]->normal;
			glm::vec4 Nwmv2 = w2v2 * Vertices[i]->normal;
			glm::vec4 Nwmv3 = w3v3 * Vertices[i]->normal;
			glm::vec4 Nwmv4 = w4v4 * Vertices[i]->normal;
			glm::vec4 blendPos = wmv1 + wmv2 + wmv3 + wmv4;
			Vertices[i]->blendedPos = blendPos;
			glm::vec4 blendNorm = Nwmv1 + Nwmv2 + Nwmv3 + Nwmv3;
			Vertices[i]->blendedNorm = blendNorm;
		}
		else {
			std::cout << "skinweights has wrong amount of nums--->" << skinweights[i].size() << std::endl;
		}
		Vertices[i]->blendedNorm = glm::normalize(Vertices[i]->blendedNorm);
		ModelVertex toParse;
		toParse.Normal = Vertices[i]->blendedNorm;
		toParse.Position = Vertices[i]->blendedPos;
		vtx.push_back(toParse);
		i++;
	}
	SetBuffers();
}

void Skin::Draw(const glm::mat4 &viewProjMtx, uint shader) {
	// Set up shader
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMtx"), 1, false, (float*)&joints[0]->WorldMat);

	glm::mat4 mvpMtx = viewProjMtx * joints[0]->WorldMat;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjMtx"), 1, false, (float*)&mvpMtx);
	//glUniform1i(glGetUniformLocation(shader, "wireFrame"), 0);

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
	glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, 0);

	// Clean up state
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(posLoc);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

void Skin::SetBuffers() {
	cnt = tris * 3;
	
	// Store vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(ModelVertex), &vtx[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Store index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(uint), &triangles[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////