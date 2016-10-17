/*
 * drawing.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#include "drawing.h"
#include "interfaces.h"

#include "fixsdk.h"
#include <vgui/ISurface.h>
#include <Color.h>
#include <cdll_int.h>
#include <mathlib/vmatrix.h>

unsigned long draw::font_handle = 0;
unsigned long draw::panel_top = 0;
unsigned long draw::font_handle_large = 0;
int draw::width = 0;
int draw::height = 0;
Color draw::white(255, 255, 255, 255);
Color draw::red(255, 64, 64, 255);
Color draw::blue(64, 64, 255, 255);
Color draw::yellow(255, 255, 0, 255);

void draw::Initialize() {
	draw::font_handle = interfaces::surface->CreateFont();
	draw::font_handle_large = interfaces::surface->CreateFont();
	interfaces::surface->SetFontGlyphSet(draw::font_handle, "Tahoma", 19, 500, 0, 0, 0x200);
	interfaces::surface->SetFontGlyphSet(draw::font_handle_large, "Tahoma", 32, 500, 0, 0, 0x200);
}

void draw::DrawString(unsigned long font, int x, int y, Color color, const wchar_t* text) {
	interfaces::surface->DrawSetTextPos(x, y);
	interfaces::surface->DrawSetTextColor(color);
	interfaces::surface->DrawSetTextFont(draw::font_handle);
	interfaces::surface->DrawPrintText(text, wcslen(text));
}

void draw::DrawString(int x, int y, Color color, bool center, const char* text, ...) {
	va_list list;
	char buffer[1024] = { '\0' };
	wchar_t string[1024] = { '\0' };
	va_start(list, text);
	vsprintf(buffer, text, list);
	va_end(list);
	swprintf(string, 1024, L"%s", buffer);
	if (center) {
		int l, h;
		draw::GetStringLength(string, l, h);
		x -= (l / 2);
	}
	draw::DrawString(draw::font_handle, x, y, color, string);
}

bool draw::WorldToScreen(Vector& origin, Vector& screen) {
	VMatrix wts = interfaces::engineClient->WorldToScreenMatrix();
	screen.z = 0;
	float w = wts[3][0] * origin[0] + wts[3][1] * origin[1] + wts[3][2] * origin[2] + wts[3][3];
	if (w > 0.001) {
		float odw = 1.0f / w;
		screen.x = (draw::width / 2) + (0.5 * ((wts[0][0] * origin[0] + wts[0][1] * origin[1] + wts[0][2] * origin[2] + wts[0][3]) * odw) * draw::width + 0.5);
		screen.y = (draw::height / 2) - (0.5 * ((wts[1][0] * origin[0] + wts[1][1] * origin[1] + wts[1][2] * origin[2] + wts[1][3]) * odw) * draw::height + 0.5);
		return true;
	}
	return false;
}

void draw::OutlineRect(int x, int y, int w, int h, Color color) {
	interfaces::surface->DrawSetColor(color);
	interfaces::surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void draw::GetStringLength(wchar_t* string, int& length, int& height) {
	//wchar_t buf[1024] = {'\0'};
	//mbstowcs(buf, string, strlen(string));
	interfaces::surface->GetTextSize(draw::font_handle, string, length, height);
}
