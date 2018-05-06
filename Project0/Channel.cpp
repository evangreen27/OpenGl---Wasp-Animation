#include "Channel.h"
//stores an array (or list) of Keyframes

Channel::Channel()
{
}

Channel::~Channel()
{
	children.clear();
}

bool Channel::Load(Tokenizer &token) {
	token.FindToken("{");
	while (1) {
		char temp[256];
		token.GetToken(temp);
		if (strcmp(temp, "extrapolate") == 0) {
			token.GetToken(temp);
			extrap_in = temp;
			token.GetToken(temp);
			extrap_out = temp;
			//std::cout << "extrap_in: " << extrap_in << " extrap_out: " << extrap_out << std::endl;
		}
		else if (strcmp(temp, "keys") == 0) {
			int num = token.GetInt();
			std::cout << "num: " << num << std::endl;
			token.FindToken("{");
			int count = 0;
			for (count = 0; count < num; count++) {
				float t = token.GetFloat();
				float v = token.GetFloat();
				token.GetToken(temp);
				std::string tang_in = temp;
				token.GetToken(temp);
				std::string tang_out = temp;

				keyframes.push_back(new Keyframe(t, v, tang_in, tang_out));

				std::cout << "time: " << t << " val: " << v << "tangin:" << temp << std::endl;
			}
			token.FindToken("}");
			token.FindToken("}");
		}
		else if (strcmp(temp, "channel") == 0) {
			Channel *ch = new Channel();
			std::cout << "creating channel " << std::endl;
			if (root) {
				ch->rootChannel = this;
				children.push_back(ch);
			}
			else {
				ch->rootChannel = rootChannel;
				rootChannel->children.push_back(ch);
			}
			//if (!root)
			//	std::cout << "Child " << rootChannel->children.size() << std::endl;
			ch->Load(token);	
			return true;
		}
		else if (strcmp(temp, "}") == 0) {
			return true;
		}
		else token.SkipLine(); // Unrecognized token
	}
}

float Channel::Evaluate(float time)
{
	/* 4 Cases to consider
	 t is before the first key (use extrapolation)
	 t is after the last key (use extrapolation)
	 t falls exactly on some key (return key value)
	 t falls between two keys (evaluate cubic equation)
	 

	 1. Find the proper span (binary search), start with linear search then optimize
	 2. Evaluate the cubic equation for the span
	*/
	float newtime = time;
	if (keyframes.size() == 0)
		return 0;
	else if (keyframes.size() == 1)
		return keyframes[0]->value;

	if (time < keyframes[0]->time) {
		//std::cout << "EXTRAP PRE" << std::endl;
		if (extrap_in == "constant") {
			return keyframes[0]->value;
		}
		//not used
		else if (extrap_in == "linear") {
			float tprime = time - keyframes[0]->time;
			float m = keyframes[0]->tangentVal_out;
			float b = keyframes[0]->value;
			float x = b - (m * tprime);
			return x;
		}
		else if (extrap_in == "cycle") {
			while (newtime < keyframes[0]->time) {
				newtime = newtime + (keyframes[1]->time - keyframes[0]->time);
			}
			//std::cout << "VALOUT: " << keyframes[0]->tangentVal_out << std::endl;
			float x = computeValue(keyframes[0], keyframes[1], newtime);
			return x;
		}
		else if (extrap_in == "cycle_offset") {
			float offset = 0;
			while (newtime < keyframes[0]->time) {
				newtime = newtime + (keyframes[1]->time - keyframes[0]->time);
				offset = offset + (keyframes[1]->value - keyframes[0]->value);
			}
			if (offset < 0)
				offset *= -1;
			glm::vec4 vals = computeCoefficients(keyframes[0], keyframes[1], offset);
			float x = computeValue(keyframes[0], keyframes[1], newtime, vals);

			return x;
		}
		//not used
		else if (extrap_in == "bounce") {
			int i = 0;
			while (newtime >keyframes[0]->time)
			{
				newtime = newtime + (keyframes[1]->time - keyframes[0]->time);
				i++;
			}

			//odd number
			if (i % 2 != 0)
			{
				newtime = keyframes[0]->time + abs((keyframes[1]->time - keyframes[0]->time) - newtime);
			}
		}
		else {
			std::cout << "something went wrong" << std::endl;
		}
	}

	else if (time > keyframes[keyframes.size()-1]->time) {
		//std::cout << "EXTRAP POST" << std::endl;
		if (extrap_in == "constant") {
			return keyframes[keyframes.size()-1]->value;
		}
		//not used
		else if (extrap_out == "linear") {
			float tprime = time - keyframes[keyframes.size() - 1]->time;
			float m = keyframes[keyframes.size() - 1]->tangentVal_out;
			float b = keyframes[keyframes.size() - 1]->value;
			float x = b + (m * tprime);
			return x;
		}
		else if (extrap_out == "cycle") {
			while (newtime < keyframes[keyframes.size()-1]->time) {
				newtime = newtime - (keyframes[keyframes.size() - 1]->time - keyframes[keyframes.size() - 2]->time);
			}

			float x = computeValue(keyframes[keyframes.size() - 2], keyframes[keyframes.size() - 1], newtime);
			return x;
		}
		else if (extrap_out == "cycle_offset") {
			float offset = 0;
			while (newtime < keyframes[keyframes.size() - 1]->time) {
				newtime = newtime - (keyframes[keyframes.size() - 1]->time - keyframes[keyframes.size() - 2]->time);
				offset = offset - (keyframes[keyframes.size() - 1]->value - keyframes[keyframes.size() - 2]->value);
			}

			if (offset < 0)
				offset *= -1;
			glm::vec4 vals = computeCoefficients(keyframes[keyframes.size() - 2], keyframes[keyframes.size() - 1], offset);
			//std::cout << " Vals.x: " << vals[0] << " Vals.y: " << vals[1] << " Vals.z: " << vals[2] << " Vals.w: " << vals[3] << std::endl;
			float x = computeValue(keyframes[keyframes.size() - 2], keyframes[keyframes.size() - 1], newtime, vals);
			return x;
		}
		//not used
		else if (extrap_out == "bounce") {
			int i = 0;
			while (newtime >keyframes[keyframes.size() - 1]->time)
			{
				newtime = newtime - (keyframes[keyframes.size() - 1]->time - keyframes[keyframes.size() - 2]->time);
				i++;
			}

			//odd number
			if (i % 2 != 0)
			{
				newtime = keyframes[0]->time + abs((keyframes[keyframes.size() - 1]->time - keyframes[keyframes.size() - 2]->time) - newtime);
			}
		}
		else {
		std::cout << "something went wrong" << std::endl;
		}
	}
	//std::cout << "HHHHHHHHHHHHHHHHHHHHHHH" << std::endl;
	//compute which keyframes time is between
	Keyframe * low = keyframes[0];
	Keyframe * high = keyframes[keyframes.size() - 1];
	int index = 0;
	for (int k = 0; k < keyframes.size(); k++) {
		if (keyframes[k]->time == time) {
			return keyframes[k]->value;
		}
		else if (time < keyframes[k]->time) {
			if (k > 0) {
				low = keyframes[k - 1];
				index = k - 1;
			}
			high = keyframes[k];
			break;
		}
	}

	//evaluate cubic between low key and high key

	float x = computeValue(low, high, time);
	return x;
    
}

void Channel::Precompute() {
	//Compute tangents from rules
	//Compute cubic coefficients from tangents & other data

	/* EXTRAPOLATION MODES - entire channel not keys
	 Constant value (hold first/last key value)
	 Linear (use tangent at first/last key)
	 Cyclic (repeat the entire channel)
	 Cyclic Offset (repeat with value offset)
	 Bounce (repeat alternating backwards & forwards)
	*/
	if (keyframes.size() == 0) {
		return;
	}
	if (keyframes.size() == 1)
	{
		keyframes[0]->tangentVal_in = 0;
		keyframes[0]->tangentVal_out = 0;
		return;
	}

		computeTang(keyframes[0], keyframes[0], 0);

	for (int i = 0; i < keyframes.size() - 1; i++) {
		computeCoefficients(keyframes[i], keyframes[i + 1]);
	}
}

void Channel::computeTang(Keyframe * low, Keyframe * high, int k) {
	
	float v = 0;

	for (int i = 0; i < keyframes.size(); i++)
	{
		Keyframe * curr = keyframes[i];
		std::string in = curr->tangent_in;

		if (in == "linear" || in == "smooth" && i == 0 || in == "smooth" && i == (keyframes.size() - 1)) //first last won't  work for smooth
		{
			if (i == 0) //first tangentIn for linear extend tangentOut
			{

				curr->tangentVal_in = 0;
			}
			else
			{
				Keyframe * prevKey = keyframes[i - 1];
				float p1 = curr->value;
				float p0 = prevKey->value;
				float t1 = curr->time;
				float t0 = prevKey->time;

				float tanIn = (p1 - p0) / (t1 - t0);
				curr->tangentVal_in=tanIn;
			}
		}
		else if (in == "flat")
		{
			curr->tangentVal_in = 0;
		}
		else if (in == "smooth")
		{
			Keyframe * prevKey = keyframes[i - 1];
			Keyframe * nextKey = keyframes[i + 1];
			float p2 = nextKey->value;
			float p0 = prevKey->value;
			float t2 = nextKey->time;
			float t0 = prevKey->time;

			float tanIn = (p2 - p0) / (t2 - t0);
			curr->tangentVal_in = tanIn;
		}

		/*----------------------------------------OUT TANGENT----------------------------------------*/

		std::string out = curr->tangent_out;
		if (out == "linear" || in == "smooth" && i == 0 || in == "smooth" && i == (keyframes.size() - 1))
		{
			//extend the tangentIn
			if (i == (keyframes.size() - 1))
				curr->tangentVal_out = curr->tangentVal_in;
			else
			{
				Keyframe * nextKey = keyframes[i + 1];
				float p0 = curr->value;
				float p1 = nextKey->value;
				float t0 = curr->time;
				float t1 = nextKey->time;

				float tanOut = (p1 - p0) / (t1 - t0);
				curr->tangentVal_out = tanOut;
			}
		}
		else if (out == "flat")
		{
			curr->tangentVal_out = 0;
		}
		else if (out == "smooth")
		{
			Keyframe * prevKey = keyframes[i - 1];
			Keyframe * nextKey = keyframes[i + 1];
			float p2 = nextKey->value;
			float p0 = prevKey->value;
			float t2 = nextKey->time;
			float t0 = prevKey->time;

			float tanOut = (p2 - p0) / (t2 - t0);
			curr->tangentVal_out = tanOut;
		}
	}
}

void Channel::computeCoefficients(Keyframe * low, Keyframe * high) {
	for (int i = 0; i < keyframes.size() - 1; i++)
	{
		//find span of curr keyframe to next keyframe
		Keyframe * curr = keyframes[i];
		Keyframe * nextKey = keyframes[i + 1];
		float p0 = curr->value;
		float p1 = nextKey->value;
		float t0 = curr->time;
		float t1 = nextKey->time;

		//outgoing tangent of first and incoming tangent of second
		float v0 = curr->tangentVal_out;
		float v1 = nextKey->tangentVal_in;

		float interpolateT = (t1 - t0);

		v0 = interpolateT * v0;
		v1 = interpolateT * v1;

		float x = (2.0f * p0) - (2.0f * p1) + v0 + v1;
		//was getting really tiny numbers that is clsoe to 0, just set to 0
		float y = (-3.0f * p0) + (3.0f * p1) - (2.0f * v0) - v1;

		float z = v0;
		float w = p0;
		curr->A = x;
		curr->B = y;
		curr->C = z;
		curr->D = w;

		//std::cout << i << ": " << "(" << x << ", " << y << ", " << z << ", " << w << ")" << std::endl;
	}

	//last keyframe has no span
	
}

glm::vec4 Channel::computeCoefficients(Keyframe * low, Keyframe * high, float val) {
	for (int i = 0; i < keyframes.size() - 1; i++)
	{
		//find span of curr keyframe to next keyframe
		Keyframe * curr = keyframes[i];
		Keyframe * nextKey = keyframes[i + 1];
		float p0 = curr->value + val;
		float p1 = nextKey->value + val;
		float t0 = curr->time;
		float t1 = nextKey->time;

		//outgoing tangent of first and incoming tangent of second
		float v0 = curr->tangentVal_out;
		float v1 = nextKey->tangentVal_in;

		float interpolateT = (t1 - t0);

		v0 = interpolateT * v0;
		v1 = interpolateT * v1;

		float x = (2.0f * p0) - (2.0f * p1) + v0 + v1;
		//was getting really tiny numbers that is clsoe to 0, just set to 0
		float y = (-3.0f * p0) + (3.0f * p1) - (2.0f * v0) - v1;

		float z = v0;
		float w = p0;
		curr->A = x;
		curr->B = y;
		curr->C = z;
		curr->D = w;

		return glm::vec4(x, y, z, w);
		//std::cout << i << ": " << "(" << x << ", " << y << ", " << z << ", " << w << ")" << std::endl;
	}
}

float Channel::computeValue(Keyframe * low, Keyframe * high, float time) {
	float u = (time - low->time) / (high->time - low->time);
	float x = (low->A*pow(u, 3)) + (low->B*pow(u, 2)) + (low->C*u) + low->D;
	return x;
}

float Channel::computeValue(Keyframe * low, Keyframe * high, float time, glm::vec4 vals) {
	float u = (time - low->time) / (high->time - low->time);
	float x = (vals[0]*pow(u, 3)) + (vals[1]*pow(u, 2)) + (vals[2]*u) + vals[3];
	return x;
}
