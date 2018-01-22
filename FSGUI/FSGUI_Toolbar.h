// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "Defines.h"

namespace FS
{
	class FSGUI_Toolbar : public FSGUI_Window
	{
	private:
		FS_VARIABLE_SETGET(bool, Vertical);
		FS_VARIABLE_SETGET(int, Spacer);
	public :
		list<IGUIElement*> m_ToolbarChildren;
	public:
		FSGUI_Toolbar(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r, FSGUI_LAYOUTSTYLE ls, bool vertical, int spacer) :
			FSGUI_Window(desktop, gui, parent, id, r, WS_VISIBLE|WS_BORDER, ls, L"", r.getWidth(), r.getHeight()),
			m_Vertical(vertical),m_Spacer(spacer)  
		{
			m_ToolbarChildren.clear();
		}

		virtual IGUIElement* addElement(IGUIElement* e)
		{
			m_ToolbarChildren.push_back(e);
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
		}

		virtual void layoutVertical()
		{
			list<IGUIElement*> c = m_ToolbarChildren;
			if (c.empty()) return;

			list<IGUIElement*>::Iterator it;

			rect<s32> r = normalizeToSelf(getClientRect());
			r.UpperLeftCorner.X += getSpacer();
			r.UpperLeftCorner += 2;
			r.LowerRightCorner -= 2;

			for (it = c.begin(); it != c.end(); it++)
			{
				if ((*it)->isVisible())
				{
					switch ((*it)->getType())
					{
						case EGUIET_BUTTON:
						{
							r.LowerRightCorner.Y = r.UpperLeftCorner.Y + (*it)->getAbsolutePosition().getHeight();
							(*it)->setRelativePosition(r);
							(*it)->updateAbsolutePosition();
							r.UpperLeftCorner.Y = r.LowerRightCorner.Y + getSpacer();
						} break;
						case EGUIET_COMBO_BOX:
						{
							r.LowerRightCorner.Y = r.UpperLeftCorner.Y + (*it)->getAbsolutePosition().getHeight();
							r.LowerRightCorner.X = r.UpperLeftCorner.X + (*it)->getAbsolutePosition().getWidth();
							(*it)->setRelativePosition(r);
							(*it)->updateAbsolutePosition();
							r.UpperLeftCorner.Y = r.LowerRightCorner.Y + getSpacer();
						} break;
						case EGUIET_MENU:
						{
							(*it)->setRelativePosition(r);
							(*it)->updateAbsolutePosition();
						} break;
						case EGUIET_EDIT_BOX:
						{
							r.LowerRightCorner.Y = r.UpperLeftCorner.Y + (*it)->getAbsolutePosition().getHeight();
							r.LowerRightCorner.X = r.UpperLeftCorner.X + (*it)->getAbsolutePosition().getWidth();
							(*it)->setRelativePosition(r);
							(*it)->updateAbsolutePosition();
							r.UpperLeftCorner.Y = r.LowerRightCorner.Y + getSpacer();
						} break;
					}
				}
			}
		}

		virtual void layoutHorizontal()
		{
			list<IGUIElement*> c;
			c = m_ToolbarChildren;
			if (c.empty()) return;

			list<IGUIElement*>::Iterator it;

			rect<s32> r = normalizeToSelf(getClientRect());
			r.UpperLeftCorner.X += getSpacer();
			r.UpperLeftCorner += 2;
			r.LowerRightCorner -= 2;

			for (it = c.begin(); it != c.end(); it++)
			{
				if ((*it)->isVisible())
				{
					switch ((*it)->getType())
					{
					case EGUIET_BUTTON:
					{
						r.LowerRightCorner.X = r.UpperLeftCorner.X + (*it)->getAbsolutePosition().getWidth();
						(*it)->setRelativePosition(r);
						(*it)->updateAbsolutePosition();
						r.UpperLeftCorner.X = r.LowerRightCorner.X + getSpacer();
					} break;
					case EGUIET_COMBO_BOX:
					{
						r.LowerRightCorner.X = r.UpperLeftCorner.X + (*it)->getAbsolutePosition().getWidth();
						(*it)->setRelativePosition(r);
						(*it)->updateAbsolutePosition();
						//						r.UpperLeftCorner.Y = r.LowerRightCorner.Y + getSpacer();
						r.UpperLeftCorner.X = r.LowerRightCorner.X + getSpacer();
					} break;
					case EGUIET_MENU:
					{
						(*it)->setRelativePosition(r);
						(*it)->updateAbsolutePosition();
					} break;
					case EGUIET_EDIT_BOX:
					{
						r.LowerRightCorner.X = r.UpperLeftCorner.X + (*it)->getAbsolutePosition().getWidth();
						(*it)->setRelativePosition(r);
						(*it)->updateAbsolutePosition();
						//						r.UpperLeftCorner.Y = r.LowerRightCorner.Y + getSpacer();
						r.UpperLeftCorner.X = r.LowerRightCorner.X + getSpacer();
					} break;
					}
				}
			}
		}

	};

} // end namespace