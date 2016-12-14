/*
 * drawing.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"

int g_nStringsSide = 0;
int g_nStringsCenter = 0;
ESPStringCompound* g_pStringsSide;
ESPStringCompound* g_pStringsCenter;

void InitStrings() {
	g_pStringsSide = new ESPStringCompound[32]();
	g_pStringsCenter = new ESPStringCompound[32]();
	ResetStrings();
}

void ResetStrings() {
	g_nStringsSide = 0;
	g_nStringsCenter = 0;
}

void AddSideString(Color fg, Color bg, const char* fmt, ...) {
	if (g_pStringsSide[g_nStringsSide].m_String) {
		delete g_pStringsSide[g_nStringsSide].m_String;
	}
	char* buffer = new char[1024]();
	va_list list;
	va_start(list, fmt);
	vsprintf(buffer, fmt, list);
	va_end(list);
	if (g_nStringsSide >= 32) {
		logging::Info("Can't attach more than %i strings to an entity", 32);
		return;
	}
	g_pStringsSide[g_nStringsSide].m_Color = fg;
	g_pStringsSide[g_nStringsSide].m_Background = bg;
	g_pStringsSide[g_nStringsSide].m_String = buffer;
	g_nStringsSide++;
}

void DrawStrings() {
	int y = 8;
	for (int i = 0; i < g_nStringsSide; i++) {
		draw::DrawString(8, y, g_pStringsSide[i].m_Color, g_pStringsSide[i].m_Background, false, g_pStringsSide[i].m_String);
		y += 14;
	}
	y = draw::height / 2;
	for (int i = 0; i < g_nStringsCenter; i++) {
		draw::DrawString(draw::width / 2, y, g_pStringsCenter[i].m_Color, g_pStringsCenter[i].m_Background, false, g_pStringsCenter[i].m_String);
		y += 14;
	}
}

void AddCenterString(Color fg, Color bg, const char* fmt, ...) {
	if (g_pStringsCenter[g_nStringsCenter].m_String) {
		delete g_pStringsCenter[g_nStringsCenter].m_String;
	}
	char* buffer = new char[1024]();
	va_list list;
	va_start(list, fmt);
	vsprintf(buffer, fmt, list);
	va_end(list);
	if (g_nStringsCenter >= 32) {
		logging::Info("Can't attach more than %i strings to an entity", 32);
		return;
	}
	g_pStringsCenter[g_nStringsCenter].m_Color = fg;
	g_pStringsCenter[g_nStringsCenter].m_Background = bg;
	g_pStringsCenter[g_nStringsCenter].m_String = buffer;
	g_nStringsCenter++;
}


// TODO globals
unsigned long draw::font_handle = 0;
unsigned long draw::panel_top = 0;
unsigned long draw::font_handle_large = 0;
int draw::width = 0;
int draw::height = 0;
Color draw::white(255, 255, 255, 255);
Color draw::red(184, 56, 59, 255);
Color draw::blue(88, 133, 162, 255);
Color draw::yellow(255, 255, 0, 255);
Color draw::black(0, 0, 0, 255);

Color colors::white(255, 255, 255, 255);
Color colors::black(0,  0,  0,   255);
Color colors::tf_red(184, 56,  59,  255);
Color colors::tf_blu(88,  133, 162, 255);
Color colors::yellow(255, 255, 0,   255);
Color colors::orange(255, 128,  0,  255);
Color colors::green(0,  255,  0,   255);
Color colors::dk_red(150, 75,  75,  180);
Color colors::dk_blu(75,  75,  150, 180);
Color colors::bg_red(64,  32,  32,  255);
Color colors::bg_blu(32,  32,  64,  255);
Color colors::dbgred(48,  16,  16,  255);
Color colors::dbgblu(16,  16,  48,  255);


Color colors::GetTeamColor(int team, bool dark) {
	if (team == 2) {
		if (dark)
			return colors::dk_red;
		else
			return colors::tf_red;
	} else if (team == 3) {
		if (dark)
			return colors::dk_blu;
		else
			return colors::tf_blu;
	}
	return colors::white;
}

Color colors::GetTeamBgColor(int team, bool dark) {
	if (team == 2) {
		return colors::bg_red;
	} else if (team == 3) {
		return colors::bg_blu;
	} else {
		return colors::black;
	}
}

Color TEAM_COLORS[4] = {
	draw::yellow, draw::white, draw::red, draw::blue
};

Color colors::GetHealthColor(int health, int max) {
	float hf = (float)health / (float)max;
	if (hf > 1) {
		return Color(64, 128, 255, 255);
	}
	return Color((hf <= 0.5 ? 1.0 : 1.0 - 2 * (hf - 0.5)) * 255, (hf <= 0.5 ? (2 * hf) : 1) * 255, 0, 255);
}

void draw::DrawRect(int x, int y, int w, int h, Color color) {
	interfaces::surface->DrawSetColor(color);
	interfaces::surface->DrawFilledRect(x, y, x + w, y + h);
}

ESPStringCompound::ESPStringCompound() {
	m_Color = draw::white;
	m_String = nullptr;
}

void draw::Initialize() {
	draw::font_handle = interfaces::surface->CreateFont();
	draw::font_handle_large = interfaces::surface->CreateFont();
	interfaces::surface->SetFontGlyphSet(draw::font_handle, "Ubuntu Mono Bold", 14, 0, 0, 0, 0x0); // Ubuntu Mono Bold
	interfaces::surface->SetFontGlyphSet(draw::font_handle_large, "TF2 BUILD", 32, 500, 0, 0, 0x200);
}

void draw::DrawString(unsigned long font, int x, int y, Color color, const wchar_t* text) {
	interfaces::surface->DrawSetTextPos(x, y);
	interfaces::surface->DrawSetTextColor(color);
	interfaces::surface->DrawSetTextFont(draw::font_handle);
	//interfaces::surface->DrawPrintText(text, wcslen(text));
	interfaces::surface->DrawUnicodeString(text, vgui::FONT_DRAW_DEFAULT);
}

void draw::DrawString(int x, int y, Color color, Color background, bool center, const char* text, ...) {
	if (!text) return;
	va_list list;
	char buffer[1024] = { '\0' };
	wchar_t string[1024] = { '\0' };
	va_start(list, text);
	vsprintf(buffer, text, list);
	va_end(list);
	swprintf(string, 1024, L"%s", buffer);
	int l, h;
	draw::GetStringLength((char*)text, l, h);
	if (center) {
		x -= (l / 2);
	}
	draw::GetStringLength((char*)text, l, h);
	Color clr = background;
	clr._color[3] = (unsigned char)180;
	draw::DrawRect(x, y + 1, l + 2, h - 4, clr);
	draw::DrawString(draw::font_handle, x, y, color, string);
}

bool draw::EntityCenterToScreen(IClientEntity* entity, Vector& out) {
	if (!entity) return false;
	Vector world;
	Vector min, max;
	entity->GetRenderBounds(min, max);
	world = entity->GetAbsOrigin();
	world.z += (min.z + max.z) / 2;
	Vector scr;
	bool succ = draw::WorldToScreen(world, scr);
	out = scr;
	return succ;
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

void draw::GetStringLength(char* string, int& length, int& height) {
	wchar_t buf[1024] = {'\0'};
	mbstowcs(buf, string, strlen(string));
	interfaces::surface->GetTextSize(draw::font_handle, buf, length, height);
}
