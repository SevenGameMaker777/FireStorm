// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "CPropertyGrid.h"
#include "FSGUI_Scrollbar.h"
#include "FSPropertyManager.h"
#include "FSObject.h"
#include "FSLevel.h"

namespace FS
{
	class FSGUI_PropertyGrid : public FSGUI_Window
	{
	private:
		FS_VARIABLE_GET(CPropertyGrid*, PropertyGrid);

	public:
		FSGUI_PropertyGrid(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minwidth, int minheight) :
//			FSGUI_Window(desktop, gui, parent, id, r, flags | WS_VSCROLL | WS_VSCROLL, ls, text, r.getWidth(), r.getHeight())
			FSGUI_Window(desktop, gui, parent, id, r, flags, ls, text, r.getWidth(), r.getHeight())
		{
//			FSGUI_Titlebar* titlebar = addTitlebar();
//			titlebar->setXOffset(20);

			m_PropertyGrid = new CPropertyGrid(getDesktop()->getDevice(),getGui(), this, 1, rect<s32>(0, 0, 20, 20));
			FSGUI_Toolbar* tb = new FSGUI_Toolbar(desktop, gui, this, 1, rect<s32>(0, 0, 30, 30), LS_TOPFILL, false, 4);
			IGUIButton* b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_PROPERTYGRID_MINIMIZEALL, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\new.jpg")));
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_PROPERTYGRID_EXPANDALL, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\open.jpg")));
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_PROPERTYGRID_TOP, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\open.jpg")));
		}

		void minimizeAll() { m_PropertyGrid->minimizeAll(); }
		void maximizeAll() { m_PropertyGrid->maximizeAll(); }


		virtual bool onLMouseButtonDown(const SEvent & e) { return true; }

		virtual bool onButtonClicked(const SEvent & e) 
		{ 
			switch (e.GUIEvent.Caller->getID())
			{
			case FSID_PROPERTYGRID_MINIMIZEALL: m_PropertyGrid->minimizeAll(); calculateLayout(); break;
			case FSID_PROPERTYGRID_EXPANDALL: m_PropertyGrid->maximizeAll(); calculateLayout(); break;
			case FSID_PROPERTYGRID_TOP: getVertScrollbar()->getScrollbar()->setPos(0);	calculateLayout(); break;
			}
			return FSGUI_Window::onButtonClicked(e);;
		}

		virtual bool onScrollbarChanged(const SEvent & e) 
		{ 
			if (e.GUIEvent.Caller == getVertScrollbar()->getScrollbar()) calculateLayout();
			return false;
		}

		virtual bool onUserEvent(const SEvent & e)
		{
			switch (e.UserEvent.UserData1)
			{
			case EET_PROPERTYGRID_MINIMIZE: calculateLayout(); break;
			}
			return false;
		}

		virtual void calculateLayout()
		{
			FSGUI_Window::calculateLayout();
		
			if (m_PropertyGrid)
			{
				rect<s32> r = normalizeToSelf(getClientRect());
				r.UpperLeftCorner += 2;
				r.LowerRightCorner -= 2;
				m_PropertyGrid->setRelativePosition(r);
				m_PropertyGrid->updateAbsolutePosition();

				int x = 0;
				int y = 0;

				if (getHorizScrollbar()) x = getHorizScrollbar()->getScrollbar()->getPos();
				if (getVertScrollbar()) y = getVertScrollbar()->getScrollbar()->getPos();

				m_PropertyGrid->calculateLayout(-x,-y);
				int th = m_PropertyGrid->getTotalHeight();
				if (th < m_PropertyGrid->getAbsolutePosition().getHeight())
					th = 0;
				getVertScrollbar()->getScrollbar()->setMax(th);
			}
		}
	
		virtual bool acceptDragDropInfo(FSGUI_DRAGDROPINFO* ddi)
		{
			if (ddi->getFrom() != getID())
				switch (ddi->getType())
				{
				case DDIT_FILENAME:
				{
					FSGUI_DRAGDROPINFO_FILENAME* tddi = dynamic_cast<FSGUI_DRAGDROPINFO_FILENAME*>(ddi);
					if (tddi)
					{
						position2di pos = getDesktop()->getDevice()->getCursorControl()->getPosition();
						IGUIElement* element = getGui()->getRootGUIElement()->getElementFromPoint(pos);
						if (element->getType() == EGUIET_EDIT_BOX)
						{
							CPropertyGrid_Base* p = (CPropertyGrid_Base*)element->getParent();
							p->setValue(tddi->getFilename());
						}
					}
				} break;
				}
			return FSGUI_Window::acceptDragDropInfo(ddi);
		}
};


} // end namespace