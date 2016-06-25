#include "lib.h"
#define IND i
#define R r
#define g 1972.0f  // przyspieszenie ziemskie
#define PI 3.1415926535
#define pOddalenie 10.0f //(6 - promieñ bili, 4-podstawowe oddalenie 
using namespace std;



// TABLICE, WEKTORY itp.
list<int> pBile;
map<int, list<int>::iterator > poz;
CPunkt3D polozenia[7]; // po³o¿enia kolorwych bil, przed uderzeniem 

float fKolory[9][4] = { { 1.0f,1.0f,1.0f,0 },{ 1.0f,0.0f,0.0f,0 },{ 1.0f,1.0f,0.0f,0 },{ 0,1,1,0 },
{ 0.55f,0.275f,0.07f,0 },{ 0.0f,0.0f,1.0f,0 },{ 1.0f,0.41f,0.71f,0 },{ 0.0f,0.0f,0.0f,0 },
{ 1.0f,1.0f,1.0f,0.5f } };

////////////////////////////////////

GLfloat ambientLight[4] =
{
	0,0,0, 1.0
};

GLfloat diffuseLight[4] =
{
	0.9,0.9,0.9,1.0
};

GLfloat specularLight[4] =
{
	0.6,0.6,0.6, 1.0
};

GLfloat lightPosition[4] =
{
	0,0,170, 1.0
};

GLfloat lightDirection[3] =
{
	0,0,-1
};

GLfloat ballAmbientMaterial[4] =
{
	0.250000, 0.207250, 0.207250, 0.922000
};

GLfloat ballDiffuseMaterial[9][4] =
{
	{ 1, 1, 1, 0.922000 },
	{ 1, 0, 0, 0.922000 },
	{ 1, 1, 0, 0.922000 },
	{ 0, 1, 1, 0.922000 },
	{ 0.55f,0.275f,0.07f,0 },
	{ 0, 0, 1, 0.922000 },
	{ 1.0f,0.41f,0.71f,0 },
	{ 0.0f,0.0f,0.0f,0 },
	{ 1.0f,1.0f,1.0f,0.5f }
};


GLfloat ballSpecularMaterial[4] =
{
	0.196648, 0.196648, 0.196648, 0.922000
};

// TABLICE, WEKTORY itp.  cd.
float ** gSiatka;  // siatka do VBO

bool xOdbite[22]; // odbite od bazy
bool yOdbite[22]; // odbite od górnej lub dolnej(na ekranie) bandy
CPunkt3D pozycje[22]; // pozycje pocz¹tkowe 
CPunkt3D poprzednie[22]; // pozycje przed ostatnim uderzeniem
unsigned long long int poprzedni[22]; // dla i-tej kuli moment w czasie jej poprzedniego rysowania
long double roznica[22];  // dla i-tej kuli ró¿nica miêdzy dwoma kolejnymi czasami rysowania

vector<unsigned int> tmpWbite; // wektor wszystkich wbitych bil, który 
							   // pos³u¿y do ich analizy pod k¹tem faulu
string sKolory[] = { "biala", "czerwona", "zolta", "zielona", "brazowa", "niebieska", "rozowa", "czarna" };
string sKomunikaty[] = { "Zadeklaruj bile. ","Nastepna bila: ", "Faul. Biala wbita do luzy. ", "Faul. Niewlasciwa bila wbita do luzy. ",
"Faul. Chybienie. ", "Faul. Uderzona niewlasciwa bila. ", "Faul. Nie wykonano uderzenia. ", "Faul. Niezadeklarowana bila. " };


// OBIEKTY
unsigned char * pix; // pomocniczy wskaŸnik na dane tekstury z tekstem 
CPunkt3D pwk;
fstream plik;
extern SDL_Window * window;
CBilard * bilard1;
CStol * stol1;
CKij * kij1;
CBila ** bile1;
CGracz ** gracze1;

// ZMIENNE
float pWindowWidth = 1000;
float pWindowHeight = 600;
float WindowWidth = 1000;
float WindowHeight = 600;
float katp; // potrzebna nam wartoœæ pocz¹tkowa k¹ta po wywo³aniu dla upuszczenia klawisza
long long int freq; // czêstotliwoœæ zegara odmierzaj¹cego czas
int wartosc;
GLfloat ballShininessMaterial = 11.264; // w³aœciwoœæ materia³u
										// k¹ty obrotu kierunku Ÿród³a œwiat³a
GLfloat light_rotatex = 0.0;
GLfloat light_rotatey = 0.0;
GLfloat ry = 0.0;
GLuint	base;			// pocz¹tek listy wyœwietlania znaków
GLuint textTexture;		// identyfikator tekstury tekstu
extern enum kolory;

int LoadTextTexture()                                    // Load Bitmaps And Convert To Textures
{
	SDL_Surface *surface;
	int Status = FALSE;                               // Status Indicator	 
	surface = SDL_LoadBMP("bFont.bmp");
	if (!surface)
	{
		cout << "Nie zaladowano texstury tekstu\n";
		system("PAUSE");
	}
	else
	{
		Status = TRUE;                            // Set The Status To TRUE
		glGenTextures(1, &textTexture);          // Create Two Texture
		glBindTexture(GL_TEXTURE_2D, textTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	}
	SDL_FreeSurface(surface);
	return Status;                                  // Return The Status
}

GLvoid BuildFont(GLvoid)								// Build Our Font Display List
{
	float	cx;											// Holds Our X Character Coord
	float	cy;											// Holds Our Y Character Coord
	base = glGenLists(256);	// Creating 256 Display Lists
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textTexture);			// Select Our Font Texture
	for (int loop = 0; loop<256; loop++)						// Loop Through All 256 Lists
	{

		cx = float(loop % 16) / 16.0f;						// X Position Of Current Character
		cy = float(loop / 16) / 16.0f;						// Y Position Of Current Character

		glNewList(base + loop, GL_COMPILE);				// Start Building A List
		glBegin(GL_QUADS);							// Use A Quad For Each Character
		glTexCoord2f(cx, cy);			// Texture Coord (Bottom Left)
		glVertex2i(0, 16);						// Vertex Coord (Bottom Left)
		glTexCoord2f(cx + 0.0625f, cy);	// Texture Coord (Bottom Right)
		glVertex2i(16, 16);						// Vertex Coord (Bottom Right)
		glTexCoord2f(cx + 0.0625f, cy + 0.0625f);			// Texture Coord (Top Right)
		glVertex2i(16, 0);						// Vertex Coord (Top Right)
		glTexCoord2f(cx, cy + 0.0625f);					// Texture Coord (Top Left)
		glVertex2i(0, 0);						// Vertex Coord (Top Left)
		glEnd();									// Done Building Our Quad (Character)
		glTranslated(10, 0, 0);						// Move To The Right Of The Character
		glEndList();									// Done Building The Display List
	}  // Loop Until All 256 Are Built
	glDisable(GL_TEXTURE_2D);

}


GLvoid KillFont(GLvoid)									// Delete The Font From Memory
{
	glDeleteLists(base, 256);							// Delete All 256 Display Lists
}



GLvoid glPrint(GLint x, GLint y, const char *string, int set)	// Where The Printing Happens
{
	if (set>1)
	{
		set = 1;
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textTexture);
	glLoadIdentity();
	glTranslated(x, y, 0);
	glListBase(base - 32 + (128 * set));
	glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

}


void glEnable2D()
{
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, WindowWidth, 0, WindowHeight, -10, 50); // ze wzglêdu na rysunki bil zFar musi byæ wiêkszy
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void glDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void translacja(float & katp, float a, float b, bool kierunek)  // translacja k¹ta
{
	// a!=0 musi tak byæ
	if (kierunek) // zwyk³y kierunek - odwrotnie do ruchu wskazówek zegara
	{
		//<0-90>
		if (b >= 0 && a<0) // (90-180>
		{
			katp = -katp + 180;
		}
		if (b<0 && a<0)  //(180-270)
		{
			katp += 180;
		}

		if (b<0 && a>0) // (270-360)
		{
			katp = -katp + 360;
		}

	}
	else
	{
		//<0-90>

		if (b <= 0 && a>0) // (90-180>
		{
			katp = -katp + 180;
		}
		if (b>0 && a>0)  //(180-270)
		{
			katp += 180;
		}

		if (b>0 && a<0) // (270-360)
		{
			katp = -katp + 360;
		}

	}
	return;
}

CPunkt3D::CPunkt3D()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;

}

CPunkt3D::CPunkt3D(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;

};

ostream& operator<<(ostream& w, const CPunkt3D & p)
{
	w << "x: " << p.x << "\n";
	w << "y: " << p.y << "\n";
	w << "z: " << p.z << "\n";
	return w;
}


istream & operator >> (istream & w, CPunkt3D & p)
{
	cout << "x: ";
	w >> p.x;
	cout << "y: ";
	w >> p.y;
	cout << "z: ";
	w >> p.z;
	return w;
}

CPunkt3D CPunkt3D::operator-(const CPunkt3D & p2)
{
	CPunkt3D wynik;
	wynik.x = p2.x - x;
	wynik.y = p2.y - y;
	wynik.z = p2.z - z;
	return wynik;
}


CPunkt3D CPunkt3D::operator+(CPunkt3D const& p2)
{
	CPunkt3D wynik;
	wynik.x = p2.x + x;
	wynik.y = p2.y + y;
	wynik.z = p2.z + z;
	return wynik;
}

float odleglosc(CPunkt3D & p1, CPunkt3D & p2)
{
	CPunkt3D v;

	v = p2 - p1;
	float wynik;
	float tmp = (v.x)*(v.x) + (v.y)*(v.y) + (v.z)*(v.z);
	wynik = sqrt(tmp);
	return wynik;
}

CFigura::~CFigura()
{
	//delete kolor;
}

CProstokat::CProstokat(CPunkt3D p, float a, float b, /*GL*/float * aKolor/*, swektor * aPrzesuniecie, skaty * aObrot*/)
{
	obrot.k1 = 0.0f;
	obrot.k2 = 0.0f;
	obrot.k3 = 0.0f;

	przesuniecie.x = 0;
	przesuniecie.y = 0;
	przesuniecie.z = 0;

	if (aKolor != NULL) // je¿eli zosta³ ustawiony
	{
		kolor = new /*GL*/float[3];
		kolor[0] = aKolor[0];// r
		kolor[1] = aKolor[1]; // g
		kolor[2] = aKolor[2];// b
	}
	else // w takim wypadku ustawiamy kolor na NULL, ¿eby nie przes³oniæ bardziej globalnego koloru np. klasy CCzolg
	{
		kolor = NULL;
	}
	wierzcholekLD.x = p.x;
	wierzcholekLD.y = p.y;
	wierzcholekLD.z = 0;

	wierzcholekLG.x = p.x;
	wierzcholekLG.y = p.y + b;
	wierzcholekLG.z = 0;

	wierzcholekPG.x = p.x + a;
	wierzcholekPG.y = p.y + b;
	wierzcholekPG.z = 0;

	wierzcholekPD.x = p.x + a;
	wierzcholekPD.y = p.y;
	wierzcholekPD.z = 0;

	this->a = a;
	this->b = b;
}

bool CProstokat::SprawdzZakres(float ax, float ay)
{
	if (ax>wierzcholekLD.x && ax<wierzcholekPD.x &&  ay>wierzcholekPD.y && ay<wierzcholekPG.y)
		return true;  // mo¿na narysowaæ z przsuniêciem
	else
		return false;
}

void CProstokat::rysuj()
{
	glPushMatrix();
	glTranslatef(przesuniecie.x, przesuniecie.y, przesuniecie.z);
	glRotatef(obrot.k3, 0, 0, 1);
	if (kolor != NULL) // kolor lokalny dla prostok¹ta
		glColor3fv(kolor);

	glBegin(GL_POLYGON);
	glVertex2f(wierzcholekLD.x, wierzcholekLD.y);
	glVertex2f(wierzcholekLG.x, wierzcholekLG.y);
	glVertex2f(wierzcholekPG.x, wierzcholekPG.y);
	glVertex2f(wierzcholekPD.x, wierzcholekPD.y);

	glEnd();
	glPopMatrix();
}

void CProstokat::przesun(swektor * aPrzesuniecie)
{
	przesuniecie.x = aPrzesuniecie->x;
	przesuniecie.y = aPrzesuniecie->y;

}

CKolo::CKolo(unsigned int alWierzcholkow, float ar, float * aKolor)
{
	r = ar;
	lWierzcholkow = alWierzcholkow;
}


CKula::CKula(swektor * aSrodek, float ar, int alPol, int alRow, float * aKolor)
{

	r = ar;
	sfera.radius = ar;
	sfera.slices = alPol;
	sfera.stacks = alRow;
	if (aSrodek != NULL)
	{
		przesuniecie.x = aSrodek->x;
		przesuniecie.y = aSrodek->y;
		przesuniecie.z = aSrodek->z;
	}
	else
	{
		przesuniecie.x = 0;
		przesuniecie.y = 0;
		przesuniecie.z = 0;
	}

	if (aKolor != NULL)
	{
		kolor = aKolor;
	}
	else
	{
		kolor = new float[4];
		kolor[0] = 1.0f;
		kolor[1] = 1.0f;
		kolor[2] = 1.0f;
		kolor[3] = 1.0f;
	}

}


CKula::~CKula()
{
	delete[] kolor;
}

float CBila::odlegloscPunktu(float tx, float ty)
{
	float odleglosc, xRoznica, yRoznica;
	xRoznica = przesuniecie.x - tx;
	yRoznica = przesuniecie.y - ty;
	odleglosc = sqrt(xRoznica*xRoznica + yRoznica*yRoznica);
	return odleglosc;
}

void CBila::rysuj()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPushMatrix();
	if (!stol1->mode)
	{
		glEnable(GL_COLOR_MATERIAL);
		glTranslatef(przesuniecie.x, przesuniecie.y, stol1->hBila);
		glRotatef(kierunek, 0, 0, 1);
		glRotatef(alfa, 0, 1, 0);
	}
	else
	{
		glScalef(2, 2, 2);
	}
	glColor4fv(kolor);
	glCallList(stol1->lBila);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

float CBila::gPromien()
{
	return r;
}

swektor * CBila::gPrzesuniecie()
{
	return &przesuniecie;
}


void CBila::fPrzesuniecie(float tx, float ty, float tz)
{
	przesuniecie.x += tx;
	przesuniecie.y += ty;
	przesuniecie.z = tz;
}

void CBila::przesunDo(float tx, float ty, float tz)
{
	przesuniecie.x = tx;
	przesuniecie.y = ty;
	przesuniecie.z = tz;
}

void CBila::wyzeruj()
{
	Vp = 0;
	Vx = 0;
	Vy = 0;
	kx = 0;
	ky = 0;
}
CKij::~CKij()
{
	delete oddalenie; //dodatkowe oddalenie koñca kija od œrodka bili
	delete koniecKija;
	delete poczatekKija;
	delete[] siatka;
	//	if(tekstura!=NULL) delete tekstura;
}
CKij::CKij()
{
	oddalenie = new swektor; //dodatkowe oddalenie koñca kija od œrodka bili
	koniecKija = new CPunkt3D;
	poczatekKija = new CPunkt3D;
	//    tekstura;
	siatka = new float*[21];
	for (int i = 0; i<21; i++)
	{
		siatka[i] = new float[3];
		siatka[i][2] = 0; // wsp. z
	}
	maxOddalenie = 90.0f;
	oddalenie->x = 0.0f;
	oddalenie->y = 0.0f;
	obrot.k3 = 0.0f;
	float pr = 4.0f;
	float k = 2.0f;
	koniecKija->x = 336.5f;
	koniecKija->y = 0.0f;
	poczatekKija->y = pr;
	dlugoscKija = 336.5f;
	przesuniecie.x = bile1[0]->przesuniecie.x;
	przesuniecie.y = bile1[0]->przesuniecie.y;
	przesuniecie.z = 6;
	for (float kat = 90; kat <= 270; kat += 10) // 19 pkt siatki (1-17)
	{
		siatka[(int)(kat / 10) - 9][0] = -335.0f + pr + pr*cos(kat / 180.0f*PI);
		siatka[(int)(kat / 10) - 9][1] = pr*sin(kat / 180.0f*PI);
	}
	siatka[19][0] = -1.5f;
	siatka[19][1] = -1.5f;
	siatka[20][0] = -1.5f;
	siatka[20][1] = 1.5f;
	kij2.radius = 3;
	kij2.slices = 10;
	kij2.stacks = 10;

	kij1.baseRadius = 1.5;
	kij1.topRadius = 3;
	r = kij1.topRadius;
	kij1.height = dlugoscKija - kij2.radius;
	kij1.slices = 10;
	kij1.stacks = 10;

	// dysk
	kij3.baseRadius = 0;
	kij3.topRadius = 1.5;
	kij3.slices = 10;
	kij3.stacks = 10;  // w³aœciwie to loops dla dysku

	qKij1 = gluNewQuadric();
	gluQuadricDrawStyle(qKij1, GLU_FILL);
	gluQuadricNormals(qKij1, GLU_SMOOTH);
	gluQuadricOrientation(qKij1, GLU_OUTSIDE);
	gluQuadricTexture(qKij1, GLU_TRUE);

	qKij2 = gluNewQuadric();
	gluQuadricDrawStyle(qKij2, GLU_FILL);
	gluQuadricNormals(qKij2, GLU_SMOOTH);
	gluQuadricOrientation(qKij2, GLU_OUTSIDE);
	gluQuadricTexture(qKij2, GLU_FALSE);

	qKij3 = gluNewQuadric();
	gluQuadricDrawStyle(qKij3, GLU_FILL);
	gluQuadricNormals(qKij3, GLU_SMOOTH);
	gluQuadricOrientation(qKij3, GLU_OUTSIDE);
	gluQuadricTexture(qKij3, GLU_FALSE);

}

void CKij::rysuj()
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTranslatef(przesuniecie.x, przesuniecie.y, przesuniecie.z);
	glRotatef(-90, 0, 1, 0);
	glRotatef(obrot.k3, 1, 0, 0);

	long double yx, a;
	float katKija = obrot.k3 + 180;
	if (katKija>360) katKija -= 360;
	yx = tan((long double)(katKija*PI) / 180.0f);
	a = yx;

	float x, y, b;

	// œrodek bia³ej bili znajduje siê na prostej kija
	x = bile1[0]->przesuniecie.x;
	y = bile1[0]->przesuniecie.y;

	if (a == 0)
	{
		a = 1.0f / 10000.0f;
		//			cout<<"hej\n";
	}
	b = y - (a*x);

	// wyznaczamy p.przeciêcia prostej kija i prostej prost. do prostej kija przechodz¹cej przez œrodek bili uderzanej
	float maxNachylenie = 15;
	float a2, b2, b3, a3, rx1, ry1, kOdl;
	CPunkt3D pprz;
	CPunkt3D bPprz; // p. przeciêcia z band¹ prostej przerywanej
	if (a != 0)
		a2 = -1 / a;
	else
		a2 = -9999.0f;
	a3 = a2;
	b3 = bile1[0]->przesuniecie.y - bile1[0]->przesuniecie.x*a3;

	bool pWarunek1;
	bool pWarunek2;

	for (int i = 1; i<22; i++)
	{
		// rozwa¿amy jedn¹ po³ówkê 
		pWarunek1 = bile1[i]->przesuniecie.y<(a3*bile1[i]->przesuniecie.x + b3) && (obrot.k3 >= 0 && obrot.k3<180.0f);
		pWarunek2 = bile1[i]->przesuniecie.y>(a3*bile1[i]->przesuniecie.x + b3) && (obrot.k3<360.0f && obrot.k3 >= 180.0f);
		if (pWarunek1 || pWarunek2) //rozwa¿amy bile powy¿ej prostej prostopad³ej o tym równaniu
		{
			b2 = bile1[i]->przesuniecie.y - bile1[i]->przesuniecie.x*a2;

			// rozwi¹zanie uk³adu równañ
			pprz.x = (b2 - b) / (a - a2);
			pprz.y = a2*pprz.x + b2;

			// min oddalenie bili pozornej od bili bia³ej 
			rx1 = bile1[i]->przesuniecie.x - pprz.x;
			ry1 = bile1[i]->przesuniecie.y - pprz.y;
			kOdl = sqrt(rx1*rx1 + ry1*ry1);
			kOdl += oddalenie->y;

			if (kOdl<bile1[0]->r)
			{
				float pOdl;
				CPunkt3D p1, p2;
				p1.x = bile1[0]->przesuniecie.x;
				p1.y = bile1[0]->przesuniecie.y;
				p2.x = bile1[i]->przesuniecie.x;
				p2.y = bile1[i]->przesuniecie.y;
				pOdl = odleglosc(p1, p2);
				zaw = sqrt(bile1[0]->r*bile1[0]->r - kOdl*kOdl);
				zaw += 1;
				zaw += r;
				nachylenie = atan(zaw / pOdl) * 180 / PI;
				if (nachylenie>maxNachylenie)
					maxNachylenie = nachylenie;
			}

		}
	}

	glRotatef(maxNachylenie, 0, 1, 0);
	glTranslatef(0, oddalenie->y, -oddalenie->x + pOddalenie);

	glBindTexture(GL_TEXTURE_2D, stol1->textures[2]);
	gluCylinder(qKij1, kij1.baseRadius, kij1.topRadius, kij1.height, kij1.slices, kij1.stacks);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0, 0, 0);
	gluDisk(qKij3, kij3.baseRadius, kij3.topRadius, kij3.slices, kij3.stacks);

	glTranslatef(0, 0, kij1.height);

	gluSphere(qKij2, kij2.radius, kij2.slices, kij2.stacks);
	glDisable(GL_COLOR_MATERIAL);

	glPopMatrix();
}

void CKij::oddal(float dx, float dy)
{
	printf("oddalenie->x: %f \n", oddalenie->x);
	if (oddalenie->x + dx<0.0f && oddalenie->x + dx>-maxOddalenie)
		oddalenie->x += dx;
	if (oddalenie->x + dx>0.0f)
		oddalenie->x = 0;
	oddalenie->y += dy;
}


CStol::CStol(CKij *& aKij, CBila ** aBile)
{

	mode = false;
	uderzenie = Mix_LoadWAV("zderzenie.wav");
	if (!uderzenie)
	{
		cout << "\nNie za³adowano dzwieku uderzenia\n";
		system("PAUSE");
	}
	textures = new GLuint[4];
	if (!LoadGLTextures())
		cout << "Nie za³adowano wszystkich tekstur\n";
	fstream dane;
	siatka = new float*[200];
	for (int i = 0; i<200; i++)
	{
		siatka[i] = new float[3];
	}
	szkielet1 = new CPunkt3D[100];
	szkielet2 = new CPunkt3D[100];
	sukno = new CPunkt3D[100];
	bandy = new CPunkt3D[100];
	luzy = new CPunkt3D[100];
	wbite = new bool[22];
	tmpBila2 = new CBila(BIALA);
	rBila2 = new CBila(PBIALA);

	bile = aBile;
	for (int i = 0; i<22; i++)
		wbite[i] = false;
	zewPromienLuzy = 15.5f;
	wewPromienLuzy = 11.0f; //~=zewPromienLuzy*sqrt(2)/2
	szerokoscBandy = 12.0f;
	dlugosc = 720.0f;
	szerokosc = 360.0f;
	rozwBandy1 = 6.0f;
	rozwBandy2 = 15.0f;
	katBandy = new float[13];

	// zgodnie z ruchem wskazówek zegara
	katBandy[1] = atan(rozwBandy1 / szerokoscBandy) * 180 / PI; // k¹t normalny bandy 
	translacja(katBandy[1], -szerokoscBandy, rozwBandy1, 1);
	katBandy[2] = atan(rozwBandy1 / szerokoscBandy) * 180 / PI; // k¹t normalny bandy 

	katBandy[3] = atan(rozwBandy2 / szerokoscBandy) * 180 / PI; // k¹t normalny bandy 
	translacja(katBandy[3], -szerokoscBandy, rozwBandy2, 1);
	katBandy[4] = atan(szerokoscBandy / rozwBandy2) * 180 / PI;
	translacja(katBandy[4], rozwBandy2, -szerokoscBandy, 1);

	katBandy[5] = atan(szerokoscBandy / rozwBandy2) * 180 / PI; // k¹t normalny bandy 
																//	translacja(katBandy[5],rozwBandy2,szerokoscBandy,1);
	katBandy[6] = atan(rozwBandy2 / szerokoscBandy) * 180 / PI;
	translacja(katBandy[6], -szerokoscBandy, -rozwBandy2, 1);

	katBandy[7] = atan(rozwBandy1 / szerokoscBandy) * 180 / PI;// k¹t normalny bandy 
	translacja(katBandy[7], szerokoscBandy, -rozwBandy1, 1);
	katBandy[8] = atan(rozwBandy1 / szerokoscBandy) * 180 / PI;// k¹t normalny bandy 
	translacja(katBandy[8], -szerokoscBandy, -rozwBandy1, 1);

	katBandy[9] = atan(rozwBandy2 / szerokoscBandy) * 180 / PI;
	translacja(katBandy[9], szerokoscBandy, -rozwBandy2, 1);
	katBandy[10] = atan(szerokoscBandy / rozwBandy2) * 180 / PI; // k¹t normalny bandy 
	translacja(katBandy[10], -rozwBandy2, szerokoscBandy, 1);

	katBandy[11] = atan(szerokoscBandy / rozwBandy2) * 180 / PI; // k¹t normalny bandy 
	translacja(katBandy[11], -rozwBandy2, -szerokoscBandy, 1);
	katBandy[12] = atan(rozwBandy2 / szerokoscBandy) * 180 / PI;
	translacja(katBandy[12], szerokoscBandy, rozwBandy2, 1);

	b0 = new float[13];
	b0[1] = -338;
	b0[2] = 1102;
	b0[3] = -203.6;
	b0[4] = -555.5;
	b0[5] = 915.5;
	b0[6] = 563.5;
	b0[7] = -742;
	b0[8] = 698;
	b0[9] = -12.4;
	b0[10] = 15.5;
	b0[11] = 344.5;
	b0[12] = 372.4;

	a0 = new float[13];
	a0[1] = 2;
	a0[2] = -2;
	a0[3] = 0.8;
	a0[4] = 1.25;
	a0[5] = -1.25;
	a0[6] = -0.8;
	a0[7] = 2;
	a0[8] = -2;
	a0[9] = 0.8;
	a0[10] = 1.25;
	a0[11] = -1.25;
	a0[12] = -0.8;

	kraniec = new CPunkt3D[13];
	kraniec[1].x = dlugosc / 2 - rozwBandy1 - wewPromienLuzy;
	kraniec[1].y = szerokosc - szerokoscBandy;
	kraniec[2].x = dlugosc / 2 + rozwBandy1 + wewPromienLuzy;
	kraniec[2].y = szerokosc - szerokoscBandy;

	kraniec[3].x = dlugosc - rozwBandy2 - zewPromienLuzy;
	kraniec[3].y = szerokosc - szerokoscBandy;
	kraniec[4].x = dlugosc - szerokoscBandy;
	kraniec[4].y = szerokosc - rozwBandy2 - zewPromienLuzy;

	kraniec[5].x = dlugosc - szerokoscBandy;
	kraniec[5].y = rozwBandy2 + zewPromienLuzy;
	kraniec[6].x = dlugosc - rozwBandy2 - zewPromienLuzy;
	kraniec[6].y = szerokoscBandy;

	kraniec[7].x = dlugosc / 2 + rozwBandy1 + wewPromienLuzy;
	kraniec[7].y = szerokoscBandy;
	kraniec[8].x = dlugosc / 2 - rozwBandy1 - wewPromienLuzy;
	kraniec[8].y = szerokoscBandy;

	kraniec[9].x = rozwBandy2 + zewPromienLuzy;
	kraniec[9].y = szerokoscBandy;
	kraniec[10].x = szerokoscBandy;
	kraniec[10].y = rozwBandy2 + zewPromienLuzy;

	kraniec[11].x = szerokoscBandy;
	kraniec[11].y = szerokosc - zewPromienLuzy - rozwBandy2;
	kraniec[12].x = rozwBandy2 + zewPromienLuzy;
	kraniec[12].y = szerokosc - szerokoscBandy;

	mi = 0.011;


	odLuzySrod = new swektor;
	odLuzySrod->x = 344.5f;
	odLuzySrod->y = 11;
	odlBazy = 172.0f;
	promienBazy = 56.0f;

	srBazy.x = odlBazy;
	srBazy.y = szerokosc / 2;

	dWektor = new swektor;
	dWektor->x = 8;
	dWektor->y = 8;

	dWektor2 = new swektor;
	dWektor2->x = 9;
	dWektor2->y = 9;

	zewBanda = new swektor;
	szerokoscZewBandy = 27.0f;
	zewBanda->x = 27.0f;
	zewBanda->y = 27.0f;
	rog = new CPunkt3D;
	rog->x = 50.0f;
	rog->y = 50.0f;

	swektor rPrzesuniecie;
	wierzcholek = new CPunkt3D;
	wierzcholek2 = new CPunkt3D;

	wierzcholek->x = zewBanda->x + rog->x;
	wierzcholek->y = zewBanda->y + rog->y;
	wierzcholek2->x = wierzcholek->x + dlugosc;
	wierzcholek2->y = wierzcholek->y + szerokosc;

	sPrzesuniecie.x = -WindowWidth / 2 + wierzcholek->x;
	sPrzesuniecie.y = -WindowHeight / 2 + wierzcholek->y;

	bile[0] = new CBila(BIALA);
	bile[16] = new CBila(ZOLTA);
	bile[17] = new CBila(ZIELONA);
	bile[18] = new CBila(BRAZOWA);
	bile[19] = new CBila(NIEBIESKA);
	bile[20] = new CBila(ROZOWA);
	bile[21] = new CBila(CZARNA);


	hSukno = 0; // -100
	hBila = hSukno + bile[0]->r;
	dhBandy = hSukno;			// -100
	ghBandy = dhBandy + 8.0f;		// -90
	ghSzkielet = ghBandy;		// -90
	dhSzkielet = ghSzkielet - 30.0f;	// -60
	hKij = hBila;
	hNoga = -180;

	light0 = new light;
	light0->position = new GLfloat[4];
	light0->position = lightPosition;
	light0->direction = new GLfloat[4];
	light0->direction = lightDirection;
	light0->specular = new GLfloat[4];
	light0->specular = specularLight;
	light0->diffuse = new GLfloat[4];
	light0->diffuse = diffuseLight;
	light0->ambient = new GLfloat[4];
	light0->ambient = ambientLight;
	light0->spot_cutoff = 180;

	bile[0]->fPrzesuniecie(odlBazy, 209.0f, hBila);
	bile[16]->fPrzesuniecie(odlBazy, 238.0f, hBila);
	bile[17]->fPrzesuniecie(odlBazy, 122.0f, hBila);
	bile[18]->fPrzesuniecie(odlBazy, 180.0f, hBila);
	bile[19]->fPrzesuniecie(360.0f, 180.0f, hBila);
	bile[20]->fPrzesuniecie(540.0f, 180.0f, hBila);
	bile[21]->fPrzesuniecie(630.0f, 180.0f, hBila);

	pozycje[0] = CPunkt3D(odlBazy, 209.0f);
	pozycje[16] = CPunkt3D(odlBazy, 238.0f);
	pozycje[17] = CPunkt3D(odlBazy, 122.0f);
	pozycje[18] = CPunkt3D(odlBazy, 180.0f);
	pozycje[19] = CPunkt3D(360.0f, 180.0f);
	pozycje[20] = CPunkt3D(540.0f, 180.0f);
	pozycje[21] = CPunkt3D(630.0f, 180.0f);

	int tmp = 1;
	float dr = bile[0]->gPromien();
	float roznica = 2.2f*dr;

	rPrzesuniecie.y = bile[20]->gPrzesuniecie()->y;
	rPrzesuniecie.x = bile[20]->gPrzesuniecie()->x + 2 * dr;
	rPrzesuniecie.z = hBila;

	bile[1] = new CBila(CZERWONA, &rPrzesuniecie);
	pozycje[1] = CPunkt3D(rPrzesuniecie.x, rPrzesuniecie.y);

	rPrzesuniecie.y -= dr;
	for (int i = 2; i <= 5; i++)
	{
		rPrzesuniecie.x += 1.9f*dr;
		for (int j = 1; j <= i; j++)
		{
			bile[tmp + j] = new CBila(CZERWONA, &rPrzesuniecie);
			pozycje[tmp + j] = CPunkt3D(rPrzesuniecie.x, rPrzesuniecie.y);
			rPrzesuniecie.y += roznica;
		}
		rPrzesuniecie.y = bile[20]->gPrzesuniecie()->y - i*dr;
		tmp += i;
	}

	aKij = new CKij(); // konstruktor kija po konstruktorach bil
	kij = aKij;


	////////////////////////////////////////////////////////////////////////////////////
	// Generowanie siatki sfery dla bili
	unsigned int size = (STACKS)*SLICES + 2;
	siatkaBili = new float*[(STACKS - 1)*SLICES + 2];
	for (int i = 0; i<size; i++)
	{
		siatkaBili[i] = new float[3];
	}

	siatkaBili[0][0] = 0;
	siatkaBili[0][1] = 0;
	siatkaBili[0][2] = -bile[0]->r;
	float d = 180.0f / bile[0]->sfera.stacks;
	float c = 360.0f / bile[0]->sfera.slices;
	float r2, alfa;
	unsigned int q;
	int next1, next2;

	float beta = -90 + d;
	for (q = 0; q<(bile[0]->sfera.stacks - 1); q++)
	{
		r2 = cos(beta / 180 * PI)*bile[0]->r;
		next1 = q*bile[0]->sfera.slices;
		alfa = 0;
		for (next2 = 1; next2 <= bile[0]->sfera.slices; next2++)
		{
			siatkaBili[next1 + next2][0] = cos(alfa / 180 * PI)*r2;
			siatkaBili[next1 + next2][1] = sin(alfa / 180 * PI)*r2;
			siatkaBili[next1 + next2][2] = sin(beta / 180 * PI)*bile[0]->r;
			alfa += c;
		}
		beta += d;
	}
	next1 = q*bile[0]->sfera.slices + 1;
	siatkaBili[next1][0] = 0;
	siatkaBili[next1][1] = 0;
	siatkaBili[next1][2] = bile[0]->r;

	////////////////////////////////////////////////////////////////////////////////////

	lBila = glGenLists(1);
	glNewList(lBila, GL_COMPILE);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3fv(siatkaBili[0]);
	glVertex3fv(siatkaBili[0]);
	for (int i = 1; i <= bile[0]->sfera.slices; i++)
	{
		glNormal3fv(siatkaBili[i]);
		glVertex3fv(siatkaBili[i]);
	}
	glNormal3fv(siatkaBili[1]);
	glVertex3fv(siatkaBili[1]);
	glEnd();

	unsigned int k;
	for (k = 0; k<(bile[0]->sfera.stacks - 2); k++)
	{
		next1 = bile[0]->sfera.slices*k; // liczba równole¿ników razy liczba kondygnacji
		next2 = bile[0]->sfera.slices*(k + 1);
		glBegin(GL_QUAD_STRIP);
		for (int j = 1; j <= bile[0]->sfera.slices; j++)
		{
			glNormal3fv(siatkaBili[next1 + j]);
			glVertex3fv(siatkaBili[next1 + j]);
			glNormal3fv(siatkaBili[next2 + j]);
			glVertex3fv(siatkaBili[next2 + j]); // punkt z nastêpnej "kondygnacji" bili
		}
		glNormal3fv(siatkaBili[next1 + 1]);
		glVertex3fv(siatkaBili[next1 + 1]);
		glNormal3fv(siatkaBili[next2 + 1]);
		glVertex3fv(siatkaBili[next2 + 1]); // punkt z nastêpnej "kondygnacji" bili	
		glEnd();
	}

	next1 = bile[0]->sfera.slices*(k);
	next2 = bile[0]->sfera.slices*(k + 1) + 1;

	glBegin(GL_TRIANGLE_FAN);

	glNormal3fv(siatkaBili[next2]);
	glVertex3fv(siatkaBili[next2]);
	for (int k = 1; k <= bile[0]->sfera.slices; k++)
	{
		glNormal3fv(siatkaBili[next1 + k]);
		glVertex3fv(siatkaBili[next1 + k]); // ostatnia "kondygnacja"
	}
	glNormal3fv(siatkaBili[next1 + 1]);
	glVertex3fv(siatkaBili[next1 + 1]);
	glEnd();

	glEndList();

	////////////////////////////////////////////////////////////////////////////////////
	// Generowanie nogi stolu

	qNoga = gluNewQuadric();
	gluQuadricDrawStyle(qNoga, GLU_FILL);
	gluQuadricNormals(qNoga, GLU_SMOOTH);
	gluQuadricOrientation(qNoga, GLU_OUTSIDE);
	gluQuadricTexture(qNoga, GLU_TRUE);

	noga.baseRadius = 15;
	noga.topRadius = 25;
	noga.height = (-hNoga) - 0.5;
	noga.slices = 10;
	noga.stacks = 10;

	lNoga = glGenLists(1);
	glNewList(lNoga, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	gluCylinder(qNoga, noga.baseRadius, noga.topRadius, noga.height, noga.slices, noga.stacks);
	glEndList();

	float dlWektor2 = sqrt((dWektor2->y*dWektor2->y) + (dWektor2->x*dWektor2->x));
	//sukno

	// triangle_fan dla naro¿nej ³uzy (0-10)
	for (int i = 0; i <= 33; i++)
	{
		siatka[i][2] = hSukno;
	}
	siatka[0][0] = zewPromienLuzy;
	siatka[0][1] = zewPromienLuzy;
	for (float kat = 0; kat <= 90; kat += 10) // 10 pkt siatki (0-9)
	{
		siatka[(int)kat / 10 + 1][0] = zewPromienLuzy*cos(kat / 180 * PI);
		siatka[(int)kat / 10 + 1][1] = zewPromienLuzy*sin(kat / 180 * PI);
	}

	// triangle_fan dla œrodkowej ³uzy (11-17)
	for (float kat = 90; kat <= 180; kat += 10) // 6 pkt siatki (11-16)
	{
		siatka[(int)kat / 10 + 3][0] = dlugosc / 2 + wewPromienLuzy*cos(kat / 180 * PI);
		siatka[(int)kat / 10 + 3][1] = wewPromienLuzy*sin(kat / 180 * PI) + siatka[1][1];
	}

	siatka[11][0] = siatka[21][0];  // wierzcho³ek triangle_fan œrodkowej ³uzy
	siatka[11][1] = siatka[12][1];


	// g³ówny prostok¹t sukna 17-20

	siatka[22][0] = zewPromienLuzy;
	siatka[22][1] = siatka[11][1];
	siatka[23][0] = zewPromienLuzy; szerokosc / 2;
	siatka[23][1] = szerokosc / 2;
	siatka[24][0] = dlugosc / 2;
	siatka[24][1] = szerokosc / 2;
	siatka[25][0] = dlugosc / 2;
	siatka[25][1] = siatka[11][1];


	// lewy górny prostok¹t sukna 21-24
	siatka[26][0] = siatka[10][0];
	siatka[26][1] = siatka[10][1];
	siatka[27][0] = 0;
	siatka[27][1] = szerokosc / 2;
	siatka[28][0] = zewPromienLuzy;
	siatka[28][1] = szerokosc / 2;
	siatka[29] = siatka[0];

	// prawy dolny prostok¹t sukna 25-28
	siatka[30][0] = siatka[1][0];
	siatka[30][1] = siatka[1][1];
	siatka[31][0] = zewPromienLuzy;
	siatka[31][1] = siatka[11][1];
	siatka[32][0] = siatka[11][0];
	siatka[32][1] = siatka[11][1];
	siatka[33][0] = dlugosc / 2 - wewPromienLuzy;
	siatka[33][1] = 0;

	// lewy górny trapez bandy 29-32

	// lewy górny trapez bandy - PODSTAWA
	for (int i = 34; i <= 37; i++)
	{
		siatka[i][2] = hSukno;
	}
	float w = 2.0f / 3.0f;
	siatka[34][0] = siatka[10][0];
	siatka[34][1] = siatka[10][1];
	siatka[35][0] = szerokoscBandy*w;
	siatka[35][1] = siatka[10][1] + siatka[35][0] * a0[10];
	siatka[36][0] = szerokoscBandy*w;
	siatka[36][1] = szerokosc / 2;
	siatka[37][0] = 0;
	siatka[37][1] = szerokosc / 2;

	float zaw = 0.75; // zawieszenie krawêdzi bandy
	for (int i = 38; i <= 41; i++)
	{
		siatka[i][2] = ghSzkielet*zaw;
	}
	siatka[38][0] = siatka[10][0];
	siatka[38][1] = siatka[10][1];
	siatka[39][0] = kraniec[10].x;
	siatka[39][1] = kraniec[10].y;
	siatka[40][0] = szerokoscBandy;
	siatka[40][1] = szerokosc / 2;
	siatka[41][0] = 0;
	siatka[41][1] = szerokosc / 2;

	for (int i = 42; i <= 45; i++)
	{
		siatka[i][2] = ghSzkielet;
	}
	siatka[42][0] = siatka[10][0];
	siatka[42][1] = siatka[10][1];
	siatka[43][0] = kraniec[10].x;
	siatka[43][1] = kraniec[10].y;
	siatka[44][0] = szerokoscBandy;
	siatka[44][1] = szerokosc / 2;
	siatka[45][0] = 0;
	siatka[45][1] = szerokosc / 2;

	// prawy dolny trapez bandy  33-36

	// prawy dolny trapez bandy - PODSTAWA
	for (int i = 46; i <= 49; i++)
	{
		siatka[i][2] = hSukno;
	}
	siatka[46][0] = siatka[1][0];
	siatka[46][1] = siatka[1][1];
	siatka[47][0] = siatka[1][0] + (kraniec[9].x - siatka[1][0])*w;
	siatka[47][1] = szerokoscBandy*w;
	siatka[48][0] = siatka[33][0] + (kraniec[8].x - siatka[33][0])*w;
	siatka[48][1] = szerokoscBandy*w;
	siatka[49][0] = siatka[33][0];
	siatka[49][1] = siatka[33][1];

	for (int i = 50; i <= 53; i++)
	{
		siatka[i][2] = ghSzkielet*zaw;
	}
	siatka[50][0] = siatka[1][0];
	siatka[50][1] = siatka[1][1];
	siatka[51][0] = kraniec[9].x;
	siatka[51][1] = kraniec[9].y;
	siatka[52][0] = kraniec[8].x;
	siatka[52][1] = kraniec[8].y;
	siatka[53][0] = siatka[33][0];
	siatka[53][1] = siatka[33][1];
	// 
	for (int i = 54; i <= 57; i++)
	{
		siatka[i][2] = ghSzkielet;
	}
	siatka[54][0] = siatka[1][0];
	siatka[54][1] = siatka[1][1];
	siatka[55][0] = kraniec[9].x;
	siatka[55][1] = kraniec[9].y;
	siatka[56][0] = kraniec[8].x;
	siatka[56][1] = kraniec[8].y;
	siatka[57][0] = siatka[33][0];
	siatka[57][1] = siatka[33][1];

	// pocz¹tek quad_strip DREWNA - SZKIELETU
	// 37-38

	for (int i = 58; i <= 98; i++)
	{
		siatka[i][2] = dhSzkielet;
	}
	siatka[58][0] = -szerokoscZewBandy;
	siatka[58][1] = szerokosc / 2;
	siatka[59][0] = 0;
	siatka[59][1] = szerokosc / 2;
	// 39-40
	siatka[60][0] = -szerokoscZewBandy;
	siatka[60][1] = zewPromienLuzy;
	siatka[61][0] = siatka[10][0];
	siatka[61][1] = siatka[10][1];

	for (float i = 0; i <= 90; i += 10) // 10 pkt siatki (0-9)
	{
		siatka[62 + (int)(i / 5)][0] = szerokoscZewBandy*cos((180 + i) / 180 * PI);
		siatka[62 + (int)(i / 5)][1] = szerokoscZewBandy*sin((180 + i) / 180 * PI);
		siatka[63 + (int)(i / 5)][0] = wewPromienLuzy*cos((135 + i * 2) / 180 * PI);
		siatka[63 + (int)(i / 5)][1] = wewPromienLuzy*sin((135 + i * 2) / 180 * PI);
	}


	siatka[82][0] = zewPromienLuzy;
	siatka[82][1] = -szerokoscZewBandy;
	siatka[83][0] = zewPromienLuzy;
	siatka[83][1] = 0;
	// 39-40

	// dolny prostok¹t bandy zew. 62-65

	siatka[84][0] = siatka[33][0];
	siatka[84][1] = -szerokoscZewBandy;
	siatka[85][0] = siatka[33][0];
	siatka[85][1] = siatka[33][1];

	// triangle_fan dla otoczenia ³uzy 66-72
	siatka[86][0] = siatka[84][0];
	siatka[86][1] = siatka[84][1];
	for (float kat = 180; kat <= 270; kat += 10) // 6 pkt siatki (69-75)
	{
		siatka[69 + (int)kat / 10][0] = dlugosc / 2 + wewPromienLuzy*cos(kat / 180 * PI);
		siatka[69 + (int)kat / 10][1] = wewPromienLuzy*sin(kat / 180 * PI) - odLuzySrod->y;
	}
	siatka[97][0] = dlugosc / 2;
	siatka[97][1] = -szerokoscZewBandy;
	siatka[98][0] = siatka[33][0];
	siatka[98][1] = siatka[33][1];

	for (int i = 99; i <= 139; i++)
	{
		siatka[i][0] = siatka[i - 41][0];
		siatka[i][1] = siatka[i - 41][1];
		siatka[i][2] = ghSzkielet;
	}

	tab = new float*[140];
	for (int i = 0; i<140; i++)
	{
		tab[i] = new float[2];
	}
	tab[0][0] = 0.172;
	tab[0][1] = 0.172;
	tab[1][0] = 0.172;
	tab[1][1] = 0;
	for (int i = 10; i <= 80; i += 10)
	{
		tab[i / 10 + 1][0] = 0.172*cos((float)i / 180 * PI);
		tab[i / 10 + 1][1] = 0.172*sin((float)i / 180 * PI);
	}
	tab[10][0] = 0;
	tab[10][1] = 0.172;


	tab[11][0] = 3.878;
	tab[11][1] = 0.122;
	tab[12][0] = 4;
	tab[12][1] = 0.122;
	for (int i = 100; i <= 170; i += 10)
	{
		tab[i / 10 + 3][0] = 4 + 0.122*cos((float)i / 180 * PI);
		tab[i / 10 + 3][1] = 0.122*sin((float)i / 180 * PI);
	}
	tab[21][0] = 3.878;
	tab[21][1] = 0;

	// g³ówny prostok¹t sukna
	tab[22][0] = 0.172;
	tab[22][1] = 0.122;  // na poziomie wewPromienLuzy
	tab[23][0] = 0.172;
	tab[23][1] = 2;
	tab[24][0] = 4;
	tab[24][1] = 2;
	tab[25][0] = 4;
	tab[25][1] = 0.122;

	// górny prostok¹t sukna

	tab[26][0] = 0;
	tab[26][1] = 0.172;
	tab[27][0] = 0;
	tab[27][1] = 2;
	tab[28][0] = 0.172;
	tab[28][1] = 2;
	tab[29][0] = 0.172;
	tab[29][1] = 0.172;

	// dolny prostok¹t sukna

	tab[30][0] = 0.172;
	tab[30][1] = 0;
	tab[31][0] = 0.172;
	tab[31][1] = 0.122;
	tab[32][0] = 3.878;
	tab[32][1] = 0.122;
	tab[33][0] = 3.878;  // ze wzglêdu na wewPromienLuzy
	tab[33][1] = 0;

	// lewa banda
	tab[34][0] = 0;
	tab[34][1] = 0.172;
	tab[35][0] = 0.133;
	tab[35][1] = 0.339;
	tab[36][0] = 0.133;
	tab[36][1] = 2;
	tab[37][0] = 0;
	tab[37][1] = 2;

	// ni¿sza kondygnacja bandy - POCHY£A
	tab[38][0] = 0;
	tab[38][1] = 0;
	tab[39][0] = 0;
	tab[39][1] = 0.089;
	tab[40][0] = 0.213;
	tab[40][1] = 0;
	tab[41][0] = 0.213;
	tab[41][1] = 0.089;

	tab[42][0] = 1.875;
	tab[42][1] = 0;
	tab[43][0] = 1.875;
	tab[43][1] = 0.089;
	tab[44][0] = 2.007;
	tab[44][1] = 0;
	tab[45][0] = 2.007;
	tab[45][1] = 0.089;

	// wy¿sza kondygnacja bandy - PROSTA

	tab[46][0] = tab[46 - 8][0];
	tab[46][1] = tab[46 - 7][1];
	tab[47][0] = tab[47 - 8][0];
	tab[47][1] = tab[46][1] + (ghSzkielet - siatka[38][2]) / 90;
	tab[48][0] = tab[48 - 8][0];
	tab[48][1] = tab[48 - 7][1];
	tab[49][0] = tab[49 - 8][0];
	tab[49][1] = tab[48][1] + (ghSzkielet - siatka[39][2]) / 90;

	tab[50][0] = tab[50 - 8][0];
	tab[50][1] = tab[50 - 7][1];
	tab[51][0] = tab[51 - 8][0];
	tab[51][1] = tab[50][1] + (ghSzkielet - siatka[40][2]) / 90;
	tab[52][0] = tab[52 - 8][0];
	tab[52][1] = tab[52 - 7][1];
	tab[53][0] = tab[53 - 8][0];
	tab[53][1] = tab[52][1] + (ghSzkielet - siatka[41][2]) / 90;

	// dolna banda
	tab[54][0] = 0.172;
	tab[54][1] = 0;
	tab[55][0] = 0.339;
	tab[55][1] = 0.133;
	tab[56][0] = 3.811;
	tab[56][1] = 0.133;
	tab[57][0] = 3.878;
	tab[57][1] = 0;


	// ni¿sza kondygnacja bandy - POCHY£A

	tab[58][0] = 0;
	tab[58][1] = 0;
	tab[59][0] = 0;
	tab[59][1] = 0.089;
	tab[60][0] = 0.213;
	tab[60][1] = 0;
	tab[61][0] = 0.213;
	tab[61][1] = 0.089;

	tab[62][0] = 3.69;
	tab[62][1] = 0;
	tab[63][0] = 3.69;
	tab[63][1] = 0.089;
	tab[64][0] = 3.9;
	tab[64][1] = 0;
	tab[65][0] = 3.9;
	tab[65][1] = 0.089;

	// wy¿sza kondygnacja bandy - PROSTA

	tab[66][0] = tab[66 - 8][0];
	tab[66][1] = tab[66 - 7][1];
	tab[67][0] = tab[67 - 8][0];
	tab[67][1] = tab[66][1] + (ghSzkielet - siatka[38][2]) / 90;
	tab[68][0] = tab[68 - 8][0];
	tab[68][1] = tab[68 - 7][1];
	tab[69][0] = tab[69 - 8][0];
	tab[69][1] = tab[68][1] + (ghSzkielet - siatka[39][2]) / 90;

	tab[70][0] = tab[70 - 8][0];
	tab[70][1] = tab[70 - 7][1];
	tab[71][0] = tab[71 - 8][0];
	tab[71][1] = tab[70][1] + (ghSzkielet - siatka[40][2]) / 90;
	tab[72][0] = tab[72 - 8][0];
	tab[72][1] = tab[72 - 7][1];
	tab[73][0] = tab[73 - 8][0];
	tab[73][1] = tab[72][1] + (ghSzkielet - siatka[41][2]) / 90;


	float a = (zewPromienLuzy - wewPromienLuzy*sin(135 / 180 * PI));
	float b = -wewPromienLuzy*cos(135 / 180 * PI);
	float tmpOdc = sqrt(a*a + b*b);
	tab[74][0] = 0;
	tab[74][1] = 0;
	tab[75][0] = 0;
	tab[75][1] = 1;

	tab[76][0] = 1;
	tab[76][1] = 0;
	tab[77][0] = 1;
	tab[77][1] = 1;

	tab[78][0] = tab[76][0] + tmpOdc / 164.5; // 164.5 - 1 d³ugoœæ tekstury // dalej od ³uzy
	tab[78][1] = 0;

	tab[79][0] = tab[77][0] + tmpOdc / 164.5;  // bli¿ej ³uzy
	tab[79][1] = 1; // szerokoscZewBandy - 1 wysokoœæ

	float ds = 20.0f / 180.0f*PI*wewPromienLuzy;
	ds /= 164.5; // przesuniêcie w tekselach tekstury
	for (float i = 10; i <= 90; i += 10) // 10 pkt siatki (0-9)
	{
		tab[78 + (int)(i / 5)][0] = tab[76 + (int)(i / 5)][0] + ds;
		tab[78 + (int)(i / 5)][1] = 0;
		tab[79 + (int)(i / 5)][0] = tab[77 + (int)(i / 5)][0] + ds;
		tab[79 + (int)(i / 5)][1] = 1;
	}
	tab[98][0] = tab[96][0] + tmpOdc / 164.5;  // dalej
	tab[98][1] = 0;
	tab[99][0] = tab[97][0] + tmpOdc / 164.5;  // bli¿ej
	tab[99][1] = 1;

	tab[100][0] = tab[98][0] + 336.5 / 164.5;
	tab[100][1] = 0;
	tab[101][0] = tab[99][0] + 336.5 / 164.5;  // bli¿ej
	tab[101][1] = 1;

	// dla TRIANGLE_FAN drewna
	tab[102][0] = tab[100][0];  // bli¿ej
	tab[102][1] = 0;

	for (int i = 87; i <= 96; i++) // 6 pkt siatki (69-75)
	{
		tab[i + 16][0] = tab[102][0] + (siatka[i][0] - siatka[86][0]) / 164.5;  // bli¿ej
		tab[i + 16][1] = (szerokoscZewBandy + siatka[i][1]) / szerokoscZewBandy;
	}

	tab[113][0] = tab[102][0] + wewPromienLuzy / 164.5;  // bli¿ej
	tab[113][1] = 0;

	tab[114][0] = tab[103][0];
	tab[114][1] = 0;
	tab[115][0] = tab[103][0];
	tab[115][1] = 1;
	tab[116][0] = tab[103][0] + odLuzySrod->y / 164.5;
	tab[116][1] = 0;
	tab[117][0] = tab[103][0] + odLuzySrod->y / 164.5;
	tab[117][1] = 1;
	for (int i = 1; i <= 9; i++) // 6 pkt siatki (69-75)
	{
		tab[116 + i * 2][0] = tab[116][0] + (2 * wewPromienLuzy*PI*10.0f / 360.0f)*i / 164.5;
		tab[116 + i * 2][1] = 0;
		tab[117 + i * 2][0] = tab[116 + i * 2][0];
		tab[117 + i * 2][1] = 1;
	}
	tab[136][0] = tab[113][0];
	tab[136][1] = 0;
	tab[137][0] = tab[113][0];
	tab[137][1] = 1;
	for (int i = 74; i <= 137; i++)
	{
		if (i != 135)
		{
			tab[i][0] /= tab[135][0];
		}
	}
	tab[135][0] /= tab[135][0];


	lStol = glGenLists(1);

	// utworzenie pierwszej listy - test rysowania punktów
	glNewList(lStol, GL_COMPILE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i <= 10; i++)
	{
		glTexCoord2fv(tab[i]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	for (int i = 11; i <= 21; i++)
	{
		glTexCoord2fv(tab[i]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	for (int i = 22; i <= 25; i++)
	{
		glTexCoord2fv(tab[i]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int i = 26; i <= 29; i++)
	{
		glTexCoord2fv(tab[i]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int i = 30; i <= 33; i++)
	{
		glTexCoord2fv(tab[i]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	////////////////////////////////////////////////
	// lewa górna banda 3D
	////////////////////////////////////////////////

	glBegin(GL_QUADS);
	for (int i = 42; i <= 45; i++)
	{
		glTexCoord2fv(tab[i - 8]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);

	for (int i = 34; i <= 37; i++)
	{
		glTexCoord2fv(tab[38 + (i - 34) * 2]);
		glVertex3fv(siatka[i]);
		glTexCoord2fv(tab[39 + (i - 34) * 2]);
		glVertex3fv(siatka[i + 4]);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);

	for (int i = 38; i <= 41; i++)
	{
		glTexCoord2fv(tab[46 + (i - 38) * 2]);
		glVertex3fv(siatka[i]);
		glTexCoord2fv(tab[47 + (i - 38) * 2]);
		glVertex3fv(siatka[i + 4]);
	}
	glEnd();
	////////////////////////////////////////////////
	// prawa dolna  banda 3D
	////////////////////////////////////////////////


	glBegin(GL_QUADS);
	for (int i = 54; i <= 57; i++)
	{
		glTexCoord2fv(tab[i]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (int i = 46; i <= 49; i++)
	{
		glTexCoord2fv(tab[58 + (i - 46) * 2]);
		glVertex3fv(siatka[i]);
		glTexCoord2fv(tab[59 + (i - 46) * 2]);
		glVertex3fv(siatka[i + 4]);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (int i = 50; i <= 53; i++)
	{
		glTexCoord2fv(tab[66 + (i - 50) * 2]);
		glVertex3fv(siatka[i]);
		glTexCoord2fv(tab[67 + (i - 50) * 2]);
		glVertex3fv(siatka[i + 4]);
	}
	glEnd();

	// SZKIELET -  DÓ£
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	glBegin(GL_QUAD_STRIP);
	for (int i = 58; i <= 85; i++)
	{
		glTexCoord2fv(tab[i + 16]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	for (int i = 86; i <= 97; i++)
	{
		glTexCoord2fv(tab[i + 16]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);

	for (int i = 99; i <= 126; i++)
	{
		glTexCoord2fv(tab[i - 25]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	for (int i = 127; i <= 138; i++)
	{
		glTexCoord2fv(tab[i - 25]);
		glVertex3fv(siatka[i]);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	// TY£
	for (int i = 58; i <= 84; i += 2)
	{
		glTexCoord2fv(tab[i + 16]);
		glVertex3fv(siatka[i]);
		glTexCoord2fv(tab[i + 17]);
		glVertex3fv(siatka[i + 41]);
	}
	glTexCoord2fv(tab[136]);
	glVertex3fv(siatka[97]);
	glTexCoord2fv(tab[137]);
	glVertex3fv(siatka[138]);
	glEnd();

	// WNÊTRZE
	glBegin(GL_QUAD_STRIP);
	for (int i = 85; i >= 59; i -= 2)
	{
		glTexCoord2fv(tab[i + 15]);
		glVertex3fv(siatka[i]);
		glTexCoord2fv(tab[i + 16]);
		glVertex3fv(siatka[i + 41]);
	}
	glEnd();

	// przy œrodkowej ³uzie
	glBegin(GL_QUAD_STRIP);
	glTexCoord2fv(tab[114]);
	glVertex3fv(siatka[98]);
	glTexCoord2fv(tab[115]);
	glVertex3fv(siatka[139]);
	for (int i = 87; i <= 96; i++)
	{
		glTexCoord2fv(tab[116 + (i - 87) * 2]);
		glVertex3fv(siatka[i]);
		glTexCoord2fv(tab[117 + (i - 87) * 2]);
		glVertex3fv(siatka[i + 41]);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(40, 40, hNoga);
	glCallList(lNoga);

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	glEndList();
}

CStol::~CStol()
{
	Mix_FreeChunk(uderzenie);
	glDeleteTextures(4, textures);
	gluDeleteQuadric(qNoga);
	glDeleteLists(lStol, 1);
	glDeleteLists(lNoga, 1);
	glDeleteLists(lBila, 1);
	for (int i = 0; i<140; i++)
		delete[] tab[i];
	delete[] tab;

	for (int i = 0; i<200; i++)
		delete[] siatka[i];
	delete[] siatka;

	for (int i = 0; i<5000; i++)
		delete[] siatkaBili[i];
	delete[] siatka;

	delete wierzcholek; // punkt w którym zwieszony jest ca³y stó³ - tak¿e wsp. z - wysokoœæ
	if (wierzcholek2) delete wierzcholek2;
	delete rog;
	delete zewBanda;
	delete kij;
	delete tmpBila2;
	delete rBila2;

	delete katBandy;
	delete kraniec;
	delete[] a0;
	delete[] b0;
	for (int i = 0; i<sizeof(bile) / sizeof(bile[0]); i++)
		delete bile[i];
	delete[] bile;  // wskaŸniki na elementy, które bêd¹ rysowane wzglêdem naro¿nika pola gry
	delete[] wbite;
	delete odLuzySrod;
	delete dWektor; // wektor o d³ugoœci promienia wewnêtrznego
	delete dWektor2;
}


//przesuniecie po obrocie wokó³ w³asnej osi i przesuniêciu
void CFigura::oPrzesuniecie(float tx, float ty, float tz)
{
	float odl_do2;
	float odl;
	glPushMatrix();
	glTranslatef(przesuniecie.x, przesuniecie.y, przesuniecie.z);
	glRotatef(obrot.k3, 0, 0, 1);
	glTranslatef(tx, ty, tz);
	float aktualny[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, aktualny);
	float docX = aktualny[12];
	float docY = aktualny[13];
	przesuniecie.x = docX;  //powinno byæ this zamiast leopard2
	przesuniecie.y = docY;

	odl_do2 = pow(przesuniecie.x, 2) + pow(przesuniecie.y, 2);
	odl = sqrt(odl_do2);  //aktualizacja odleg³oœci do porównania z promieniami toru
	gObrot.k3 = acos(docX / odl)*180.0f / PI; // w stopniach zwraca wartosc z [0;180]
	glPopMatrix();
}
void CStol::narysujKrawedz()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(0, zewPromienLuzy, hSukno);
	glVertex3f(kraniec[10].x + 1.5, kraniec[10].y, hSukno);
	glVertex3f(kraniec[10].x + 1.5, szerokosc / 2, hSukno);
	glVertex3f(0, szerokosc / 2, hSukno);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(zewPromienLuzy, 0, hSukno);
	glVertex3f(kraniec[9].x, kraniec[9].y + 1.5, hSukno);
	glVertex3f(kraniec[8].x, kraniec[8].y + 1.5, hSukno);
	glVertex3f(dlugosc / 2 - wewPromienLuzy, 0, hSukno);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}
void CStol::rysuj()
{
	static long long int p = 0, k = 0;
	long double roz;
	static float rx = 0, rz = 0, ty = 0, tx = 0, tz = 0;

	float brazowy[4];
	brazowy[0] = 0.36f;
	brazowy[1] = 0.2f;
	brazowy[2] = 0.09f;
	brazowy[3] = 0.2f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	float eyex2, eyey2, eyez2, centerx2, centery2, centerz2;
	//	bilard1->shot_con2=bilard1->shot_con1/3;
	//	bilard1->overview_con2=bilard1->overview_con1/4.5;
	// uaktualnianie kamer
	if (bilard1->currentCam == CAM_OVERVIEW)
	{
		centerx2 = bilard1->cam[bilard1->currentCam]->center.x;
		centery2 = bilard1->cam[bilard1->currentCam]->center.y;

		eyex2 = centerx2 - cos(bilard1->overview_angle / 180 * PI)*bilard1->overview_con1;
		eyey2 = centery2 - sin(bilard1->overview_angle / 180 * PI)*bilard1->overview_con1;
		eyez2 = H_SUKNO + bilard1->overview_con2;

		bilard1->cam[CAM_OVERVIEW]->eye = CPunkt3D(eyex2, eyey2, eyez2);
	}
	if (bilard1->currentCam == CAM_SHOT)
	{
		centerx2 = bile1[0]->przesuniecie.x + sPrzesuniecie.x;
		centery2 = bile1[0]->przesuniecie.y + sPrzesuniecie.y;
		centerz2 = bilard1->cam[bilard1->currentCam]->center.z;

		/*		if(bile[0]->Vp>0) // œledzenie bili bia³ej
		{
		eyex2=centerx2-cos(bile[0]->kierunek/180*PI)*bilard1->shot_con1;
		eyey2=centery2-sin(bile[0]->kierunek/180*PI)*bilard1->shot_con1;
		}
		*/
		eyex2 = centerx2 - cos(kij1->obrot.k3 / 180 * PI)*bilard1->shot_con1;
		eyey2 = centery2 - sin(kij1->obrot.k3 / 180 * PI)*bilard1->shot_con1;
		eyez2 = H_SUKNO + bilard1->shot_con2;

		bilard1->cam[CAM_SHOT]->eye = CPunkt3D(eyex2, eyey2, eyez2);
		bilard1->cam[CAM_SHOT]->center = CPunkt3D(centerx2, centery2, centerz2);
	}
	else if (bilard1->currentCam == CAM_TEST)
	{
		bilard1->cam[CAM_TEST]->eye = CPunkt3D(sPrzesuniecie.x + bile1[0]->przesuniecie.x, sPrzesuniecie.y + bile1[0]->przesuniecie.y, H_SUKNO * 3 / 4);
		bilard1->cam[CAM_TEST]->center = CPunkt3D(sPrzesuniecie.x + bile1[0]->przesuniecie.x, sPrzesuniecie.y + bile1[0]->przesuniecie.y, H_SUKNO);
	}

	float eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz;
	eyex = bilard1->cam[bilard1->currentCam]->eye.x;
	eyey = bilard1->cam[bilard1->currentCam]->eye.y;
	eyez = bilard1->cam[bilard1->currentCam]->eye.z;

	centerx = bilard1->cam[bilard1->currentCam]->center.x;
	centery = bilard1->cam[bilard1->currentCam]->center.y;
	centerz = bilard1->cam[bilard1->currentCam]->center.z;

	upx = bilard1->cam[bilard1->currentCam]->up.x;
	upy = bilard1->cam[bilard1->currentCam]->up.y;
	upz = bilard1->cam[bilard1->currentCam]->up.z;

	// ustawienie bry³y rzutowania
	if (bilard1->currentCam == CAM_TOP)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();   // zachowuje star¹ macierz projekcji
		glLoadIdentity();
		glOrtho(-WindowWidth / 2, WindowWidth / 2, -WindowHeight / 2, WindowHeight / 2, -H_SUKNO / 2, 2000);
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();	// przywraca star¹ macierz projekcji

	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
	glTranslatef(sPrzesuniecie.x, sPrzesuniecie.y, H_SUKNO);
	glScalef(WindowWidth / pWindowWidth, WindowHeight / pWindowHeight, 1);

	glPushMatrix();
	glTranslatef(dlugosc / 2, szerokosc / 2, 0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0->specular);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0->diffuse);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, light0->spot_cutoff);
	glLightfv(GL_LIGHT0, GL_POSITION, light0->position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0->direction);


	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_BLEND);

	glCallList(lStol);
	if (bilard1->currentCam == CAM_TOP)stol1->narysujKrawedz();

	// ŒRODKOWE NOGI STO£U
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);

	glTranslatef(dlugosc / 2, 40, hNoga);
	glCallList(lNoga);
	glTranslatef(0, szerokosc - 80, 0);
	glCallList(lNoga);

	glPopMatrix();

	glPushMatrix();
	glTranslatef(dlugosc, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(lStol);
	if (bilard1->currentCam == CAM_TOP)stol1->narysujKrawedz();

	glTranslatef(0, szerokosc, 0);
	glScalef(1, -1, 1);
	glCallList(lStol);
	if (bilard1->currentCam == CAM_TOP)stol1->narysujKrawedz();

	glTranslatef(dlugosc, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(lStol);
	if (bilard1->currentCam == CAM_TOP)stol1->narysujKrawedz();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glLineWidth(1);

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	for (float kat = 90; kat >= 0; kat -= 5)
	{
		glVertex3f(odlBazy - promienBazy*cos(kat / 180 * PI), szerokosc / 2 + promienBazy*sin(kat / 180 * PI), hSukno);
	}
	for (float kat = 350; kat >= 270; kat -= 5)
	{
		glVertex3f(odlBazy - promienBazy*cos(kat / 180 * PI), szerokosc / 2 + promienBazy*sin(kat / 180 * PI), hSukno);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(odlBazy, szerokosc, hSukno);
	glVertex3f(odlBazy, 0, hSukno);
	glEnd();

	if (bilard1->bialaPrzytrzymana)
	{
		kij->przesunDo(bile1[0]->przesuniecie.x, bile1[0]->przesuniecie.y);
	}
	glPushMatrix();

	if (!bilard1->uderzenie)
	{
		bool wyznaczona = false;
		float pDlugosc = 7;
		float odstep = 3;
		float pOdl;
		long double yx, a, py, px;
		py = sin(kij->obrot.k3 / 180 * PI);
		px = cos(kij->obrot.k3 / 180 * PI);
		yx = tan((long double)(kij->obrot.k3*PI) / 180.0f);
		a = yx;
		CPunkt3D pKreski, kKreski;
		float odlMin = 720.0f;
		unsigned int indMin;
		bool rysowanie = true;
		float t1 = px*pDlugosc;
		float t2 = px*odstep;
		float r = bile1[0]->r;
		bool warunek1, warunek2;
		float x, y, b, xm, ym; // do wzoru na prost¹ y=ax+b

							   // œrodek bia³ej bili znajduje siê na prostej kija
		x = bile1[0]->przesuniecie.x;
		y = bile1[0]->przesuniecie.y;

		if (a == 0)
		{
			a = 1.0f / 10000.0f;
		}
		b = y - (a*x);

		// wyznaczamy p.przeciêcia prostej kija i prostej prost. do prostej kija przechodz¹cej przez œrodek bili uderzanej
		float minOddalenie = 2000.0f;
		float bOddalenie = 0.0f;
		float a2, b2, b3, a3, rx1, ry1, rx2, ry2, kOdl;
		CPunkt3D pprz;
		CPunkt3D bPprz; // p. przeciêcia z band¹ prostej przerywanej
		if (a != 0)
			a2 = -1 / a;
		else
			a2 = -9999.0f;
		a3 = a2;
		b3 = bile1[0]->przesuniecie.y - bile1[0]->przesuniecie.x*a3;

		float h;
		bool pWarunek1;
		bool pWarunek2;

		bool rWarunek1;
		bool rWarunek2;

		for (int i = 1; i<22; i++)
		{
			pWarunek1 = bile1[i]->przesuniecie.y>(a3*bile1[i]->przesuniecie.x + b3) && (kij->obrot.k3 >= 0 && kij->obrot.k3<180.0f);
			pWarunek2 = bile1[i]->przesuniecie.y<(a3*bile1[i]->przesuniecie.x + b3) && (kij->obrot.k3<360.0f && kij->obrot.k3 >= 180.0f);
			if (pWarunek1 || pWarunek2) //rozwa¿amy bile powy¿ej prostej prostopad³ej o tym równaniu
			{
				b2 = bile1[i]->przesuniecie.y - bile1[i]->przesuniecie.x*a2;

				// rozwi¹zanie uk³adu równañ
				pprz.x = (b2 - b) / (a - a2);
				pprz.y = a2*pprz.x + b2;

				// min oddalenie bili pozornej od bili bia³ej 

				rx1 = bile1[i]->przesuniecie.x - pprz.x;
				ry1 = bile1[i]->przesuniecie.y - pprz.y;
				kOdl = sqrt(rx1*rx1 + ry1*ry1);

				if (kOdl<(2 * r)) // pierwsza napotkana przy tym warunku jest dotykana przez bia³¹
				{
					h = sqrt(4 * r*r - kOdl*kOdl);
					tmpBila2->przesuniecie.x = pprz.x - px*h;
					tmpBila2->przesuniecie.y = pprz.y - py*h;
					rx2 = bile1[0]->przesuniecie.x - tmpBila2->przesuniecie.x;
					ry2 = bile1[0]->przesuniecie.y - tmpBila2->przesuniecie.y;
					//odleg³oœæ: bia³a - p. przeciêcia prostych
					bOddalenie = sqrt(rx2*rx2 + ry2*ry2);

					if (bOddalenie<minOddalenie) // dlatego te¿ sprawdzamy która jest najbli¿ej 
					{
						rWarunek1 = (tmpBila2->przesuniecie.x >= stol1->szerokoscBandy + r && tmpBila2->przesuniecie.x <= stol1->dlugosc - r - stol1->szerokoscBandy);
						rWarunek2 = (tmpBila2->przesuniecie.y >= r + stol1->szerokoscBandy && tmpBila2->przesuniecie.y <= stol1->szerokosc - r - stol1->szerokoscBandy);
						if (rWarunek1 && rWarunek2) // je¿li nie le¿y poza polem gry
						{
							wyznaczona = true;
							rBila2->przesuniecie.x = tmpBila2->przesuniecie.x;
							rBila2->przesuniecie.y = tmpBila2->przesuniecie.y;
							rBila2->przesuniecie.z = tmpBila2->przesuniecie.z;
							minOddalenie = bOddalenie;
							odlMin = kOdl;
							indMin = i;
						}
					}
				}
			}
		}

		// mo¿na zmieniæ na przesuwanie funkcyjne bili

		if (!bilard1->bialaPrzytrzymana)  // je¿eli bia³a nie jest w rêce rysujemy tor bili bia³ej i pozorn¹ bilê bia³¹
		{

			for (float dx = 10; rysowanie == true; dx += 10)
			{
				pKreski.x = bile1[0]->przesuniecie.x + (dx*px);
				pKreski.y = bile1[0]->przesuniecie.y + (dx*py);
				kKreski.x = bile1[0]->przesuniecie.x + (dx + pDlugosc)*px;
				kKreski.y = bile1[0]->przesuniecie.y + (dx + pDlugosc)*py;
				warunek1 = pKreski.x>(stol1->dlugosc - stol1->szerokoscBandy - r) || pKreski.y>(stol1->szerokosc - stol1->szerokoscBandy - r);
				warunek2 = pKreski.x<(stol1->szerokoscBandy + r) || pKreski.y<(stol1->szerokoscBandy + r);

				if (pKreski.x<(stol1->szerokoscBandy + r))
				{
					if (!wyznaczona)
					{
						rBila2->przesuniecie.x = (stol1->szerokoscBandy + r);
						rBila2->przesuniecie.y = rBila2->przesuniecie.x*a + b;
					}
					rysowanie = false;
				}
				if (pKreski.x>(stol1->dlugosc - stol1->szerokoscBandy - r))
				{
					if (!wyznaczona)
					{
						rBila2->przesuniecie.x = stol1->dlugosc - stol1->szerokoscBandy - r;
						rBila2->przesuniecie.y = rBila2->przesuniecie.x*a + b;
					}
					rysowanie = false;
				}
				if (pKreski.y>(stol1->szerokosc - stol1->szerokoscBandy - r))
				{
					if (!wyznaczona)
					{
						rBila2->przesuniecie.y = stol1->szerokosc - stol1->szerokoscBandy - r;
						rBila2->przesuniecie.x = (rBila2->przesuniecie.y - b) / a;
					}
					rysowanie = false;
				}
				if (pKreski.y<(stol1->szerokoscBandy + r))
				{
					if (!wyznaczona)
					{
						rBila2->przesuniecie.y = (stol1->szerokoscBandy + r);
						rBila2->przesuniecie.x = (rBila2->przesuniecie.y - b) / a;
					}
					rysowanie = false;
				}

				if (dx>minOddalenie)
				{
					rysowanie = false;
				}

				if (rysowanie)
				{
					glLineWidth(1);
					glColor3f(1.0f, 1.0f, 1.0f);
					glBegin(GL_LINE_STRIP);
					glVertex3f(pKreski.x, pKreski.y, hSukno);
					glVertex3f(kKreski.x, kKreski.y, hSukno);
					glEnd();
				}
			}

			rBila2->przesuniecie.z = hBila;

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			rBila2->rysuj();
			glDisable(GL_BLEND);

		}
	}

	long double suma = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&p));
	for (int i = 0; i<22; i++)
	{
		if (!wbite[i])
		{
			bile[i]->rysuj();

		}

	}
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&k));
	roz = static_cast<long double>(k - p) / freq;

	glPopMatrix();
	if (!bilard1->uderzenie && !bilard1->bialaPrzytrzymana && !bilard1->koniec && bilard1->currentCam != CAM_SHOT)kij->rysuj();

	long long int x, z;

	glLoadIdentity();
	glFlush();

	if (bilard1->animacja)
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&k));

	}
}


void CStol::odbicie(int i, int j, float odleglosc, float xodl, float yodl)
{

	float p, px, py, pz, dp, dpx, dpy, dpz;
	float kosalfa;
	px = (bile[i]->Vx - bile[j]->Vx)*bile[j]->waga;
	py = (bile[i]->Vy - bile[j]->Vy)*bile[j]->waga;
	p = sqrt(pow(px, 2) + pow(py, 2));

	// mo¿na trochê przyspieszyæ funkcjê eliminuj¹c masê z liczenia (masa jednostkowa)
	if (p >= 0)
	{
		kosalfa = ((xodl*px) + (yodl*py)/*+(zodl*pz)  */) / (odleglosc*p);      // policz kosinus alfa
	}
	if (kosalfa>0)
	{

		dp = p*kosalfa * 2 * bile[i]->waga / (bile[j]->waga + bile[i]->waga);
		dpx = (xodl / odleglosc)*dp;
		dpy = (yodl / odleglosc)*dp;

		bile[j]->Vx += dpx / bile[j]->waga;
		bile[j]->Vy += dpy / bile[j]->waga;
		bile[j]->Vp = sqrt(bile[j]->Vy*bile[j]->Vy + bile[j]->Vx*bile[j]->Vx);
		bile1[j]->kx = bile[j]->Vx / bile[j]->Vp;
		bile1[j]->ky = bile[j]->Vy / bile[j]->Vp;

		bile[j]->kierunek = atan(abs(bile[j]->Vy / bile[j]->Vx));  // kat predkosci
		bile[j]->kierunek *= 180 / PI;
		translacja(bile[j]->kierunek, bile[j]->Vx, bile[j]->Vy, 1); // k¹t ustawiamy w zale¿noœci od æwiartki

		bile[i]->Vx -= dpx / bile[i]->waga;
		bile[i]->Vy -= dpy / bile[i]->waga;
		bile[i]->Vp = sqrt(bile[i]->Vy*bile[i]->Vy + bile[i]->Vx*bile[i]->Vx);
		bile[i]->kx = bile[i]->Vx / bile[i]->Vp;
		bile[i]->ky = bile[i]->Vy / bile[i]->Vp;

		bile[i]->kierunek = atan(abs(bile[i]->Vy / bile[i]->Vx));  // kat predkosci
		bile[i]->kierunek *= 180 / PI;
		translacja(bile[i]->kierunek, bile[i]->Vx, bile[i]->Vy, 1); // k¹t ustawiamy w zale¿noœci od æwiartki

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&poprzedni[j]));
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&poprzedni[i]));
	}

}

void  CStol::sprawdzLuze(unsigned short int nrLuzy, unsigned short int nrBili)
{
	CPunkt3D t[3];
	float odlKant[3];
	luza = true;  // bila w obszarze ³uzy
	bool odbita = false;
	bool luzaOdbite[3];
	bool warunek;		// czy bila 
	bool odbWarunek1;	// czy bila odbi³a siê od lewej bandy - 1. czy potencjalny p. styku z band¹ ³uzy le¿y powy¿ej prostej
						//					&&					2. czy wsp. y p. styku le¿y powy¿ej wsp.y krañca ³uzy
	bool odbWarunek2;   // -||- od prawej
	static bool odbicie = false; // czy zosta³a ju¿ odbita od bandy bila

	float srodekX = bile1[nrBili]->przesuniecie.x;
	float srodekY = bile1[nrBili]->przesuniecie.y;
	float norKat, rozKat, katp, r;
	r = bile1[0]->r;

	luzaOdbite[1] = false;
	luzaOdbite[2] = false;

	t[1].x = r*cos(katBandy[2 * nrLuzy - 1] / 180 * PI) + srodekX;  // punkt styku z band¹ ³uzy
	t[1].y = r*sin(katBandy[2 * nrLuzy - 1] / 180 * PI) + srodekY;

	t[2].x = r*cos(katBandy[2 * nrLuzy] / 180 * PI) + srodekX;  // punkt styku z band¹ ³uzy
	t[2].y = r*sin(katBandy[2 * nrLuzy] / 180 * PI) + srodekY;

	switch (nrLuzy) // ustawiamy warunek wbicia w zale¿noœci od numeru £uzy
	{
	case 1:
		warunek = pow((srodekX - dlugosc / 2), 2) + pow((srodekY - szerokosc), 2)<pow(wewPromienLuzy, 2);
		odbWarunek1 = t[1].y>(a0[2 * nrLuzy - 1] * t[1].x + b0[2 * nrLuzy - 1]) && t[1].y>kraniec[1].y;
		odbWarunek2 = t[2].y>(a0[2 * nrLuzy] * t[2].x + b0[2 * nrLuzy]) && t[2].y>kraniec[2].y;
		break;
	case 2:
		warunek = pow((srodekX - dlugosc), 2) + pow((srodekY - 360), 2)<pow(zewPromienLuzy, 2);
		odbWarunek1 = t[1].y>(a0[2 * nrLuzy - 1] * t[1].x + b0[2 * nrLuzy - 1]) && t[1].y>kraniec[3].y;
		odbWarunek2 = t[2].y<(a0[2 * nrLuzy] * t[2].x + b0[2 * nrLuzy]) && t[2].y>kraniec[4].y;
		break;
	case 3:
		warunek = pow((srodekX - dlugosc), 2) + pow((srodekY - 0), 2)<pow(zewPromienLuzy, 2);
		odbWarunek1 = t[1].y>(a0[2 * nrLuzy - 1] * t[1].x + b0[2 * nrLuzy - 1]) && t[1].y<kraniec[5].y;
		odbWarunek2 = t[2].y<(a0[2 * nrLuzy] * t[2].x + b0[2 * nrLuzy]) && t[2].y<kraniec[6].y;
		break;
	case 4:
		warunek = pow((srodekX - szerokosc), 2) + pow((srodekY), 2)<pow(wewPromienLuzy, 2);
		odbWarunek1 = t[1].y<(a0[2 * nrLuzy - 1] * t[1].x + b0[2 * nrLuzy - 1]) && t[1].y<kraniec[7].y;
		odbWarunek2 = t[2].y<(a0[2 * nrLuzy] * t[2].x + b0[2 * nrLuzy]) && t[2].y<kraniec[8].y;
		break;
	case 5:
		warunek = pow((srodekX - 0), 2) + pow((srodekY - 0), 2)<pow(zewPromienLuzy, 2);
		odbWarunek1 = t[1].y<(a0[2 * nrLuzy - 1] * t[1].x + b0[2 * nrLuzy - 1]) && t[1].y<kraniec[9].y;
		odbWarunek2 = t[2].y>(a0[2 * nrLuzy] * t[2].x + b0[2 * nrLuzy]) && t[2].y<kraniec[10].y;
		break;
	case 6:
		warunek = pow((srodekX - 0), 2) + pow((srodekY - szerokosc), 2)<pow(zewPromienLuzy, 2);
		odbWarunek1 = t[1].y<(a0[2 * nrLuzy - 1] * t[1].x + b0[2 * nrLuzy - 1]) && t[1].y>kraniec[11].y;
		odbWarunek2 = t[2].y>(a0[2 * nrLuzy] * t[2].x + b0[2 * nrLuzy]) && t[1].y>kraniec[12].y;
		break;
	}

	if (bile1[nrBili]->Vx == 0)
	{
		if (bile1[nrBili]->Vy>0)
			katp = 90;
		else
			katp = 270;
	}
	else
	{
		katp = atan(abs(bile1[nrBili]->Vy / bile1[nrBili]->Vx));  // kat predkosci
		katp *= 180 / PI;
		translacja(katp, bile1[nrBili]->Vx, bile1[nrBili]->Vy, 1); // k¹t ustawiamy w zale¿noœci od æwiartki
	}

	odlKant[1] = sqrt(pow(kraniec[2 * nrLuzy - 1].x - srodekX, 2) + pow(kraniec[2 * nrLuzy - 1].y - srodekY, 2)); // równanie okrêgu dla bili
	odlKant[2] = sqrt(pow(kraniec[2 * nrLuzy].x - srodekX, 2) + pow(kraniec[2 * nrLuzy].y - srodekY, 2)); // równanie okrêgu dla bili
	static bool kant = false;
	float rx, ry;

	if ((odlKant[1]<(r + 0.1) || odlKant[2]<(r + 0.1)) && !kant) // uderzenie w kant 
																 // !kant -	( je¿eli jeszcze nie rozpoznano tego uderzenia w kant)
	{
		kant = true;
		if (odlKant[1]<(r + 0.1))
		{
			rx = kraniec[2 * nrLuzy - 1].x - srodekX;
			ry = kraniec[2 * nrLuzy - 1].y - srodekY;
			norKat = atan(abs(ry / rx));
		}
		else
		{
			rx = kraniec[2 * nrLuzy].x - srodekX;
			ry = kraniec[2 * nrLuzy].y - srodekY;
		}
		norKat = atan(abs(ry / rx));
		norKat *= 180 / PI;
		translacja(norKat, rx, ry, 1); // katp przez referencjê  // chyba niepotrzebne kat zawsze < 90 

									   // k¹t linii kant-œrodek bili - wzglêdem,której bêdzie odbita bila
		if ((kraniec[2].x - srodekX)<0) // odbicie od prawej strony ³uzy
		{
			if ((kraniec[2].y - srodekY)>0)// dla czêœci górnej(niemo¿liwe) - w³aœciwie niepotrzebne
			{
				norKat += 180;
			}
		}
		rozKat = norKat - katp;
		rozKat = (int)(rozKat + norKat) % 360;
		rozKat = (int)(rozKat + 180) % 360;
		bile1[nrBili]->Vp *= bilard1->wspEn;
		bile1[nrBili]->kx = cos(rozKat / 180 * PI);
		bile1[nrBili]->Vx = bile1[nrBili]->Vp*bile1[nrBili]->kx;
		bile1[nrBili]->ky = sin(rozKat / 180 * PI);
		bile1[nrBili]->Vy = bile1[nrBili]->Vp*bile1[nrBili]->ky;
		bile1[nrBili]->kierunek = atan(abs(bile1[nrBili]->Vy / bile1[nrBili]->Vx));  // kat predkosci
		bile1[nrBili]->kierunek *= 180 / PI;
		translacja(bile1[nrBili]->kierunek, bile1[nrBili]->Vx, bile1[nrBili]->Vy, 1); // k¹t ustawiamy w zale¿noœci od æwiartki

	}
	if ((odlKant[1]>(r + 0.1)) && (odlKant[2]>(r + 0.1)))
	{
		kant = false;
	}

	odbita = false; // zak³adamy, ¿e nie ma odbicia wzglêdem ¿adnej œciany

					// odbicie od prawej œciany œrodkowej górnej ³uzy, patrz¹c ze strony sto³u
					// odbicie od lewej œciany œrodkowej górnej ³uzy, patrz¹c ze strony sto³u

					// wystarczy jedna zmienna 

	if (odbWarunek1 && !kant)
	{
		odbita = true;
		luzaOdbite[1] = true; // czy jest powy¿ej prostej bandy
	}

	if (odbWarunek2 && !kant)
	{
		odbita = true;
		luzaOdbite[2] = true; // czy jest powy¿ej prostej bandy
	}
	if (odbita)
	{
		if (!odbicie)
		{
			odbicie = true;
			if (luzaOdbite[1])
				norKat = katBandy[2 * nrLuzy - 1];
			if (luzaOdbite[2])
				norKat = katBandy[2 * nrLuzy];
			rozKat = norKat - katp;
			if (rozKat<-180) rozKat += 360;
			rozKat = (int)(rozKat + norKat) % 360;
			rozKat = (int)(rozKat + 180) % 360;  //  uzyskany k¹t odbicia od kantu
			bile1[nrBili]->Vp *= bilard1->wspEn;
			bile1[nrBili]->kx = cos(rozKat / 180 * PI);
			bile1[nrBili]->Vx = bile1[nrBili]->Vp*bile1[nrBili]->kx;
			bile1[nrBili]->ky = sin(rozKat / 180 * PI);
			bile1[nrBili]->Vy = bile1[nrBili]->Vp*bile1[nrBili]->kx;
			bile1[nrBili]->kierunek = atan(abs(bile1[nrBili]->Vy / bile1[nrBili]->Vx));  // kat predkosci
			bile1[nrBili]->kierunek *= 180 / PI;
			translacja(bile1[nrBili]->kierunek, bile1[nrBili]->Vx, bile1[nrBili]->Vy, 1); // k¹t ustawiamy w zale¿noœci od æwiartki
		}

	}
	else
	{
		odbicie = false;
	}
	//czy zosta³a wbita do tej ³uzy bila

	if (warunek) // bila zosta³a wbita
	{
		tmpWbite.push_back(nrBili);
		wbite[nrBili] = true;
		bile1[nrBili]->wyzeruj();
	}
	return;
}

int CStol::LoadGLTextures()
{
	SDL_Surface *surface[4];
	string nazwy[4];
	unsigned char tmp;

	nazwy[0] = "texSukno.bmp";
	nazwy[1] = "texBanda.bmp";
	nazwy[2] = "texKij.bmp";
	nazwy[3] = "texBila.bmp";
	glGenTextures(4, textures);

	for (int i = 0; i<3; i++)
	{

		surface[i] = SDL_LoadBMP(nazwy[i].c_str());
		if (!surface[i])
		{
			cout << "Nie za³adowano texstury: " << i << "\n";
			system("PAUSE");
			return 0;
		}

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, surface[i]->w, surface[i]->h, GL_BGR, GL_UNSIGNED_BYTE, surface[i]->pixels);
		SDL_FreeSurface(surface[i]);

	}

	return 1;
}

CBilard::CBilard(CStol * wsk, CGracz ** aGracze, string * sGracz1, string * sGracz2)
{
	counter = 0;
	pCounter = 0;
	sKomunikat = false;
	faul = false;
	wznowienie = false;
	pauza = false;
	stol = wsk;
	gracze = aGracze;
	gracze[0] = new CGracz(sGracz1);
	gracze[1] = new CGracz(sGracz2);

	LoadTextTexture();
	BuildFont();
	remis = false;
	przerwa = false;

	if (!font)
	{
		cout << "Nie zaladowano czcionki-blad\n";
		system("PAUSE");
	}
	cam = new camera*[4];
	for (int i = 0; i<4; i++)
	{
		cam[i] = new camera;
	}
	shot_con1 = 150;
	shot_con2 = 50;
	overview_con1 = stol->dlugosc / 2;
	overview_con2 = 100;
	overview_angle = 0;
	cam[CAM_TOP]->eye = CPunkt3D(0, 0, 0);
	cam[CAM_TOP]->center = CPunkt3D(0, 0, H_SUKNO);
	cam[CAM_TOP]->up = CPunkt3D(0, 1, 0);

	cam[CAM_OVERVIEW]->eye = CPunkt3D(stol->sPrzesuniecie.x - (stol->dlugosc / 2 - overview_con1), stol->sPrzesuniecie.y + stol->szerokosc / 2, H_SUKNO + overview_con2);
	cam[CAM_OVERVIEW]->center = CPunkt3D(0, stol->sPrzesuniecie.y + stol->szerokosc / 2, H_SUKNO);
	cam[CAM_OVERVIEW]->up = CPunkt3D(0, 0, 1);

	cam[CAM_SHOT]->eye.z = H_SUKNO + shot_con2;
	cam[CAM_SHOT]->center.z = H_SUKNO;
	cam[CAM_SHOT]->up = CPunkt3D(0, 0, 1);

	cam[CAM_TEST]->eye.z = H_SUKNO * 3 / 4;
	cam[CAM_TEST]->center.z = H_SUKNO;
	cam[CAM_TEST]->up = CPunkt3D(0, 1, 0);

	currentCam = CAM_TOP;

	kPrzytrzymany = false;
	kPrzesuniecie = 0.0f;

	bPozycje = new float*[6];
	for (int i = 0; i<6; i++)
	{
		bPozycje[i] = new float[3];
		bPozycje[i][0] = 0.9*WindowWidth;
		bPozycje[i][1] = 0.9*WindowHeight - i * 30;
		bPozycje[i][2] = -20;
	}

	hText = -NEAR;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*> (&freq));
	zmiana = false;
	zmCzas = false;
	zmGracza = false;
	nowyRuch = false;
	bialaPrzytrzymana = false;

	poczatekAnimacji = false;
	plik.open("dane.txt", ios::out);
	nastepna = CZERWONA;
	poczatekKolorowych = false;

	poczatek = true; // potrzebna dla wyznaczenia pocz¹tku liczenia czasu gry
	pierwszeUderzenie = false; // czy zasz³o pierwsze uderzenie
	pierwszaUderzona = BRAK; // je¿eli pozostanie brak gdy prêdkoœci siê wyzeruj¹ tzn., ¿e chybiona
	bialaUderzona = false;
	wspEn = 0.95; // wsp. zachowania Energii dla odbiæ
	wartosc = 0;  // dla timera 
	lCzerwonych = 15;
	lKolorowych = 6;

	odd = new float[3];
	odd[0] = 0.1*WindowWidth;
	odd[1] = 0.6*WindowWidth;
	odd[2] = 0.3*WindowWidth;

	tPozycje = new float*[2];
	for (int i = 0; i<2; i++)
		tPozycje[i] = new float[9];
	for (int i = 0; i<2; i++)  // jeden przebieg pêtli - wyniki gracz
	{
		for (int j = 0; j<5; j++)
		{
			tPozycje[i][j] = (1 - (0.05 + 0.03*j))*WindowHeight;
		}
	}

	// pozycja komunikatu gry
	tPozycje[0][5] = odd[2];			   // x
	tPozycje[1][5] = 0.01*WindowWidth;  // y

										// pozycje dla paska si³y uderzenia
	pHeight = 200;
	pWidth = 20;
	border = 3;
	qHeight = 20;
	qWidth = pWidth - 2 * border;
	tHeight = pHeight - 2 * border;
	tWidth = qWidth;

	tPozycje[0][6] = WindowWidth*0.9;
	tPozycje[1][6] = WindowHeight*0.1;

	// pozycja paska przezroczystego -  wzglêdem paska g³ównego
	tPozycje[0][7] = border;
	tPozycje[1][7] = border;

	// pozycja wzglêdem paska przezroczystego
	tPozycje[0][8] = 0;
	tPozycje[1][8] = pHeight - 2 * border - qHeight;

	napisy = new string*[2];
	for (int i = 0; i<5; i++)
	{
		napisy[i] = new string[5];
	}
	napisy[0][0] = "Gracz: ";
	napisy[0][0].append(sGracz1->c_str());
	napisy[1][0] = "Gracz: ";
	napisy[1][0].append(sGracz2->c_str());

	buf = new char[4];

	_itoa(gracze[0]->sPunkty, buf, 10);
	napisy[0][1] = "Punkty: ";
	napisy[0][1].append(buf);
	_itoa(gracze[1]->sPunkty, buf, 10);
	napisy[1][1] = "Punkty: ";
	napisy[1][1].append(buf);

	_itoa(gracze[0]->seria, buf, 10);
	napisy[0][2] = "Break: ";
	napisy[0][2].append(buf);
	_itoa(gracze[1]->seria, buf, 10);
	napisy[1][2] = "Break: ";
	napisy[1][2].append(buf);

	int minuty, sekundy;

	// pocz¹tkowe: czas i czas ruchu
	for (int i = 0; i<2; i++)
	{
		napisy[i][3] = "Czas ruchu: 00:";
		if (gracze[i]->czasRuchu<10)
			napisy[i][3].append("0");
		_itoa(gracze[i]->czasRuchu, buf, 10);
		napisy[i][3].append(buf);

		napisy[i][4] = "Czas: ";
		minuty = gracze[i]->czas / 60;
		sekundy = gracze[i]->czas % 60;
		if (minuty<10)
			napisy[i][4].append("0");
		_itoa(minuty, buf, 10);
		napisy[i][4].append(buf);
		napisy[i][4].append(":");
		if (sekundy<10)
			napisy[i][4].append("0");
		_itoa(sekundy, buf, 10);
		napisy[i][4].append(buf);
	}

	odbite = new bool*[22];
	for (int i = 0; i<22; i++)
	{
		odbite[i] = new bool[22];
	}
	for (int i = 0; i<22; i++)
	{
		for (int j = 0; j<22; j++)
			odbite[i][j] = false;
	}
	prevc = new float*[22];
	for (int i = 0; i<22; i++)
	{
		prevc[i] = new float[22];
	}

	animacja = false;
	uderzenie = false;
	zerowe = false;
	koniec = false;
	aktualnyGracz = (bool)0;
	pPrzytrzymany = false;
	lPrzytrzymany = false;
	wychylenie = 0.0f;
	stala = 15.0f;  // oddalenie kija a prêdkoœæ
	maxVp = stala*kij1->maxOddalenie;
	bialaWRece = true; // na pocz¹tku frame'a
}

CBilard::~CBilard()
{
	KillFont();

	glDeleteTextures(1, &textTexture);
	plik.close();
	delete gracze[0];
	delete gracze[1];
	for (int i = 0; i<sizeof(odbite) / sizeof(odbite[0]); i++)
	{
		delete odbite[i];
	}
	delete[] odbite;

	//dane dotycz¹ce wyœwietlania wyników
	for (int i = 0; i<2; i++)
	{
		delete napisy[i];
	}
	delete[] napisy;
	for (int i = 0; i<5; i++)
	{
		delete tPozycje[i];
	}
	delete[] tPozycje;
	delete[] buf;
}

void CBilard::rysuj()
{
	long long int p, k;
	long double roznica;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&p));
	stol1->rysuj();
	if (bilard1->nowyRuch && !bilard1->poczatek)
	{
		SDL_Delay(1500);
		bilard1->przerwa = true;
	}
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&k));
	roznica = static_cast<long double>(k - p) / freq;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&p));
	if (!bilard1->uderzenie) bilard1->pokazStanGry();
	glFlush();
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&k));
	roznica = static_cast<long double>(k - p) / freq;
}



///////////////////  NIE DO KOÑCA DOBRY POMYS£ - B£ÊDENE I ZA PROSTE PODEJŒCIE 
//		DO ZNAJDOWANIA NOWEJ POZYCJI

bool CBilard::czyZajety(CPunkt3D & p1, short int & iZajety)
{
	CPunkt3D iBila;
	iZajety = -1;
	for (int i = 0; i<22; i++)
	{
		iBila.x = bile1[i]->przesuniecie.x;
		iBila.y = bile1[i]->przesuniecie.y;
		if (odleglosc(iBila, p1)<2 * bile1[0]->r) // punkt jest zajêty przez i-t¹ bilê
		{
			iZajety = i; // bila która nie pozwala na postawienie bili w p1
			return true;
		}
	}
	return false;
}

void CBilard::ustawBile(kolory bKolor)
{
	CPunkt3D nPunkt;
	bool zajety = false;
	short int indZajety, pIndZajety; // pIndZajety - indeks bili która zajmuje miejsce kolorowej
	if (bKolor == BIALA)
	{
		if (!czyZajety(pozycje[0], indZajety)) // czy jest zajêty punkt kolorowej bili, indZajety - przez referencjê
		{
			bile1[0]->przesunDo(pozycje[0].x, pozycje[0].y);
			return;
		}

		do // sprawdzamy ka¿dy kolejny mo¿liwy punkt do postawienia bili
		{
			nPunkt.x = bile1[indZajety]->przesuniecie.x;
			nPunkt.y = bile1[indZajety]->przesuniecie.y - 2 * bile1[0]->r;
		} while (czyZajety(nPunkt, indZajety));
		bile1[0]->przesunDo(nPunkt.x, nPunkt.y); // najbli¿ej górnej bandy
		return;
	}


	if (!czyZajety(pozycje[14 + bKolor], pIndZajety)) // czy jest zajêty punkt kolorowej bili
	{
		bile1[14 + bKolor]->przesunDo(pozycje[14 + bKolor].x, pozycje[14 + bKolor].y);
		return;
	}
	else // pozosta³e kolory
	{
		for (int i = 21; i >= 16; i--) // sprawdzamy najbli¿szy dostêpny punkt 
		{
			if ((i - 14) != bKolor)
			{
				if (!czyZajety(pozycje[i], indZajety)) // najwy¿ej punktowany niezajêty punkt
				{
					bile1[14 + bKolor]->przesunDo(pozycje[i].x, pozycje[i].y);
					return;
				}
			}
		}
	}

	// mo¿e zdarzyæ siê przypadek, ¿e ka¿dy punkt ju¿ bêdzie zajêty - MA£O PRAWDOPODOBNE


	do // sprawdzamy ka¿dy kolejny mo¿liwy punkt do postawienia bili
	{
		nPunkt.x = bile1[pIndZajety]->przesuniecie.x + 2 * bile1[0]->r;
		nPunkt.y = bile1[pIndZajety]->przesuniecie.y;
	} while (czyZajety(nPunkt, pIndZajety));

	bile1[14 + bKolor]->przesunDo(nPunkt.x, nPunkt.y); // najbli¿ej górnej bandy
	return;
}

void CBilard::pokazStanGry()
{
	bool set = 0; // zbiór znaków: czcionka zwyk³a 0 pogrubiona 1
	bool bKomunikat = true;
	glEnable2D();
	glDisable(GL_LIGHTING);
	glPushMatrix();
	if (nastepna == DO_WYBORU && (currentCam == CAM_SHOT || currentCam == CAM_OVERVIEW))
	{
		stol->mode = true;
		for (int i = 16; i <= 21; i++)
		{
			glLoadIdentity();
			glTranslatef(bPozycje[i - 16][0], bPozycje[i - 16][1], bPozycje[i - 16][2]);
			glScalef(1.1, 1.1, 0);
			bile1[0]->rysuj();
		}
		for (int i = 16; i <= 21; i++)
		{
			glLoadIdentity();
			glTranslatef(bPozycje[i - 16][0], bPozycje[i - 16][1], bPozycje[i - 16][2]);
			bile1[i]->rysuj();
		}


		stol->mode = false;
	}

	glPopMatrix();
	glEnable(GL_BLEND);
	glTranslatef(tPozycje[0][6], tPozycje[1][6], 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1, 1, 1, 0.2);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, pHeight);
	glVertex2f(pWidth, pHeight);
	glVertex2f(pWidth, 0);
	glEnd();

	glTranslatef(tPozycje[0][7], tPozycje[1][7], 0);
	glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);

	glColor4f(1, 1, 1, 0.0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, tHeight);
	glVertex2f(tWidth, tHeight);
	glVertex2f(tWidth, 0);
	glEnd();

	glTranslatef(tPozycje[0][8], tPozycje[1][8] + kPrzesuniecie, 0);
	glBlendFunc(GL_ONE, GL_ZERO);

	glColor4f(0, 0, 1, 1);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, qHeight);
	glVertex2f(qWidth, qHeight);
	glVertex2f(qWidth, 0);
	glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	if (bilard1->zmiana)
	{
		bKomunikat = false;
		napisy[0][1] = napisy[0][1].substr(0, 8);
		napisy[1][1] = napisy[1][1].substr(0, 8);
		napisy[0][2] = napisy[0][2].substr(0, 7);
		napisy[1][2] = napisy[1][2].substr(0, 7);

		_itoa(gracze[0]->sPunkty, buf, 10);
		napisy[0][1].append(buf);
		_itoa(gracze[1]->sPunkty, buf, 10);
		napisy[1][1].append(buf);
		_itoa(gracze[0]->seria, buf, 10);
		napisy[0][2].append(buf);
		_itoa(gracze[1]->seria, buf, 10);
		napisy[1][2].append(buf);
		bilard1->zmiana = false;
	}
	if (bilard1->zmCzas)
	{
		bool k = bilard1->aktualnyGracz;
		napisy[k][3] = napisy[k][3].substr(0, 15);  // czas ruchu
		napisy[k][4] = napisy[k][4].substr(0, 6);	  // pozosta³y czas

		if (gracze[k]->czasRuchu<10)
			napisy[k][3].append("0");
		_itoa(gracze[k]->czasRuchu, buf, 10);
		napisy[k][3].append(buf);

		int minuty, sekundy;
		minuty = gracze[k]->czas / 60;
		sekundy = gracze[k]->czas % 60;
		if (minuty<10)
			napisy[k][4].append("0");
		_itoa(minuty, buf, 10);
		napisy[k][4].append(buf);
		napisy[k][4].append(":");
		if (sekundy<10)
			napisy[k][4].append("0");
		_itoa(sekundy, buf, 10);
		napisy[k][4].append(buf);
	}
	if (zmGracza)  // zmiana czasu na 00:00 gracza, któremu skoñczy³ siê czas
				   // - nie jest ju¿ aktualnym graczem
	{
		bKomunikat = false;
		bool k = !bilard1->aktualnyGracz;
		napisy[k][3] = napisy[k][3].substr(0, 15);  // czas ruchu
		napisy[k][4] = napisy[k][4].substr(0, 6);	  // pozosta³y czas

		if (gracze[k]->czasRuchu<10)
			napisy[k][3].append("0");
		_itoa(gracze[k]->czasRuchu, buf, 10);
		napisy[k][3].append(buf);
		
		int minuty, sekundy;
		minuty = gracze[k]->czas / 60;
		sekundy = gracze[k]->czas % 60;
		if (minuty<10)
			napisy[k][4].append("0");
		_itoa(minuty, buf, 10);
		napisy[k][4].append(buf);
		napisy[k][4].append(":");
		if (sekundy<10)
			napisy[k][4].append("0");
		_itoa(sekundy, buf, 10);
		napisy[k][4].append(buf);
		bilard1->zmGracza = false;
	}

	// Displaying game informations of both players
	for (int i = 0; i<2; i++)
	{
		glPrint(odd[i], tPozycje[i][0], napisy[i][0].c_str(), set);

		glPrint(odd[i], tPozycje[i][1], napisy[i][1].c_str(), set);

		glPrint(odd[i], tPozycje[i][2], napisy[i][2].c_str(), set);

		glPrint(odd[i], tPozycje[i][3], napisy[i][3].c_str(), set);

		glPrint(odd[i], tPozycje[i][4], napisy[i][4].c_str(), set);
	}

	if (bKomunikat)
	{
		if (bilard1->nastepna == DO_WYBORU)
		{
			if (bilard1->faul)
			{
				if (!bilard1->sKomunikat)
				{
					bilard1->komunikat += sKomunikaty[0];
					if (bilard1->bialaWRece)
						bilard1->komunikat += "Biala w rece. ";
					bilard1->sKomunikat = true;
				}
			}
			else
			{
				bilard1->komunikat = sKomunikaty[0];
			}
		}
		else if (!bilard1->koniec)
		{

			if (bilard1->faul)
			{
				if (!bilard1->sKomunikat)
				{
					bilard1->komunikat += sKomunikaty[1];
					bilard1->komunikat += sKolory[(int)bilard1->nastepna];
					bilard1->komunikat += ". ";
					if (bilard1->bialaWRece)
						bilard1->komunikat += "Biala w rece. ";
					bilard1->sKomunikat = true;
				}
			}
			else
			{
				bilard1->komunikat = sKomunikaty[1];
				bilard1->komunikat += sKolory[(int)bilard1->nastepna];
				bilard1->komunikat += ". ";
				if (bilard1->bialaWRece)
					bilard1->komunikat += "Biala w rece. ";
			}
		}
		tPozycje[0][5] = (stol1->rog->x + stol1->dlugosc / 2) - (float)bilard1->komunikat.size() / 2 * 10;
		glPrint(tPozycje[0][5], tPozycje[1][5], bilard1->komunikat.c_str(), 0);

	}
	glFlush();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glDisable2D();

}

void ruch(int x, int y)  // dla ruchu myszk¹
{
	float tx = (x / WindowWidth)*pWindowWidth;
	float ty = (1 - y / WindowHeight)*pWindowHeight;
	float a, h;
	long long int p, k;
	if (bilard1->kPrzytrzymany)
	{
		float dy = ty - pwk.y;
		if ((bilard1->kPrzesuniecie + dy) >= -bilard1->tPozycje[1][8] && (bilard1->kPrzesuniecie + dy) <= 0)
			bilard1->kPrzesuniecie += dy;

		kij1->oddal(dy*(kij1->maxOddalenie / bilard1->tPozycje[1][8]));
		pwk.x = tx;  // nowy punkt odniesienia dla nastêpnego przesuniêcia
		pwk.y = ty;
	}
	if (bilard1->lPrzytrzymany)  // 
	{
		if (bilard1->bialaPrzytrzymana)   // bia³a w rêce
		{
			static CPunkt3D poprzedni;
			bool zmiana = true;
			float tmpOdl;
			CPunkt3D punkt;
			CPunkt3D bBila;
			CPunkt3D iBila;
			bBila.x = bile1[0]->przesuniecie.x;
			bBila.y = bile1[0]->przesuniecie.y;
			punkt.x = tx - stol1->wierzcholek->x; // w uk³. wspó³rzêdnych sto³u
			punkt.y = ty - stol1->wierzcholek->y;

			a = stol1->srBazy.x - punkt.x;
			h = punkt.y - stol1->srBazy.y;
			float a0 = tan(h / a);
			tmpOdl = (a*a + h*h);
			if (punkt.x <= stol1->srBazy.x && tmpOdl <= pow(stol1->promienBazy, 2))
				bile1[0]->przesunDo(punkt.x, punkt.y);
			else if (punkt.x>stol1->odlBazy)
			{
				if (punkt.y>(stol1->srBazy.y + stol1->promienBazy))
					punkt.y = stol1->srBazy.y + stol1->promienBazy;
				if (punkt.y<(stol1->srBazy.y - stol1->promienBazy))
					punkt.y = stol1->srBazy.y - stol1->promienBazy;
				bile1[0]->przesunDo(stol1->srBazy.x, punkt.y);
			}
			else if (tmpOdl>pow(stol1->promienBazy, 2))
			{
				punkt.x = stol1->srBazy.x - (a / sqrt(tmpOdl))*stol1->promienBazy;
				punkt.y = stol1->srBazy.y + (h / sqrt(tmpOdl))*stol1->promienBazy;
				bile1[0]->przesunDo(punkt.x, punkt.y);
			}
			punkt.x = bile1[0]->przesuniecie.x;
			punkt.y = bile1[0]->przesuniecie.y;
			for (int i = 1; i<22; i++)
			{
				iBila.x = bile1[i]->przesuniecie.x;
				iBila.y = bile1[i]->przesuniecie.y;

				tmpOdl = odleglosc(punkt, iBila);
				if (tmpOdl<2 * bile1[0]->r)
				{
					punkt.x = pozycje[0].x;
					punkt.y = pozycje[0].y;
					bile1[0]->przesunDo(punkt.x, punkt.y);
					break;
				}

			}

		}
		else if (bilard1->currentCam != CAM_TOP)
		{
			kij1->obrot.k3 += (tx - pwk.x) / (WindowWidth) * 200;
			pwk.x = tx;
			if (kij1->obrot.k3 >= 360) kij1->obrot.k3 -= 360;
			else if (kij1->obrot.k3<0) kij1->obrot.k3 += 360;
		}
		else
		{
			float konKat;
			float kRoznica;
			// œrodek bili jest pocz¹tkiem uk³adu wspó³rzêdnych
			a = tx - (bile1[0]->przesuniecie.x + stol1->wierzcholek->x);
			h = ty - (bile1[0]->przesuniecie.y + stol1->wierzcholek->y);
			if (a != 0)
			{
				konKat = atan(abs(h / a));
				konKat *= 180 / PI;
				translacja(konKat, a, h, 0);
			}
			else if (h >= 0)
			{
				konKat = 270;
			}
			else if (h<0)
			{
				konKat = 90;
			}

			//podzia³ na æwiartki w uk³. wsp. bia³ej bili
			//obrót przeciwnie do ruchu wsk. zegara
			kRoznica = konKat - kij1->obrot.k3;
			kij1->obrot.k3 = kij1->obrot.k3 + kRoznica; // nie powinno byc wiêksze niz 360
		}
	}
	if (bilard1->pPrzytrzymany)  // 
	{
		float alfa, beta, x1, y1, x2, y2, a1, b1, a2, b2;
		CPunkt3D pp1;
		CPunkt3D pp2;
		// œrodek bili jest pocz¹tkiem uk³adu wspó³rzêdnych
		x1 = tx - (bile1[0]->przesuniecie.x + stol1->wierzcholek->x);  // we wspó³rzêdnych bia³ej bili
		y1 = ty - (bile1[0]->przesuniecie.y + stol1->wierzcholek->y);
		x2 = pwk.x - (bile1[0]->przesuniecie.x + stol1->wierzcholek->x);
		y2 = pwk.y - (bile1[0]->przesuniecie.y + stol1->wierzcholek->y);
		// przesuniêce kija wzglêdem nowego punktu

		pwk.x = tx;  // nowy punkt odniesienia dla nastêpnego przesuniêcia
		pwk.y = ty;

		float kijWsp;
		if (kij1->obrot.k3 == 90 || kij1->obrot.k3 == 270)
			kijWsp = 25000; // przybl. nieskoñczonoœci - dla k¹tów 90 i 270
		else if (kij1->obrot.k3 == 180)
			kijWsp = 0;
		else
			kijWsp = tan(kij1->obrot.k3 / 180 * PI);

		if (kijWsp == 0) a1 = 25000; // przybl. nieskoñczonoœci - dla k¹tów 0 i 180
		else a1 = -1 / kijWsp; // przybli¿enia 0

		b1 = y1 - a1*x1;

		a2 = a1;
		b2 = y2 - a1*x2;

		pp1.x = b1 / (kijWsp - a1);
		pp1.y = kijWsp*pp1.x;
		pp2.x = b2 / (kijWsp - a2);
		pp2.y = kijWsp*pp2.x;
		a = pp1.x - pp2.x;
		h = pp1.y - pp2.y;
		float odl;
		odl = sqrt(a*a + h*h);
		if (kij1->obrot.k3 >= 0.0f && kij1->obrot.k3<90.0f)
		{
			if (pp1.x>pp2.x)
			{
				kij1->oddal(odl);
			}
			else
			{
				kij1->oddal(-odl);
			}
		}

		if (kij1->obrot.k3>90.0f && kij1->obrot.k3 <= 180.0f)
		{
			if (pp1.x<pp2.x)
			{
				kij1->oddal(odl);
			}
			else
			{
				kij1->oddal(-odl);
			}

		}
		if (kij1->obrot.k3>180.0f && kij1->obrot.k3<270.0f)
		{
			if (pp1.x<pp2.x)
			{
				kij1->oddal(odl);
			}
			else
			{
				kij1->oddal(-odl);
			}
		}

		if (kij1->obrot.k3>270.0f && kij1->obrot.k3<360.0f)
		{
			if (pp1.x>pp2.x)
			{
				kij1->oddal(odl);
			}
			else
			{
				kij1->oddal(-odl);
			}

		}
	}

}

void CKij::fObrot(float kat)
{
	obrot.k3 = kat;
}

void CKij::fPrzesuniecie(float tx, float ty, float tz)
{
	przesuniecie.x += tx;
	przesuniecie.y += ty;
	przesuniecie.y += tz;
}

void CKij::przesunDo(float tx, float ty, float tz)
{
	przesuniecie.x = tx;
	przesuniecie.y = ty;
	przesuniecie.z = tz;
}

CGracz::CGracz(string * aNazwa)
{
	nazwa = new string();
	nazwa = aNazwa;
	czas = 1200;
	uCzasRuchu = 30;
	czasRuchu = uCzasRuchu;

	maxBreak = 0;
	seria = 0;
	sPunkty = 0;
	koniecCzasu = false;
}

CGracz::~CGracz()
{
	delete nazwa;
}


void CGracz::zmniejszCzas()
{
	czasRuchu--;
	czas--;
}

unsigned int CGracz::rczas()
{
	return czas;
}

unsigned int CGracz::rczasRuchu()
{
	return czasRuchu;
}

void mouse(int button, int state, int x, int y)
{
	bool deklarowana = false; // po naciœniêciu klawisz myszki zawsze false, aby kij nie obraca³ siê podczas deklaracji
	bool wObszarze = false;
	bool bila = false; // czy naciœniêcie klawisza w obszarze kolorowej bili do wyboru
	float tx = x;
	float ty = pWindowHeight - y;

	if (tx >= stol1->wierzcholek->x && tx <= stol1->wierzcholek2->x && ty >= stol1->wierzcholek->y && ty <= stol1->wierzcholek2->y)
		wObszarze = true;

	float xNowy, yNowy, xRoz, a, h, yRoz, b2, a2, b, brzut, arzut, odlOdcinka, wsp, odl;
	if (!bilard1->animacja && !bilard1->uderzenie && !bilard1->koniec && !bilard1->pauza)  // niemo¿liwa zmiana obrotu kija po wykonaniu uderzenia
	{
		if (button == SDL_BUTTON_LEFT && state == SDL_PRESSED)   // obrót kija wokó³ bili 
		{
			CPunkt3D pozycja;
			pozycja.x = bilard1->tPozycje[0][8] + bilard1->tPozycje[0][7] + bilard1->tPozycje[0][6];
			pozycja.y = bilard1->tPozycje[1][8] + bilard1->tPozycje[1][7] + bilard1->tPozycje[1][6];
			if (tx >= pozycja.x && tx <= (pozycja.x + bilard1->qWidth) && ty >= pozycja.y && ty <= (pozycja.y + bilard1->qHeight))
			{
				bilard1->kPrzytrzymany = true;
				pwk.x = tx;  // nowy punkt odniesienia dla nastêpnego przesuniêcia
				pwk.y = ty;
				bilard1->lPrzytrzymany = false;
				return;
			}
			if (bilard1->nastepna == DO_WYBORU)
			{
				CPunkt3D tmp1;
				CPunkt3D tmp2;
				if (bilard1->currentCam == CAM_SHOT || bilard1->currentCam == CAM_OVERVIEW)
				{
					tmp1.x = tx;
					tmp1.y = ty;
					for (int i = 16; i <= 21; i++)
					{
						tmp2.x = bilard1->bPozycje[i - 16][0];
						tmp2.y = bilard1->bPozycje[i - 16][1];
						if (odleglosc(tmp1, tmp2) <= 2 * bile1[i]->r)
						{
							deklarowana = true;
							bilard1->nastepna = (kolory)(i - 14); // wybrana nastepna bila
							break;
						}
					}
				}
				else
				{
					tmp1.x = tx - stol1->wierzcholek->x;
					tmp1.y = ty - stol1->wierzcholek->y;
					for (int i = 16; i<22; i++)
					{
						tmp2.x = bile1[i]->przesuniecie.x;
						tmp2.y = bile1[i]->przesuniecie.y;
						if (odleglosc(tmp1, tmp2) <= bile1[i]->r)
						{
							deklarowana = true;
							bilard1->nastepna = (kolory)(i - 14); // wybrana nastepna bila
							break;
						}
					}
				}
			}
			bilard1->lPrzytrzymany = true;
			// œrodek bili jest pocz¹tkiem uk³adu wspó³rzêdnych

			if (bilard1->bialaWRece)
			{
				CPunkt3D tmp1;
				CPunkt3D tmp2;
				tmp1.x = tx - stol1->wierzcholek->x;
				tmp1.y = ty - stol1->wierzcholek->y;
				tmp2.x = bile1[0]->przesuniecie.x;
				tmp2.y = bile1[0]->przesuniecie.y;

				if (odleglosc(tmp1, tmp2) <= bile1[0]->r)
					bilard1->bialaPrzytrzymana = true;

			}

			if (!bilard1->pPrzytrzymany && !deklarowana)
			{
				if (bilard1->currentCam == CAM_TOP)
				{

					a = tx - (bile1[0]->przesuniecie.x + stol1->wierzcholek->x);
					h = ty - (bile1[0]->przesuniecie.y + stol1->wierzcholek->y);
					if (a != 0)
					{
						katp = atan(abs(h / a));
						katp *= 180 / PI;
						translacja(katp, a, h, 0);
						kij1->obrot.k3 = katp;
					}
					else
					{
						if (h>0)
							katp = 90;
						if (h<0)
							katp = 270;
					}

				}
				else
				{
					pwk.x = tx; // punkt odniesienie dla obracania kija
				}
			}

		}

		if (button == SDL_BUTTON_LEFT && state == SDL_RELEASED)   // obrót kija wokó³ bili 
		{
			bilard1->lPrzytrzymany = false;
			if (bilard1->bialaPrzytrzymana)
				bilard1->bialaPrzytrzymana = false;
			if (bilard1->kPrzytrzymany)
			{
				bilard1->kPrzytrzymany = false;
				if (kij1->oddalenie->x<0)
				{
					bilard1->animacja = true;
					bilard1->wychylenie = -kij1->oddalenie->x;
				}
			}
		}

		if (button == SDL_BUTTON_RIGHT && state == SDL_PRESSED)   // zwiêkszanie/zmniejszanie si³y
		{
			if (bilard1->nastepna != DO_WYBORU && bilard1->currentCam == CAM_TOP)bilard1->pPrzytrzymany = true; // blokada uderzenia dla deklaracji
			pwk.x = tx; // punkt odniesienia dla ruchu kija
			pwk.y = ty;
		}

		if (button == SDL_BUTTON_RIGHT && state == SDL_RELEASED)   // uderzenie
		{
			bilard1->pPrzytrzymany = false;
			if (kij1->oddalenie->x<0)
			{
				bilard1->animacja = true;
				bilard1->wychylenie = -kij1->oddalenie->x;
			}
		}
	}
}



void timer()
{
	static long long int tPoczatek = 0;
	static long long int tKoniec = 0;
	long double tRoznica = 0;
	unsigned __int64 counterStop;
	long long int czas;
	long double counter; // miniony czas od pocz¹tku rozgrywki
	long long int start1 = 0;
	long long int start2 = 0; //zmienne zegarowe
	long long int end;
	long double difference = 0;
	float odlx;
	float odly;
	float srodekX;
	float srodekY;
	float r, c, rx, ry, prevc;
	CBila * tmp1;

	// trzeba zrobiæ dla ka¿dego z graczy z osobna
	if (!bilard1->koniec)
	{


		if (bilard1->poczatek) // dla pierwszego ruchu
		{
			bilard1->poczatek = false;
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&gracze1[bilard1->aktualnyGracz]->poczatekRuchu));
		}
		if (bilard1->nowyRuch)// start timera
		{
			if (bilard1->przerwa)
			{
				bilard1->przerwa = false;
				bilard1->nowyRuch = false;
			}
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&gracze1[bilard1->aktualnyGracz]->poczatekRuchu));

		}
		// czas miniony od pocz¹tku ruchu

		if (bilard1->pauza)
		{
			bilard1->pCounter = bilard1->counter;
		}
		else if (bilard1->pCounter != 0) // wznowienie
		{

			if (!bilard1->wznowienie) // gracz wznawia grê
			{
				QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&gracze1[bilard1->aktualnyGracz]->poczatekRuchu));
				bilard1->wznowienie = true;
			}
			else // po wznowieniu gry
			{
				QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&czas)); // powinno byæ rzutowanie na long long (chyba)
				bilard1->counter = bilard1->pCounter + static_cast<long double> (czas - gracze1[bilard1->aktualnyGracz]->poczatekRuchu) / freq;
			}
		}
		else // gracz jeszcze nie za¿¹da³ pauzy - pCounter==0
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&czas)); // powinno byæ rzutowanie na long long (chyba)
			bilard1->counter = static_cast<long double> (czas - gracze1[bilard1->aktualnyGracz]->poczatekRuchu) / freq;
		}

		if (!bilard1->pauza)
		{

			if (bilard1->uderzenie)
			{
				bilard1->zerowe = true; // za³o¿enie - wszystkie prêdkoœci zerowe, je¿eli zasz³o uderzenie
				long double roz, suma = 0;
				long long int a, b;
				for (int i = 0; i<22; i++)
				{

					if (!stol1->wbite[i] && bile1[i]->Vp>0)  // jeœli nie jest wbita i jest niezerowa prêdkoœæ
					{
						//zmniejszenie prêdkoœci

						// zamieniæ na clock
						QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&counterStop));
						roznica[i] = static_cast<long double> (counterStop - poprzedni[i]) / freq;

						QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&poprzedni[i]));

						//ustawianie bie¿¹cej prêdkoœci i przesuniêcia po odstêpie czasiu 
						// od ostatniego rysowania tej bili - roznica[i]
						bile1[i]->Vp -= (stol1->mi*g*roznica[i] + bile1[i]->Vp*bile1[i]->Vp*0.0008*roznica[i]);

						if (bile1[i]->Vp<0)bile1[i]->Vp = 0;
						bile1[i]->dPrzesuniecie = bile1[i]->Vp*roznica[i];
						bile1[i]->alfa += 360.0f*(bile1[i]->dPrzesuniecie / (12 * PI));
						bile1[i]->Vx = bile1[i]->Vp*bile1[i]->kx;
						bile1[i]->Vy = bile1[i]->Vp*bile1[i]->ky;

						bile1[i]->fPrzesuniecie(bile1[i]->Vx*roznica[i], bile1[i]->Vy*roznica[i]);

						bilard1->zerowe = false;
						srodekX = bile1[i]->przesuniecie.x;
						srodekY = bile1[i]->przesuniecie.y;
						r = bile1[i]->r;

						stol1->luza = false; // czy jest pomiêdzy krañcami ³uzy

						if (srodekX>stol1->kraniec[1].x && srodekX<stol1->kraniec[2].x && srodekY>(stol1->szerokosc - stol1->szerokoscBandy - r))  // przedzia³ na uderzenie w kanty i wnêtrze ³uzy œrodkowej
						{
							stol1->sprawdzLuze(1, i);
						}

						if (srodekX>stol1->kraniec[3].x && srodekY>stol1->kraniec[4].y)  // przedzia³ na uderzenie w kanty i wnêtrze ³uzy œrodkowej
						{
							stol1->sprawdzLuze(2, i);
						}

						if (srodekX>stol1->kraniec[6].x && srodekY<stol1->kraniec[5].y)  // przedzia³ na uderzenie w kanty i wnêtrze ³uzy œrodkowej
						{
							stol1->sprawdzLuze(3, i);
						}

						if (srodekX>stol1->kraniec[8].x && srodekX<stol1->kraniec[7].x && srodekY<(stol1->szerokoscBandy + r)) // przedzia³ na uderzenie w kanty i wnêtrze ³uzy œrodkowej
						{
							stol1->sprawdzLuze(4, i);
						}

						if (srodekX<stol1->kraniec[9].x && srodekY<stol1->kraniec[10].y)  // przedzia³ na uderzenie w kanty i wnêtrze ³uzy œrodkowej
						{
							stol1->sprawdzLuze(5, i);
						}

						if (srodekX<stol1->kraniec[12].x && srodekY>stol1->kraniec[11].y)  // przedzia³ na uderzenie w kanty i wnêtrze ³uzy œrodkowej
						{
							stol1->sprawdzLuze(6, i);
						}


						float odlSr = r + stol1->szerokoscBandy;
						if ((srodekX>(stol1->dlugosc - odlSr) || srodekX<odlSr) && !xOdbite[i] && !stol1->luza) // odbicie wzglêdem bandy
						{

							xOdbite[i] = true;

							bile1[i]->Vx = (-bile1[i]->Vx)*bilard1->wspEn;
							bile1[i]->Vy *= bilard1->wspEn;
							bile1[i]->Vp *= bilard1->wspEn;
							bile1[i]->kx = (-bile1[i]->kx)*bilard1->wspEn;
							bile1[i]->ky *= bilard1->wspEn;
							bile1[i]->kierunek = atan(abs(bile1[i]->Vy / bile1[i]->Vx));  // kat predkosci
							bile1[i]->kierunek *= 180 / PI;
							translacja(bile1[i]->kierunek, bile1[i]->Vx, bile1[i]->Vy, 1); // k¹t ustawiamy w zale¿noœci od æwiartki

						}
						else if (srodekX>odlSr &&  srodekX<stol1->dlugosc - odlSr)
						{
							xOdbite[i] = false;
						}

						if ((srodekY<odlSr || srodekY>(stol1->szerokosc - odlSr)) && !yOdbite[i] && !stol1->luza)
						{
							yOdbite[i] = true;
							bile1[i]->Vy = (-bile1[i]->Vy)*bilard1->wspEn;
							bile1[i]->Vx *= bilard1->wspEn;
							bile1[i]->Vp *= bilard1->wspEn;
							bile1[i]->ky = (-bile1[i]->ky)*bilard1->wspEn;
							bile1[i]->kx *= bilard1->wspEn;
							bile1[i]->kierunek = atan(abs(bile1[i]->Vy / bile1[i]->Vx));  // kat predkosci
							bile1[i]->kierunek *= 180 / PI;
							translacja(bile1[i]->kierunek, bile1[i]->Vx, bile1[i]->Vy, 1); // k¹t ustawiamy w zale¿noœci od æwiartki
						}
						else if (srodekY>odlSr &&  srodekY<(stol1->szerokosc - odlSr))
						{
							yOdbite[i] = false;
						}

						for (int j = 0; j<22; j++)  // mo¿na jeszcze odliczyæ modele statyczne
						{
							QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&a));
							if (stol1->wbite[j] || i == j) continue;
							rx = bile1[j]->przesuniecie.x - srodekX;
							ry = bile1[j]->przesuniecie.y - srodekY;
							c = sqrt(rx*rx + ry*ry);  // odleg³oœæ pomiêdzy œrodkami bil
							if (c>cOdl2) continue;
							// np. bila mo¿e byæ w odleg³oœci 11.99 od drugiej - nie wychodzi z uderzenia
							// po kolejnym wywo³aniu timera w skrajnym przypadku mo¿e byæ 3-4 jednostki dalej

							if (c>cOdl && bilard1->odbite[i][j] && bilard1->odbite[j][i]) // koniec uderzenia miêdzy bilami i i j
							{
								bilard1->odbite[i][j] = false;
								bilard1->odbite[j][i] = false;
							}

							if (c>cOdl) continue;
							if (c<cOdl)
							{
								if (!bilard1->odbite[i][j] && !bilard1->odbite[j][i]) // jeszcze nie odbite
								{
									stol1->uderzenie->volume = 150 * (bile1[i]->Vp + bile1[j]->Vp) / (2 * bilard1->maxVp);
									Mix_PlayChannel(-1, stol1->uderzenie, 0);
									if (i == 0 && !bilard1->pierwszeUderzenie)
									{
										bilard1->pierwszeUderzenie = true;
										if (j <= 15 && j>0)
											bilard1->pierwszaUderzona = CZERWONA;
										else  //kolorowe
											bilard1->pierwszaUderzona = static_cast<kolory>(j - 14);
									}
									bilard1->odbite[i][j] = true;  // odbicie wzajemne bil - 
									bilard1->odbite[j][i] = true;
									stol1->odbicie(i, j, c, rx, ry);  // procedura obs³ugi odbicia bil - w klasie CStol jest wskaŸnik na bile, dlatego mo¿emy modyfikowaæ prêdkosci
									bile1[i]->Vx *= bilard1->wspEn;
									bile1[i]->Vy *= bilard1->wspEn;
									bile1[i]->Vp *= bilard1->wspEn;
									bile1[i]->kx *= bilard1->wspEn;
									bile1[i]->ky *= bilard1->wspEn;
									bile1[j]->Vx *= bilard1->wspEn;
									bile1[j]->Vy *= bilard1->wspEn;
									bile1[j]->Vp *= bilard1->wspEn;
									bile1[j]->kx *= bilard1->wspEn;
									bile1[j]->ky *= bilard1->wspEn;
								}
								// sytuacja gdy bila zosta³a ju¿ uderzona, ale jeszcze jest w przedziale do 2*r 
								// z powodu b³êdu wynikaj¹cego z próbkowania czasu
								else if (bilard1->odbite[i][j] && bilard1->odbite[j][i])
								{
									if (c<bilard1->prevc[i][j]) // bila zosta³a uderzona przez inn¹ bilê podczas w ychodzenia z uderzenia z dan¹ bil¹
									{

										stol1->odbicie(i, j, c, rx, ry);  // ponowienie odbicia bili
									}
								}
							}

						}

					}
				}
			}
			// sprawdzamy czy skoñczy³ siê czas i gracz nie odda³ w ogóle uderzenia lub czy po uderzeniu wszystkie prêdkoœci siê wyzerowa³y

			if (!bilard1->uderzenie && !bilard1->zmGracza) // nie zosta³a jeszcze uderzona bila
			{
				if (bilard1->animacja)
				{
					if (kij1->oddalenie->x<0.0f)
					{
						kij1->oddal(2.0f*bilard1->wychylenie / 15.0f);   // przybli¿anie kija co jakiœ czas - tworzenie animacji ruchu kija a¿ do uderzenia w bile				
					}
					else  // je¿eli animacja siê w ogóle rozpoczê³a i zakoñczy³a
					{
						bilard1->kPrzesuniecie = 0.0f;
						bilard1->uderzenie = true;
						bilard1->poczatekAnimacji = false;
						bilard1->animacja = false;
						bilard1->bialaUderzona = true; // gracz uderzy³ bia³¹ bilê
						bile1[0]->Vp = bilard1->stala*bilard1->wychylenie;

						bile1[0]->kierunek = kij1->obrot.k3;
						bile1[0]->Vx = bile1[0]->Vp*cos(kij1->obrot.k3 / 180 * PI);
						bile1[0]->Vy = bile1[0]->Vp*sin(kij1->obrot.k3 / 180 * PI);
						bile1[0]->kx = bile1[0]->Vx / bile1[0]->Vp;  // uwzglêdnia znak
						bile1[0]->ky = bile1[0]->Vy / bile1[0]->Vp;
						pBile.push_back(0);

						// odnoœniki czasowe dla ka¿dej z bil - potrzebne do rysowania bili zgodnie z odstêpami czasu
						for (int i = 0; i<22; i++) QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&poprzedni[i]));
						for (int i = 0; i<22; i++) roznica[i] = 0;

					}
				}
				else if ((bilard1->counter - (int)bilard1->counter)>0.99 && (bilard1->counter - (int)bilard1->counter)<1.01) // liczenie czasu 
				{
					if (!bilard1->zmCzas)
					{
						bilard1->zmCzas = true;  // zapobieganie ponownej zmianie czasu w tym przedziale

						gracze1[bilard1->aktualnyGracz]->zmniejszCzas();
						if (gracze1[bilard1->aktualnyGracz]->rczas() == 0)
						{
							bilard1->koniec = true;
							gracze1[bilard1->aktualnyGracz]->koniecCzasu = true;
						}
						if (gracze1[bilard1->aktualnyGracz]->rczasRuchu() == 0) // koniec czasu na ruch
						{
							gracze1[bilard1->aktualnyGracz]->koniecCzasu = true;
						}
					}
				}
				else
				{
					bilard1->zmCzas = false;
				}
			}

			// koniec ruchu aktualnego gracza
			if ((bilard1->zerowe || gracze1[bilard1->aktualnyGracz]->koniecCzasu))
			{
				if (gracze1[bilard1->aktualnyGracz]->koniecCzasu) // zmienne potrzebne do zmian
				{
					bilard1->zmCzas = true;
					bilard1->zmGracza = true;
				}
				// czas na spadek bili do ³uzy

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				bilard1->pokazStanGry(); // uaktualnienie czasu do 00:00, dopiero póŸniej stan gry ze zmian¹ punktów
				bilard1->rysuj();
				SDL_GL_SwapWindow(window);

				bilard1->zmiana = true;  // zmiana stanu punktów
				bilard1->nowyRuch = true;
				if (bilard1->bialaWRece) // czy poprzednie uderzenie by³o z rêki
				{
					bilard1->pBialaWRece = true;
					bilard1->bialaWRece = false;
				}
				else
				{
					bilard1->pBialaWRece = false;
				}

				bilard1->bialaWRece = false; //za³o¿enie przed liczeniem ruchu
				gracze1[bilard1->aktualnyGracz]->koniecCzasu = false;
				bilard1->obecna = bilard1->nastepna;

				bilard1->uderzenie = false;
				bool nastepnyGracz = false;
				bilard1->faul = false; // zak³adamy, ¿e faul nie zosta³ pope³niony, true - je¿eli tak
				bilard1->zerowe = false;
				bool przeciwnik = !bilard1->aktualnyGracz;
				unsigned int wMax;
				unsigned int dodPunkty = 0; // punkty, które bed¹ dodane aktualnemu graczowi
											// lub przeciwnikowi, je¿eli zosta³ pope³niony faul 

				kolory kolorWbitej = BRAK;

				for (int k = 0; k<tmpWbite.size(); k++)
				{
					kolorWbitej = bile1[tmpWbite[k]]->sKolor;
					if (kolorWbitej == BIALA) // chyba
					{
						bilard1->komunikat = sKomunikaty[2];
						bilard1->faul = true;
						nastepnyGracz = true;
						kolorWbitej = BIALA;
						dodPunkty = max(bilard1->obecna, 4);
						stol1->wbite[0] = false; // do wyœwietlenia ponownie
						bilard1->ustawBile(BIALA);
						bilard1->bialaWRece = true; // trzeba zrobiæ obs³ugê myszk¹ dla tego stanu dla nast. gracza
						continue;
					}
					else if (kolorWbitej != bilard1->obecna) //wbita bila nie jest w³aœciw¹ bil¹ CZERWONA lub KOLOROWA
					{
						bilard1->komunikat = sKomunikaty[3];
						bilard1->faul = true;
						nastepnyGracz = true;
						dodPunkty = max((int)dodPunkty, (int)kolorWbitej); // max(4,nastepna, wbita);
						if (kolorWbitej == CZERWONA) // niewa³aœciwa kolorowa
						{
							stol1->wbite[tmpWbite[k]] = true;
							bilard1->lCzerwonych--;
						}
						else
						{
							// ustawiamy spowrotem na stole
							stol1->wbite[tmpWbite[k]] = false;
							bilard1->ustawBile(kolorWbitej);

							// funkcja szukaj¹ca miejsca
						}

						// chyba niepotrzebne - dla nastêpnego gracza wyznaczamy póŸniej bilê
						if (bilard1->lCzerwonych == 0)  // nie mamy do wyznaczenia czerwonej bili
						{
							if (!bilard1->poczatekKolorowych) // faul przy bili z deklaracji
							{
								bilard1->nastepna = DO_WYBORU;
							}
							else // powtarzamy wbijanie kolorowej z kolejnoœci
							{
								bilard1->nastepna = bilard1->obecna;
							}
						}
						else  // mamy do wyznaczenia czerwon¹ bilê
						{
							bilard1->nastepna = CZERWONA;
						}
						continue;
					}
					else // je¿eli odpowiednia bila jest wbita
					{
						if (bilard1->obecna == CZERWONA) // czerwona
						{
							bilard1->lCzerwonych--; // zostawiamy bilê w kieszeni nawet je¿eli zosta³ pope³niony faul
							stol1->wbite[tmpWbite[k]] = true;

							if (!bilard1->faul) // dot¹d nie wykyto faulu
							{
								dodPunkty += bilard1->obecna; // dodajemy z racji mo¿liwoœci prawid³owego wbicia
															  // kilku czerwonych
								bilard1->nastepna = DO_WYBORU;
								bilard1->wybor = true;  // chyba niepotrzebne, wystarczy to co jest wy¿ej
							}

						}
						else //kolorowa prawid³owa
						{
							if (!bilard1->faul) // nie ma dotychczas faulu
							{
								dodPunkty = kolorWbitej;  // tylko jedna 
								if (bilard1->lCzerwonych == 0)
								{
									if (bilard1->lKolorowych == 6) // nie 
									{
										if (!bilard1->poczatekKolorowych) // wbita ostatnia z deklaracji
										{
											stol1->wbite[tmpWbite[k]] = false;
											bilard1->ustawBile(kolorWbitej);

											if (tmpWbite.size() == 1) // mamy pewnoœæ, ¿e toprawid³owe wbicie kolorowej
											{
												bilard1->poczatekKolorowych = true;
												bilard1->nastepna = ZOLTA; // wybiermay pierwsz¹ z serii
											}
										}
										else // ju¿ wbita ¿ó³ta
										{

											if (tmpWbite.size() == 1)  // nie ma faulu tylko gdy ta bila jest 
											{
												stol1->wbite[tmpWbite[k]] = true;
												bilard1->lKolorowych--;
												bilard1->nastepna = ZIELONA;
											}
											else // bêdzie faul
											{
												bilard1->nastepna = ZOLTA; // powtarzamy i wyci¹gamy z kieszeni
												bilard1->ustawBile(ZOLTA);
												stol1->wbite[tmpWbite[k]] = false;
											}
										}
									}
									else // wbita zielona lub wy¿sza
									{
										if (tmpWbite.size() == 1)
										{
											stol1->wbite[tmpWbite[k]] = true;
											bilard1->nastepna = static_cast<kolory>(bilard1->obecna + 1); // zsota³a ju¿ wbita conajmniej jedna kolorowa z serii
											bilard1->lKolorowych--;
										}
										else // bêdzie faul
										{
											bilard1->nastepna = bilard1->obecna; // powtarzamy i wyci¹gamy z kieszeni
											bilard1->ustawBile(bilard1->obecna);
											stol1->wbite[tmpWbite[k]] = false;
										}
									}
								}
								else  // kolorowa po czerwonej - lCZerwonych>0
								{
									stol1->wbite[tmpWbite[k]] = false;
									bilard1->ustawBile(kolorWbitej);
									bilard1->nastepna = CZERWONA;
								}
								if (tmpWbite.size() == 1)  // nie ma i nie bêdzie faulu
									break;
							}
							else // faul ju¿ zosta³ pope³niony
							{
								stol1->wbite[tmpWbite[k]] = false;
								bilard1->ustawBile(kolorWbitej);
								if (bilard1->lCzerwonych == 0)
								{
									if (bilard1->lKolorowych == 6)
									{
										if (!bilard1->poczatekKolorowych)
											bilard1->nastepna = DO_WYBORU;
										else
											bilard1->nastepna = ZOLTA;
										// powinno byæ do wyboru (chyba)
									}
									else
									{
										if (bilard1->lKolorowych>0)bilard1->nastepna = bilard1->obecna; // powtarzamy wbijanie bili wbitej z faulem
									}

								}
								else
								{
									bilard1->nastepna = CZERWONA;
								}
							}


						}

					}
				}

				if (tmpWbite.size() == 0)
				{
					nastepnyGracz = true;
					if (bilard1->bialaUderzona && bilard1->pierwszaUderzona == BRAK) // CHYBIONA - bia³a nie uderzy³a ¿adnej bili
					{
						if (bilard1->pBialaWRece) bilard1->bialaWRece = true;
						bilard1->faul = true;
						bilard1->komunikat = sKomunikaty[4];
						dodPunkty = max((int)bilard1->obecna, 4);
					}
					else if (bilard1->pierwszaUderzona == bilard1->obecna) // bila zosta³a trafiona, ale nie zosta³a wbita
					{
						dodPunkty = 0;
					}
					else if (bilard1->pierwszaUderzona != bilard1->obecna && bilard1->pierwszaUderzona != BRAK)	// pierwsza trafiona bila inna ni¿ powinna
					{
						bilard1->komunikat = sKomunikaty[5];
						wMax = max((int)bilard1->pierwszaUderzona, max((int)bilard1->obecna, 4)); // wartoœæ pocz¹tkowa
						if (dodPunkty <= 7) // gdyby wbi³o siê wiêcej ni¿ 7 bil czerwonych
							dodPunkty = max(wMax, dodPunkty); // bierzemy pod uwagê wartoœæ wiêksza z dwóch mo¿liwych tu fauli
						else
							dodPunkty = 7;
						bilard1->faul = true;
						nastepnyGracz = true;
					}

				}
				else  // trzeba opró¿niæ wektor wbitych po przegl¹dniêciu;
				{
					tmpWbite.clear();
				}


				if (bilard1->obecna == DO_WYBORU) // gracz nie zd¹¿y³ zadeklarowaæ
				{
					bilard1->komunikat = sKomunikaty[7];
					dodPunkty = 7;
					bilard1->faul = true;
					nastepnyGracz = true;
				}
				else if (!bilard1->bialaUderzona)
					// nieuderzona bia³a bila - czas na ruch skoñczy³ siê
				{
					if (bilard1->pBialaWRece) bilard1->bialaWRece = true;
					bilard1->komunikat = sKomunikaty[6];
					bilard1->faul = true;
					nastepnyGracz = true;
					dodPunkty = max(4, (int)bilard1->obecna); // punkty dla nastepnego gracza
				}

				if (bilard1->remis && bilard1->faul) // koniec frema'a - gry - gracz sfaulowa³ przy wbiciu czarnej
				{
					bilard1->koniec = true;
				}

				if (!bilard1->faul)
				{
					if (bilard1->lKolorowych == 0) // nie ma ju¿ nastêpnej bili do wyznaczenia							
					{
						if (gracze1[bilard1->aktualnyGracz]->sPunkty == gracze1[przeciwnik]->sPunkty)
						{
							bilard1->nastepna = CZARNA;
							bilard1->bialaWRece = true;
							bilard1->remis = true;
						}
						else
							bilard1->koniec = true;
					}
					gracze1[bilard1->aktualnyGracz]->sPunkty += dodPunkty;
					gracze1[bilard1->aktualnyGracz]->seria += dodPunkty;

				}
				else
					gracze1[przeciwnik]->sPunkty += dodPunkty;

				if (nastepnyGracz)
				{
					bilard1->sKomunikat = false; // nie stworzono jeszcze komunikatu
												 // sprawdzamy czy obecny break jest wiêkszy od maksymalnego
					gracze1[bilard1->aktualnyGracz]->maxBreak = max(gracze1[bilard1->aktualnyGracz]->maxBreak, gracze1[bilard1->aktualnyGracz]->seria);
					gracze1[bilard1->aktualnyGracz]->seria = 0; // zerujemy break aktualnego gracza;
					bilard1->aktualnyGracz = !bilard1->aktualnyGracz;
					if (bilard1->lCzerwonych>0)
					{
						bilard1->nastepna = CZERWONA;
					}
					else  if (bilard1->lKolorowych == 6 && !bilard1->poczatekKolorowych)// mo¿na daæ jeszcze nastêpnemu graczowi deklaracje jeœli ostatnia deklaracja zosta³a zepsuta
					{
						bilard1->nastepna = DO_WYBORU;
					}
					else
					{
						bilard1->nastepna = static_cast<kolory>(8 - bilard1->lKolorowych);
					}
				}

				//przydzielamy nowy czas dla gracza wykonuj¹cego nastêpny ruch
				gracze1[bilard1->aktualnyGracz]->czasRuchu = gracze1[0]->uCzasRuchu;
				bilard1->zmCzas = true;

				//zerujemy zmienne
				bilard1->pierwszeUderzenie = false;
				bilard1->pierwszaUderzona = BRAK;
				bilard1->bialaUderzona = false;

				if (bilard1->bialaWRece) bile1[0]->przesunDo(pozycje[0].x, pozycje[0].y);
				kij1->obrot.k3 = bile1[0]->kierunek;
				//przesuniêcie kija do bia³ej bili
				kij1->przesunDo(bile1[0]->przesuniecie.x, bile1[0]->przesuniecie.y);


				// mo¿e i niepotrzebne 
				for (int k = 0; k<22; k++) // potrzebne do ewentualnego grania z pozycji poprzedniej przy chybieniu
				{
					poprzednie[k].x = bile1[k]->przesuniecie.x;
					poprzednie[k].y = bile1[k]->przesuniecie.y;
				}

				for (int k = 0; k<22; k++) // potrzebne do ewentualnego grania z pozycji poprzedniej przy chybieniu
				{
					bile1[k]->alfa = 0;
				}
				//zmiana gracza
				// pocz¹tek ndla nowego ruchu


			}

		} // if(!bilard1->pauza)

	} // if(!bilard1->koniec)

	if (bilard1->koniec)
	{
		bilard1->zwyciezca = (gracze1[0]->sPunkty>gracze1[1]->sPunkty) ? 0 : 1;
		bilard1->komunikat = "Wygra³: " + *(gracze1[bilard1->zwyciezca]->nazwa);
		// komunikat koñca gry
		cout << "Koniec\n";
		// wyœwietlamy zwyciêzce i zapisujemy go do listy najwiêkszych break'ów w pliku

	}
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&tKoniec));



}

bool klawisze[256];
void keyboard(int key)
{

	switch (key)
	{
	case SDLK_1: bilard1->currentCam = CAM_TOP;	break;
	case SDLK_2: bilard1->currentCam = CAM_OVERVIEW;		break;
	case SDLK_3: bilard1->currentCam = CAM_SHOT;		break;

		// obrót wokól osi z
	case SDLK_p:
		bilard1->pauza = !bilard1->pauza;
		break;
	case SDLK_a: if (bilard1->currentCam == CAM_OVERVIEW)
	{
		bilard1->overview_angle -= 1.5;
		if (bilard1->overview_angle<0)
			bilard1->overview_angle += 360;
	}
				 break;

	case SDLK_d: if (bilard1->currentCam == CAM_OVERVIEW)
	{
		bilard1->overview_angle += 1.5;
		if (bilard1->overview_angle>360)
			bilard1->overview_angle -= 360;
	}
				 break;

				 // promieñ widoku
	case SDLK_q:
		if (bilard1->currentCam == CAM_OVERVIEW &&  bilard1->overview_con1 <= 600)  bilard1->overview_con1 += 2.0;
		if (bilard1->currentCam == CAM_SHOT && bilard1->shot_con1 <= 200)  bilard1->shot_con1 += 2.0;
		break;
	case SDLK_e:
		if (bilard1->currentCam == CAM_OVERVIEW && bilard1->overview_con1 >= 100)  bilard1->overview_con1 -= 2.0;
		if (bilard1->currentCam == CAM_SHOT && bilard1->shot_con1 >= 50)  bilard1->shot_con1 -= 2.0;
		break;
	case SDLK_z:   // oœ Z
		if (bilard1->currentCam == CAM_SHOT && bilard1->shot_con2 <= bilard1->shot_con1) bilard1->shot_con2 += 2.0;
		else if (bilard1->currentCam == CAM_OVERVIEW && bilard1->overview_con2 <= bilard1->overview_con1) bilard1->overview_con2 += 2.0;
		break;
	case SDLK_c:
		if (bilard1->currentCam == CAM_SHOT && bilard1->shot_con2 >= 30) bilard1->shot_con2 -= 2.0;
		else if (bilard1->currentCam == CAM_OVERVIEW && bilard1->overview_con2 >= 30) bilard1->overview_con2 -= 2.0;
		break;

	case SDLK_LEFT:
		kij1->obrot.k3 += 0.1;
		if (kij1->obrot.k3 >= 360)
		{
			kij1->obrot.k3 -= 360;
		}
		if (kij1->obrot.k3<0.1 && kij1->obrot.k3>0)
		{
			kij1->obrot.k3 = 0;
		}
		break;
	case SDLK_RIGHT:
		kij1->obrot.k3 -= 0.1;
		if (kij1->obrot.k3<0.1 && kij1->obrot.k3>0)
		{
			kij1->obrot.k3 = 0;
		}
		if (kij1->obrot.k3<0)
		{
			kij1->obrot.k3 += 360.0f;
		}
		break;

	}
}
