// include the needed header files
#include "FSLogger.h"

// unclutter the global namespace
namespace FS
{
	FSLogger logger;					//	global logger variable
	FSLogger* globalLogger = 0;			// pointer to current active logger
	list<LogText*> gTextStrings;		// the list of messages

										/// set up the pointer to the logger variable
										// copy all log strings to the new class
	void setGlobalLogger(FSLogger* l)
	{
		// if the logger is valid, copy all of the curent logger strings to it
		if (l)
		{
			// get an iterator
			list<LogText*>::Iterator it;

			// run through the list
			for (it = gTextStrings.begin(); it != gTextStrings.end();)
			{
				// send the item to the new logger
				l->print((*it)->m_Type, (*it)->m_Text, false);

				// increment the iterator
				it++;
			}
		}

		// remember the pointer
		globalLogger = l;
	}

	// get pointer to a valid logger, if it doesnt exist then get pointer to global logger
	FSLogger* getGlobalLogger() { if (!globalLogger) globalLogger = &logger; return globalLogger; }


	// clear all messages
	// run through the list and remove all of the messages
	void FSLogger::clear()
	{
		// get an iterator
		list<LogText*>::Iterator it;

		// run through the list
		for (it = gTextStrings.begin(); it != gTextStrings.end();)
		{
			// delete the object
			delete((*it));

			// erase the object from the list. This auto increments the iterator so no need to do that here
			it = gTextStrings.erase(it);
		}

		// do a final clear of the list
		gTextStrings.clear();
	}

	// override this function to route messages as desired
	// base class just adds the message to the list and printf's them to the console
	void FSLogger::print(FSLOGTYPE type, stringc text, bool addtolist, bool printtoconsole)
	{
		// if the type is not NONE, prefix the message with the type string
		if (type != FSL_NONE) printf_s("%s - ", FSLOGTYPE_STRING[(int)type]);

		// print it to the console
		if (printtoconsole) printf_s("%s\n", text.c_str());

		// add the message to the list if desired
		if (addtolist) gTextStrings.push_back(new LogText(type, text));
	}

	// parse the text for the next string
	int FSLogger::getString(stringc &text, char *szTypes, int index)
	{
		// parse the char* variable
		// while it is not a % or an end of line character
		while ((szTypes[index] != '%') && (szTypes[index] != '\0'))
		{
			// add the character to the string
			text += szTypes[index];

			// increment the index
			index++;
		}

		// return the new index position
		return index - 1;
	}

	/*
	%i - integer
	%f - float
	%c - char
	%s - char*
	%v - vector3df
	*/
	void FSLogger::log(FSLOGTYPE type, char *szTypes, ...)
	{
		if (!logMessages) return;

		// temporary variables
		va_list vl;
		int i;
		stringc text;

		//  szTypes is the last argument specified; you must access 
		//  all others using the variable-argument macros.
		va_start(vl, szTypes);

		// Step through the list.
		for (i = 0; szTypes[i] != '\0'; ++i)
		{
			// depending on what followed the % character
			switch (szTypes[i])
			{
				// i or d (integer values)
			case 'i':
			case 'd':text += va_arg(vl, int); break;

				// f (float / double value)
			case 'f': text += va_arg(vl, double); break;

				// c (single character value)
				//case 'c': text += va_arg(vl, char); break;

				// v (irrlicht vector3df value)
			case 'v': { vector3df vec = va_arg(vl, vector3df); text += vec.X; text += " "; text += vec.Y; text += " "; text += vec.Z; } break;

				// r (irrlicht rect<s32> value)
			case 'r': { rect<s32> r = va_arg(vl, rect<s32>); text += r.UpperLeftCorner.X; text += " "; text += r.UpperLeftCorner.Y; text += " "; text += r.LowerRightCorner.X; text += " "; text += r.LowerRightCorner.Y; } break;

				// s (char* value)
			case 's': text += va_arg(vl, char*); break;

				// % (hmmmm)
			case '%': break;

				// default to reading the entire string
			default: i = getString(text, szTypes, i); break;
			}
		}

		// end the search
		va_end(vl);

		// send the string to the logger print function
		print(type, text);
	}

	// simple method to get event type in string form
	stringc FSLogger::getEventType(const SEvent & e)
	{
		switch (e.EventType)
		{
		case EET_USER_EVENT:
		{
			return stringc(stringc(e.UserEvent.UserData1) + stringc(" ") + stringc(e.UserEvent.UserData2));
		} break;

		case EET_JOYSTICK_INPUT_EVENT:
		{
			return stringc("joystick input event");
		} break;

		case EET_LOG_TEXT_EVENT:
		{
			return stringc(e.LogEvent.Text);
		} break;

		case EET_KEY_INPUT_EVENT:
		{
			return stringc(e.KeyInput.Key);
		} break;

		case EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
				ENUMTOSTRING(EGET_ELEMENT_FOCUS_LOST)
					ENUMTOSTRING(EGET_ELEMENT_FOCUSED)
					ENUMTOSTRING(EGET_ELEMENT_HOVERED)
					ENUMTOSTRING(EGET_ELEMENT_LEFT)
					ENUMTOSTRING(EGET_ELEMENT_CLOSED)
					ENUMTOSTRING(EGET_BUTTON_CLICKED)
					ENUMTOSTRING(EGET_SCROLL_BAR_CHANGED)
					ENUMTOSTRING(EGET_CHECKBOX_CHANGED)
					ENUMTOSTRING(EGET_LISTBOX_CHANGED)
					ENUMTOSTRING(EGET_LISTBOX_SELECTED_AGAIN)
					ENUMTOSTRING(EGET_FILE_SELECTED)
					ENUMTOSTRING(EGET_DIRECTORY_SELECTED)
					ENUMTOSTRING(EGET_FILE_CHOOSE_DIALOG_CANCELLED)
					ENUMTOSTRING(EGET_MESSAGEBOX_YES)
					ENUMTOSTRING(EGET_MESSAGEBOX_NO)
					ENUMTOSTRING(EGET_MESSAGEBOX_OK)
					ENUMTOSTRING(EGET_MESSAGEBOX_CANCEL)
					ENUMTOSTRING(EGET_EDITBOX_ENTER)
					ENUMTOSTRING(EGET_EDITBOX_CHANGED)
					ENUMTOSTRING(EGET_EDITBOX_MARKING_CHANGED)
					ENUMTOSTRING(EGET_TAB_CHANGED)
					ENUMTOSTRING(EGET_MENU_ITEM_SELECTED)
					ENUMTOSTRING(EGET_COMBO_BOX_CHANGED)
					ENUMTOSTRING(EGET_SPINBOX_CHANGED)
					ENUMTOSTRING(EGET_TABLE_CHANGED)
					ENUMTOSTRING(EGET_TABLE_HEADER_CHANGED)
					ENUMTOSTRING(EGET_TABLE_SELECTED_AGAIN)
					ENUMTOSTRING(EGET_TREEVIEW_NODE_DESELECT)
					ENUMTOSTRING(EGET_TREEVIEW_NODE_SELECT)
					ENUMTOSTRING(EGET_TREEVIEW_NODE_EXPAND)
					ENUMTOSTRING(EGET_TREEVIEW_NODE_COLLAPSE)
			}
		} break;

		case EET_MOUSE_INPUT_EVENT:
		{
			switch (e.MouseInput.Event)
			{
				ENUMTOSTRING(EMIE_LMOUSE_PRESSED_DOWN)
					ENUMTOSTRING(EMIE_RMOUSE_PRESSED_DOWN)
					ENUMTOSTRING(EMIE_MMOUSE_PRESSED_DOWN)
					ENUMTOSTRING(EMIE_LMOUSE_LEFT_UP)
					ENUMTOSTRING(EMIE_RMOUSE_LEFT_UP)
					ENUMTOSTRING(EMIE_MMOUSE_LEFT_UP)
					ENUMTOSTRING(EMIE_MOUSE_MOVED)
					ENUMTOSTRING(EMIE_MOUSE_WHEEL)
					ENUMTOSTRING(EMIE_LMOUSE_DOUBLE_CLICK)
					ENUMTOSTRING(EMIE_RMOUSE_DOUBLE_CLICK)
					ENUMTOSTRING(EMIE_MMOUSE_DOUBLE_CLICK)
					ENUMTOSTRING(EMIE_LMOUSE_TRIPLE_CLICK)
					ENUMTOSTRING(EMIE_RMOUSE_TRIPLE_CLICK)
					ENUMTOSTRING(EMIE_MMOUSE_TRIPLE_CLICK)
			}
		} break;
		}

		// unknown event type
		return stringc("Unknown event?");
	}

	// simpler irrlicht SEvent logging
	void FSLogger::logEvent(stringc text, SEvent e, int id, stringc name)
	{
		switch (e.EventType)
		{
		case EET_MOUSE_INPUT_EVENT:	log(FSL_EVENT, "MOUSE_EVENT   %d %s %s %s", id, name.c_str(), text.c_str(), getEventType(e)); break;
		case EET_KEY_INPUT_EVENT:
			if (e.KeyInput.PressedDown)	log(FSL_EVENT, "KEYDOWN       %d %s %s (%s)", id, name.c_str(), text.c_str(), keyCodeToStringc(e.KeyInput.Key));
			else						log(FSL_EVENT, "KEYUP         %d %s %s (%s)", id, name.c_str(), text.c_str(), keyCodeToStringc(e.KeyInput.Key));
			break;
		case EET_GUI_EVENT:	log(FSL_EVENT, "GUI_EVENT     %d %s %s from %d (%s)", id, name.c_str(), text.c_str(), e.GUIEvent.Caller->getID(), getEventType(e)); break;
		case EET_USER_EVENT:	log(FSL_EVENT, "USER_EVENT    %d %s %s", id, name.c_str(), text.c_str()); break;
		case EET_LOG_TEXT_EVENT:	log(FSL_EVENT, "LOG_EVENT     %d %s %s %s", id, name.c_str(), text.c_str(), e.LogEvent.Text); break;
		default:						log(FSL_EVENT, "UNKNOWN_EVENT %d %s %s", id, name.c_str(), text.c_str()); break;
		}
	}

	// simple keycode to string converter
	// http://irrlicht.sourceforge.net/forum/viewtopic.php?t=34584
	stringc FSLogger::keyCodeToStringc(EKEY_CODE e)
	{
		if (e == KEY_LBUTTON)    return stringc("KEY_LBUTTON");
		if (e == KEY_RBUTTON)    return stringc("KEY_RBUTTON");
		if (e == KEY_CANCEL)     return stringc("KEY_CANCEL");
		if (e == KEY_MBUTTON)    return stringc("KEY_MBUTTON");
		if (e == KEY_XBUTTON1)   return stringc("KEY_XBUTTON1");
		if (e == KEY_XBUTTON2)   return stringc("KEY_XBUTTON2");
		if (e == KEY_BACK)       return stringc("KEY_BACK");
		if (e == KEY_TAB)        return stringc("KEY_TAB");
		if (e == KEY_CLEAR)      return stringc("KEY_CLEAR");
		if (e == KEY_RETURN)     return stringc("KEY_RETURN");
		if (e == KEY_SHIFT)      return stringc("KEY_SHIFT");
		if (e == KEY_CONTROL)    return stringc("KEY_CONTROL");
		if (e == KEY_MENU)       return stringc("KEY_MENU");
		if (e == KEY_PAUSE)      return stringc("KEY_PAUSE");
		if (e == KEY_CAPITAL)    return stringc("KEY_CAPITAL");
		if (e == KEY_KANA)       return stringc("KEY_KANA");
		if (e == KEY_HANGUEL)    return stringc("KEY_HANGUEL");
		if (e == KEY_HANGUL)     return stringc("KEY_HANGUL");
		if (e == KEY_JUNJA)      return stringc("KEY_JUNJA");
		if (e == KEY_FINAL)      return stringc("KEY_FINAL");
		if (e == KEY_HANJA)      return stringc("KEY_HANJA");
		if (e == KEY_KANJI)      return stringc("KEY_KANJI");
		if (e == KEY_ESCAPE)     return stringc("KEY_ESCAPE");
		if (e == KEY_CONVERT)    return stringc("KEY_CONVERT");
		if (e == KEY_NONCONVERT) return stringc("KEY_NONCONVERT");
		if (e == KEY_ACCEPT)     return stringc("KEY_ACCEPT");
		if (e == KEY_MODECHANGE) return stringc("KEY_MODECHANGE");
		if (e == KEY_SPACE)      return stringc("KEY_SPACE");
		if (e == KEY_PRIOR)      return stringc("KEY_PRIOR");
		if (e == KEY_NEXT)       return stringc("KEY_NEXT");
		if (e == KEY_END)        return stringc("KEY_END");
		if (e == KEY_HOME)       return stringc("KEY_HOME");
		if (e == KEY_LEFT)       return stringc("KEY_LEFT");
		if (e == KEY_UP)         return stringc("KEY_UP");
		if (e == KEY_RIGHT)      return stringc("KEY_RIGHT");
		if (e == KEY_DOWN)       return stringc("KEY_DOWN");
		if (e == KEY_SELECT)     return stringc("KEY_SELECT");
		if (e == KEY_PRINT)      return stringc("KEY_PRINT");
		if (e == KEY_EXECUT)     return stringc("KEY_EXECUT");
		if (e == KEY_SNAPSHOT)   return stringc("KEY_SNAPSHOT");
		if (e == KEY_INSERT)     return stringc("KEY_INSERT");
		if (e == KEY_DELETE)     return stringc("KEY_DELETE");
		if (e == KEY_HELP)       return stringc("KEY_HELP");
		if (e == KEY_KEY_0)      return stringc("KEY_KEY_0");
		if (e == KEY_KEY_1)      return stringc("KEY_KEY_1");
		if (e == KEY_KEY_2)      return stringc("KEY_KEY_2");
		if (e == KEY_KEY_3)      return stringc("KEY_KEY_3");
		if (e == KEY_KEY_4)      return stringc("KEY_KEY_4");
		if (e == KEY_KEY_5)      return stringc("KEY_KEY_5");
		if (e == KEY_KEY_6)      return stringc("KEY_KEY_6");
		if (e == KEY_KEY_7)      return stringc("KEY_KEY_7");
		if (e == KEY_KEY_8)      return stringc("KEY_KEY_8");
		if (e == KEY_KEY_9)      return stringc("KEY_KEY_9");
		if (e == KEY_KEY_A)      return stringc("KEY_KEY_A");
		if (e == KEY_KEY_B)      return stringc("KEY_KEY_B");
		if (e == KEY_KEY_C)      return stringc("KEY_KEY_C");
		if (e == KEY_KEY_D)      return stringc("KEY_KEY_D");
		if (e == KEY_KEY_E)      return stringc("KEY_KEY_E");
		if (e == KEY_KEY_F)      return stringc("KEY_KEY_F");
		if (e == KEY_KEY_G)      return stringc("KEY_KEY_G");
		if (e == KEY_KEY_H)      return stringc("KEY_KEY_H");
		if (e == KEY_KEY_I)      return stringc("KEY_KEY_I");
		if (e == KEY_KEY_J)      return stringc("KEY_KEY_J");
		if (e == KEY_KEY_K)      return stringc("KEY_KEY_K");
		if (e == KEY_KEY_L)      return stringc("KEY_KEY_L");
		if (e == KEY_KEY_M)      return stringc("KEY_KEY_M");
		if (e == KEY_KEY_N)      return stringc("KEY_KEY_N");
		if (e == KEY_KEY_O)      return stringc("KEY_KEY_O");
		if (e == KEY_KEY_P)      return stringc("KEY_KEY_P");
		if (e == KEY_KEY_Q)      return stringc("KEY_KEY_Q");
		if (e == KEY_KEY_R)      return stringc("KEY_KEY_R");
		if (e == KEY_KEY_S)      return stringc("KEY_KEY_S");
		if (e == KEY_KEY_T)      return stringc("KEY_KEY_T");
		if (e == KEY_KEY_U)      return stringc("KEY_KEY_U");
		if (e == KEY_KEY_V)      return stringc("KEY_KEY_V");
		if (e == KEY_KEY_W)      return stringc("KEY_KEY_W");
		if (e == KEY_KEY_X)      return stringc("KEY_KEY_X");
		if (e == KEY_KEY_Y)      return stringc("KEY_KEY_Y");
		if (e == KEY_KEY_Z)      return stringc("KEY_KEY_Z");
		if (e == KEY_LWIN)       return stringc("KEY_LWIN");
		if (e == KEY_RWIN)       return stringc("KEY_RWIN");
		if (e == KEY_APPS)       return stringc("KEY_APPS");
		if (e == KEY_SLEEP)      return stringc("KEY_SLEEP");
		if (e == KEY_NUMPAD0)    return stringc("KEY_NUMPAD0");
		if (e == KEY_NUMPAD1)    return stringc("KEY_NUMPAD1");
		if (e == KEY_NUMPAD2)    return stringc("KEY_NUMPAD2");
		if (e == KEY_NUMPAD3)    return stringc("KEY_NUMPAD3");
		if (e == KEY_NUMPAD4)    return stringc("KEY_NUMPAD4");
		if (e == KEY_NUMPAD5)    return stringc("KEY_NUMPAD5");
		if (e == KEY_NUMPAD6)    return stringc("KEY_NUMPAD6");
		if (e == KEY_NUMPAD7)    return stringc("KEY_NUMPAD7");
		if (e == KEY_NUMPAD8)    return stringc("KEY_NUMPAD8");
		if (e == KEY_NUMPAD9)    return stringc("KEY_NUMPAD9");
		if (e == KEY_MULTIPLY)   return stringc("KEY_MULTIPLY");
		if (e == KEY_ADD)        return stringc("KEY_ADD");
		if (e == KEY_SEPARATOR)  return stringc("KEY_SEPARATOR");
		if (e == KEY_SUBTRACT)   return stringc("KEY_SUBTRACT");
		if (e == KEY_DECIMAL)    return stringc("KEY_DECIMAL");
		if (e == KEY_DIVIDE)     return stringc("KEY_DIVIDE");
		if (e == KEY_F1)         return stringc("KEY_F1");
		if (e == KEY_F2)         return stringc("KEY_F2");
		if (e == KEY_F3)         return stringc("KEY_F3");
		if (e == KEY_F4)         return stringc("KEY_F4");
		if (e == KEY_F5)         return stringc("KEY_F5");
		if (e == KEY_F6)         return stringc("KEY_F6");
		if (e == KEY_F7)         return stringc("KEY_F7");
		if (e == KEY_F8)         return stringc("KEY_F8");
		if (e == KEY_F9)         return stringc("KEY_F9");
		if (e == KEY_F10)        return stringc("KEY_F10");
		if (e == KEY_F11)        return stringc("KEY_F11");
		if (e == KEY_F12)        return stringc("KEY_F12");
		if (e == KEY_F13)        return stringc("KEY_F13");
		if (e == KEY_F14)        return stringc("KEY_F14");
		if (e == KEY_F15)        return stringc("KEY_F15");
		if (e == KEY_F16)        return stringc("KEY_F16");
		if (e == KEY_F17)        return stringc("KEY_F17");
		if (e == KEY_F18)        return stringc("KEY_F18");
		if (e == KEY_F19)        return stringc("KEY_F19");
		if (e == KEY_F20)        return stringc("KEY_F20");
		if (e == KEY_F21)        return stringc("KEY_F21");
		if (e == KEY_F22)        return stringc("KEY_F22");
		if (e == KEY_F23)        return stringc("KEY_F23");
		if (e == KEY_F24)        return stringc("KEY_F24");
		if (e == KEY_NUMLOCK)    return stringc("KEY_NUMLOCK");
		if (e == KEY_SCROLL)     return stringc("KEY_SCROLL");
		if (e == KEY_LSHIFT)     return stringc("KEY_LSHIFT");
		if (e == KEY_RSHIFT)     return stringc("KEY_RSHIFT");
		if (e == KEY_LCONTROL)   return stringc("KEY_LCONTROL");
		if (e == KEY_RCONTROL)   return stringc("KEY_RCONTROL");
		if (e == KEY_LMENU)      return stringc("KEY_LMENU");
		if (e == KEY_RMENU)      return stringc("KEY_RMENU");
		if (e == KEY_PLUS)       return stringc("KEY_PLUS");
		if (e == KEY_COMMA)      return stringc("KEY_COMMA");
		if (e == KEY_MINUS)      return stringc("KEY_MINUS");
		if (e == KEY_PERIOD)     return stringc("KEY_PERIOD");
		if (e == KEY_ATTN)       return stringc("KEY_ATTN");
		if (e == KEY_CRSEL)      return stringc("KEY_CRSEL");
		if (e == KEY_EXSEL)      return stringc("KEY_EXSEL");
		if (e == KEY_EREOF)      return stringc("KEY_EREOF");
		if (e == KEY_PLAY)       return stringc("KEY_PLAY");
		if (e == KEY_ZOOM)       return stringc("KEY_ZOOM");
		if (e == KEY_PA1)        return stringc("KEY_PA1");
		if (e == KEY_OEM_CLEAR)  return stringc("KEY_OEM_CLEAR");
		return stringc("UNKNOWN");
	}
} // end namespace



