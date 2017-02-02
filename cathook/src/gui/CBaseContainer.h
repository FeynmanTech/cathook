/*
 * CBaseContainer.h
 *
 *  Created on: Jan 30, 2017
 *      Author: nullifiedcat
 */

#ifndef CBASECONTAINER_H_
#define CBASECONTAINER_H_

#include "CBaseWidget.h"

class CBaseContainer : public CBaseWidget, public virtual IWidget {
public:
	CBaseContainer(std::string name = "unnamed", IWidget* parent = nullptr);
	virtual ~CBaseContainer();

	void AddChild(IWidget* child);
	int ChildCount();
	IWidget* ChildByIndex(int idx);
	IWidget* ChildByName(std::string name);
	IWidget* ChildByPoint(int x, int y);

	virtual bool ConsumesKey(ButtonCode_t key);
	virtual void Draw(int x, int y);
	virtual void DrawBounds(int x, int y);
	virtual void Hide();
	virtual void OnFocusLose();
	virtual void OnKeyPress(ButtonCode_t key, bool repeat);
	virtual void OnKeyRelease(ButtonCode_t key);
	virtual void OnMouseLeave();
	virtual void OnMousePress();
	virtual void OnMouseRelease();
	virtual void Update();

	virtual void SortByZIndex();
	void UpdateHovers();

	virtual void MoveChildren();

	IWidget* GetHoveredChild();
	IWidget* GetFocusedChild();
	IWidget* GetPressedChild();
	void HoverOn(IWidget* child);
	void FocusOn(IWidget* child);
	void PressOn(IWidget* child);

	IWidget* m_pHoveredChild;
	IWidget* m_pFocusedChild;
	IWidget* m_pPressedChild;

	std::vector<IWidget*> m_children;
};

#endif /* CBASECONTAINER_H_ */
