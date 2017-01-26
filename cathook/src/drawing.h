/*
 * drawing.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef DRAWING_H_
#define DRAWING_H_

#include "fixsdk.h"
#include <Color.h>

class CachedEntity;
class Vector;
class IClientEntity;

struct ESPStringCompound {
	ESPStringCompound();
	~ESPStringCompound();
	char* m_String;
	bool m_bColored;
	Color m_Color;
	Color m_Background;
};

extern Color TEAM_COLORS[4];

namespace colors {
extern Color pink;

extern Color white;
extern Color black;

extern Color red,    blu;
extern Color red_b,  blu_b;  // Background
extern Color red_v,  blu_v;  // Vaccinator
extern Color red_u,  blu_u;  // Ubercharged
extern Color yellow; // Deprecated
extern Color orange;
extern Color green;

void Init();

Color Transparent(Color base, float mod = 0.5f);
Color FromHSL(float h, float s, float l);
Color RainbowCurrent();
Color Health(int health, int max);
Color EntityF(CachedEntity* ent);
Color EntityB(CachedEntity* ent);

}

void InitStrings();
void ResetStrings();
void AddCenterString(Color fg, Color bg, const char* fmt, ...);
void AddSideString(Color fg, Color bg, const char* fmt, ...);
void DrawStrings();

extern ESPStringCompound* g_pStringsSide;
extern int g_nStringsSide;
extern ESPStringCompound* g_pStringsCenter;
extern int g_nStringsCenter;


namespace draw {

extern unsigned long font_handle;
extern unsigned long font_handle_menu;
extern unsigned long panel_top;
extern int width;
extern int height;

void Initialize();

void DrawWideString(unsigned long font, int x, int y, unsigned color, const wchar_t* text);
void DrawString(unsigned long font, int x, int y, unsigned color, const char* text);
void DrawFormatString(unsigned long font, int x, int y, unsigned color, const char* text, ...);
void DrawShadowString(unsigned long font, int x, int y, unsigned color, const char* text, ...);
void DrawFormatShadowString(unsigned long font, int x, int y, unsigned color, const char* text, ...);

void DrawString(unsigned long font, int x, int y, Color color, const wchar_t* text);
void DrawString(int x, int y, Color color, Color background, bool center, const char* text, ...);
void DrawString(int x, int y, Color color, const char* text, ...);
void DrawRect(int x, int y, int w, int h, Color color);
void DrawLine(int x, int y, int dx, int dy, Color color);
bool WorldToScreen(Vector &origin, Vector &screen);
bool EntityCenterToScreen(CachedEntity* entity, Vector& out);
void OutlineRect(int x, int y, int w, int h, Color color);
void GetStringLength(char* string, int& length, int& height);
void DrawString(unsigned font_handle, int x, int y, Color color, const char* text, ...);

}

#endif /* DRAWING_H_ */
