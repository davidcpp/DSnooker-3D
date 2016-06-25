// SDL_Snooker_NEW_PROJECT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lib.h"


extern CBilard * bilard1;
extern CStol * stol1;
extern CKij * kij1;
extern CBila ** bile1;
extern CGracz ** gracze1;

extern enum kolory;
//extern map<unsigned short int, AUX_RGBImageRec* > teksturyBil; // bilard
extern map<kolory, float* > koloryBil;			   // snooker


extern float WindowWidth;
extern float  WindowHeight;
extern float pWindowWidth;
extern float  pWindowHeight;

bool rysowanie = true;

extern int wartosc;
extern long long int freq;
extern fstream plik;
/*

GLfloat LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]=	{ 0.0f, 0.0f, 2.0f, 1.0f };
GLuint tTexture;


int LoadGLTextures()
{
SDL_Surface *surface;
string nazwa;

unsigned char tmp;

nazwa="texSukno.bmp";

glGenTextures( 1, &tTexture);

surface=SDL_LoadBMP(nazwa.c_str());
if(!surface)
{
cout<<"Nie za³adowano texstury \n";
system("PAUSE");
return 0;
}

glBindTexture( GL_TEXTURE_2D, tTexture );

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h,0,GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
SDL_FreeSurface( surface );



return 1;
}

void rysuj()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
glLoadIdentity();									// Reset The View
glTranslatef(0.0f,0.0f,-5);

glBindTexture(GL_TEXTURE_2D, tTexture);

glBegin(GL_QUADS);
// Front Face
glNormal3f( 0.0f, 0.0f, 1.0f);
glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
// Back Face
glNormal3f( 0.0f, 0.0f,-1.0f);
glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
// Top Face
glNormal3f( 0.0f, 1.0f, 0.0f);
glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
// Bottom Face
glNormal3f( 0.0f,-1.0f, 0.0f);
glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
// Right face
glNormal3f( 1.0f, 0.0f, 0.0f);
glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
// Left Face
glNormal3f(-1.0f, 0.0f, 0.0f);
glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
glEnd();

}
int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
if (!LoadGLTextures())								// Jump To Texture Loading Routine
{
return FALSE;									// If Texture Didn't Load Return FALSE
}

glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
glClearDepth(1.0f);									// Depth Buffer Setup
glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
glEnable(GL_LIGHT1);							// Enable Light One
return TRUE;										// Initialization Went OK
}
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
if (height==0)										// Prevent A Divide By Zero By
{
height=1;										// Making Height Equal One
}

glViewport(0,0,width,height);						// Reset The Current Viewport

glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
glLoadIdentity();									// Reset The Projection Matrix

// Calculate The Aspect Ratio Of The Window
gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
glLoadIdentity();									// Reset The Modelview Matrix
}
*/

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_NORMALIZE);
	//	glEnable(GL_POLYGON_SMOOTH);
	//	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	return TRUE;										// Initialization Went OK
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

														// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, NEAR, FAR);
	WindowWidth = width;
	WindowHeight = height;

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

SDL_Window * window = NULL;
SDL_GLContext glcontext;
SDL_Event zdarzenie;
bool wyjscie = false;
string sgracz1;
string sgracz2;
int main(int argc, char * args[])
{

	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		printf("SDL_Init Error: %s", SDL_GetError());
		system("PAUSE");
	}
	//atexit(SDL_Quit);

	window = SDL_CreateWindow("Snooker 3D v1.0", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WindowWidth, WindowHeight,  /*SDL_SWSURFACE |*/ SDL_WINDOW_OPENGL/* |SDL_WINDOW_RESIZABLE| SDL_WINDOW_INPUT_GRABBED*/);
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		cout << "Audio!\n";
		system("PAUSE");
	}
	glcontext = SDL_GL_CreateContext(window);

	SDL_HideWindow(window);
	sgracz1 = "gracz1";
	sgracz2 = "gracz2";
	cout << "Podaj nazwe pierwszego gracza: \n";
	getline(cin, sgracz1);
	cout << "Podaj nazwe drugiego gracza: \n";
	getline(cin, sgracz2);
	gracze1 = new CGracz*[2];
	bile1 = new CBila*[22];
	stol1 = new CStol(kij1, bile1);
	bilard1 = new CBilard(stol1, gracze1, &sgracz1, &sgracz2);
	//	TTF_Init();
	if (!InitGL())
	{
		system("PAUSE");
		printf("InitGL-Failure!!!\n");
	}
	SDL_ShowWindow(window);
	ReSizeGLScene(WindowWidth, WindowHeight);

	int counter = 0;
	long long int k, pFPS, pStanGry, pDisplay;
	long double roznica, roznica2, roznica3, fps;
	long long int a, b;
	long double c;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&pFPS));
	pDisplay = pFPS;
	pStanGry = pFPS;
	while (!wyjscie)
	{

		while (SDL_PollEvent(&zdarzenie))
		{
			switch (zdarzenie.type)
			{
			case SDL_QUIT:
				wyjscie = true;
				break;
			case SDLK_ESCAPE:
				wyjscie = true;
				break;

			case SDL_WINDOWEVENT:
				if (zdarzenie.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					ReSizeGLScene(zdarzenie.window.data1, zdarzenie.window.data2);
				}
				break;
			case SDL_MOUSEMOTION:

				ruch(zdarzenie.motion.x, zdarzenie.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouse(zdarzenie.button.button, zdarzenie.button.state, zdarzenie.button.x, zdarzenie.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				mouse(zdarzenie.button.button, zdarzenie.button.state, zdarzenie.button.x, zdarzenie.button.y);
				break;
			case SDL_MOUSEWHEEL:
				if ((bilard1->shot_con1)>10 && (bilard1->shot_con1)<300)
					bilard1->shot_con1 -= zdarzenie.wheel.y;
				bilard1->shot_con2 = bilard1->shot_con1 / 3;
				break;

			case SDL_KEYDOWN:
				keyboard(zdarzenie.key.keysym.sym);
				break;
			}
		}

		timer();

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&k));
		roznica = static_cast<long double>(k - pDisplay) / freq;
		roznica2 = static_cast<long double>(k - pFPS) / freq;

		// Refreshing the GL Scene
		if (roznica>0.003)
		{
			counter++;
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&pDisplay));
			bilard1->rysuj();
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&a));
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&b));
			SDL_GL_SwapWindow(window);


			c = static_cast<long double>(b - a) / freq;

		}

		if (roznica2>1)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&pFPS));
			fps = (long double)counter / roznica2;

			counter = 0;

		}

	}

	SDL_GL_DeleteContext(glcontext);

	// Done! Close the window, clean-up and exit the program.
	SDL_DestroyWindow(window);
	//	TTF_Quit();
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}