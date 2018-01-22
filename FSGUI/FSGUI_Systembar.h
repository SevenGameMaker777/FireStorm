// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_Toolbar.h"
#include "FSGUI_Titlebar.h"

namespace FS
{
	class FSGUI_Systembar : public FSGUI_Toolbar
	{
	private:
	public:
		FSGUI_Systembar(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, wchar_t* text) :
			FSGUI_Toolbar(desktop, gui, parent, id, rect<s32>(0, 0, 30, SYSTEMBAR_HEIGHT), LS_RIGHTFILL, false,2)
		{
			addElement(gui->addButton(rect<s32>(0, 0, 20, SYSTEMBAR_HEIGHT), this, FSID_MINIMIZE, L"-"));
		}

		virtual bool onButtonClicked(const SEvent & e)
		{
			switch (e.GUIEvent.Caller->getID())
			{
				case FSID_MINIMIZE: return false; break; // do not allow the user to minimize this window
			}
			return FSEventHandler::onButtonClicked(e);
		}

	};

} // end namespace