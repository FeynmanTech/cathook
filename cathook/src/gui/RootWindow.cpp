/*
 * RootWindow.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "RootWindow.h"
#include "TitleBar.h"
#include "CTextLabel.h"

#include "../common.h"

RootWindow::RootWindow() : CBaseWindow(0, "root") {
	IWidget* wgt = new CBaseWindow(this, "testwindow");
	this->m_nSizeX = draw::width;
	this->m_nSizeY = draw::height;
	CTextLabel* text = new CTextLabel(wgt, "testlabel");
	text->SetText("Ayy Lmao!");
	text->SetOffset(50, 50);
	wgt->AddChild(text);
	wgt->SetOffset(200, 200);
	IWidget* title = new TitleBar(wgt, "Test Window");
	wgt->AddChild(title);
	this->AddChild(wgt);
}

RootWindow::~RootWindow() {

}
