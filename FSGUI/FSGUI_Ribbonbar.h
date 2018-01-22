// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_Scrollbar.h"

namespace FS
{
#define RIBBONBAR_WIDTH 30
#define RIBBONBAR_HEIGHT 30

	class FSGUI_Ribbonbar : public FSGUI_Window
	{
	private:
		FS_VARIABLE_SETGET(bool, Vertical);
		FS_VARIABLE_SETGET(int, Spacer);
		FS_VARIABLE_SETGET(int, Counter);
		FS_VARIABLE_SETGET(int, SelectedItem);

	public:
		array<IGUIElement*> m_RibbonbarChildren;
	public:
		FSGUI_Ribbonbar(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r, long flags, FSGUI_LAYOUTSTYLE ls, bool vertical, int spacer) :
			FSGUI_Window(desktop, gui, parent, id, r, flags, ls, L"", r.getWidth(), r.getHeight()),
			m_Vertical(vertical), m_Spacer(spacer)
		{
			m_RibbonbarChildren.clear();
			m_Counter = 0;
		}

		void destroyControls()
		{
			m_SelectedItem = -1;
			array<IGUIElement*> c = m_RibbonbarChildren;
			if (c.empty()) return;
			for (int x = 0; x < c.size(); x++)
			{
				m_RibbonbarChildren[x]->remove();
			}
			m_RibbonbarChildren.clear();
		}

		void calculateSelectedItem(int mouseX, int mouseY)
		{
			m_SelectedItem = -1;
			array<IGUIElement*> c = m_RibbonbarChildren;
			if (c.empty()) return;

			for (u32 x = m_Counter; x < c.size(); x++)
			{
				if (m_RibbonbarChildren[x]->getAbsolutePosition().isPointInside(position2di(mouseX,mouseY)))
					m_SelectedItem = x;
			}
			calculateLayout();
		}

		virtual void draw()
		{
			FSGUI_Window::draw();

			if ((m_SelectedItem > -1) && (m_SelectedItem < m_RibbonbarChildren.size()))
			{
				getDesktop()->getDriver()->draw2DRectangleOutline(m_RibbonbarChildren[m_SelectedItem]->getAbsolutePosition());
			}
		}

		void calculateScrollbarHeight()
		{
			int size = getClientRect().getHeight() / getClientRect().getWidth();
			if (m_RibbonbarChildren.size() > size)
				size = m_RibbonbarChildren.size() - size;
			getVertScrollbar()->getScrollbar()->setMax(size-1);
		}

		virtual IGUIElement* addElement(IGUIElement* e)
		{
			m_RibbonbarChildren.push_back(e);
			calculateScrollbarHeight();
			return e;
		}

		virtual void setLayoutStyle(FSGUI_LAYOUTSTYLE ls)
		{
			FSGUI_Window::setLayoutStyle(ls);
			switch (ls)
			{
			case LS_LEFTFILL:
			case LS_RIGHTFILL:
				setVertical(true);
				break;
			case LS_TOPFILL:
			case LS_BOTTOMFILL:
				setVertical(false);
				break;
			}
		}

		virtual void calculateLayout()
		{
			FSGUI_Window::calculateLayout();

			if (getVertical()) layoutVertical();
			else layoutHorizontal();

			calculateScrollbarHeight();
		}

		virtual void layoutVertical()
		{
			array<IGUIElement*> c = m_RibbonbarChildren;
			if (c.empty()) return;

			rect<s32> r = normalizeToSelf(getClientRect());
			r.UpperLeftCorner.X += getSpacer();
			r.UpperLeftCorner += 2;
			r.LowerRightCorner -= 2;

			for (int x = 0; x < c.size(); x++) c[x]->setVisible(false);
			for (int x = m_Counter; x < c.size(); x++) c[x]->setVisible(true);

			for (int x = m_Counter; x<c.size(); x++)
			{
				if (m_RibbonbarChildren[x]->isVisible())
				{
					switch (m_RibbonbarChildren[x]->getType())
					{
					case EGUIET_IMAGE:
					{
//						r.LowerRightCorner.Y = r.UpperLeftCorner.Y + m_RibbonbarChildren[x]->getAbsolutePosition().getHeight();
						r.LowerRightCorner.Y = r.UpperLeftCorner.Y + r.getWidth();
						r.LowerRightCorner.X = r.UpperLeftCorner.X + r.getWidth();
						m_RibbonbarChildren[x]->setRelativePosition(r);
						m_RibbonbarChildren[x]->updateAbsolutePosition();
						r.UpperLeftCorner.Y = r.LowerRightCorner.Y + getSpacer();
					} break;
					}
				}
			}
		}

		virtual void layoutHorizontal()
		{
			array<IGUIElement*> c;
			c = m_RibbonbarChildren;
			if (c.empty()) return;

			rect<s32> r = normalizeToSelf(getClientRect());
			r.UpperLeftCorner.X += getSpacer();
			r.UpperLeftCorner += 2;
			r.LowerRightCorner -= 2;

			for (int x = m_Counter; x<c.size(); x++)
			{
				if (m_RibbonbarChildren[x]->isVisible())
				{
					switch (m_RibbonbarChildren[x]->getType())
					{
					case EGUIET_IMAGE:
					{
						r.LowerRightCorner.X = r.UpperLeftCorner.X + m_RibbonbarChildren[x]->getAbsolutePosition().getWidth();
						m_RibbonbarChildren[x]->setRelativePosition(r);
						m_RibbonbarChildren[x]->updateAbsolutePosition();
						r.UpperLeftCorner.X = r.LowerRightCorner.X + getSpacer();
					} break;
					}
				}
			}
		}

		virtual bool onMouseWheel(const SEvent &e)
		{
			if (e.MouseInput.Wheel < 0)
				getVertScrollbar()->getScrollbar()->setPos(getVertScrollbar()->getScrollbar()->getPos() + 1);
			else
				getVertScrollbar()->getScrollbar()->setPos(getVertScrollbar()->getScrollbar()->getPos() - 1);
			m_Counter = getVertScrollbar()->getScrollbar()->getPos();
			calculateLayout();

			calculateScrollbarHeight();
			return FSGUI_Window::onMouseWheel(e);
		}
		
		virtual bool onScrollbarChanged(const SEvent &e)
		{
			m_Counter = getVertScrollbar()->getScrollbar()->getPos();
			calculateLayout();
			return FSGUI_Window::onScrollbarChanged(e);
		}

		virtual bool onMouseMoved(const SEvent &e)
		{
			calculateSelectedItem(e.MouseInput.X, e.MouseInput.Y);
			return FSGUI_Window::onMouseMoved(e);
		}

		virtual bool onLMouseButtonDown(const SEvent &e)
		{
			calculateSelectedItem(e.MouseInput.X, e.MouseInput.Y);
			return true;
		}
	};

} // end namespace#pragma once
