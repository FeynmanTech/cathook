/*
 * controls.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef CONTROLS_H_
#define CONTROLS_H_

#define VERTICAL_SPACING 14
#define LIST_WIDTH 100

class GUI_List {
public:
	GUI_List(const char* name);
	void Draw(int x, int y);
	void AddElement(IGUIListElement* element);

	IGUIListElement* m_pFirst;
	IGUIListElement* m_pLast;
	const char* m_pszListName;
};

class IGUIListElement {
public:
	virtual ~IGUIListElement();
	virtual void Draw(int x, int y) = 0;
	virtual void KeyEvent(ButtonCode_t key) = 0;
	IGUIListElement* m_pNext;
	IGUIListElement* m_pPrev;
};

class GUIListElement_Var : public IGUIListElement {
public:
	GUIListElement_Var(ICatVar* var);
	void Draw(int x, int y);
	void KeyEvent(ButtonCode_t key);

	ICatVar* m_pCatVar;
};

#endif /* CONTROLS_H_ */
