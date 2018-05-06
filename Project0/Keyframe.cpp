#include "Keyframe.h"
// stores time, value, tangents, cubics…

Keyframe::Keyframe()
{
}

Keyframe::Keyframe(float t, float v, std::string tang_in, std::string tang_out)
{
	time = t;
	value = v;
	tangent_in = tang_in;
	tangent_out = tang_out;
}

Keyframe::~Keyframe()
{
}

void Keyframe::computeTang() {

}