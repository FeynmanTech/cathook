/*
 * IWidget.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef IWIDGET_H_
#define IWIDGET_H_

#include "../fixsdk.h"
#include "../inputsystem/ButtonCode.h"

enum PositionMode {
	ABSOLUTE = 0,
	INLINE,
	INLINE_BLOCK,
	FLOATING
};

class IWidget {
public:
		virtual ~IWidget();

		virtual void Update() = 0;

		virtual void OnMouseEnter() = 0;
		virtual void OnMouseLeave() = 0;
		virtual void OnMousePress() = 0;
		virtual void OnMouseRelease() = 0;
		virtual void OnKeyPress(ButtonCode_t key) = 0;
		virtual void OnKeyRelease(ButtonCode_t key) = 0;

		virtual void OnFocusGain() = 0;
		virtual void OnFocusLose() = 0;

		virtual void Show() = 0;
		virtual void Hide() = 0;
		virtual bool IsVisible() = 0;

		virtual void DrawBounds() = 0;
		virtual void SetOffset(int x, int y) = 0;
		virtual void GetOffset(int& x, int& y) = 0;
		virtual void GetAbsolutePosition(int& x, int& y) = 0;
		virtual void Draw() = 0;
		virtual void GetSize(int& width, int& height) = 0;
		virtual void SetMaxSize(int width, int height) = 0;
		virtual bool ShouldResizeToFit() = 0;

		virtual PositionMode GetPositionMode() = 0;
		virtual void SetPositionMode(PositionMode) = 0;

		virtual IWidget* GetParent() = 0;

		virtual const char* GetName() = 0;

		virtual int GetChildrenCount() = 0;
		virtual IWidget* GetChildByIndex(int idx) = 0;
		virtual IWidget* GetChildByName(const char* name) = 0;
		virtual IWidget* GetChildByPoint(int x, int y) = 0;
		virtual void AddChild(IWidget* child) = 0;
};


#endif /* IWIDGET_H_ */
