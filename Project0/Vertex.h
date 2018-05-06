////////////////////////////////////////
// Vertex.h
////////////////////////////////////////

#pragma once

#include "core.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class Vertex {

public:
	Vertex();
	~Vertex();

	glm::vec4 position = { 0.0f,0.0f,0.0f,1.0f };
	glm::vec4 normal = { 0.0f,0.0f,0.0f,0.0f };
	glm::vec4 blendedPos = { 0.0f,0.0f,0.0f,1.0f };
	glm::vec4 blendedNorm = { 0.0f,0.0f,0.0f,0.0f };

private:

};