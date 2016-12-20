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

namespace colors {

Color pink;

Color white;
Color black;

Color red,    blu;
Color red_b,  blu_b;  // Background
Color red_i,  blu_i;  // Invis
Color red_u,  blu_u;

Color yellow; // Deprecated
Color orange;
Color green;

}

/*Color colors::white(255, 255, 255, 255);
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
Color colors::dbgblu(16,  16,  48,  255);*/

void colors::Init() {
	using namespace colors;
	pink = Color(255, 105, 180, 255);
	white = Color(255, 255, 255, 255);
	black = Color(0, 0, 0, 255);
	red = Color(237, 42, 42, 255);
	blu = Color(28, 108, 237, 255);
	red_b = Color(64, 32, 32, 178);
	blu_b = Color(32, 32, 64, 178);
	red_i = Color(252, 159, 159, 255);
	blu_i = Color(159, 191, 252, 255);
	red_u = Color(216, 34, 186, 255);
	blu_u = Color(167, 75, 252, 255);
	yellow = Color(255, 255, 0, 255);
	green = Color(0, 255, 0, 255);
}

Color colors::EntityB(CachedEntity* ent) {
	using namespace colors;
	Color result = Transparent(black);
	if (IsPlayer(ent->m_pEntity) || IsBuilding(ent->m_pEntity)) {
		if (ent->m_iTeam == TEAM_BLU) result = blu_b;
		else if (ent->m_iTeam == TEAM_RED) result = red_b;

		if (IsPlayer(ent->m_pEntity)) {
			if (GetRelation(ent->m_pEntity) == relation::DEVELOPER) result = Transparent(black, 0.6f);
		}

		if (!ent->m_bIsVisible) result = Transparent(result, 0.833f);
	}
	return result;
}

Color colors::EntityF(CachedEntity* ent) {
	using namespace colors;
	Color result = white;
	if (ent->m_iClassID == ClassID::CBaseAnimating) {
		item_type type = GetItemType(ent->m_pEntity);
		if (type != item_null) {
			if (type >= item_medkit_small && type <= item_medkit_large) {
				result = green;
			} else if (type >= item_ammo_small && type <= item_ammo_large) {
				// White.
			} else if (type >= item_mp_strength && type <= item_mp_crit) {
				int skin = ent->m_pEntity->GetSkin();
				if (skin == 1) {
					result = red;
				} else if (skin == 2) {
					result = blu;
				} else {
					result = yellow;
				}
			}
		}
	}
	if (ent->m_iClassID == ClassID::CCurrencyPack) {
		result = green; // TODO currency pack (red)
	}

	if (IsPlayer(ent->m_pEntity) || IsBuilding(ent->m_pEntity)) {
		if (ent->m_iTeam == TEAM_BLU) result = blu;
		else if (ent->m_iTeam == TEAM_RED) result = red;
		if (IsPlayer(ent->m_pEntity)) {
			if (IsPlayerInvulnerable(ent->m_pEntity)) {
				if (ent->m_iTeam == TEAM_BLU) result = blu_u;
				else if (ent->m_iTeam == TEAM_RED) result = red_u;
			}
			if (ent->Var<int>(netvar.iCond) & cond::cloaked) {
				if (ent->m_iTeam == TEAM_BLU) result = blu_i;
				else if (ent->m_iTeam == TEAM_RED) result = red_i;
			}
			switch (GetRelation(ent->m_pEntity)) {
			case FRIEND:
				result = green; break;
			case RAGE:
				result = yellow; break;
			case DEVELOPER:
				result = RainbowCurrent(); break;
			}
		}
		if (!ent->m_bIsVisible) result = Transparent(result);
	}

	return result;
}

Color colors::Transparent(Color base, float mod /*= 0.5f*/) {
	Color result = base;
	result[3] *= mod;
	return result;
}

Color colors::RainbowCurrent() {
	return colors::FromHSL(fabs(sin(interfaces::gvars->curtime / 2.0f)) * 360.0f, 0.85f, 0.9f);
}

Color colors::FromHSL(float h, float s, float v) {
	double      hh, p, q, t, ff;
	long        i;

	if(s <= 0.0) {       // < is bogus, just shuts up warnings
		return Color(v * 255, v * 255, v * 255, 255);
	}
	hh = h;
	if(hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch(i) {
	case 0:
		return Color(v * 255, t * 255, p * 255, 255);
	case 1:
		return Color(q * 255, v * 255, p * 255, 255);
	case 2:
		return Color(p * 255, v * 255, t * 255, 255);
	case 3:
		return Color(p * 255, q * 255, v * 255, 255);
		break;
	case 4:
		return Color(t * 255, p * 255, v * 255, 255);
	case 5:
	default:
		return Color(v * 255, p * 255, q * 255, 255);
	}
}

Color colors::Health(int health, int max) {
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
	m_Color = colors::white;
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

void draw::DrawString(int x, int y, Color color, const char* text, ...) {
	if (!text) return;
	va_list list;
	char buffer[1024] = { '\0' };
	wchar_t string[1024] = { '\0' };
	va_start(list, text);
	vsprintf(buffer, text, list);
	va_end(list);
	swprintf(string, 1024, L"%s", buffer);
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
