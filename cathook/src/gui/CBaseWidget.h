/*
 * CBaseWidget.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef CBASEWIDGET_H_
#define CBASEWIDGET_H_

#include "IWidget.h"

#include <KeyValues.h>

class CBaseWidget : public virtual IWidget {
public:
	inline ~CBaseWidget() {
		delete [] m_pszName;
	}

	KeyValues::AutoDelete m_pKeyValues;

	inline KeyValues* GetKeyValues() {
		return m_pKeyValues;
	}

	inline CBaseWidget(IWidget* parent, const char* name) : m_pKeyValues("widgetkv") {
		m_pParentWidget = parent;
		m_bMouseInside = false;
		m_bMousePressed = false;
		m_nOffsetX = 0;
		m_nOffsetY = 0;
		m_nSizeX = 0;
		m_nSizeY = 0;
		m_nChildCount = 0;
		m_pszName = new char[128];
		m_pChildrenList = 0;
		strncpy((char*)m_pszName, name, 127);
		m_iMode = PositionMode::INLINE_BLOCK;
		m_bVisible = true;
		m_nMaxX = 0;
		m_nMaxY = 0;
		m_bFocused = false;
	}


	inline virtual bool ConsumesKey(ButtonCode_t key) { return false; }

	inline virtual void SetMaxSize(int w, int h) {
		if (w >= 0)
			m_nMaxX = w;
		if (h >= 0)
			m_nMaxY = h;
	}

	inline virtual void OnFocusGain() { m_bFocused = true; }
	inline virtual void OnFocusLose() { m_bFocused = false; }
	inline virtual bool ShouldResizeToFit() { return true; }
	virtual void DrawBounds();
	inline virtual void Update() {};
	inline virtual void OnMouseEnter() { m_bMouseInside = true; };
	inline virtual void OnMouseLeave() { m_bMouseInside = false; };
	inline virtual void OnMousePress() { m_bMousePressed = true; };
	inline virtual void OnMouseRelease() { m_bMousePressed = false; };
	inline virtual void OnKeyPress(ButtonCode_t key) {};
	inline virtual void OnKeyRelease(ButtonCode_t key) {};

	inline virtual void SetOffset(int x, int y) {
		m_nOffsetX = x;
		m_nOffsetY = y;
	}
	inline virtual void GetOffset(int& x, int& y) {
		x = m_nOffsetX;
		y = m_nOffsetY;
	}
	inline virtual void Draw() {};
	inline virtual void GetSize(int& width, int& height) {
		width = m_nSizeX;
		height = m_nSizeY;
	}

	inline virtual void GetAbsolutePosition(int& x, int &y) {
		int ox = 0;
		int oy = 0;
		GetOffset(ox, oy);
		IWidget* parent = GetParent();
		while (parent) {
			int dx, dy;
			parent->GetOffset(dx, dy);
			ox += dx;
			oy += dy;
			parent = parent->GetParent();
		}
		x = ox;
		y = oy;
	}

	inline virtual IWidget* GetParent() {
		return m_pParentWidget;
	}

	inline virtual void SetPositionMode(PositionMode mode) {
		m_iMode = mode;
	}

	inline PositionMode GetPositionMode() {
		return m_iMode;
	}

	inline virtual int GetChildrenCount() {
		return m_nChildCount;
	}

	inline virtual void Show() {
		m_bVisible = true;
	}

	inline virtual void Hide() {
		m_bVisible = false;
		OnFocusLose();
	}

	inline virtual bool IsVisible() {
		return m_bVisible;
	}

	virtual IWidget* GetChildByIndex(int idx) {
		if (idx < 0 || idx >= m_nChildCount) return 0;
		return m_pChildrenList[idx];
	}

	virtual IWidget* GetChildByName(const char* name) {
		for (int i = 0; i < m_nChildCount; i++) {
			if (!strcmp(name, m_pChildrenList[i]->GetName())) return m_pChildrenList[i];
		}
		return 0;
	}

	virtual IWidget* GetChildByPoint(int x, int y) {
		for (int i = m_nChildCount - 1; i >= 0; i--) {
			IWidget* child = m_pChildrenList[i];
			int ox, oy;
			child->GetOffset(ox, oy);
			int sx, sy;
			child->GetSize(sx, sy);
			if (x >= ox && x <= (ox + sx) && y >= oy && y <= (oy + sy)) {
				return child;
			}
		}
		return 0;
	}

	virtual void AddChild(IWidget* child) {
		m_pChildrenList[m_nChildCount] = child;
		m_nChildCount++;
	}

	virtual const char* GetName() {
		return m_pszName;
	}

	const char* m_pszName;
	int m_nChildCount;
	bool m_bMouseInside;
	bool m_bMousePressed;
	PositionMode m_iMode;
	int m_nSizeX;
	int m_nSizeY;
	bool m_bVisible;
	int m_nOffsetX;
	int m_nOffsetY;
	int m_nMaxX;
	int m_nMaxY;
	bool m_bFocused;
	IWidget** m_pChildrenList;
	IWidget* m_pParentWidget;

};


#endif /* CBASEWIDGET_H_ */
