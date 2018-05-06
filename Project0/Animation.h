//stores an array of Channels
#pragma once
#include "Joint.h"
#include "Vertex.h"
#include "Triangle.h"
#include <iostream>
#include "Channel.h"
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class Animation {


public:
	Animation(char * path);
	~Animation();
	bool Load(Tokenizer & token);
	glm::vec3 Evaluate(float time, glm::vec3 &p, int index);

	int numchannels;
	Channel * rootChannel;
	float time_start;
	float time_end;

private:

};