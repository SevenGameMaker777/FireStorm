// include this file only once
#pragma once

// incldue the needed header files
#include "FSUtils.h"
#include "FSLogger.h"

// unclutter the global namespace
namespace FS
{
	// simple define to make typing all of this easier
	// if the function return true, then return true otherwise return false
	#define FS_DEFAULT_EVENTHANDLER(x,y)	case x: return (##y(e)); break;

	#define FS_DEFAULT_EVENTHANDLER_GUI(x,y)		case x: if (getLogGui()) { FS_LOG_EVENT(#x, e, getEventHandlerId(), getEventHandlerName()); } return (##y(e)); break;
	#define FS_DEFAULT_EVENTHANDLER_MOUSE(x,y)		case x: if (getLogMouse()) { FS_LOG_EVENT(#x, e, getEventHandlerId(), getEventHandlerName()); } return (##y(e)); break;		
	#define FS_DEFAULT_EVENTHANDLER_MOUSEMOVE(x,y)	case x: if (getLogMouseMove()) { FS_LOG_EVENT(#x, e, getEventHandlerId(), getEventHandlerName()); } return (##y(e)); break;
	#define FS_DEFAULT_EVENTHANDLER_EX(x,y)			case x: if (getLogUserEvent()) { FS_LOG_EVENT(#x, e, getEventHandlerId(), getEventHandlerName()); } return (##y(e)); break;
	#define FS_DEFAULT_EVENTHANDLER_USER(x,y)		case x: if (getLogUserEvent()) { FS_LOG_EVENT(#x, e, getEventHandlerId(), getEventHandlerName()); } return (##y(e)); break;
	#define FS_DEFAULT_EVENTHANDLER_TEXTLOG(x,y)	case x: if (getLogTextlogEvent()) { FS_LOG_EVENT(#x, e, getEventHandlerId(), getEventHandlerName()); } return (##y(e)); break;

	// variable declaration
	#define FS_VARIABLE(x,y) private : x m_##y;
	#define FS_VARIABLE_GET(x,y) private : x m_##y; public : virtual x get##y() { return m_##y; }
	#define FS_VARIABLE_SET(x,y) private : x m_##y; public : virtual void set##y(x value) { m_##y = value; }
	#define FS_VARIABLE_SETGET(x,y) private : x m_##y; public : virtual void set##y(x value) { m_##y = value; } virtual x get##y() { return m_##y; }

	/*
	class to parse irrlicht events into class function calls
	*/
	class FSEventHandler : public IEventReceiver
	{
	private:
		FS_VARIABLE_SETGET(int, EventHandlerId);				// the id of this event handler (for logging purposes)
		FS_VARIABLE_SETGET(stringc, EventHandlerName);			// the name of this event handler (for logging purposes)
		FS_VARIABLE_SETGET(IEventReceiver*, ReceiverParent);	// the parent of this handler
		FS_VARIABLE_SETGET(bool, LogKeyboard);					// toggle keyboard event logging
		FS_VARIABLE_SETGET(bool, LogGui);						// toggle gui event logging
		FS_VARIABLE_SETGET(bool, LogMouse);						// toggle mouse event logging
		FS_VARIABLE_SETGET(bool, LogMouseMove);					// toggle mouse movement event logging
		FS_VARIABLE_SETGET(bool, LogUserEvent);					// toggle event logging
		FS_VARIABLE_SETGET(bool, LogTextlogEvent);				// toggle event logging

																// Enumeration for UP, DOWN, PRESSED and RELEASED key states. Also used for mouse button states.
		enum keyStatesENUM { UP, DOWN, PRESSED, RELEASED };

		// Keyboard key states.
		keyStatesENUM keyState[KEY_KEY_CODES_COUNT];

	public:
		// constructor
		FSEventHandler::FSEventHandler():IEventReceiver()
		{
			// remeber these
			m_EventHandlerId = 0;
			m_EventHandlerName = FS_DEFAULT;
			for (int i = 0; i < KEY_KEY_CODES_COUNT; i++) keyState[i] = RELEASED;
			m_ReceiverParent = 0;
			setLogGui(false);
			setLogMouse(false);
			setLogMouseMove(false);
			setLogKeyboard(false);
			setLogUserEvent(false);
			setLogTextlogEvent(false);
		}

		// destructor
		virtual FSEventHandler::~FSEventHandler()
		{
			m_EventHandlerName = "DELETED";		// change to deleted for debugging
			m_ReceiverParent = 0;	// forget the parent
		}

		// simple default event handler functions
		virtual bool onElementLostFocus(const SEvent & e) { return false; }
		virtual bool onElementFocused(const SEvent & e) { return false; }
		virtual bool onElementHovered(const SEvent & e) { return false; }
		virtual bool onElementLeft(const SEvent & e) { return false; }
		virtual bool onElementClosed(const SEvent & e) { return false; }
		virtual bool onButtonClicked(const SEvent & e) { return false; }
		virtual bool onScrollbarChanged(const SEvent & e) { return false; }
		virtual bool onCheckboxChanged(const SEvent & e) { return false; }
		virtual bool onListboxChanged(const SEvent & e) { return false; }
		virtual bool onListboxSelectedAgain(const SEvent & e) { return false; }
		virtual bool onFileSelected(const SEvent & e) { return false; }
		virtual bool onDirectorySelected(const SEvent & e) { return false; }
		virtual bool onFileChooseDialogCancelled(const SEvent & e) { return false; }
		virtual bool onMessageboxYes(const SEvent & e) { return false; }
		virtual bool onMessageboxNo(const SEvent & e) { return false; }
		virtual bool onMessageboxOk(const SEvent & e) { return false; }
		virtual bool onMessageboxCancel(const SEvent & e) { return false; }
		virtual bool onEditboxEnter(const SEvent & e) { return false; }
		virtual bool onEditboxChanged(const SEvent & e) { return false; }
		virtual bool onEditboxMarkingChanged(const SEvent & e) { return false; }
		virtual bool onTabChanged(const SEvent & e) { return false; }
		virtual bool onMenuItemSelected(const SEvent & e) { return false; }
		virtual bool onComboboxChanged(const SEvent & e) { return false; }
		virtual bool onSpinboxChanged(const SEvent & e) { return false; }
		virtual bool onTableChanged(const SEvent & e) { return false; }
		virtual bool onTableHeaderChanged(const SEvent & e) { return false; }
		virtual bool onTableSelectedAgain(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeDeselect(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeSelect(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeExpand(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeCollapse(const SEvent & e) { return false; }

		virtual bool onLMouseButtonDown(const SEvent & e) { return false; }
		virtual bool onRMouseButtonDown(const SEvent & e) { return false; }
		virtual bool onMMouseButtonDown(const SEvent & e) { return false; }
		virtual bool onLMouseButtonUp(const SEvent & e) { return false; }
		virtual bool onRMouseButtonUp(const SEvent & e) { return false; }
		virtual bool onMMouseButtonUp(const SEvent & e) { return false; }
		virtual bool onMouseMoved(const SEvent & e) { return false; }
		virtual bool onMouseWheel(const SEvent & e) { return false; }
		virtual bool onLMouseDoubleClick(const SEvent & e) { return false; }
		virtual bool onRMouseDoubleClick(const SEvent & e) { return false; }
		virtual bool onMMouseDoubleClick(const SEvent & e) { return false; }
		virtual bool onLMouseTripleClick(const SEvent & e) { return false; }
		virtual bool onRMouseTripleClick(const SEvent & e) { return false; }
		virtual bool onMMouseTripleClick(const SEvent & e) { return false; }

		virtual bool onUserEvent(const SEvent & e)
		{
			switch (e.UserEvent.UserData1)
			{
				FS_DEFAULT_EVENTHANDLER_USER(EVT_SCREENRESIZED, onScreenResized);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_LEVEL_LOAD, onLevelLoad);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_LEVEL_UNLOAD, onLevelUnload);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_LEVEL_DELETED, onLevelDeleted);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_LEVEL_REMOVED, onLevelRemoved);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_LEVEL_ADDED, onLevelAdded);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_LEVEL_CHANGED, onLevelChanged);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_OBJECT_DESTROYED, onObjectDestroyed);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_OBJECT_REMOVED, onObjectRemoved);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_OBJECT_ADDED, onObjectAdded);
				FS_DEFAULT_EVENTHANDLER_EX(EVT_OBJECT_SELECTED, onObjectSelected);
			}
			return false;
		}

		virtual bool onScreenResized(const SEvent & e) { return false; }
		virtual bool onLevelLoad(const SEvent & e) { return false; }
		virtual bool onLevelUnload(const SEvent & e) { return false; }
		virtual bool onLevelDeleted(const SEvent & e) { return false; }
		virtual bool onLevelRemoved(const SEvent & e) { return false; }
		virtual bool onLevelAdded(const SEvent & e) { return false; }
		virtual bool onLevelChanged(const SEvent & e) { return false; }
		virtual bool onObjectDestroyed(const SEvent & e) { return false; }
		virtual bool onObjectRemoved(const SEvent & e) { return false; }
		virtual bool onObjectAdded(const SEvent & e) { return false; }
		virtual bool onObjectSelected(const SEvent & e) { return false; }

		virtual bool onJoystickInputEvent(const SEvent & e) { return false; }
		virtual bool onLogTextEvent(const SEvent & e) { return false; }

		virtual bool onMouseEvent(const SEvent & e)
		{
			// determine which mouse event type
			switch (e.MouseInput.Event)
			{
				// call the appropriate function call
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_LMOUSE_PRESSED_DOWN, onLMouseButtonDown);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_RMOUSE_PRESSED_DOWN, onRMouseButtonDown);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_MMOUSE_PRESSED_DOWN, onMMouseButtonDown);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_LMOUSE_LEFT_UP, onLMouseButtonUp);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_RMOUSE_LEFT_UP, onRMouseButtonUp);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_MMOUSE_LEFT_UP, onMMouseButtonUp);
				FS_DEFAULT_EVENTHANDLER_MOUSEMOVE(EMIE_MOUSE_MOVED, onMouseMoved);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_MOUSE_WHEEL, onMouseWheel);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_LMOUSE_DOUBLE_CLICK, onLMouseDoubleClick);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_RMOUSE_DOUBLE_CLICK, onRMouseDoubleClick);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_MMOUSE_DOUBLE_CLICK, onMMouseDoubleClick);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_LMOUSE_TRIPLE_CLICK, onLMouseTripleClick);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_RMOUSE_TRIPLE_CLICK, onRMouseTripleClick);
				FS_DEFAULT_EVENTHANDLER_MOUSE(EMIE_MMOUSE_TRIPLE_CLICK, onMMouseTripleClick);
			}
			return false;
		}

		virtual bool onGUIEvent(const SEvent& e)
		{
			// determine which gui event type
			switch (e.GUIEvent.EventType)
			{
				// call the appropriate function call
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_ELEMENT_FOCUS_LOST, onElementLostFocus);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_ELEMENT_FOCUSED, onElementFocused);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_ELEMENT_HOVERED, onElementHovered);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_ELEMENT_LEFT, onElementLeft);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_ELEMENT_CLOSED, onElementClosed);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_BUTTON_CLICKED, onButtonClicked);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_SCROLL_BAR_CHANGED, onScrollbarChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_CHECKBOX_CHANGED, onCheckboxChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_LISTBOX_CHANGED, onListboxChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_LISTBOX_SELECTED_AGAIN, onListboxSelectedAgain);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_FILE_SELECTED, onFileSelected);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_DIRECTORY_SELECTED, onDirectorySelected);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_FILE_CHOOSE_DIALOG_CANCELLED, onFileChooseDialogCancelled);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_MESSAGEBOX_YES, onMessageboxYes);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_MESSAGEBOX_NO, onMessageboxNo);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_MESSAGEBOX_OK, onMessageboxOk);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_MESSAGEBOX_CANCEL, onMessageboxCancel);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_EDITBOX_ENTER, onEditboxEnter);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_EDITBOX_CHANGED, onEditboxChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_EDITBOX_MARKING_CHANGED, onEditboxMarkingChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TAB_CHANGED, onTabChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_MENU_ITEM_SELECTED, onMenuItemSelected);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_COMBO_BOX_CHANGED, onComboboxChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_SPINBOX_CHANGED, onSpinboxChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TABLE_CHANGED, onTableChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TABLE_HEADER_CHANGED, onTableHeaderChanged);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TABLE_SELECTED_AGAIN, onTableSelectedAgain);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TREEVIEW_NODE_DESELECT, onTreeviewNodeDeselect);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TREEVIEW_NODE_SELECT, onTreeviewNodeSelect);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TREEVIEW_NODE_EXPAND, onTreeviewNodeExpand);
				FS_DEFAULT_EVENTHANDLER_GUI(EGET_TREEVIEW_NODE_COLLAPSE, onTreeviewNodeCollapse);
			}
			return false;
		}

		virtual bool onKeyboardEvent(const SEvent& e)
		{
			if (getLogKeyboard()) { FS_LOG_EVENT("EET_KEY_INPUT_EVENT", e, getEventHandlerId(), getEventHandlerName()); }

			if (e.KeyInput.PressedDown == true)
			{
				if (keyState[e.KeyInput.Key] != DOWN)
					keyState[e.KeyInput.Key] = PRESSED;
				else
					keyState[e.KeyInput.Key] = DOWN;
			}
			else
				if (keyState[e.KeyInput.Key] != UP)
					keyState[e.KeyInput.Key] = RELEASED;
			if (e.KeyInput.PressedDown == true)
			{
				if (OnKeyInputPressedEvent(e)) return true;
			}
			else
			{
				if (OnKeyInputReleasedEvent(e)) return true;
			}
			return false;
		}

		// keyboard events
		virtual bool OnKeyInputPressedEvent(const SEvent& e) { return false; };
		virtual bool OnKeyInputReleasedEvent(const SEvent& e) { return false; };
		virtual bool IsKeyPressed(EKEY_CODE keycode) { return (keyState[keycode] == PRESSED); };
		virtual bool IsKeyDown(EKEY_CODE keycode) { return (keyState[keycode] == DOWN || keyState[keycode] == PRESSED); };
		virtual bool IsKeyUp(EKEY_CODE keycode) { return (keyState[keycode] == UP || keyState[keycode] == RELEASED); };
		virtual bool IsKeyReleased(EKEY_CODE keycode) { return (keyState[keycode] == RELEASED); };

		// override the onevent and parse the events into function calls
		virtual bool OnEvent(const SEvent& e)
		{
			switch (e.EventType)
			{
				// if this is a gui event
			case EET_GUI_EVENT:
			{
				return onGUIEvent(e);
			} break;
			case EET_MOUSE_INPUT_EVENT:
			{
				return onMouseEvent(e);
			}
			case EET_KEY_INPUT_EVENT:
			{
				return onKeyboardEvent(e);
			} break;

			case EET_JOYSTICK_INPUT_EVENT:
			{
				if (onJoystickInputEvent(e)) return true;
			} break;
			case EET_LOG_TEXT_EVENT:
			{
				if (getLogTextlogEvent()) { FS_LOG_EVENT("EET_LOG_TEXT_EVENT", e, getEventHandlerId(), getEventHandlerName()); }
				if (onLogTextEvent(e)) return true;
			} break;
			case EET_USER_EVENT:
			{
				if (getLogUserEvent())
				{
					FS_LOG_EVENT("EET_USER_EVENT", e, getEventHandlerId(), getEventHandlerName());
				}
				if (onUserEvent(e)) return true;
			} break;
			}

			// we did not handle this event
			return false;
		}
	};

} // end namespace
