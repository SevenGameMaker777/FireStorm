#pragma once
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "line.h"
#include "textarea.h"


namespace FS
{
	class FSGUI_TextAreaView : public FSGUI_Window
	{
	public:
		TextArea* m_TextArea;
		int lineCount;

		FSGUI_TextAreaView(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
				DWORD flags, FSGUI_LAYOUTSTYLE ls, int minwidth, int minheight, stringw text) :
			FSGUI_Window(desktop, gui, parent, id, r, flags, ls, L"", minwidth, minheight)
		{
			lineCount = 0;
			// Create a new textarea
			m_TextArea = new TextArea(gui, this, getID());
			m_TextArea->setShowScrollBar(true);
			m_TextArea->setMaxLines(222);
			m_TextArea->setAutoScroll(false);
		}

		virtual ~FSGUI_TextAreaView()
		{
		}

		virtual void calculateLayout()
		{
			FSGUI_Window::calculateLayout();
			rect<s32> r = getAbsolutePosition();
			r.LowerRightCorner -= r.UpperLeftCorner;
			r.UpperLeftCorner -= r.UpperLeftCorner;
			r.UpperLeftCorner.X += 5;
			r.UpperLeftCorner.Y += 22;
			r.LowerRightCorner.X -= 10;
			r.LowerRightCorner.Y -= 4;

			if (m_TextArea)
			{
				m_TextArea->setRelativePosition(r);
				m_TextArea->updateAbsolutePosition();
				m_TextArea->update = true;
			}
		}

		virtual void addTextToEnd(stringc text, SColor color = SColor(255,255,0,0))
		{
			// Create new line
			Line* line = new Line();

			line->addString(stringc(lineCount) + stringc(" ") + text, color);

			// Add new line to chatBox
			m_TextArea->addLine(line);

			lineCount++;
		}

		virtual void addTextToEnd(stringc text, stringc text2, SColor color = SColor(255, 255, 0, 0))
		{
			// Create new line
			Line* line = new Line();

			line->addString(stringc(lineCount) + stringc(" ") + text + " " + text2, color);

			// Add new line to chatBox
			m_TextArea->addLine(line);

			lineCount++;
		}

	};

} // end namespace