// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_Toolbar.h"
#include "FSGUI_Titlebar.h"

#define SCROLLBAR_WIDTH		20
#define SCROLLBAR_HEIGHT	20

namespace FS
{
	class FSGUI_Scrollbar : public FSGUI_Window
	{
	private:
		FS_VARIABLE_GET(IGUIScrollBar*, Scrollbar);
		FS_VARIABLE_GET(bool, Vertical);
	public:
		FSGUI_Scrollbar(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, bool vertical) :
			FSGUI_Window(desktop, gui, parent, id, rect<s32>(0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT), WS_VISIBLE,
			(vertical == true) ? LS_RIGHTFILL : LS_BOTTOMFILL, L"", SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT)
		{
			m_Vertical = vertical;
			m_Scrollbar = gui->addScrollBar(!vertical, rect<s32>(0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT), this, id);
		}

		virtual void calculateLayout()
		{
			FSGUI_Window::calculateLayout();

			if (m_Scrollbar)
			{
				rect<s32> r = normalizeToSelf(getClientRect());
				if (m_Vertical)
					r.UpperLeftCorner.X = r.LowerRightCorner.X - SCROLLBAR_WIDTH;
				else
					r.UpperLeftCorner.Y = r.LowerRightCorner.Y - SCROLLBAR_HEIGHT;
				m_Scrollbar->setRelativePosition(r);
				m_Scrollbar->updateAbsolutePosition();
			}
		}
	};

} // end namespace