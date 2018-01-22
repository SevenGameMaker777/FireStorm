#pragma once

#include "FSUtils.h"
#include "FSEventHandler.h"

namespace FS
{
	typedef class FSGUI_Desktop FSDesktop;
	typedef class FSGUI_Titlebar FSGUI_Titlebar;
	typedef class FSGUI_Menubar FSGUI_Menubar;
	typedef class FSGUI_Systembar FSGUI_Systembar;
	typedef class FSGUI_Toolbar FSGUI_Toolbar;
	typedef class FSGUI_Scrollbar FSGUI_Scrollbar;


	enum FSGUI_LAYOUTSTYLE { LS_NONE, LS_REST, LS_FILL, LS_TOPFILL, LS_BOTTOMFILL, LS_LEFTFILL, LS_RIGHTFILL };
	enum FSGUI_SIZEDIR { MP_CLIENT, MP_LEFT, MP_TOP, MP_RIGHT, MP_BOTTOM, MP_NONE };

	enum FSGUI_DRAGDROPINFO_TYPE { DDIT_BASE, DDIT_FILENAME };
	enum FSGUI_DRAGDROPINFO_TYPE2 { DDIT2_TEXTURE, DDIT2_STATIC, DDIT2_ANIMATED };
	class FSGUI_DRAGDROPINFO
	{
		FS_VARIABLE_GET(FSGUI_Desktop*, Desktop);
		FS_VARIABLE_GET(FSGUI_DRAGDROPINFO_TYPE, Type);
		FS_VARIABLE_GET(FSGUI_DRAGDROPINFO_TYPE2, Type2);
		FS_VARIABLE_GET(int, From);
		FS_VARIABLE_GET(int, ControlID);
		FS_VARIABLE_GET(stringc, Text);
		FS_VARIABLE_GET(IGUIStaticText*, TextElement);

	public:
		FSGUI_DRAGDROPINFO(FSGUI_Desktop* desktop, FSGUI_DRAGDROPINFO_TYPE type, FSGUI_DRAGDROPINFO_TYPE2 type2, int from, int controlid, stringc text);
		virtual ~FSGUI_DRAGDROPINFO();
	};

	class FSGUI_DRAGDROPINFO_FILENAME : public FSGUI_DRAGDROPINFO
	{
		FS_VARIABLE_GET(stringc, Filename);
		FS_VARIABLE_GET(FilePickerType, Filetype);
	public:
		FSGUI_DRAGDROPINFO_FILENAME(FSGUI_Desktop* desktop, FSGUI_DRAGDROPINFO_TYPE2 type2, int from, int controlid, FilePickerType ft, stringc fn)
			: FSGUI_DRAGDROPINFO(desktop, DDIT_FILENAME, type2, from, controlid, fn), m_Filename(fn), m_Filetype(ft)
		{
		}
	};


	class FSGUI_Window : public IGUIElement, public FSEventHandler
	{
	private:
		stringc m_GUIType;
		FSGUI_Desktop* m_Desktop;
		rect<s32> m_ClientRect;
		DWORD m_Flags;
		FSGUI_LAYOUTSTYLE m_LayoutStyle;
		int m_MinWidth;
		int m_MinHeight;
		FSGUI_Window* m_FSGUIParent;
		bool m_Dockable;
		bool m_CanDragDrop;
	public:
		stringc getGUIType() { return m_GUIType; }
		list<FSGUI_Window*> m_FSGUIChildren;
		FS_VARIABLE_GET(FSGUI_Scrollbar*, VertScrollbar);
		FS_VARIABLE_GET(FSGUI_Scrollbar*, HorizScrollbar);

	public:
		rect<s32> getClientRect() { return m_ClientRect; }
		DWORD getFlags() { return m_Flags; }
		FSGUI_LAYOUTSTYLE getLayoutStyle() { return m_LayoutStyle; }
		int getMinWidth() { return m_MinWidth; }
		int getMinHeight() { return m_MinHeight; }
		FSGUI_Window* getFSGUIParent() { return m_FSGUIParent; }
		bool getDockable() { return m_Dockable; }
		bool getCanDragDrop() { return m_CanDragDrop; }
		IGUIEnvironment*	getGui();
		IVideoDriver*		getDriver();
		FSGUI_Desktop*		getDesktop();

		void setLayoutStyle(FSGUI_LAYOUTSTYLE ls) { m_LayoutStyle = ls; }
		void setDockable(bool v) { m_Dockable = v; }
		void setCanDragDrop(bool v) { m_CanDragDrop = v; }
	public:
		FSGUI_Window(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minwidth, int minheight);
		virtual ~FSGUI_Window();

		FSGUI_Window* getFSGUI_WindowAt(int x, int y);

		virtual void draw();
		virtual void drawFrame();
		virtual void drawClientArea();

		virtual rect<s32> updateClientRect();
		virtual void calculateLayout();

		virtual  FSGUI_SIZEDIR mousePosition(int x, int y);
		virtual bool canResize(FSGUI_SIZEDIR mp);

		rect<s32> normalizeToSelf(rect<s32> r);
		rect<s32> normalizeToParent(rect<s32> r);

		virtual	bool isFlagged(long flag) { return (bool)(m_Flags & flag); }
		virtual void setFlag(long flag) { m_Flags |= flag; }
		virtual void removeFlag(long flag) { m_Flags &= ~flag; }
		virtual void setVisible(bool value) 
		{ 
			IGUIElement::setVisible(value); 
			if (value == true) setFlag(WS_VISIBLE); else removeFlag(WS_VISIBLE); 
			list<FSGUI_Window*> c = m_FSGUIChildren;
			if (c.empty()) return;
			list<FSGUI_Window*>::Iterator it = c.begin();
			for (it; it != c.end(); it++) (*it)->setVisible(value);
		}

		stringc	getDirectory(stringc dir);

		void showChildWindow(int id, bool value);

		void addFSGUIChild(FSGUI_Window* child);
		FSGUI_Window* getFSGUIChild(int id);

		virtual FSGUI_DRAGDROPINFO* getDragDropInfo() { return 0; }
		virtual bool acceptDragDropInfo(FSGUI_DRAGDROPINFO* ddi);

		virtual bool onLMouseButtonDown(const SEvent & e);
		virtual bool onRMouseButtonDown(const SEvent & e);

		virtual FSGUI_Titlebar* addTitlebar();
		virtual FSGUI_Menubar* addMenubar();
		virtual FSGUI_Systembar* addSystembar();
		virtual FSGUI_Toolbar* addToolbar(int id, rect<s32> r, FSGUI_LAYOUTSTYLE ls, bool vertical, int spacer);

	public:

		virtual bool OnEvent(const SEvent & e);
		virtual void OnSize(rect<s32> r, bool recalculate = true);
		virtual void OnSizeFSGUIChild(FSGUI_Window* child, rect<s32> r, bool recalculate = false);
		virtual bool onButtonClicked(const SEvent & e);

	};

}