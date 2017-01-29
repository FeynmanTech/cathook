/*
 * drawing.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef DRAWING_H_
#define DRAWING_H_

class CachedEntity;
class Vector;
class IClientEntity;

struct ESPStringCompound {
	ESPStringCompound();
	~ESPStringCompound();
	char* m_String;
	bool m_bColored;
	int m_nColor;
	//Color m_Color;
	//Color m_Background;
};

namespace fonts {

extern unsigned long ESP;
extern unsigned long MENU;
extern unsigned long MENU_BIG;

const int ESP_HEIGHT = 14;
const int MENU_HEIGHT = 12;
const int MENU_BIG_HEIGHT = 30;

}

namespace colors {
extern int pink;

extern int white;
extern int black;

extern int red,    blu;
extern int red_b,  blu_b;  // Background
extern int red_v,  blu_v;  // Vaccinator
extern int red_u,  blu_u;  // Ubercharged
extern int yellow; // Deprecated
extern int orange;
extern int green;

void Init();

int Create(int r, int g, int b, int a);

int Transparent(int base, float mod = 0.5f);
int FromHSL(float h, float s, float l);
int RainbowCurrent();
int Health(int health, int max);
int EntityF(CachedEntity* ent);
int EntityB(CachedEntity* ent);

}

void InitStrings();
void ResetStrings();
void AddCenterString(int fg, const char* fmt, ...);
void AddSideString(int fg, const char* fmt, ...);
void DrawStrings();

extern ESPStringCompound* g_pStringsSide;
extern int g_nStringsSide;
extern ESPStringCompound* g_pStringsCenter;
extern int g_nStringsCenter;


namespace draw {

extern int width;
extern int height;

void Initialize();

void String (unsigned long font, int x, int y, int color, int shadow, const char* text);
void WString(unsigned long font, int x, int y, int color, int shadow, const wchar_t* text);
void FString(unsigned long font, int x, int y, int color, int shadow, const char* text, ...);

/*void DrawString(unsigned long font, int x, int y, Color color, const wchar_t* text);
void DrawString(int x, int y, Color color, Color background, bool center, const char* text, ...);
void DrawString(int x, int y, Color color, const char* text, ...);*/
void DrawRect(int x, int y, int w, int h, int color);
void DrawLine(int x, int y, int dx, int dy, int color);
bool WorldToScreen(Vector &origin, Vector &screen);
bool EntityCenterToScreen(CachedEntity* entity, Vector& out);
void OutlineRect(int x, int y, int w, int h, int color);
void GetStringLength(unsigned long font, char* string, int& length, int& height);
//void DrawString(unsigned font_handle, int x, int y, Color color, const char* text, ...);

}

#endif /* DRAWING_H_ */
