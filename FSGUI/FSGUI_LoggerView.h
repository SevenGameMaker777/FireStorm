#pragma once

#include "FSUtils.h"
#include "FSLogger.h"
#include "FSGUI_Window.h"
#include "FSGUI_TextAreaView.h"

namespace FS
{

	class FSGUI_LoggerView : public FSGUI_TextAreaView, public FSLogger
	{
	public:
		FSGUI_LoggerView(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, int minwidth, int minheight, stringc text) :
			FSGUI_TextAreaView(desktop, gui, parent, id, r, flags, ls, minwidth, minheight, text)
		{
		}

		virtual ~FSGUI_LoggerView()
		{
		}

		virtual void print(FSLOGTYPE type, stringc text, bool addtolist = true, bool printtoconsole = true)
		{
			FSLogger::print(type, text, false);

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

			addText(t, color);
		}

		virtual void addText(stringc text, SColor color = SColor(255, 0, 0, 0))
		{
			FSGUI_TextAreaView::addTextToEnd(text, color);
		}

		virtual void addTextToEnd(stringc text, stringc text2, SColor color = SColor(255, 0, 0, 0))
		{
			FSGUI_TextAreaView::addTextToEnd(text, text2, color);
		}

	};

} // end namespace