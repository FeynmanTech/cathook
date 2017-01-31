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

void AddSideString(int fg, const char* fmt, ...) {
	if (!g_pStringsSide) return;
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
	g_pStringsSide[g_nStringsSide].m_nColor = fg;
	g_pStringsSide[g_nStringsSide].m_String = buffer;
	g_pStringsSide[g_nStringsSide].m_bColored = true;
	g_nStringsSide++;
}

void DrawStrings() {
	int y = 8;
	for (int i = 0; i < g_nStringsSide; i++) {
		//draw::DrawString(8, y, g_pStringsSide[i].m_Color, g_pStringsSide[i].m_Background, false, g_pStringsSide[i].m_String);
		draw::String(fonts::ESP, 8, y, g_pStringsSide[i].m_nColor, 2, g_pStringsSide[i].m_String);
		y += 14;
	}
	y = draw::height / 2;
	for (int i = 0; i < g_nStringsCenter; i++) {
		draw::String(fonts::ESP, draw::width / 2, y, g_pStringsCenter[i].m_nColor, 2, g_pStringsCenter[i].m_String);
		y += 14;
	}
}

ESPStringCompound::~ESPStringCompound() {
	//if (m_String) delete [] m_String;
}

void AddCenterString(int fg, const char* fmt, ...) {
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
	g_pStringsCenter[g_nStringsCenter].m_nColor = fg;
	g_pStringsCenter[g_nStringsCenter].m_String = buffer;
	g_pStringsCenter[g_nStringsCenter].m_bColored = true;
	g_nStringsCenter++;
}


// TODO globals
unsigned long fonts::ESP = 0;
unsigned long fonts::MENU = 0;
unsigned long fonts::MENU_BIG = 0;
int draw::width = 0;
int draw::height = 0;

namespace colors {

int pink;

int white;
int black;

int red,    blu;
int red_b,  blu_b;  // Background
int red_v,  blu_v;  // Invis
int red_u,  blu_u;

int yellow; // Deprecated
int orange;
int green;

}

int colors::Create(int r, int g, int b, int a) {
	unsigned char bytes[4];
	bytes[0] = r;
	bytes[1] = g;
	bytes[2] = b;
	bytes[3] = a;
	return *(int*)bytes;
}

void colors::Init() {
	using namespace colors;
	pink = Create(255, 105, 180, 255);
	white = Create(255, 255, 255, 255);
	black = Create(0, 0, 0, 255);
	red = Create(237, 42, 42, 255);
	blu = Create(28, 108, 237, 255);
	red_b = Create(64, 32, 32, 178);
	blu_b = Create(32, 32, 64, 178);
	red_v = Create(196, 102, 108, 255);
	blu_v = Create(102, 182, 196, 255);
	red_u = Create(216, 34, 186, 255);
	blu_u = Create(167, 75, 252, 255);
	yellow = Create(255, 255, 0, 255);
	green = Create(0, 255, 0, 255);
}

int colors::EntityB(CachedEntity* ent) {
	using namespace colors;
	int result = Transparent(black);
	if (ent->m_Type == ENTITY_PLAYER || ent->m_Type == ENTITY_BUILDING) {
		if (ent->m_iTeam == TEAM_BLU) result = blu_b;
		else if (ent->m_iTeam == TEAM_RED) result = red_b;

		if (ent->m_Type == ENTITY_PLAYER) {
			if (GetRelation(ent) == relation::DEVELOPER) result = Transparent(black, 0.6f);
		}

		if (!ent->IsVisible()) result = Transparent(result, 0.833f);
	}
	return result;
}

int colors::EntityF(CachedEntity* ent) {
	using namespace colors;
	int result = white;
	if (ent->m_iClassID == ClassID::CBaseAnimating) {
		item_type type = GetItemType(ent);
		if (type != item_null) {
			if (type >= item_medkit_small && type <= item_medkit_large) {
				result = green;
			} else if (type >= item_ammo_small && type <= item_ammo_large) {
				// White.
			} else if (type >= item_mp_strength && type <= item_mp_crit) {
				int skin = RAW_ENT(ent)->GetSkin();
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
		if (CE_BYTE(ent, netvar.bDistributed))
			result = red;
		else
			result = green;
	}

	if (ent->m_Type == ENTITY_PROJECTILE) {
		if (ent->m_iTeam == TEAM_BLU) result = blu;
		else if (ent->m_iTeam == TEAM_RED) result = red;
		if (ent->m_bCritProjectile) {
			if (ent->m_iTeam == TEAM_BLU) result = blu_u;
			else if (ent->m_iTeam == TEAM_RED) result = red_u;
		}
	}

	if (ent->m_Type == ENTITY_PLAYER || ent->m_Type == ENTITY_BUILDING) {
		if (ent->m_iTeam == TEAM_BLU) result = blu;
		else if (ent->m_iTeam == TEAM_RED) result = red;
		if (ent->m_Type == ENTITY_PLAYER) {
			if (IsPlayerInvulnerable(ent)) {
				if (ent->m_iTeam == TEAM_BLU) result = blu_u;
				else if (ent->m_iTeam == TEAM_RED) result = red_u;
			}
			if (HasCondition(ent, TFCond_UberBulletResist)) {
				if (ent->m_iTeam == TEAM_BLU) result = blu_v;
				else if (ent->m_iTeam == TEAM_RED) result = red_v;
			}
			switch (GetRelation(ent)) {
			case FRIEND:
				result = green; break;
			case RAGE:
				result = yellow; break;
			case DEVELOPER:
				result = RainbowCurrent(); break;
			}
		}
		if (g_phESP->v_bVisCheck->GetBool())
			if (!ent->IsVisible()) result = Transparent(result);
	}

	return result;
}

int colors::Transparent(int base, float mod /*= 0.5f*/) {
	unsigned char* result = (unsigned char*)&base;
	result[3] *= mod;
	return *(int*)result;
}

int colors::RainbowCurrent() {
	return colors::FromHSL(fabs(sin(interfaces::gvars->curtime / 2.0f)) * 360.0f, 0.85f, 0.9f);
}

int colors::FromHSL(float h, float s, float v) {
	double      hh, p, q, t, ff;
	long        i;

	if(s <= 0.0) {       // < is bogus, just shuts up warnings
		return colors::Create(v * 255, v * 255, v * 255, 255);
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
		return colors::Create(v * 255, t * 255, p * 255, 255);
	case 1:
		return colors::Create(q * 255, v * 255, p * 255, 255);
	case 2:
		return colors::Create(p * 255, v * 255, t * 255, 255);
	case 3:
		return colors::Create(p * 255, q * 255, v * 255, 255);
		break;
	case 4:
		return colors::Create(t * 255, p * 255, v * 255, 255);
	case 5:
	default:
		return colors::Create(v * 255, p * 255, q * 255, 255);
	}
}

int colors::Health(int health, int max) {
	float hf = (float)health / (float)max;
	if (hf > 1) {
		return colors::Create(64, 128, 255, 255);
	}
	return colors::Create((hf <= 0.5 ? 1.0 : 1.0 - 2 * (hf - 0.5)) * 255, (hf <= 0.5 ? (2 * hf) : 1) * 255, 0, 255);
}

void draw::DrawRect(int x, int y, int w, int h, int color) {
	interfaces::surface->DrawSetColor(*reinterpret_cast<Color*>(&color));
	interfaces::surface->DrawFilledRect(x, y, x + w, y + h);
}

ESPStringCompound::ESPStringCompound() {
	m_String = nullptr;
	m_bColored = false;
	m_nColor = colors::white;
}

void draw::Initialize() {
	fonts::ESP = interfaces::surface->CreateFont();
	fonts::MENU = interfaces::surface->CreateFont();
	fonts::MENU_BIG = interfaces::surface->CreateFont();

	interfaces::surface->SetFontGlyphSet(fonts::ESP, "TF2 Build", fonts::ESP_HEIGHT, 0, 0, 0, 0x0); // or Ubuntu Mono Bold
	interfaces::surface->SetFontGlyphSet(fonts::MENU, "Verdana", fonts::MENU_HEIGHT, 0, 0, 0, 0x0);
	interfaces::surface->SetFontGlyphSet(fonts::MENU_BIG, "Verdana Bold", fonts::MENU_BIG_HEIGHT, 0, 0, 0, 0x0);
}

void draw::DrawLine(int x, int y, int dx, int dy, int color) {
	interfaces::surface->DrawSetColor(*reinterpret_cast<Color*>(&color));
	interfaces::surface->DrawLine(x, y, x + dx, y + dy);
}

bool draw::EntityCenterToScreen(CachedEntity* entity, Vector& out) {
	if (!entity) return false;
	Vector world;
	Vector min, max;
	RAW_ENT(entity)->GetRenderBounds(min, max);
	world = entity->m_vecOrigin;
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

void draw::OutlineRect(int x, int y, int w, int h, int color) {
	interfaces::surface->DrawSetColor(*reinterpret_cast<Color*>(&color));
	interfaces::surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void draw::GetStringLength(unsigned long font, char* string, int& length, int& height) {
	wchar_t buf[1024] = {'\0'};
	mbstowcs(buf, string, strlen(string));
	interfaces::surface->GetTextSize(font, buf, length, height);
}

void draw::String (unsigned long font, int x, int y, int color, int shadow, const char* text) {
	bool newlined = false;
	for (int i = 0; i < strlen(text); i++) {
		if (text[i] == '\n') {
			newlined = true; break;
		}
	}
	if (newlined) {
		int w, h;
		GetStringLength(font, "W", w, h);
		char ch[1024];
		memcpy(ch, text, 1024);
		int s = 0;
		int n = 0;
		for (int i = 0; i < strlen(text); i++) {
			if (ch[i] == '\n') {
				ch[i] = 0;
				draw::String(font, x, y + n * (h), color, shadow, &ch[0] + s);
				n++;
				s = i + 1;
			}
		}
		draw::String(font, x, y + n * (h), color, shadow, &ch[0] + s);
	} else {
		wchar_t string[1024] = { '\0' };
		mbstowcs(string, text, 1024);
		draw::WString(font, x, y, color, shadow, string);
	}
}

void draw::String(unsigned long font, int x, int y, int color, int shadow, std::string text) {
	draw::String(font, x, y, color, shadow, text.c_str());
}

void draw::WString(unsigned long font, int x, int y, int color, int shadow, const wchar_t* text) {
	if (shadow) {
		unsigned char alpha = (color >> 24);
		int black_t = ((alpha == 255) ? colors::black : colors::Create(0, 0, 0, alpha / shadow));
		if (shadow > 0) {
			draw::WString(font, x + 1, y + 1, black_t, false, text);
		}
		if (shadow > 1) {
			draw::WString(font, x - 1, y + 1, black_t, false, text);
			draw::WString(font, x - 1, y - 1, black_t, false, text);
			draw::WString(font, x + 1, y - 1, black_t, false, text);
			draw::WString(font, x + 1, y, black_t, false, text);
			draw::WString(font, x, y + 1, black_t, false, text);
			draw::WString(font, x, y - 1, black_t, false, text);
			draw::WString(font, x - 1, y, black_t, false, text);
		}
	}
	interfaces::surface->DrawSetTextPos(x, y);
	interfaces::surface->DrawSetTextColor(*reinterpret_cast<Color*>(&color));
	interfaces::surface->DrawSetTextFont(font);
	interfaces::surface->DrawUnicodeString(text);
}

void draw::FString(unsigned long font, int x, int y, int color, int shadow, const char* text, ...) {
	va_list list;
	char buffer[2048] = { '\0' };
	va_start(list, text);
	vsprintf(buffer, text, list);
	va_end(list);
	draw::String(font, x, y, color, shadow, buffer);
}

std::pair<int, int> draw::GetStringLength(unsigned long font, std::string string) {
	int l, h;
	draw::GetStringLength(font, (char*)string.c_str(), l, h);
	return std::make_pair(l, h);
}

