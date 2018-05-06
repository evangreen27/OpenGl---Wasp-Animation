////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Tester.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

static Tester *TESTER=0;
int main(int argc, char **argv) {
	glutInit(&argc, argv);

	TESTER=new Tester("Skeleton",argc,argv);
	glutMainLoop();
	delete TESTER;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// These are really HACKS to make glut call member functions instead of static functions
static void display()									{TESTER->Draw();}
static void idle()										{TESTER->Update();}
static void resize(int x,int y)							{TESTER->Resize(x,y);}
static void keyboard(unsigned char key,int x,int y)		{TESTER->Keyboard(key,x,y);}
static void mousebutton(int btn,int state,int x,int y)	{TESTER->MouseButton(btn,state,x,y);}
static void mousemotion(int x, int y)					{TESTER->MouseMotion(x,y);}

////////////////////////////////////////////////////////////////////////////////

Tester::Tester(const char *windowTitle,int argc,char **argv) {
	WinX=800;
	WinY=600;
	LeftDown=MiddleDown=RightDown=false;
	MouseX=MouseY=0;

	// Create the window
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( WinX, WinY );
	glutInitWindowPosition( 100, 100 );
	WindowHandle = glutCreateWindow( windowTitle );
	glutSetWindowTitle( windowTitle );
	glutSetWindow( WindowHandle );

	// Background color
	glClearColor( 0., 0., 0., 1. );

	// Callbacks
	glutDisplayFunc( display );
	glutIdleFunc( idle );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mousebutton );
	glutMotionFunc( mousemotion );
	glutPassiveMotionFunc( mousemotion );
	glutReshapeFunc( resize );

	// Initialize GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Initialize components
	Program=new ShaderProgram("Model.glsl",ShaderProgram::eRender);
	//-------------------------

	testAnim = new Animation("wasp_walk.anim");

	if (argc == 3) {
		testSkele = new Skeleton(argv[1]);
		testSkin = new Skin(argv[2], testSkele->Root);
	}
	else {
		testSkele = new Skeleton("wasp.skel");
		testSkin = new Skin("wasp.skin", testSkele->Root);
	}
	player = new Player(testSkele, testAnim);
	Cam = new Camera;
	Cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

Tester::~Tester() {
	delete Program;
	delete testSkele;
	delete Cam;

	glFinish();
	glutDestroyWindow(WindowHandle);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Update() {
	// Update the components in the world
	testSkele->Update();
	testSkin->Update(testSkele->Root);
	testSkin->UpdateVerts();
	player->Update();

	Cam->Update();

	// Tell glut to re-display the scene
	glutSetWindow(WindowHandle);
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Reset() {
	Cam->Reset();
	Cam->SetAspect(float(WinX)/float(WinY));

	testSkele->Reset();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Draw() {
	// Begin drawing scene
	glViewport(0, 0, WinX, WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw components
	//testSkele->Draw(Cam->GetViewProjectMtx(),Program->GetProgramID());
	testSkin->Draw(Cam->GetViewProjectMtx(), Program->GetProgramID());

	// Finish drawing scene
	glFinish();
	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Quit() {
	glFinish();
	glutDestroyWindow(WindowHandle);
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Resize(int x,int y) {
	WinX = x;
	WinY = y;
	Cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Keyboard(int key,int x,int y) {
	switch(key) {
		case 0x1b:		// Escape
			Quit();
			break;
		case 'r':
			Reset();
			break;
		case 'n':
			//next DOF
			if (currRot == 2) {
				if (Skeleton::joints.size() > Skeleton::jntnum + 1)
					Skeleton::jntnum++;
				else
					Skeleton::jntnum = 0;
				currRot = 0;
			}
			else if (currRot == 0) {
				currRot = 1;
			}
			else {
				currRot = 2;
			}
			break;
		case 'b':
			//prev DOF
			if (currRot == 2) {
				currRot = 1;
			}
			else if (currRot == 1) {
				currRot = 0;
			}
			else {
				currRot = 0;
				if (Skeleton::jntnum - 1 >= 0)
					Skeleton::jntnum--;
				else
					Skeleton::jntnum = Skeleton::joints.size()-1;
			}
			break;
		case 'c':
			//decrease rot
			if (currRot == 0) {
				std::cout << "Joint # " << Skeleton::jntnum << " pose.x -0.2" << std::endl;
				Skeleton::joints[Skeleton::jntnum]->pose.x -= 0.2;
			}
			else if (currRot == 1) {
				std::cout << "Joint # " << Skeleton::jntnum << " pose.y -0.2" << std::endl;
				Skeleton::joints[Skeleton::jntnum]->pose.y -= 0.2;
			}
			else {
				std::cout << "Joint # " << Skeleton::jntnum << " pose.z -0.2" << std::endl;
				Skeleton::joints[Skeleton::jntnum]->pose.z -= 0.2;
			}
			break;
		case 'v':
			//increase rot
			if (currRot == 0) {
				std::cout << "Joint # " << Skeleton::jntnum << " pose.x +0.2" << std::endl;
				Skeleton::joints[Skeleton::jntnum]->pose.x += 0.2;
			}
			else if (currRot == 1) {
				std::cout << "Joint # " << Skeleton::jntnum << " pose.y +0.2" << std::endl;
				Skeleton::joints[Skeleton::jntnum]->pose.y += 0.2;
			}
			else {
				std::cout << "Joint # " << Skeleton::jntnum << " pose.z +0.2" << std::endl;
				Skeleton::joints[Skeleton::jntnum]->pose.z += 0.2;
			}
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::MouseButton(int btn,int state,int x,int y) {
	if(btn==GLUT_LEFT_BUTTON) {
		LeftDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_MIDDLE_BUTTON) {
		MiddleDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_RIGHT_BUTTON) {
		RightDown = (state==GLUT_DOWN);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::MouseMotion(int nx,int ny) {
	int maxDelta=100;
	int dx = glm::clamp(nx - MouseX,-maxDelta,maxDelta);
	int dy = glm::clamp(-(ny - MouseY),-maxDelta,maxDelta);

	MouseX = nx;
	MouseY = ny;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if(LeftDown) {
		const float rate=1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth()+dx*rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline()-dy*rate,-90.0f,90.0f));
	}
	if(RightDown) {
		const float rate=0.005f;
		float dist=glm::clamp(Cam->GetDistance()*(1.0f-dx*rate),0.01f,1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////
