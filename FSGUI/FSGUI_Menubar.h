// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_Toolbar.h"
#include "FSGUI_Titlebar.h"
#include "FSGUI_Menu.h"
#include "defines.h"


namespace FS
{
	class FSGUI_Menubar : public FSGUI_Toolbar
	{
	private:
		FSGUI_Menu* m_Menu;
	public:
		FSGUI_Menu* getMenu() { return m_Menu; }
	public:
		FSGUI_Menubar(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, wchar_t* text) :
			FSGUI_Toolbar(desktop, gui, parent, id, rect<s32>(0, 0, 32, MENUBAR_HEIGHT), LS_TOPFILL, false, 2)
		{
			m_Menu = new FSGUI_Menu(desktop->getDevice(), gui, this, 22, rect<s32>(0, 0, 30, 30));
			addElement(m_Menu);
			getDesktop()->addToRenderLastList(m_Menu);
		}

		virtual void drawClientArea()
		{
			IGUISkin* skin = getGui()->getSkin();
			SColor s = skin->getColor(EGDC_3D_DARK_SHADOW);
			if ((getDesktop()->getFocus() == this) || (getDesktop()->getFocus() == getParent())) s = skin->getColor(EGDC_3D_HIGH_LIGHT);
			rect<s32> r = getClientRect();
			getDesktop()->getDriver()->draw2DRectangle(s, r, 0);
		}
	};

} // end namespace