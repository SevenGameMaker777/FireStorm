#pragma once

#include "FSUtils.h"
#include "Defines.h"
#include "FSLogger.h"
#include "FSApplication.h"
#include "FSLevelManager.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_Window.h"
#include "FSGUI_LoggerTabView.h"
#include "FSGUI_Menubar.h"
#include "FSGUI_LevelEditor.h"

#define ID_SHOW_LOGGINGVIEWER 5000
#define ID_SHOW_PREFABVIEWER 5001
#define ID_SHOW_PROPERTYVIEWER 5002
#define ID_SHOW_ASSETVIEWER 5003
#define ID_SHOW_OBJECTVIEWER 5004
#define ID_SHOW_OBJECTMAKER 5005
#define ID_SHOW_MAINCHARACTER 5006

namespace FS
{

	class FSGUI_Editor : public FSGUI_Window
	{
	private:
		FSLevel * m_Level;
		FSGUI_Toolbar * m_Toolbar;
		FSGUI_Menubar* m_Menubar;
		FSGUI_LoggerTabView* m_LogViewer;
		FSGUI_LevelEditor* m_LevelEditor;
	public:
		void setLevel(FSLevel* level)
		{
			m_Level = level;
			if (m_LevelEditor) m_LevelEditor->setLevel(level);
			//onNewLevel(FS_DEFAULT);
		}

		FSLevel* getLevel() { return m_Level; }

		FSGUI_Editor(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minwidth, int minheight, FSLevel* level) :
			FSGUI_Window(desktop, gui, parent, id, r, flags, ls, text, minwidth, minheight)
		{
			m_Level = level;
			createGui();
		}

		void createGui()
		{
			// menubars are unique and need special attention
			// we add our own menu. we had to modify the irrlicht menu slightly so
			// that it functions properly in our framework
			m_Menubar = addMenubar();

			// create the actual menu
			IGUIContextMenu* mb = m_Menubar->getMenu();
			buildMenu(mb);

			// create the application level toolbar - in this case an editor type application
			m_Toolbar = addToolbar(FSID_MAINTOOLBAR, rect<s32>(0, 0, TOOLBAR_HEIGHT, TOOLBAR_HEIGHT), LS_TOPFILL, false, 4);
			IGUIButton* b = (IGUIButton*)m_Toolbar->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), m_Toolbar, FSID_FILE_NEW, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\new.jpg")));
			b->setScaleImage(true);
			b = (IGUIButton*)m_Toolbar->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), m_Toolbar, FSID_FILE_OPEN, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\open.jpg")));
			b->setScaleImage(true);
			b = (IGUIButton*)m_Toolbar->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), m_Toolbar, FSID_FILE_SAVE, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\save.jpg")));
			b->setScaleImage(true);
			b = (IGUIButton*)m_Toolbar->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), m_Toolbar, FSID_FILE_SAVEAS, L""));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\saveas.jpg")));
			b->setScaleImage(true);

			// add a logger view to log all of the messages
			m_LogViewer = new FSGUI_LoggerTabView(getDesktop(), getGui(), this, FSID_LOGGERVIEW, rect<s32>(0, 0, 100, 200), WS_VISIBLE | WS_THICKFRAME | WS_TITLEBAR | WS_SYSTEMBAR, LS_LEFTFILL, 10, 10, L"Logger View");

			// add a level viewer
			m_LevelEditor = new FSGUI_LevelEditor(getDesktop(), getGui(), this, FSID_LEVELVIEWER, rect<s32>(0, 0, 300, 200), WS_VISIBLE | WS_TITLEBAR | WS_EX_TRANSPARENT, LS_REST, L"Level Viewer", 100, 100, getLevel());

			// set the loggerview as the global logger
			// so that all messages go to it
			setGlobalLogger(m_LogViewer);
		}

		virtual bool onMouseWheel(const SEvent & e)
		{
			return FSGUI_Window::onMouseWheel(e);
		}

		// easier to have this function separated
		// and makes the code easier to follow
		void buildMenu(IGUIContextMenu* menu)
		{
			// add the top level menu items
			menu->addItem(L"File", -1, true, true);
			menu->addItem(L"View", 1999, true, true);
			menu->addItem(L"Help", -1, true, true);

			// the FILE menu
			gui::IGUIContextMenu* submenu;
			submenu = menu->getSubMenu(0);
			submenu->addItem(L"New FSLevel File...", FSID_FILE_NEW);
			submenu->addItem(L"Open FSLevel File...", FSID_FILE_OPEN);
			submenu->addItem(L"Save FSLevel File...", FSID_FILE_SAVE);
			submenu->addItem(L"Save SLevel File As...", FSID_FILE_SAVEAS);
			submenu->addSeparator();
			submenu->addItem(L"Quit", FSID_QUIT);

			// the VIEW menu
			submenu = menu->getSubMenu(1);
			submenu->addItem(L"Logging Viewer", ID_SHOW_LOGGINGVIEWER, true, false, false);
			submenu->addItem(L"Prefab Viewer", ID_SHOW_PREFABVIEWER, true, false, false);
			submenu->addItem(L"Poperties Viewer", ID_SHOW_PROPERTYVIEWER, true, false, false);
			submenu->addItem(L"Asset Viewer", ID_SHOW_ASSETVIEWER, true, false, false);
			submenu->addItem(L"Object Viewer", ID_SHOW_OBJECTVIEWER, true, false, false);
			submenu->addItem(L"ObjectMaker Viewer", ID_SHOW_OBJECTMAKER, true, false, false);
			submenu->addItem(L"Main Character Viewer", ID_SHOW_MAINCHARACTER, true, false, false);

			// the HELP menu
			submenu = menu->getSubMenu(2);
			submenu->addItem(L"About", FSID_ABOUT, true, false, false);
		}

		bool onButtonClicked(const SEvent & e)
		{
			stringc path;
			switch (e.GUIEvent.Caller->getID())
			{
				// file new - just clear the current level
			case FSID_FILE_NEW:
				onNewLevel(FS_DEFAULT);
				// we used up this event
				return true;
				break;

				// file open - show a file open dialog box
				// when a file is selected, it will send a fileselected  event
				// that we react to below
			case FSID_FILE_OPEN:
				// open the file dialog in the file save directory
				path = getDirectory("GameSaveDirectory");
				// show the file open dialog
				getGui()->addFileOpenDialog(L"LoadFile", true, this, FSID_FILE_OPEN, true, (char*)path.c_str());
				// we used up this event
				return true;
				break;

				// file save and saveas can be handled the same
				// when a file is selected, it will send a fileselected  event
				// that we react to below
			case FSID_FILE_SAVE:
			case FSID_FILE_SAVEAS:
				// open the file dialog in the file save directory
				path = getDirectory("GameSaveDirectory");
				// show the file open dialog
				getGui()->addFileOpenDialog(L"LoadFile", true, this, FSID_FILE_SAVE, true, (char*)path.c_str());
				// we used up this event
				return true;
				break;
			}
			return FSGUI_Window::onButtonClicked(e);
		}

		// react to this event
		// the user selected something on the menubar
		bool onMenuItemSelected(const SEvent & e)
		{
			stringc path;
			IGUIContextMenu* menu = (IGUIContextMenu*)e.GUIEvent.Caller;

			// based on which menu item we selected
			switch (menu->getItemCommandId(menu->getSelectedItem()))
			{
				// file new - just clear the current level
			case FSID_FILE_NEW:
				onNewLevel(FS_DEFAULT);
				// we used up this event
				return true;
				break;

				// file open - show a file open dialog box
				// when a file is selected, it will send a fileselected  event
				// that we react to below
			case FSID_FILE_OPEN:
				// open the file dialog in the file save directory
				path = getDirectory("GameSaveDirectory");
				// show the file open dialog
				getGui()->addFileOpenDialog(L"LoadFile", true, this, FSID_FILE_OPEN, true, (char*)path.c_str());
				// we used up this event
				return true;
				break;

				// file save and saveas can be handled the same
				// when a file is selected, it will send a fileselected  event
				// that we react to below
			case FSID_FILE_SAVE:
			case FSID_FILE_SAVEAS:
				// open the file dialog in the file save directory
				path = getDirectory("GameSaveDirectory");
				// show the file open dialog
				getGui()->addFileOpenDialog(L"LoadFile", true, this, FSID_FILE_SAVE, true, (char*)path.c_str());
				// we used up this event
				return true;
				break;

				// toggle window visibility
			case ID_SHOW_LOGGINGVIEWER:
				if (m_LogViewer) m_LogViewer->setVisible(!m_LogViewer->isVisible());
				getDesktop()->getDesktopWindow()->calculateLayout();
				// we used up this event
				return true;
				break;
			case ID_SHOW_PREFABVIEWER:
				if (m_LevelEditor)
				{
					FSGUI_Window* w = m_LevelEditor->getFSGUIChild(FSID_PREFABMAKER);
					if (w) w->setVisible(!w->isVisible());
					getDesktop()->getDesktopWindow()->calculateLayout();
				}
				// we used up this event
				return true;
				break;
			case ID_SHOW_PROPERTYVIEWER:
				if (m_LevelEditor)
				{
					FSGUI_Window* w = m_LevelEditor->getFSGUIChild(FSID_PROPERTYGRID);
					if (w) w->setVisible(!w->isVisible());
					getDesktop()->getDesktopWindow()->calculateLayout();
				}
				// we used up this event
				return true;
				break;
			case ID_SHOW_ASSETVIEWER:
				if (m_LevelEditor)
				{
					FSGUI_Window* w = m_LevelEditor->getFSGUIChild(FSID_ASSETVIEWER);
					if (w) w->setVisible(!w->isVisible());
					getDesktop()->getDesktopWindow()->calculateLayout();
				}
				// we used up this event
				return true;
				break;
			case ID_SHOW_OBJECTVIEWER:
				if (m_LevelEditor)
				{
					FSGUI_Window* w = m_LevelEditor->getFSGUIChild(FSID_OBJECTVIEWER);
					if (w) w->setVisible(!w->isVisible());
					getDesktop()->getDesktopWindow()->calculateLayout();
				}
				// we used up this event
				return true;
				break;
			case ID_SHOW_OBJECTMAKER:
				if (m_LevelEditor)
				{
					FSGUI_Window* w = m_LevelEditor->getFSGUIChild(FSID_OBJECTMAKER);
					if (w) w->setVisible(!w->isVisible());
					getDesktop()->getDesktopWindow()->calculateLayout();
				} break;

				// we used up this event
				return true;
				break;

			}

			// call the base class
			return FSGUI_Window::onMenuItemSelected(e);
		}

		// react to this event
		bool onFileSelected(const SEvent & e)
		{
			// a few temproary variables
			IGUIFileOpenDialog* f;
			stringc filename;

			// based on which file dialog is sending the event
			switch (e.GUIEvent.Caller->getID())
			{
				// open a file
			case FSID_FILE_OPEN:
				// get a pointer to the file dialog
				f = (IGUIFileOpenDialog*)e.GUIEvent.Caller;
				// extract the selected filename
				filename = SHORTGAMESAVEPATH(stringc(f->getFileName()));
				// load the level
				onNewLevel(filename);
				// we used this event
				return true;
				break;

				// file save and saveas can be handled the same
			case FSID_FILE_SAVE:
			case FSID_FILE_SAVEAS:
				// get a pointer to the file dialog
				f = (IGUIFileOpenDialog*)e.GUIEvent.Caller;
				// extract the selected filename
				filename = f->getFileName();
				// save the level to disk
				if (getLevel()) getLevel()->saveToDisk(SHORTGAMESAVEPATH(stringc(filename)));
				// we used this event
				return true;
				break;
			}

			// call the base class
			return FSGUI_Window::onFileSelected(e);
		}

		// load a new level
		void onNewLevel(stringc filename)
		{
			FS_LOG(FSL_ERROR, "On New Level");

			// if we have an active level
			if (getLevel())
			{
				// set the selected object to 0
				getLevel()->setSelectedObject(0);

				// clear the level of all objects, messages, events, etc...
				getLevel()->clear();

				// load the level from file
				if (filename != FS_DEFAULT) getLevel()->loadFromDisk(GAMESAVEPATH(filename));
				else
				{
					getLevel()->getSmgr()->getActiveCamera()->setPosition(vector3df(0, 100, 100));
					getLevel()->getSmgr()->getActiveCamera()->setTarget(vector3df(0, 0, 0));
				}
			}
		}
	};



} // end namepsace