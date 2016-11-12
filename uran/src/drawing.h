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

class Vector;
class IClientEntity;

struct ESPStringCompound {
	ESPStringCompound();
	char* m_String;
	Color m_Color;
};

extern Color TEAM_COLORS[4];

namespace colors {
extern Color white;
extern Color black;
extern Color tf_red;
extern Color tf_blu;
extern Color yellow;
extern Color orange;
extern Color green;
extern Color dk_red;
extern Color dk_blu;
Color GetTeamColor(int team, bool dark);
Color GetHealthColor(int health, int max);
}

namespace draw {

extern unsigned long font_handle;
extern unsigned long font_handle_large;
extern unsigned long panel_top;
extern int width;
extern int height;
extern Color white;
extern Color blue;
extern Color red;
extern Color yellow;
extern Color black;

void Initialize();
void DrawString(unsigned long font, int x, int y, Color color, const wchar_t* text);
void DrawString(int x, int y, Color color, bool center, const char* text, ...);
void DrawRect(int x, int y, int w, int h, Color color);
bool WorldToScreen(Vector &origin, Vector &screen);
bool EntityCenterToScreen(IClientEntity* entity, Vector& out);
void OutlineRect(int x, int y, int w, int h, Color color);
void GetStringLength(char* string, int& length, int& height);

}

#endif /* DRAWING_H_ */
