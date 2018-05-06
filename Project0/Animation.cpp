#include "Animation.h"

//stores an array of Channels

Animation::Animation(char * path)
{
	//initialize animation
//	glGenBuffers(1, &VertexBuffer);
//	glGenBuffers(1, &IndexBuffer);

	//load animation from file
	Tokenizer token;
	token.Open(path);
	token.FindToken("animation");
	// Parse tree
	Load(token);
	std::cout << "done reading" << std::endl;
	token.Close();
	rootChannel->Precompute();
	for (Channel* ch : rootChannel->children) {
		ch->Precompute();
	}
}

Animation::~Animation()
{
}

bool Animation::Load(Tokenizer &token) {
	token.FindToken("{");
	while (1) {
		char temp[256];
		token.GetToken(temp);
		if (strcmp(temp, "range") == 0) {
			time_start = token.GetFloat();
			time_end = token.GetFloat();
			std::cout << "time_start: " << time_start << " time_end: " << time_end << std::endl;
		}
		else if (strcmp(temp, "numchannels") == 0) {
			numchannels = token.GetInt();
			std::cout << "numchannels: " << numchannels << std::endl;
		}
		else if (strcmp(temp, "channel") == 0) {
			rootChannel = new Channel();
			rootChannel->root = true;
			rootChannel->Load(token);
			return true;
		}
		else if (strcmp(temp, "}") == 0) return true;
		else token.SkipLine(); // Unrecognized token
	}
}

glm::vec3 Animation::Evaluate(float time, glm::vec3 & p, int index)
{
	float valx;
	float valy;
	float valz;

	//root channel / skeleton
	if (index == 0) {
		valx = rootChannel->Evaluate(time);
		valy = rootChannel->children[0]->Evaluate(time);
		valz = rootChannel->children[1]->Evaluate(time);
	}
	else {
		//std::cout << "idx: " << index << std::endl;
		valx = rootChannel->children[index]->Evaluate(time);
		valy = rootChannel->children[index+1]->Evaluate(time);
		valz = rootChannel->children[index+2]->Evaluate(time);
	}
	glm::vec3 val = { valx,valy,valz };
	if(index == 0)
		return val;
	p = val;
	return val;
}
