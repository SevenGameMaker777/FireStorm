#include "FSApplication.h"
#include "FSEngine.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_Titlebar.h"
#include "FSGUI_Systembar.h"
#include "FSGUI_Menubar.h"
#include "FSGUI_Toolbar.h"
#include "FSGUI_Scrollbar.h"

namespace FS
{
	FSGUI_DRAGDROPINFO::FSGUI_DRAGDROPINFO(FSGUI_Desktop* desktop, FSGUI_DRAGDROPINFO_TYPE type, FSGUI_DRAGDROPINFO_TYPE2 type2, int from, int controlid, stringc text)
		: m_Desktop(desktop), m_Type(type), m_From(from), m_ControlID(controlid)
	{
		stringw t(text);
		dimension2d<u32> dim = desktop->getGui()->getSkin()->getFont()->getDimension(t.c_str());
		rect<s32> r(0, 0, dim.Width + 4, dim.Height);
		m_TextElement = desktop->getGui()->addStaticText(t.c_str(), r, true, false, 0, ID_CURSORTEXT, true);
		m_TextElement->setVisible(true);
	}

	FSGUI_DRAGDROPINFO::~FSGUI_DRAGDROPINFO()
	{
		if (m_TextElement) m_TextElement->remove(); m_TextElement = 0;
	}

	IGUIEnvironment*	FSGUI_Window::getGui() { return Environment; }
	IVideoDriver*		FSGUI_Window::getDriver() { return getDesktop()->getDriver(); }
	FSGUI_Desktop*		FSGUI_Window::getDesktop() { return m_Desktop; }
	stringc				FSGUI_Window::getDirectory(stringc dir) { return getDesktop()->getApplication()->getDirectory(dir); }

	FSGUI_Window::FSGUI_Window(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* p, int id, rect<s32> r, DWORD flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minwidth, int minheight)
		: IGUIElement(EGUIET_ELEMENT, gui, p ? p : gui->getRootGUIElement(), id, r),
		FSEventHandler(),
		m_Flags(flags), m_LayoutStyle(ls), m_MinWidth(minwidth), m_MinHeight(minheight)
	{
		m_GUIType = "FSGUI_WINDOW";
		m_FSGUIChildren.clear();
		m_FSGUIParent = p;
		m_Desktop = desktop;
		m_ClientRect = r;
		setVisible(isFlagged(WS_VISIBLE));
		setText(text);
		if (m_FSGUIParent) m_FSGUIParent->addFSGUIChild(this);
		m_Dockable = true;

		setCanDragDrop(true);
		if (isFlagged(WS_TITLEBAR)) addTitlebar();
		if (isFlagged(WS_SYSTEMBAR)) addSystembar();

		m_VertScrollbar = 0;
		m_HorizScrollbar = 0;
		if (isFlagged(WS_VSCROLL)) m_VertScrollbar = new FSGUI_Scrollbar(getDesktop(), getGui(), this, 1, true);
		if (isFlagged(WS_HSCROLL)) m_HorizScrollbar = new FSGUI_Scrollbar(getDesktop(), getGui(), this, 1, false);

	}


	FSGUI_Window::~FSGUI_Window()
	{
		m_VertScrollbar = 0;
		m_HorizScrollbar = 0;
	}

	FSGUI_Window* FSGUI_Window::getFSGUI_WindowAt(int x, int y)
	{
		FSGUI_Window* target = 0;
		core::list<FSGUI_Window*>::Iterator it = m_FSGUIChildren.getLast();

		if (isVisible())
		{
			while (it != m_FSGUIChildren.end())
			{
				target = (*it)->getFSGUI_WindowAt(x, y);
				if (target)	return target;
				--it;
			}
		}

		if (isVisible() && isPointInside(position2di(x, y)))
			target = this;

		return target;
	}

	void FSGUI_Window::addFSGUIChild(FSGUI_Window* child)
	{
		m_FSGUIChildren.push_back(child);
	}

	FSGUI_Window* FSGUI_Window::getFSGUIChild(int id)
	{
		list<FSGUI_Window*> c = m_FSGUIChildren;
		if (c.empty()) return NULL;

		list<FSGUI_Window*>::Iterator it = c.begin();
		for (it; it != c.end(); it++)
			if ((*it)->getID() == id) return (*it);
		return NULL;
	}

	bool FSGUI_Window::onLMouseButtonDown(const SEvent & e)
	{
		return FSEventHandler::onLMouseButtonDown(e);
	}

	bool FSGUI_Window::onRMouseButtonDown(const SEvent & e)
	{
		return FSEventHandler::onRMouseButtonDown(e);
	}

	void FSGUI_Window::showChildWindow(int id, bool value)
	{
		FSGUI_Window* w = (FSGUI_Window*)getElementFromId(id, true);
		if (w) w->setVisible(value);
	}

	void FSGUI_Window::draw()
	{
		if ((isVisible()) && (isFlagged(WS_VISIBLE)))
		{
			drawClientArea();
			drawFrame();
			IGUIElement::draw();
		}
	}

	void FSGUI_Window::drawFrame()
	{
		IGUISkin* skin = getGui()->getSkin();

		if ((!isFlagged(WS_THICKFRAME)) && (!isFlagged(WS_BORDER)) && (!isFlagged(WS_DLGFRAME))) return;

		rect<s32> r = getAbsolutePosition();
		r.LowerRightCorner -= 1;

		for (int x = 0; x < getDesktop()->getBorderSize(getFlags()); x++)
		{
			position2di tl(r.UpperLeftCorner.X, r.UpperLeftCorner.Y);
			position2di tr(r.LowerRightCorner.X, r.UpperLeftCorner.Y);
			position2di bl(r.UpperLeftCorner.X, r.LowerRightCorner.Y);
			position2di br(r.LowerRightCorner.X, r.LowerRightCorner.Y);

			getDesktop()->getDriver()->draw2DLine(tl, tr, skin->getColor(EGDC_3D_LIGHT));
			getDesktop()->getDriver()->draw2DLine(tl, bl, skin->getColor(EGDC_3D_LIGHT));
			getDesktop()->getDriver()->draw2DLine(bl, br, skin->getColor(EGDC_3D_SHADOW));
			getDesktop()->getDriver()->draw2DLine(tr, br, skin->getColor(EGDC_3D_SHADOW));

			r.UpperLeftCorner += 1;
			r.LowerRightCorner -= 1;
		}

		updateAbsolutePosition();
		r = getAbsolutePosition();
		position2di s;
		position2di e;
		s.set((r.LowerRightCorner.X - r.UpperLeftCorner.X) / 4 + r.UpperLeftCorner.X, r.UpperLeftCorner.Y);
		e.set(r.LowerRightCorner.X, r.UpperLeftCorner.Y);
		getDesktop()->getDriver()->draw2DLine(s, e, skin->getColor(EGDC_HIGH_LIGHT));
		s.set(r.LowerRightCorner.X - 1, r.UpperLeftCorner.Y);
		e.set(r.LowerRightCorner.X - 1, r.UpperLeftCorner.Y + ((r.LowerRightCorner.Y - r.UpperLeftCorner.Y) * 3 / 4));
		getDesktop()->getDriver()->draw2DLine(s, e, skin->getColor(EGDC_HIGH_LIGHT));

	}

	void FSGUI_Window::drawClientArea()
	{
		IGUISkin* skin = getGui()->getSkin();

		if (!isFlagged(WS_EX_TRANSPARENT))
		{
			rect<s32> r = getClientRect();
			getDesktop()->getDriver()->draw2DRectangle(skin->getColor(EGDC_3D_DARK_SHADOW), r, 0);
		}
	}

	rect<s32> FSGUI_Window::normalizeToSelf(rect<s32> r)
	{
		rect<s32> result = r;
		result.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		result.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
		return result;
	}

	rect<s32> FSGUI_Window::normalizeToParent(rect<s32> r)
	{
		rect<s32> result = r;
		result.LowerRightCorner -= Parent->getAbsolutePosition().UpperLeftCorner;
		result.UpperLeftCorner -= Parent->getAbsolutePosition().UpperLeftCorner;
		return result;
	}

	bool FSGUI_Window::OnEvent(const SEvent & e)
	{
		if (FSEventHandler::OnEvent(e)) return true;
		return IGUIElement::OnEvent(e);
	}

	bool FSGUI_Window::onButtonClicked(const SEvent & e)
	{
		switch (e.GUIEvent.Caller->getID())
		{
		case FSID_MINIMIZE: setVisible(!isVisible()); getDesktop()->getDesktopWindow()->calculateLayout();  return true; break;
		}

		return FSEventHandler::onButtonClicked(e);
	}

	rect<s32> FSGUI_Window::updateClientRect()
	{
		m_ClientRect = getAbsolutePosition();
		m_ClientRect.UpperLeftCorner += getDesktop()->getBorderSize(getFlags());
		m_ClientRect.LowerRightCorner -= getDesktop()->getBorderSize(getFlags());
		return m_ClientRect;
	}

	void FSGUI_Window::calculateLayout()
	{
		updateClientRect();


		list<FSGUI_Window*>::Iterator it;
		if (m_FSGUIChildren.empty()) return;

		for (it = m_FSGUIChildren.begin(); it != m_FSGUIChildren.end(); it++)
		{
			if ((*it)->isFlagged(WS_VISIBLE))
			{
				rect<s32> r = m_ClientRect;

				switch ((*it)->getLayoutStyle())
				{
				case LS_REST:
				{
					OnSizeFSGUIChild((*it), r, false);
				} break;
				case LS_LEFTFILL:
				{
					r.LowerRightCorner.X = r.UpperLeftCorner.X + (*it)->getAbsolutePosition().getWidth();
					OnSizeFSGUIChild((*it), r, false);
				} break;
				case LS_RIGHTFILL:
				{
					r.UpperLeftCorner.X = r.LowerRightCorner.X - (*it)->getAbsolutePosition().getWidth();
					OnSizeFSGUIChild((*it), r, false);
				} break;
				case LS_TOPFILL:
				{
					r.LowerRightCorner.Y = r.UpperLeftCorner.Y + (*it)->getAbsolutePosition().getHeight();
					OnSizeFSGUIChild((*it), r, false);
				} break;
				case LS_BOTTOMFILL:
				{
					r.UpperLeftCorner.Y = r.LowerRightCorner.Y - (*it)->getAbsolutePosition().getHeight();
					OnSizeFSGUIChild((*it), r, false);
				} break;
				default: break;
				}
			}
		}

		for (it = m_FSGUIChildren.begin(); it != m_FSGUIChildren.end(); it++)
		{
			if ((*it)->isFlagged(WS_VISIBLE))
			{
				(*it)->calculateLayout();
			}
		}
	}

	FSGUI_SIZEDIR FSGUI_Window::mousePosition(int x, int y)
	{
		rect<s32> r = getAbsolutePosition();
		if (x < r.UpperLeftCorner.X + getDesktop()->getBorderSize(getFlags())) return MP_LEFT;
		if (y < r.UpperLeftCorner.Y + getDesktop()->getBorderSize(getFlags())) return MP_TOP;
		if (x > r.LowerRightCorner.X - getDesktop()->getBorderSize(getFlags())) return MP_RIGHT;
		if (y > r.LowerRightCorner.Y - getDesktop()->getBorderSize(getFlags())) return MP_BOTTOM;
		if (r.isPointInside(position2di(x, y))) return MP_CLIENT;
		return MP_NONE;
	}

	bool FSGUI_Window::canResize(FSGUI_SIZEDIR mp)
	{
		if (!isFlagged(WS_THICKFRAME)) return false;
		if (getLayoutStyle() == LS_NONE) return true;

		switch (mp)
		{
		case MP_NONE: return false; break;
		case MP_CLIENT: return false; break;
		case MP_LEFT: return (getLayoutStyle() == LS_RIGHTFILL); break;
		case MP_RIGHT: return (getLayoutStyle() == LS_LEFTFILL); break;
		case MP_TOP: return (getLayoutStyle() == LS_BOTTOMFILL); break;
		case MP_BOTTOM: return (getLayoutStyle() == LS_TOPFILL); break;
		}
		return false;
	}

	void FSGUI_Window::OnSize(rect<s32> r, bool recalculate)
	{
		setRelativePosition(r);
		updateAbsolutePosition();
		if (recalculate)
		{
			if (getFSGUIParent()) getFSGUIParent()->calculateLayout();
		}
		else calculateLayout();
	}

	void FSGUI_Window::OnSizeFSGUIChild(FSGUI_Window* w, rect<s32> r, bool recalculate)
	{
		if (!w) return;

		// remmeber our current size
		rect<s32> s = w->getAbsolutePosition();
		rect<s32> pc = m_ClientRect;

		switch (w->m_LayoutStyle)
		{
		case LS_LEFTFILL:
			//			if (r.getHeight() < w->getMinHeight()) r.LowerRightCorner.Y = r.UpperLeftCorner.Y + w->getMinHeight();
			if (r.getWidth() < w->getMinWidth()) r.LowerRightCorner.X = r.UpperLeftCorner.X + w->getMinWidth();
			if ((pc.getWidth() < 1) && (s.getWidth() < r.getWidth())) return;
			if ((s.getWidth() < w->getMinWidth()) && (s.getWidth() > r.getWidth())) return;
			//		if (r.LowerRightCorner.X > pc.LowerRightCorner.X) r.LowerRightCorner.X = pc.LowerRightCorner.X;
			//			if ( r.getWidth() < w->getMinimumAllowedWidth()) r.LowerRightCorner.X = r.UpperLeftCorner.X + w->getMinimumAllowedWidth();
			m_ClientRect.UpperLeftCorner.X = r.LowerRightCorner.X;
			break;
		case LS_TOPFILL:
			//			if (r.getWidth() < w->getMinWidth()) r.LowerRightCorner.X = r.UpperLeftCorner.X + w->getMinWidth();
			if (r.getHeight() < w->getMinHeight()) r.LowerRightCorner.Y = r.UpperLeftCorner.Y + w->getMinHeight();
			if (pc.getHeight() < 1) if (s.getHeight() < r.getHeight()) return;
			if (s.getHeight() < w->getMinHeight()) if (s.getHeight() > r.getHeight()) return;
			//		if (r.LowerRightCorner.Y > pc.LowerRightCorner.Y) r.LowerRightCorner.Y = pc.LowerRightCorner.Y;
			//			if (r.getHeight() < w->getMinimumAllowedHeight()) r.LowerRightCorner.Y = r.UpperLeftCorner.Y + w->getMinimumAllowedHeight();
			m_ClientRect.UpperLeftCorner.Y = r.LowerRightCorner.Y;
			break;
		case LS_RIGHTFILL:
			//			if (r.getHeight() < w->getMinHeight()) r.LowerRightCorner.Y = r.UpperLeftCorner.Y + w->getMinHeight();
			if (r.getWidth() < w->getMinWidth()) r.UpperLeftCorner.X = r.LowerRightCorner.X - w->getMinWidth();
			if (pc.getWidth() < 1) if (s.getWidth() < r.getWidth()) return;
			if (s.getWidth() < w->getMinWidth()) if (s.getWidth() > r.getWidth()) return;
			//		if (r.UpperLeftCorner.X < pc.UpperLeftCorner.X) r.UpperLeftCorner.X = pc.UpperLeftCorner.X;
			//			if (r.getWidth() < w->getMinimumAllowedWidth()) r.UpperLeftCorner.X = r.LowerRightCorner.X - w->getMinimumAllowedWidth();
			m_ClientRect.LowerRightCorner.X = r.UpperLeftCorner.X;
			break;
		case LS_BOTTOMFILL:
			//			if (r.getWidth() < w->getMinWidth()) r.LowerRightCorner.X = r.UpperLeftCorner.X + w->getMinWidth();
			if (r.getHeight() < w->getMinHeight()) r.UpperLeftCorner.Y = r.LowerRightCorner.Y - w->getMinHeight();
			if (pc.getHeight() < 1)	if (s.getHeight() < r.getHeight()) return;
			if (s.getHeight() < w->getMinHeight()) if (s.getHeight() > r.getHeight()) return;
			//		if (r.UpperLeftCorner.Y < pc.UpperLeftCorner.Y) r.UpperLeftCorner.Y = pc.UpperLeftCorner.Y;
			//			if (r.getHeight() < w->getMinimumAllowedHeight()) r.UpperLeftCorner.Y = r.LowerRightCorner.Y - w->getMinimumAllowedHeight();
			m_ClientRect.LowerRightCorner.Y = r.UpperLeftCorner.Y;
			break;
		case LS_REST:
			//			if (r.getWidth() < w->getMinWidth()) r.LowerRightCorner.X = r.UpperLeftCorner.X + w->getMinWidth();
			//			if (r.getHeight() < w->getMinHeight()) r.LowerRightCorner.Y = r.UpperLeftCorner.Y + w->getMinHeight();
			//			if (r.getWidth() < w->getMinWidth()) r.UpperLeftCorner.X = r.LowerRightCorner.X - w->getMinWidth();
			//			if (r.getHeight() < w->getMinHeight()) r.UpperLeftCorner.Y = r.LowerRightCorner.Y - w->getMinHeight();
			break;
		}
		// normalize to parent
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		w->setRelativePosition(r);
		w->updateAbsolutePosition();
		if (recalculate) calculateLayout();
	}

	bool FSGUI_Window::acceptDragDropInfo(FSGUI_DRAGDROPINFO* ddi)
	{
		IGUIElement* e = getDesktop()->getGui()->getRootGUIElement()->getElementFromPoint(getDesktop()->getDevice()->getCursorControl()->getPosition());
		if (e)
		{
			switch (ddi->getType())
			{
			case DDIT_FILENAME:
			{
				FSGUI_DRAGDROPINFO_FILENAME* tddi = dynamic_cast<FSGUI_DRAGDROPINFO_FILENAME*>(ddi);
				if (tddi)
				{
					switch (e->getType())
					{
					case EGUIET_BUTTON:
					case EGUIET_EDIT_BOX:
					case EGUIET_STATIC_TEXT:
					{
						e->setText(stringw(tddi->getFilename()).c_str());
						return true;
					} break;
					}
				}
				return true;
			} break;
			}
		}
		return false;
	}

	FSGUI_Titlebar* FSGUI_Window::addTitlebar()
	{
		FSGUI_Titlebar* tb = new FSGUI_Titlebar(getDesktop(), Environment, this, getID() + TITLEBAR_FSID_OFFSET, L"");
		tb->setText(getText());
		return tb;
	}

	FSGUI_Menubar* FSGUI_Window::addMenubar()
	{
		FSGUI_Menubar* tb = new FSGUI_Menubar(getDesktop(), Environment, this, getID() + MENU_FSID_OFFSET, L"");
		return tb;
	}

	FSGUI_Toolbar* FSGUI_Window::addToolbar(int id, rect<s32> r, FSGUI_LAYOUTSTYLE ls, bool vertical, int spacer)
	{
		FSGUI_Toolbar* tb = new FSGUI_Toolbar(getDesktop(), Environment, this, id, r, ls, vertical, spacer);
		return tb;
	}

	FSGUI_Systembar* FSGUI_Window::addSystembar()
	{
		FSGUI_Titlebar* tb = (FSGUI_Titlebar*)getFSGUIChild(getID() + TITLEBAR_FSID_OFFSET);
		if (tb)
		{
			FSGUI_Systembar* sb = new FSGUI_Systembar(getDesktop(), Environment, tb, getID(), L"Systembar");
			return sb;
		}
		return NULL;
	}

} // end namespace