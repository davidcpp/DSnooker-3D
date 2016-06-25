#pragma once
#include <windows.h>	
//#include <SDL\SDL_image.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>
#include <SDL_main.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <ctime>
//#include <GL/freeglut.h>
#include <GL\GL.H>
#include <GL\GLU.H>

#define H_SUKNO -732
// dla bil
#define SLICES 18  // musi byæ liczba która jest dzielnikiem 360 i 180
#define STACKS 18
#define RADIUS 6
#define NEAR 1.0f
#define FAR 10000.0f
const float cOdl = 2 * RADIUS + 0.01;
const float cOdl2 = 2 * RADIUS + 3;
using namespace std;

extern float fKolory[9][4];

enum kolory { BIALA = 0, CZERWONA, ZOLTA, ZIELONA, BRAZOWA, NIEBIESKA, ROZOWA, CZARNA, PBIALA, DO_WYBORU, BRAK }; // mo¿e zostaæ u¿yte dla snooker

enum rodzajFaulu { BIALA_WBITA = 4, N_WBITA = 6, N_DOTKNIETA = 5, };
enum gra { BILARD = 0, SNOOKER };

void ruch(int x, int y);
void mouse(int button, int state, int x, int y);
void timer();
void keyboard(int key);
// void display(void);
void translacja(float & katp, float a, float b, bool kierunek); // translacja k¹ta
void RenderText(TTF_Font *font, const GLubyte& R, const GLubyte& G, const GLubyte& B,
	const double& X, const double& Y, const double& Z, const std::string& text);

extern GLfloat ballAmbientMaterial[4];
extern GLfloat ballDiffuseMaterial[9][4];
extern GLfloat ballSpecularMaterial[4];
extern GLfloat ballShininessMaterial;

struct swektor
{
	float x;
	float y;
	float z;
};

struct skaty
{
	float k1;
	float k2;
	float k3;
};

struct quadric
{
	GLdouble radius;
	GLint slices;
	GLint stacks;
	GLdouble height;
	GLdouble baseRadius;
	GLdouble topRadius;
};

struct light
{
	GLfloat * position;
	GLfloat * direction;
	GLfloat * ambient;
	GLfloat * diffuse;
	GLfloat * specular;
	GLfloat spot_cutoff;
	GLfloat spot_exponent;
	GLfloat constant_attenuation;
	GLfloat linear_attenuation;
	GLfloat quadric_attenuation;
};

struct material
{
	GLfloat * ambient;
	GLfloat * diffuse;
	GLfloat * specular;
	GLfloat shininess;
	GLfloat * emission;
};




class CPunkt3D
{

public:
	CPunkt3D();
	CPunkt3D(float x, float y, float z = 0);
	~CPunkt3D() {};

	friend ostream& operator<<(ostream& w, const CPunkt3D & p);
	friend istream & operator >> (istream & w, CPunkt3D & p);
	friend float odleglosc(CPunkt3D & p1, CPunkt3D & p2);
	CPunkt3D operator-(const CPunkt3D & p2);
	CPunkt3D operator+(const CPunkt3D & p2);

	float x;
	float y;
	float z;

};

struct camera
{
	CPunkt3D eye;
	CPunkt3D center;
	CPunkt3D up;
};



class CFigura
{
protected:
	float pole;
	float obwod;
	/*GL*/float * kolor;
	swektor przesuniecie;
	skaty obrot; // k¹t obrotu wokó³ w³asnej osi
	skaty gObrot; // k¹t obrotu wokó³ gloabalnych osi
public:
	CFigura() {};

	~CFigura();
	//virtual void wyswietl()=0;
	virtual void rysuj() = 0;

	void oPrzesuniecie(float tx, float ty, float tz);
	//virtual float obliczPole();
	//virtual float  obliczObwod();
};

class CProstokat : public CFigura
{
	float a; //szerokoœæ
	float b; //wysokoœæ
public:
	CPunkt3D wierzcholekLG;
	CPunkt3D wierzcholekLD;
	CPunkt3D wierzcholekPG;
	CPunkt3D wierzcholekPD;


	CProstokat() {};
	CProstokat(CPunkt3D p, float a, float b, /*GL*/float * aKolor = NULL /*,swektor * aPrzesuniecie=NULL, skaty * aObrot=NULL */);
	~CProstokat() {};
	//void wyswietl();
	void obroc(skaty * aObrot); // ustawia obrót
	void przesun(swektor * aPrzesuniecie); // ustawia wektor przesuniêcia
	void rysuj();
	friend void keyboard(int key); // ta funkcja bêdzie zmieniaæ k¹ty i przesuniêcia
	bool SprawdzZakres(float ax, float ay); // sprawdza czy punkt o wsp. (ax, ay) znajduje siê w prostok¹cie
											//float obliczPole();
											//float obliczObwod();

};




class Tor : public CFigura
{

	//CPunkt3D * torWew;
	//CPunkt3D * torZew;
	unsigned int lWierzcholkow;
	//CProstokat * zCzolg;
	float * kolorToru; // dla ca³ego czo³gu kolorem jes pole klasy CFigura
public:
	Tor() {};
	Tor(unsigned int alWierzcholkow, float aRw, float aRz);
	//CCzolg(CProstokat * aCzolg, CProstokat * aLufa, /*GL*/float * aKolor=NULL/*, bool aOpcja=false*/); 
	//domyœlnie widok z profilu, jeœli opcja=true to widok z góry
	~Tor() {};
	//void wyswietl();
	//void obrocLufe(float kat);
	//void przesunCzolg(swektor * przesuniecie);
	void rysuj();
	void zmienKolorToru(float * aKolor);
	float rw; //promieñ wewnêtrzny
	float rz; //promieñ zewnêtrzny
			  //	friend void Keyboard(unsigned char znak, int x, int y); // ta funkcja bêdzie zmieniaæ k¹ty i przesuniêcia
			  //void oPrzesuniecie(float tx,float ty,float tz ); //przesuniecie z uwzglêdnieniem obrotu
			  //float obliczPole();
			  //float obliczObwod();

};

class CKolo : public CFigura
{
protected:
	float r; // promieñ ko³a lub okrêgu
	float * kolor;
	int lWierzcholkow;
public:
	CKolo() {};
	~CKolo() {};
	CKolo(unsigned int alWierzcholkow, float ar, float * aKolor = NULL);
	virtual void rysuj() = 0;

};

/*class CBila : public CKolo  // klasa dla bili bilardowej
{
AUX_RGBImageRec *tekstura;
unsigned short int nr;
bool rodzaj; //0-po³ówka; 1-ca³a;
public:
CBila(){};
//CBila();

void Rysuj()


};*/


class CKula : public CFigura
{
protected:
	GLUquadric * qSfera;
	quadric sfera;
	float r;
	float ** siatka;
	//	float * kolor;
	//	CPunkt3D * srodek;

public:
	CKula() {};
	CKula(swektor * aSrodek, float ar, int alPol, int alRow, float * aKolor = NULL);
	~CKula();
	//	void genSphere();

	//	virtual void rysuj()=0; 

};



class CBila : public CKula  // klasa dla bili bilardowej
{
	//do snookera trzeba u¿yæ materia³ów i œwiat³a lub smaego koloru
	//	AUX_RGBImageRec *tekstura;
	//unsigned short int nr;
	//	bool rodzaj; //0-po³ówka; 1-ca³a;
	kolory sKolor;// informacja o kolorze bili a zarazem o liczbie punktów za ni¹ (enum)

				  //energia r.obrotowego = energia r.postepowego
				  //swektor * przesuniecie; //przesuniecie wzglêdem rogu pola gry
	unsigned int eko;
	unsigned int ekp; // energia kinetyczna ruchu postêpowego
	unsigned int w;
	long double Vp;
	float waga; // w gramm
	float Vx;
	float Vy;
	float kx;
	float ky;
	float kierunek; // kat wzglêdem poziomu
	float alfa;
	float dPrzesuniecie; // przesuniêcie bili w ostatnim odstêpie czasu
	material * material0;
public:
	CBila() {};
	~CBila() {};
	CBila(kolory aKolor, swektor * aSrodek = NULL)
		: CKula(aSrodek, RADIUS, SLICES, STACKS, fKolory[aKolor]), sKolor(aKolor), alfa(0), dPrzesuniecie(0), kierunek(0), Vp(0), Vx(0), Vy(0), waga(150)
	{
		material0 = new material;
		material0->ambient = new GLfloat[4];
		material0->ambient = ballAmbientMaterial;
		material0->diffuse = new GLfloat[4];
		material0->diffuse = ballDiffuseMaterial[sKolor];
		material0->specular = new GLfloat[4];
		material0->specular = ballSpecularMaterial;
		material0->shininess = ballShininessMaterial;
	};

	//	CBila(kolory aKolor=BRAK, swektor * aPrzesuniecie=NULL);


	//CBila(unsigned short int aNr, bool aRodzaj )  //rodzaj - polowka czy ca³a
	//{}; // dla bilarda
	float odlegloscPunktu(float x, float y);
	friend void mouse(int button, int state, int x, int y);
	friend void timer();
	friend void ruch(int x, int y);

	void rysuj();
	void wyzeruj();  // zeruje prêdkoœci
	float gPromien();
	void fPrzesuniecie(float tx, float ty, float tz = 0);
	void przesunDo(float tx, float ty, float tz = -50);
	swektor * gPrzesuniecie();

	friend class CStol;
	friend class CBilard;
	friend class CKij;

};

class CKij : public CFigura // klasa dla gracza
{
	//	swektor * przesuniecie; // to samo co œrodek bili wzglêdem rogu pola gry
	swektor * oddalenie; //dodatkowe oddalenie koñca kija od œrodka bili
	CPunkt3D * koniecKija;
	CPunkt3D * poczatekKija;

	GLUquadricObj * qKij1;
	GLUquadricObj * qKij2;
	GLUquadricObj * qKij3;  // dysk na czubku kija
	quadric kij1;
	quadric kij2;
	quadric kij3;

	material * material0;
	//	AUX_RGBImageRec * tekstura;
	float ** siatka;
	float dlugoscKija;
	float maxOddalenie;
	float zaw; // zawieszenie nad bil¹ 
	float r; // promieñ przekroju w najszerszym miejscu 
	float nachylenie;
public:
	CKij();
	~CKij();
	void fObrot(float kat);
	void fPrzesuniecie(float tx, float ty, float tz = 0);
	void przesunDo(float tx, float ty, float tz = 6);
	void oddal(float dx, float dy = 0.0f);

	friend void mouse(int button, int state, int x, int y);
	friend void timer();
	friend void ruch(int x, int y);
	friend void keyboard(int key);
	void rysuj();
	friend class CStol;
	friend class CBilard;
	friend class CBila;
};
class CStol : public CFigura  // klasa dla sto³u bilardowego
{
	//	AUX_RGBImageRec * gTekstura; // tekstra dla pokrycia sto³u
	//	AUX_RGBImageRec * bocznaTekstura; // tekstra bandy sto³u
	// float * bKolor;
	//float * gKolor;
	//siatki dla poszczególnych elelmentów sto³u

	Mix_Chunk * uderzenie;
	GLUquadric * qNoga;
	quadric noga;
	light  * light0;
	bool mode; // zwyk³a bila, lub grafika do deklaracji

	float ** siatka;
	CPunkt3D * szkielet1;
	CPunkt3D * szkielet2;
	CPunkt3D * sukno;
	CPunkt3D * bandy;
	CPunkt3D * luzy;

	swektor sPrzesuniecie; // sta³e przesuniêcie dla uk³adu wspó³rzêdnych kamery

						   // wysokoœci na których s¹ zawieszone elementy
	float hSukno;
	float ghSzkielet;
	float dhSzkielet;
	float dhBandy;
	float ghBandy;
	float hBila;
	float hKij;
	float hNoga;
	float hStol;
	/*
	unsigned int texSukno;
	unsigned int texBanda;
	unsigned int texBila;
	unsigned int texKij;*/

	GLuint lStol; // display list id of snooker table quarter
	GLuint lBila; // display list id of snooker ball
	GLuint lNoga; // display list id of snooker table leg
	GLuint * textures;
	float ** siatkaBili;

	CPunkt3D * wierzcholek; // punkt w którym zwieszony jest ca³y stó³ - tak¿e wsp. z - wysokoœæ
	CPunkt3D * wierzcholek2;
	CPunkt3D * rog;
	swektor * zewBanda;
	CKij * kij;
	CBila ** bile;  // wskaŸniki na elementy, które bêd¹ rysowane wzglêdem naro¿nika pola gry
	bool * wbite; // 0-jest na stole; 1-wbita do ³uzy (je¿eli bia³a[0] wbita-faul) informacja które rysowaæ // 22 dla snookera
	CBila * tmpBila2;
	CBila * rBila2;

	//bool ** odbicia;  // czy 
	float szerokosc;
	float dlugosc;

	float * katBandy;
	float * b0; // wspó³czynniki prostych band ³uz
	float * a0;
	CPunkt3D * kraniec; // sta³e wsp. zale¿ne od sto³u
	CPunkt3D  srBazy;
	bool * warunek;  // warunki wpadania bil do kieszeni
	bool luza; // czy jest w obszarze ³uzy

	float rozwBandy1;  // rozwarcie bandy (³uzy œrodkowe)
	float rozwBandy2;  // rozwarcie bandy (³uzy naroznikowe)
	float szerokoscBandy; // wewnêt
	float szerokoscZewBandy; // wewnêt
	float wysokoscBandy;
	float wewPromienLuzy; // od ³uzy
	float zewPromienLuzy; // od sukna 
	float odlBazy;
	float promienBazy;
	float mi; // opóŸnienie dla danego stolu [j/s^2]
	swektor * odLuzySrod;
	swektor * dWektor; // wektor o d³ugoœci promienia wewnêtrznego
	swektor * dWektor2; // wektor przesuniêcia zaokr¹gleñ ³uz od krawêdzi
	float ** tab;
public:

	CStol(CKij *& aKij, CBila ** aBile); //domyœlnie tworzy stol dla bilardu
	~CStol();
	void rysuj();
	void narysujKrawedz();
	void sprawdzLuze(unsigned short int nrLuzy, unsigned short int nrBili);
	void odbicie(int i, int j, float odleglosc, float odlx, float odly);
	int LoadGLTextures();
	friend void mouse(int button, int state, int x, int y);
	friend void timer();
	friend void ruch(int x, int y);

	friend class CBilard;
	friend class CBila;
	friend class CKij;

	/*



	friend class CKolo;
	friend class CFigura;
	*/

};

class CGracz  // klasa dla gracza
{
	int sPunkty;
	unsigned int czas;
	unsigned int czasRuchu;
	unsigned int uCzasRuchu;
	bool koniecCzasu;
	bool aktywny;
	//	bool faul;
	//	unsigned int sKara; 
	unsigned int seria;  // seria - bilard, bie¿¹cy break - snooker
	unsigned int maxBreak;
	string * nazwa;
	long long int pocztekGry;
	long long int poczatekRuchu;
public:
	CGracz() {};
	CGracz(string * aNazwa);
	~CGracz();

	void zmniejszCzas();
	unsigned int rczas();
	unsigned int rczasRuchu();

	friend void timer();
	friend class CBilard; //CBilard mo¿e korzystaæ z pól CGracz (nie odwrotnie)
};

enum Camera { CAM_OVERVIEW, CAM_TOP, CAM_SHOT, CAM_TEST };

class CBilard  // klasa dla rozgrywki
{
	long double counter;
	long double pCounter; // liczenie czasu po pauzie
	bool sKomunikat; // czy komunikat zosta³ stworzony - po faulu
	bool wznowienie;
	bool pauza; // pauza na ¿¹danie gracza
	bool przerwa; // przerwa w czasie po wykonaniu ruchu
	bool zmGracza; // zmiana gracza poup³ywie czasu na ruch
	float ** bPozycje;   // pozycje grafik bil do wyboru
	bool camShot;
	float maxVp;
	Camera currentCam;
	SDL_Surface *surface;
	GLubyte* dst_image;
	GLubyte* image_data_temp;
	unsigned int * textTextures;
	TTF_Font *font;
	camera ** cam;
	CStol * stol;
	CGracz ** gracze;
	CPunkt3D * pn;  // punkt naciœniêcia przy uderzeniu
	kolory pierwszaUderzona;
	//	bool kolorowe; // czy zaczê³a siê ju¿ seria kolorowych
	bool zwyciezca; // gracz 1 lub 2 (0-1)
	bool remis;
	bool zmiana; // zmiana stanu gry
	bool bialaWRece;
	bool pBialaWRece; // poprzednio bia³a w rêce 
	bool pierwszeUderzenie;
	bool poczatekKolorowych;
	bool poczatek;
	bool poczatekAnimacji;
	bool zmCzas; // zmniejszenie czasu u jednego z graczy
	bool faul;
	bool nowyRuch;
	long long int pocztekGry;
	unsigned int lKolorowych;
	unsigned int lCzerwonych;
	bool bialaUderzona;
	bool wybor;
	bool ** odbite;
	float ** prevc; // poprzednia odleg³oœæ miêdzy bilami przy odbiciu 
	CPunkt3D * t; // punkt styku bili z band¹ ³uzy


				  // zmienne potrzebne do wyœwietlania przebiegu gry
	string komunikat;
	string ** napisy; // blok tekstu z wynikami - dla jednego gracza
	string tLinia;
	char * buf;
	float ** tPozycje;  // pozycje wyœwietlania wyników - oœ y
	float * odd;		// pozycje wyœwietlania wyników - oœ x

	float pHeight;
	float pWidth;
	float border;
	float qHeight;
	float qWidth;
	float tHeight;
	float tWidth;
	float kPrzesuniecie;	// przesuniêcie kwadratu od paska si³y

	float predkoscAnimacji;
	float hText; //g³êbokoœæ zawieszenie tekstu
	bool aktualnyGracz; // 0 - I gracz; 1 - II gracz;
	bool koniec;
	float wychylenie;
	//float silaUderzenia;
	gra typGry;
	bool animacja; // czy ma zacz¹æ animacjê kij¹ po puszczeniu klawiszas
	bool uderzenie; // zmienna ustawiona na true po uderzeniu jej (prawy klawisz->(przytrzymywany)->puszczenie->animacja
	bool zerowe; // true - prêdkoœci wszystkich bil wyzerowa³y siê, po uderzeniu; false - bile s¹ w ruchu, lub nie zasz³o uderzenie
	float wspEn;
	float stala;
	float liczbaBil;
	bool lPrzytrzymany;
	bool pPrzytrzymany;
	bool bialaPrzytrzymana;
	bool kPrzytrzymany; // przytrzymany kwadrat do zwiêkszania si³y bili
	kolory nastepna; //kolor nastêpnej bili do wbicia
	kolory obecna; //kolor poprzedniej bili do wbicia
				   //float kat;
public:
	float shot_con1; // promieñ oddalenia kamery od bili bia³ej w CAM_SHOT
	float shot_con2; // wysokoœæ zawieszenia kamery nad sto³em w CAM_SHOT
	float overview_con1; // promieñ oddalenia kamery od œrodka sto³u w CAM_OVERVIEW
	float overview_con2; // wysokoœæ zawieszenia kamery nad sto³em CAM_OVERVIEW
	float overview_angle; // promieñ oddalenia kamery od œrodka sto³u w CAM_OVERVIEW
	CBilard() {};
	CBilard(CStol * wsk, CGracz ** aGracze, string * sGracz1, string * sGracz2);
	//CBilard(gra aTypGry){};
	~CBilard();

	//	void nieWbita(bool gracz); 
	//	void wbita(bool gracz, kolory bWbita); 
	void pokazStanGry();
	void ustawBile(kolory bKolor);
	bool czyZajety(CPunkt3D & p1, short int & iZajety/*, float & odl*/); // czy wskazany punkt jest zajêty przez inn¹ bilê
																		 //	unsigned int LoadTextTexture(string name);

	friend void RenderText(TTF_Font *font, const GLubyte& R, const GLubyte& G, const GLubyte& B,
		const double& X, const double& Y, const double& Z, const std::string& text);

	friend void mouse(int button, int state, int x, int y);
	friend void timer();
	friend void ruch(int x, int y);
	friend void keyboard(int key);

	friend class CStol; // przy rysowaniu kija potrzebuje informacji czy zasz³o uderzenie
	friend class CBila;
	/*
	friend class CKij;


	friend class CGracz;*/
	void rysuj(); // rysuje wszystkie elementy rozgrywki
};


