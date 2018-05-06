/*stores pointer to an animation & pointer to
skeleton. Keeps track of time, accesses animation
data & poses the skeleton.*/
#include "Core.h"
#include "Animation.h"
#include "Skeleton.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class Player {


public:
	Player();
	Player(Skeleton * s, Animation * a);
	~Player();

	void Update();


	Animation * animptr;
	Skeleton * skelptr;
	float timepassed = 0.0f;
private:

};