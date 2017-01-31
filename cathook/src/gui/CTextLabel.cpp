/*
 * CTextLabel.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#include "CTextLabel.h"

#include "../common.h"
#include "../sdk.h"

CTextLabel::CTextLabel(std::string name, IWidget* parent, std::string text) : CBaseWidget(name, parent) {
	SetText(text);
}

void CTextLabel::SetPadding(int x, int y) {
	Props()->SetInt("padding_x", x);
	Props()->SetInt("padding_y", y);
}

void CTextLabel::SetText(std::string text) {
	Props()->SetString("text", text.c_str());
	auto padding = std::make_pair(Props()->GetInt("padding_x"), Props()->GetInt("padding_y"));
	auto size = draw::GetStringLength(fonts::MENU, text);
	SetSize(size.first + padding.first * 2, size.second + padding.second * 2);
}

std::string CTextLabel::GetText() {
	return std::string(Props()->GetString("text", ""));
}

void CTextLabel::Draw(int x, int y) {
	draw::String(fonts::MENU, x, y, colors::white, 1, GetText());
}
