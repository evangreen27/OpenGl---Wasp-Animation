//stores an array (or list) of Keyframes
#pragma once

#include "Core.h"
#include "Keyframe.h"
#include "Joint.h"
#include "Vertex.h"
#include <iostream>
#include <vector>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class Channel {


public:
	Channel();
	~Channel();

	bool Load(Tokenizer & token);
	float Evaluate(float time);	void Precompute();

	void computeTang(Keyframe * low, Keyframe * high, int k);

	void computeCoefficients(Keyframe * low, Keyframe * high);

	glm::vec4 computeCoefficients(Keyframe * low, Keyframe * high, float val);

	float computeValue(Keyframe * low, Keyframe * high, float time);

	float computeValue(Keyframe * low, Keyframe * high, float time, glm::vec4 vals);

	std::vector<Keyframe *> keyframes;
	std::string extrap_in;
	std::string extrap_out;
	float numkeys;
	std::vector<Channel *> children;
	bool root = false;
	Channel * rootChannel;
private:
	
};