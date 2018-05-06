#include "Player.h"
/*stores pointer to an animation & pointer to
skeleton. Keeps track of time, accesses animation
data & poses the skeleton.*/

Player::Player()
{
}

Player::Player(Skeleton * s, Animation * a)
{
	skelptr = s;
	animptr = a;
	timepassed = a->time_start;
}

Player::~Player()
{
}

void Player::Update() {
	if (timepassed >= animptr->time_end) {
		timepassed = animptr->time_start + 0.001f;
	}
	else
		timepassed += 0.001f;
	//std::cout << "timepassed: " << timepassed << std::endl;
	if (timepassed > animptr->time_start && timepassed < animptr->time_end) {
		glm::vec3 newpos = animptr->Evaluate(timepassed, skelptr->Root->pose, 0);
		skelptr->Root->offset = newpos;
		 animptr->Evaluate(timepassed, skelptr->Root->pose, 2);
		int chanidx = 5;
		for (int i = 1; i < skelptr->joints.size(); i++) {
			animptr->Evaluate(timepassed, skelptr->joints[i]->pose, chanidx);
			//std::cout << "posex: " << news.x << " posey: " << news.y << " posez: " << news.z << std::endl;
			chanidx += 3;
		}
	}
}
