/*
 * RootWindow.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "RootWindow.h"
#include "TitleBar.h"
#include "CTextLabel.h"
#include "CCheckbox.h"
#include "CBaseButton.h"
#include "CTextInput.h"
#include "CSplitContainer.h"
#include "CSlider.h"
#include "CTooltip.h"

#include "../common.h"

void B1Callback(IWidget* thisptr) {
	IWidget* label = thisptr->GetParent()->GetChildByName("td");
	CTextLabel* tl = dynamic_cast<CTextLabel*>(label);
	if (tl) {
		char* text = strfmt("wow! this[\"%s\"] = %i", "test_value", thisptr->GetKeyValues()->GetInt("test_value"));
		tl->SetText(text);
		delete [] text;
	}
}

void TICallback(IWidget* thisptr, const char* old, const char* newc) {
	IWidget* label = thisptr->GetParent()->GetChildByName("td");
	CTextLabel* tl = dynamic_cast<CTextLabel*>(label);
	if (tl) {
		char* text = strfmt("wow! text: %s", newc);
		tl->SetText(text);
		delete [] text;
	}
}

RootWindow::RootWindow() : CBaseWindow(0, "root") {
	g_pGUI->m_pTooltip = new CTooltip(this, "tooltip");
	this->AddChild(g_pGUI->m_pTooltip);
	CBaseWindow* ws = new CBaseWindow(this, "splitwindow");
	ws->SetPositionMode(ABSOLUTE);
	TitleBar* wst = new TitleBar(ws, "Window Layout Test");
	ws->AddChild(wst);
	ws->SetMaxSize(500, 0);
	//ws->SetMaxSize(500, 300);
	CSplitContainer* sc1 = new CSplitContainer(ws, "sc1");
	sc1->m_nSizeX = 480;
	sc1->m_nMaxX = 480;
	//sc1->SetMaxSize(480, -1);
	sc1->AddChild(new CTextLabel(sc1, "tl1", ":thinking:"));
	CBaseButton* ccb1 = new CBaseButton(sc1, "b1");
	ccb1->SetText("Ayy Lmao");
	CSlider* sl = new CSlider(ws, "sl");
	sl->GetKeyValues()->SetString("cvar", "cat_fov");
	sl->Setup(10.0f, 150.0f);
	sl->SetCallback([](CSlider* slider, float newv, float oldv) {
		interfaces::cvar->FindVar(slider->GetKeyValues()->GetString("cvar"))->SetValue(newv);
	});
	sc1->AddChild(ccb1);
//sc1->AddChild(new CTextLabel(sc1, "tl3", "wow"));
	ws->AddChild(sc1);
	CSplitContainer* sc2 = new CSplitContainer(ws, "sc2");
	sc2->m_nSizeX = 480;
	sc2->m_nMaxX = 480;
	sc2->AddChild(new CTextLabel(sc2, "tl1", "1"));
	sc2->AddChild(new CTextLabel(sc2, "tl2", "2"));
	sc2->AddChild(new CTextLabel(sc2, "tl3", "3"));
	sc2->AddChild(new CTextLabel(sc2, "tl4", "4"));
	ws->AddChild(sc2);
	CSplitContainer* sc3 = new CSplitContainer(ws, "sc3");
	sc3->m_nSizeX = 480;
		sc3->m_nMaxX = 480;
	sc3->AddChild(new CTextLabel(sc3, "tl1", "ayy"));
	sc3->AddChild(new CTextLabel(sc3, "tl2", "lmao"));
	ws->AddChild(sc3);
	AddChild(ws);
	ws->AddChild(sl);

	CBaseWindow* wgt = new CBaseWindow(this, "testwindow");
	IWidget* title = new TitleBar(wgt, "Test Window");
	wgt->SetPositionMode(ABSOLUTE);
	wgt->AddChild(title);
	wgt->SetMaxSize(0, 100);
	this->m_nSizeX = draw::width;
	this->m_nSizeY = draw::height;
	/*CCheckbox* cb1 = new CCheckbox(wgt, "cb1");
	CTextLabel* t1 = new CTextLabel(wgt, "t1", "checkbox");
	CCheckbox* cb2 = new CCheckbox(wgt, "cb2");
	CTextLabel* t2 = new CTextLabel(wgt, "t2", "wow!!");
	CCheckbox* cb3 = new CCheckbox(wgt, "cb3");
	CTextLabel* t3 = new CTextLabel(wgt, "t3", "cool");
	cb1->SetPositionMode(PositionMode::INLINE_BLOCK);
	cb2->SetPositionMode(PositionMode::INLINE_BLOCK);
	cb3->SetPositionMode(PositionMode::INLINE_BLOCK);
	t1->SetPositionMode(PositionMode::INLINE);
	t2->SetPositionMode(PositionMode::INLINE);
	t3->SetPositionMode(PositionMode::INLINE);
	wgt->AddChild(cb1);
	wgt->AddChild(t1);
	wgt->AddChild(cb2);
	wgt->AddChild(t2);
	wgt->AddChild(cb3);
	wgt->AddChild(t3);*/
	CBaseButton* b1 = new CBaseButton(wgt, "b1");
	b1->SetText("Press me!");
	b1->SetCallback(B1Callback);
	b1->SetPositionMode(INLINE_BLOCK);
	b1->GetKeyValues()->SetInt("test_value", 1337);
	wgt->AddChild(b1);
	wgt->SetOffset(200, 200);
	CTextLabel* td = new CTextLabel(wgt, "td", "");
	CTextInput* ti = new CTextInput(wgt, "ti");
	ti->SetMaxWidth(16);
	ti->SetPositionMode(INLINE_BLOCK);
	ti->SetCallback(TICallback);
	wgt->AddChild(ti);
	wgt->AddChild(td);
	td->SetPositionMode(INLINE_BLOCK);
	this->AddChild(wgt);
}

RootWindow::~RootWindow() {

}
