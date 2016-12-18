/*
 * controls.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "../cvwrapper.h"
#include "../sdk.h"

#define VERTICAL_SPACING 14
#define LIST_WIDTH 200

class IGUIListElement;

class GUI_List {
public:
	GUI_List(const char* id, const char* title);
	void Draw();
	void Move(int x, int y);
	void AddElement(IGUIListElement* element);
	void KeyEvent(ButtonCode_t key);
	void SelectNext();
	void SelectPrev();

	int x, y;
	IGUIListElement* m_pSelected;
	IGUIListElement* m_pFirst;
	IGUIListElement* m_pLast;
	int m_nElementCount;
	const char* m_pszListID;
	const char* m_pszListTitle;
};

class IGUIListElement {
public:
	virtual ~IGUIListElement();
	virtual void Draw(int x, int y, bool selected) = 0;
	virtual void KeyEvent(ButtonCode_t key) = 0;
	IGUIListElement* m_pNext;
	IGUIListElement* m_pPrev;
	int m_nIndex;
	GUI_List* m_pParentList;
};

class GUIListElement_Var : public IGUIListElement {
public:
	GUIListElement_Var(CatVar* var);
	void Draw(int x, int y, bool selected);
	void KeyEvent(ButtonCode_t key);

	CatVar* m_pCatVar;
};

class GUIListElement_SubList : public IGUIListElement {
public:
	GUIListElement_SubList(GUI_List* list);
	void Draw(int x, int y, bool selected);
	void KeyEvent(ButtonCode_t key);

	GUI_List* m_pList;
};

#endif /* CONTROLS_H_ */
