// incldue the needed header files
#include "FSGUI_Desktop.h"
#include "FSApplication.h"
#include "FSEngine.h"
#include "FSGUI_Titlebar.h"

#define DOCKING_RANGE 20

// unclutter the global namespace
namespace FS
{
	// unsafe object pointers
	IrrlichtDevice*		FSGUI_Desktop::getDevice() { return getApplication()->getDevice(); }
	IGUIEnvironment*	FSGUI_Desktop::getGui() { return getApplication()->getGui(); }
	ISceneManager*		FSGUI_Desktop::getSmgr() { return getApplication()->getSmgr(); }
	IVideoDriver*		FSGUI_Desktop::getDriver() { return getApplication()->getDriver(); }
	ICursorControl*		FSGUI_Desktop::getCursorControl() { return getDevice()->getCursorControl(); }

	// set all variables to a known value
	void FSGUI_Desktop::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSGUI_Desktop::initialize()");

		// initialize all variables
		m_Application = 0;
		m_Focus = 0;
		m_Sizing = 0;
		m_SizeDir = MP_NONE;
		m_Docking = 0;
		m_DesktopWindow = 0;
		m_RenderLastList.clear();
		m_TempRenderLast = 0;
		m_DragDropInfo = 0;
		m_Dragging = 0;
		m_UseRenderLast = true;
	}

	// allows for better error handling
	bool FSGUI_Desktop::create(FSApplication* app)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSGUI_Desktop::create()");

		// remember this
		m_Application = app;

		// create the main window
		m_DesktopWindow = new FSGUI_Window(this, getGui(), 0, ID_DESKTOP, getApplication()->getEngine()->getScreenRect(), WS_VISIBLE | WS_EX_TRANSPARENT, LS_REST, L"Desktop", 0, 0);

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made
	bool FSGUI_Desktop::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSGUI_Desktop::cleanup()");

		setDragDropInfo(0);

		if (!m_RenderLastList.empty())
		{
			list<IGUIElement*>::Iterator it;
			for (it = m_RenderLastList.begin(); it != m_RenderLastList.end();)
			{
				delete((*it));
				it = m_RenderLastList.erase(it);
			}
		}
		m_RenderLastList.clear();

		// forget this
		m_Application = 0;
		m_Focus = 0;
		m_Sizing = 0;
		m_Docking = 0;

		// always return false from a cleanup function
		return false;
	}

	int FSGUI_Desktop::getBorderSize(long flags)
	{
		if (flags & WS_THICKFRAME) return 4;
		if (flags & WS_BORDER) return 2;
		return 0;
	}

	void FSGUI_Desktop::draw()
	{
		IGUISkin* skin = getGui()->getSkin();

#if(0)
		if (getFocus() && (!getFocus()->isFlagged(WS_EX_TRANSPARENT)) )
		{
			getDevice()->setWindowCaption(stringw(getFocus()->getID()).c_str());
			getDriver()->draw2DRectangle(skin->getColor(EGDC_3D_FACE), getFocus()->getAbsolutePosition());
		}
#endif

		if (getUseRenderLast())
		{
			if (!m_RenderLastList.empty())
			{
				list<IGUIElement*>::Iterator it;
				for (it = m_RenderLastList.begin(); it != m_RenderLastList.end(); it++)
				{
					if (((*it)->isVisible()) && ((*it)->getParent()->isVisible())) (*it)->draw();
				}
			}
			if (getTempRenderLast())
			{
				if (((getTempRenderLast())->isVisible()) && ((getTempRenderLast())->getParent()->isVisible()))
					getTempRenderLast()->draw();
			}
		}

	}

	void FSGUI_Desktop::addToRenderLastList(IGUIElement* w)
	{
		m_RenderLastList.push_back(w);
	}

	FSGUI_Window* FSGUI_Desktop::getFSGUI_WindowAt(int x, int y)
	{
		if (!m_DesktopWindow) return NULL;
		if (m_DesktopWindow->m_FSGUIChildren.empty()) return NULL;

		return m_DesktopWindow->getFSGUI_WindowAt(x, y);
	}

	bool FSGUI_Desktop::OnEvent(const SEvent &e)
	{
		if (getDesktopWindow()->isVisible()) return FSEventHandler::OnEvent(e);
		return false;
	}

	bool FSGUI_Desktop::onScreenResized(const SEvent & e)
	{
		if (getDesktopWindow())
		{
			rect<s32> r = getApplication()->getEngine()->getScreenRect();
			getDesktopWindow()->setRelativePosition(r);
			getDesktopWindow()->updateAbsolutePosition();
			getDesktopWindow()->calculateLayout();
		}
		return false;
	}

	bool FSGUI_Desktop::onMouseMoved(const SEvent & e)
	{
		if (getGui()->getFocus())
		{
			if (getGui()->getFocus()->getType() == EGUIET_CONTEXT_MENU) return false;
			if (getGui()->getFocus()->getType() == EGUIET_MENU) return false;
		}

		if (getDragging()) return false;

		getCursorControl()->setActiveIcon(ECI_NORMAL);

		if (getSizing())
		{
			if (getFocus() && getFocus() != getDesktopWindow())
			{
				FSGUI_Window* FSParent = dynamic_cast<FSGUI_Window*>(getFocus()->getParent());
				rect<s32> r = getFocus()->getAbsolutePosition();
				switch (getSizeDir())
				{
					case MP_CLIENT: setSizing(false); break;
					case MP_NONE: setSizing(false); break;
					case MP_TOP: getCursorControl()->setActiveIcon(ECI_SIZENS); FSParent->OnSizeFSGUIChild(getFocus(), rect<s32>(r.UpperLeftCorner.X, e.MouseInput.Y, r.LowerRightCorner.X, r.LowerRightCorner.Y)); break;
					case MP_BOTTOM: getCursorControl()->setActiveIcon(ECI_SIZENS); FSParent->OnSizeFSGUIChild(getFocus(), rect<s32>(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, r.LowerRightCorner.X, e.MouseInput.Y)); break;
					case MP_LEFT: getCursorControl()->setActiveIcon(ECI_SIZEWE); FSParent->OnSizeFSGUIChild(getFocus(), rect<s32>(e.MouseInput.X, r.UpperLeftCorner.Y, r.LowerRightCorner.X, r.LowerRightCorner.Y)); break;
					case MP_RIGHT: getCursorControl()->setActiveIcon(ECI_SIZEWE); FSParent->OnSizeFSGUIChild(getFocus(), rect<s32>(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, e.MouseInput.X, r.LowerRightCorner.Y)); break;
				}
			}
			return true;
		}
		else
			if (getDocking())
			{
				if (!getFocus())
				{
					setDocking(false);
					return false;
				}

				if (getFocus())
				{
					FSGUI_LAYOUTSTYLE ls = LS_NONE;
					rect<s32> r = getFocus()->getAbsolutePosition();
					r.LowerRightCorner -= r.UpperLeftCorner;
					r.UpperLeftCorner -= r.UpperLeftCorner;
					r.UpperLeftCorner.X = e.MouseInput.X;
					r.UpperLeftCorner.Y = e.MouseInput.Y;

					rect<s32> rr = getFocus()->getFSGUIParent()->getClientRect();
					if (e.MouseInput.X < rr.UpperLeftCorner.X + DOCKING_RANGE)
					{
						ls = LS_LEFTFILL;
						getFocus()->setLayoutStyle(ls);
					}
					else
					if (e.MouseInput.X > rr.LowerRightCorner.X - DOCKING_RANGE - getDockingWidth(getFocus(), LS_RIGHTFILL))
					{
						ls = LS_RIGHTFILL;
						getFocus()->setLayoutStyle(ls);
					}
					else
					if (e.MouseInput.Y < rr.UpperLeftCorner.Y + DOCKING_RANGE)
					{
						ls = LS_TOPFILL;
						getFocus()->setLayoutStyle(ls);
					}
					else
					if (e.MouseInput.Y > rr.LowerRightCorner.Y - DOCKING_RANGE - getDockingHeight(getFocus(), LS_BOTTOMFILL))
					{
						ls = LS_BOTTOMFILL;
						getFocus()->setLayoutStyle(ls);
					}
					else
					{
						r.LowerRightCorner.X = r.UpperLeftCorner.X + getDockingWidth(getFocus(), LS_NONE);
						r.LowerRightCorner.Y = r.UpperLeftCorner.Y + getDockingHeight(getFocus(), LS_NONE);

						//r.LowerRightCorner.X = r.UpperLeftCorner.X + getFocus()->getAbsolutePosition().getWidth();
						//r.LowerRightCorner.Y = r.UpperLeftCorner.Y + getFocus()->getAbsolutePosition().getHeight();
						getFocus()->setLayoutStyle(ls);
					}
					getFocus()->getFSGUIParent()->OnSizeFSGUIChild(getFocus(), r, true);
					return true;
				}
			}
			else
				if (e.MouseInput.isLeftPressed())
				{
					// see if we have selected a titlebar type window
					FSGUI_Titlebar* tb = dynamic_cast<FSGUI_Titlebar*>(getFocus());
					if (tb && tb->getFSGUIParent() && tb->getFSGUIParent()->getDockable())
					{
						beginDocking();
						return true;
					}
					else
					{
						setSizing(false);
						setDocking(false);
						setSizeDir(MP_NONE);
						return true;
					}
				}
				else
				{
					setFocus(getFSGUI_WindowAt(e.MouseInput.X, e.MouseInput.Y));
					if (getFocus())
					{
						FSGUI_SIZEDIR sd = getFocus()->mousePosition(e.MouseInput.X, e.MouseInput.Y);
						if (getFocus()->canResize(sd))
						switch (sd)
							{
								case MP_NONE: getCursorControl()->setActiveIcon(ECI_NORMAL); break;
								case MP_LEFT: getCursorControl()->setActiveIcon(ECI_SIZEWE); break;
								case MP_RIGHT: getCursorControl()->setActiveIcon(ECI_SIZEWE); break;
								case MP_TOP: getCursorControl()->setActiveIcon(ECI_SIZENS); break;
								case MP_BOTTOM: getCursorControl()->setActiveIcon(ECI_SIZENS); break;
							}
						}
						return true;
					}
		return false;
	}

	bool FSGUI_Desktop::onLMouseButtonUp(const SEvent & e)
	{
		if (m_Dragging) endDragging(e.MouseInput.X, e.MouseInput.Y);
		setSizing(false);
		endDocking();
		setDocking(false);
		setFocus(0);
		return false;
	}

	bool FSGUI_Desktop::onLMouseButtonDown(const SEvent & e)
	{
		int x = e.MouseInput.X;
		int y = e.MouseInput.Y;
		setFocus(getFSGUI_WindowAt(x, y));
		if (getFocus())
		{
			FSGUI_SIZEDIR sd = getFocus()->mousePosition(x, y);

			if (getFocus()->canResize(sd))
			{
				switch (sd)
				{
				case MP_NONE: break;
				case MP_LEFT:
				case MP_RIGHT:
				case MP_TOP:
				case MP_BOTTOM:
					setSizing(true);
					setSizeDir(sd);
					return true;
					break;
				}
			}
			else
				if (getFocus()->getCanDragDrop())
				{
					beginDragging(getFocus()->getDragDropInfo());
				}
			/*
			else
			{
			// see if we have selected a titlebar type window
			FSGUI_Titlebar* tb = dynamic_cast<FSGUI_Titlebar*>(getFocus());
			if (tb && tb->getFSGUIParent() && tb->getFSGUIParent()->getDockable())
			{
			beginDocking();
			}
			else
			{
			setSizing(false);
			setDocking(false);
			setSizeDir(MP_NONE);
			}
			}
			*/
		}
		return false;
	}

	void FSGUI_Desktop::beginDocking()
	{
		if (!getFocus()) return;

		setFocus(getFocus()->getFSGUIParent());
		if (getFocus())
		{
			if (getFocus()->getLayoutStyle() == LS_REST) return;

			setOldLayoutStyle(getFocus()->getLayoutStyle());
			setOldWindowSize(getFocus()->getAbsolutePosition());

			getFocus()->setLayoutStyle(LS_NONE);
			getDesktopWindow()->calculateLayout();

			if (getOldLayoutStyle() != LS_NONE)
			{
				rect<s32> r = getFocus()->getAbsolutePosition();
				r.LowerRightCorner -= r.UpperLeftCorner;
				r.UpperLeftCorner -= r.UpperLeftCorner;
				r.UpperLeftCorner.X = getApplication()->getEngine()->getDevice()->getCursorControl()->getPosition().X;
				r.UpperLeftCorner.Y = getApplication()->getEngine()->getDevice()->getCursorControl()->getPosition().Y;
				r.LowerRightCorner.X = r.UpperLeftCorner.X + getDockingWidth(getFocus(), LS_NONE);
				r.LowerRightCorner.Y = r.UpperLeftCorner.Y + getDockingHeight(getFocus(), LS_NONE);
				getFocus()->getFSGUIParent()->OnSizeFSGUIChild(getFocus(), r, true);
				setTempRenderLast(getFocus());
			}

			getDesktopWindow()->calculateLayout();
		}
		setDocking(true);
	}

	void FSGUI_Desktop::endDocking()
	{
		if (getFocus())
		{
			if (getOldLayoutStyle() == LS_NONE)
			{
			}
			else
				if (getFocus()->getLayoutStyle() == LS_NONE)
				{
					getFocus()->setLayoutStyle(getOldLayoutStyle());
					getFocus()->setRelativePosition(getOldWindowSize());
				}
		}
		getDesktopWindow()->calculateLayout();
		setDocking(false);
		setTempRenderLast(0);
	}

	int FSGUI_Desktop::getDockingWidth(FSGUI_Window* w, FSGUI_LAYOUTSTYLE ls)
	{
		return 100;
	}

	int FSGUI_Desktop::getDockingHeight(FSGUI_Window* w, FSGUI_LAYOUTSTYLE ls)
	{
		return 100;
	}

	void FSGUI_Desktop::beginDragging(FSGUI_DRAGDROPINFO* ddi)
	{
		if (m_Dragging) return;

		setDragDropInfo(ddi);
		if (ddi)
		{
			m_Dragging = true;
			//m_Busy = true;
			getDevice()->getCursorControl()->setActiveIcon(ECI_HAND);
		}
	}

	void FSGUI_Desktop::endDragging(int mousex, int mousey)
	{
		if (getDragDropInfo())
		{
			FSGUI_Window* w = getFSGUI_WindowAt(mousex, mousey);
			if (w) w->acceptDragDropInfo(getDragDropInfo());
			setDragDropInfo(0);
		}
		m_Dragging = false;
		//m_Busy = false;
		getDevice()->getCursorControl()->setActiveIcon(ECI_NORMAL);
	}

	void FSGUI_Desktop::setDragDropInfo(FSGUI_DRAGDROPINFO* ddi)
	{
		if (m_DragDropInfo) delete(m_DragDropInfo); m_DragDropInfo = 0;
		m_DragDropInfo = ddi;
	}

}