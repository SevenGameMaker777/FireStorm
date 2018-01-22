// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "defines.h"

namespace FS
{
	class FSGUI_Titlebar : public FSGUI_Window
	{
	private:
		IGUIStaticText* m_Textbox;
		FS_VARIABLE_SETGET(int, XOffset);
	public:
		FSGUI_Titlebar(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, wchar_t* text) :
			FSGUI_Window(desktop, gui, parent, id, rect<s32>(0, 0, 100, TITLEBAR_HEIGHT), WS_VISIBLE | WS_DLGFRAME, LS_TOPFILL, text, 10, TITLEBAR_HEIGHT)
		{
			m_XOffset = 0;
			m_Textbox = gui->addStaticText(text, rect<s32>(0, 0, 10, 10), true, false, this, -1, false);
			FS_LOG(FSL_DEBUG, "FSGUI_Titlebar::FSGUI_Titlebar()");
		}

		virtual void setText(const wchar_t* text)
		{
			FS_LOG(FSL_DEBUG, "FSGUI_Titlebar::setText()");
			IGUIElement::setText(text);
			if (m_Textbox) m_Textbox->setText(text);
		}

		virtual void calculateLayout()
		{
			FSGUI_Window::calculateLayout();

			if (m_Textbox)
			{
				rect<s32> r = normalizeToSelf(getClientRect());
				r.UpperLeftCorner += 3;
				r.LowerRightCorner -= 3;
				r.UpperLeftCorner.X += getXOffset();
				m_Textbox->setRelativePosition(r);
				m_Textbox->updateAbsolutePosition();
			}
		}

		virtual void drawClientArea()
		{
			IGUISkin* skin = getGui()->getSkin();
			SColor s = skin->getColor(EGDC_3D_DARK_SHADOW);
			if ((getDesktop()->getFocus() == this) || (getDesktop()->getFocus() == getParent())) s = skin->getColor(EGDC_3D_HIGH_LIGHT);
			rect<s32> r = getClientRect();
			getDesktop()->getDriver()->draw2DRectangle(s, r, 0);
		}

		virtual bool onButtonClicked(const SEvent & e)
		{
			switch (e.GUIEvent.Caller->getID())
			{
			case FSID_MINIMIZE: return false; break;
			}
			return FSEventHandler::onButtonClicked(e);
		}
	};

} // end namespace