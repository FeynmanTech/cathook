/*
 * RootWindow.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "RootWindow.h"
#include "CTextLabel.h"
#include "CCheckbox.h"
#include "CBaseButton.h"
#include "CTextInput.h"
#include "CSplitContainer.h"
#include "CSlider.h"
#include "CTooltip.h"
#include "CBaseContainer.h"
#include "CDropdown.h"

#include "../common.h"
#include "CTitleBar.h"

void B1Callback(CBaseButton* thisptr) {
	CBaseContainer* container = dynamic_cast<CBaseContainer*>(thisptr->GetParent());
	CTextLabel* label = dynamic_cast<CTextLabel*>(container->ChildByName("td"));
	if (label) {
		char* text = strfmt("wow! this[\"%s\"] = %i", "test_value", thisptr->Props()->GetInt("test_value"));
		label->SetText(text);
		delete [] text;
	}
}

void TICallback(CTextInput* thisptr, std::string olds, std::string news) {
	CBaseContainer* container = dynamic_cast<CBaseContainer*>(thisptr->GetParent());
	CTextLabel* tl = dynamic_cast<CTextLabel*>(container->ChildByName("td"));
	if (tl) {
		char* text = strfmt("wow! text: %s", news.c_str());
		tl->SetText(text);
		delete [] text;
	}
}

RootWindow::RootWindow() : CBaseWindow("root") {

}

void RootWindow::Setup() {
	g_pGUI->m_pTooltip = new CTooltip();
	AddChild(g_pGUI->m_pTooltip);
	CBaseWindow* ws = new CBaseWindow("splitwindow");
	ws->SetPositionMode(ABSOLUTE);
	TitleBar* wst = new TitleBar(ws, "Window Layout Test");
	ws->AddChild(wst);
	ws->SetMaxSize(500, 0);
	//ws->SetMaxSize(500, 300);
	CSplitContainer* sc1 = new CSplitContainer("sc1", ws);
	ws->AddChild(sc1);
	sc1->SetMaxSize(480, -1);
	sc1->SetSize(480, -1);
	sc1->SetMaxSize(480, -1);
	sc1->AddChild(new CTextLabel("tl1", sc1, ":thinking:"));
	CBaseButton* ccb1 = new CBaseButton("b1", sc1);
	ccb1->SetText("nut");
	CSlider* sl = new CSlider("sl", ws);
	sl->Props()->SetString("cvar", "cat_fov");
	sl->Setup(10.0f, 150.0f);
	sl->SetValue(13.37f);
	sl->SetCallback([](CSlider* slider, float oldv, float newv) {
		interfaces::cvar->FindVar(slider->Props()->GetString("cvar"))->SetValue(newv);
	});
	sc1->AddChild(ccb1);
//sc1->AddChild(new CTextLabel(sc1, "tl3", "wow"));
	//ws->AddChild(sc1);
	CSplitContainer* sc2 = new CSplitContainer("sc2", ws);
	sc2->SetMaxSize(480, -1);
	sc2->SetSize(480, -1);
	sc2->AddChild(new CTextLabel("tl1", sc2, "1"));
	sc2->AddChild(new CTextLabel("tl2", sc2, "2"));
	sc2->AddChild(new CTextLabel("tl3", sc2, "3"));
	sc2->AddChild(new CTextLabel("tl4", sc2, "4"));
	ws->AddChild(sc2);
	CSplitContainer* sc3 = new CSplitContainer("sc3", ws);
	sc3->SetMaxSize(480, -1);
	sc3->SetSize(480, -1);
	sc3->AddChild(new CTextLabel("tl1", sc3, ":ok_hand:"));
	sc3->AddChild(new CTextLabel("tl2", sc3, ":skin-tone-1:"));
	CDropdown* dr = new CDropdown("dr1", sc3);
	dr->AddValue("testing");
	dr->AddValue("dropdown?!");
	dr->AddValue("wow!");
	sc3->AddChild(dr);
	ws->AddChild(sc3);
	AddChild(ws);
	ws->AddChild(sl);

	CBaseWindow* wgt = new CBaseWindow("testwindow", this);
	IWidget* title = new TitleBar(wgt, "Test Window");
	wgt->SetPositionMode(ABSOLUTE);
	wgt->AddChild(title);
	wgt->SetMaxSize(0, 100);
	this->SetSize(draw::width, draw::height);
	CBaseButton* b1 = new CBaseButton("b1", wgt);
	b1->SetText("Press me!");
	b1->SetCallback(B1Callback);
	b1->SetPositionMode(INLINE);
	b1->Props()->SetInt("test_value", 1337);
	wgt->AddChild(b1);
	wgt->SetOffset(200, 200);
	CTextLabel* td = new CTextLabel("td", wgt, "");
	CTextInput* ti = new CTextInput("ti", wgt);
	ti->SetMaxWidth(16);
	ti->SetPositionMode(INLINE);
	ti->SetCallback(TICallback);
	wgt->AddChild(ti);
	wgt->AddChild(td);
	td->SetPositionMode(INLINE);
	this->AddChild(wgt);
}

RootWindow::~RootWindow() {

}
