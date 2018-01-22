#pragma once

#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_TextAreaView.h"
#include "FSGUI_LoggerView.h"
#include "FSGUI_Toolbar.h"

#define FSID_TABBER		800
#define FSID_TAB_DEBUG	801
#define FSID_TAB_WARNING	802
#define FSID_TAB_ERROR	803
#define FSID_TAB_ALL			804
#define FSID_TEXTAREA_DEBUG	805
#define FSID_TEXTAREA_WARNING	806
#define FSID_TEXTAREA_ERROR	807
#define FSID_TEXTAREA_ALL	808
#define FSID_SHOWMESSAGES 809
#define FSID_CLEARMESSAGES 810

namespace FS
{

	class FSGUI_LoggerTabView : public FSGUI_Window, public FSLogger
	{
		int lineCount;
		IGUITabControl* m_Tabber;
		IGUITab* m_All;
		IGUITab* m_Debug;
		IGUITab* m_Warning;
		IGUITab* m_Error;
		TextArea* m_TextAreaAll;
		TextArea* m_TextAreaDebug;
		TextArea* m_TextAreaWarning;
		TextArea* m_TextAreaError;
	public:
		FSGUI_LoggerTabView(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, int minwidth, int minheight, stringc text) :
			FSGUI_Window(desktop, gui, parent, id, r, flags, ls, (wchar_t*)stringw(text).c_str(), minwidth, minheight)
		{
			lineCount = 0;

			// create the application level toolbar - in this case an editor type application
			FSGUI_Toolbar* tb = addToolbar(FSID_MAINTOOLBAR, rect<s32>(0, 0, 36, 36), LS_TOPFILL, false, 4);
			IGUIButton* b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_SHOWMESSAGES, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\text.jpg")));
			b->setIsPushButton(true);
			b->setPressed(logMessages);

			IGUIButton* b1 = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_CLEARMESSAGES, L""));
			b1->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\clear.jpg")));
			b1->setIsPushButton(false);

			m_Tabber = getGui()->addTabControl(rect<s32>(0, 0, 100, 100), this, true, true, FSID_TABBER);
			m_All = m_Tabber->addTab(L"All", FSID_TAB_ALL);
			m_Debug = m_Tabber->addTab(L"Debug", FSID_TAB_DEBUG);
			m_Warning = m_Tabber->addTab(L"Warning", FSID_TAB_WARNING);
			m_Error = m_Tabber->addTab(L"Error", FSID_TAB_ERROR);

			m_TextAreaAll = new TextArea(gui, m_All, FSID_TEXTAREA_ALL);
			m_TextAreaAll->setShowScrollBar(true);
			m_TextAreaAll->setMaxLines(222);
			m_TextAreaAll->setAutoScroll(false);

			m_TextAreaDebug = new TextArea(gui, m_Debug, FSID_TEXTAREA_DEBUG);
			m_TextAreaDebug->setShowScrollBar(true);
			m_TextAreaDebug->setMaxLines(222);
			m_TextAreaDebug->setAutoScroll(false);

			m_TextAreaWarning = new TextArea(gui, m_Warning, FSID_TEXTAREA_WARNING);
			m_TextAreaWarning->setShowScrollBar(true);
			m_TextAreaWarning->setMaxLines(222);
			m_TextAreaWarning->setAutoScroll(false);

			m_TextAreaError = new TextArea(gui, m_Error, FSID_TEXTAREA_ERROR);
			m_TextAreaError->setShowScrollBar(true);
			m_TextAreaError->setMaxLines(222);
			m_TextAreaError->setAutoScroll(false);
		}

		virtual ~FSGUI_LoggerTabView()
		{
		}

		virtual void calculateLayout()
		{
			FSGUI_Window::calculateLayout();

			if (m_Tabber)
			{
				rect<s32> r = getClientRect();
				r.LowerRightCorner -= r.UpperLeftCorner;
				r.UpperLeftCorner -= r.UpperLeftCorner;
				r.UpperLeftCorner.X += 4;
				r.UpperLeftCorner.Y += 40;
				r.LowerRightCorner -= 4;
				r.UpperLeftCorner.Y += 30;
				r.LowerRightCorner.Y += 60;
				m_Tabber->setRelativePosition(r);
				m_Tabber->updateAbsolutePosition();

				r = m_Tabber->getAbsolutePosition();
				r.LowerRightCorner -= r.UpperLeftCorner;
				r.UpperLeftCorner -= r.UpperLeftCorner;
				r.LowerRightCorner.Y -= 60;

				m_TextAreaAll->setRelativePosition(r);
				m_TextAreaAll->updateAbsolutePosition();
				m_TextAreaAll->update = true;

				m_TextAreaDebug->setRelativePosition(r);
				m_TextAreaDebug->updateAbsolutePosition();
				m_TextAreaDebug->update = true;

				m_TextAreaWarning->setRelativePosition(r);
				m_TextAreaWarning->updateAbsolutePosition();
				m_TextAreaWarning->update = true;
				
				m_TextAreaError->setRelativePosition(r);
				m_TextAreaError->updateAbsolutePosition();
				m_TextAreaError->update = true;
			}
		}

		virtual bool onButtonClicked(const SEvent & e)
		{
			switch (e.GUIEvent.Caller->getID())
			{
			case FSID_SHOWMESSAGES: logMessages = !logMessages; break;
			case FSID_CLEARMESSAGES: getGlobalLogger()->clear(); 
				clear();
				break;
			}
			return FSGUI_Window::onButtonClicked(e);
		}

		virtual void clear()
		{
			m_TextAreaAll->clear();
			m_TextAreaDebug->clear();
			m_TextAreaWarning->clear();
			m_TextAreaError->clear();
		}

		virtual bool onTabChanged(const SEvent & e) { return FSGUI_Window::onTabChanged(e); }

		virtual void print(FSLOGTYPE type, stringc text, bool addtolist = true, bool printtoconsole = true)
		{
			FSLogger::print(type, text, false);

			if (!logMessages) return;

			SColor color = SColor(255, 0, 0, 0);

			// display the text
			stringc t(FSLOGTYPE_STRING[(int)type]);
			t += " ";
			t += text;

			switch (type)
			{
			case FSL_DEBUG: color = SColor(255, 0, 0, 0); break;
			case FSL_WARNING: color = SColor(255, 0, 255, 255); break;
			case FSL_ERROR: color = SColor(255, 255, 0, 0); break;
			}

			// Create new line
			Line* line = new Line();
			line->addString(stringc(lineCount) + stringc(" ") + text, color);
			m_TextAreaAll->addLine(line);
			lineCount++;

			switch (type)
			{
			case FSL_DEBUG:
			{
				color = SColor(255, 0, 0, 0);
					// Create new line
					Line* line = new Line();
					line->addString(stringc(lineCount) + stringc(" ") + text, color);
					m_TextAreaDebug->addLine(line);
				}break;
				case FSL_WARNING: 
				{
					color = SColor(255, 0, 255, 255);
					Line* line = new Line();
					line->addString(stringc(lineCount) + stringc(" ") + text, color);
					m_TextAreaWarning->addLine(line);
				}	break;
				case FSL_ERROR: 
				{
					color = SColor(255, 255, 0, 0);
					Line* line = new Line();
					line->addString(stringc(lineCount) + stringc(" ") + text, color);
					m_TextAreaError->addLine(line);
				}	break;
			}
		}
	};

} // end namespace