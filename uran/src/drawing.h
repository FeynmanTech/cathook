/*
 * drawing.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef DRAWING_H_
#define DRAWING_H_

class Color;
class Vector;

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

void Initialize();
void DrawString(unsigned long font, int x, int y, Color color, const wchar_t* text);
void DrawString(int x, int y, Color color, bool center, const char* text, ...);
bool WorldToScreen(Vector &origin, Vector &screen);
void OutlineRect(int x, int y, int w, int h, Color color);
void GetStringLength(wchar_t* string, int& length, int& height);

}

#endif /* DRAWING_H_ */
