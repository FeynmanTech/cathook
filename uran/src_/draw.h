/*
 * draw.h
 *
 *  Created on: Oct 2, 2016
 *      Author: nullifiedcat
 */

#ifndef DRAW_H_
#define DRAW_H_

#include <stdint.h>
#include <Color.h>

namespace uran {
namespace draw {

unsigned long font_handle;

void Init() {
	font_handle = uran::i_Surface->CreateFont();
	uran::i_Surface->SetFontGlyphSet(font_handle, "TF2_BUILD", 14, 500, 0, 0, 0x200);
}

void DrawString(int x, int y, Color color, const wchar_t* text) {
	uran::i_Surface->DrawSetTextPos(x, y);
	uran::i_Surface->DrawSetTextFont(font_handle);
	uran::i_Surface->DrawSetTextColor(color);
	uran::i_Surface->DrawPrintText(text, wcslen(text));
}

void DrawString(int x, int y, Color color, const char* text, ...) {
	char buffer[1024] = {'\0'};
	wchar_t string[1024] = {'\0'};
	va_list list;
	va_start(list, text);
	vsprintf(buffer, text, list);
	va_end(list);
	swprintf(string, 1024, L"%s", buffer);
	DrawString(x, y, color, string);
}

void DrawRect(int x, int y, int w, int h, Color color);
void DrawOutline(int x, int y, int w, int h, Color color);

}
}

#endif /* DRAW_H_ */
