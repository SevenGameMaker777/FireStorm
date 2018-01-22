#pragma once

#include "FSGUI_Window.h"
#include "FSLevel.h"
#include "FSApplication.h"
#include "FSEngine.h"
#include "FSSceneNodeAnimatorFPS.h"
#include "FSGUI_Desktop.h"

namespace FS
{
	class FSGUI_LevelViewer : public FSGUI_Window
	{
	public:
		FSLevel* m_Level;
		FSLevel* getLevel() { return m_Level; }


		FSGUI_LevelViewer(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minwidth, int minheight, FSLevel* level) :
			FSGUI_Window(desktop, gui, parent, id, r, flags, ls, text, minwidth, minheight)
		{
			setLevel(level);
		}

		virtual ~FSGUI_LevelViewer()
		{
			m_Level = 0;
		}

		virtual bool setLevel(FSLevel* level)
		{
			m_Level=0;
			m_Level = level;
			return m_Level !=0;
		}

		// rendering functions
		virtual void drawClientArea()
		{
			if ((getClientRect().getWidth() > 20) && (getClientRect().getHeight() > 20))
			{
				if ((getLevel()) && (getLevel()->getCamera()))
				{
					const rect<s32> r = getClientRect();
					rect<s32> oldViewport = getDesktop()->getDevice()->getVideoDriver()->getViewPort();
					getLevel()->getCamera()->setAspectRatio((f32)getClientRect().getWidth() / (f32)getClientRect().getHeight());
					getLevel()->getSmgr()->setActiveCamera(getLevel()->getCamera());
					getDesktop()->getDevice()->getVideoDriver()->setViewPort(r);
//					if (getLevel()->getEngine()->getRenderer()) getLevel()->getEngine()->getRenderer()->getLightManager()->setRenderWindow(r);
					if (m_Level) m_Level->render();
					getDesktop()->getDevice()->getVideoDriver()->setViewPort(oldViewport);
				}
				else
				{
					FSGUI_Window::drawClientArea();
				}
			}
		}

	};
} // end namespace
