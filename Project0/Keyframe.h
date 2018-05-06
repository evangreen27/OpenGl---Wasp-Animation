// stores time, value, tangents, cubics…
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class Keyframe {


public:
	Keyframe();
	Keyframe(float t, float v, std::string tang_in, std::string tang_out);
	~Keyframe();

	void computeTang();

	float time;
	float value;
	std::string tangent_in;
	std::string tangent_out;
	float tangentVal_in;
	float tangentVal_out;
	float A, B, C, D;
private:

};