// incldue this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FSEventhandler.h"
#include "FSLogger.h"
#include "FSGUI_Window.h"

// unclutter the global namespace
namespace FS
{
	typedef class FSApplication FSApplication;

	class FSGUI_Desktop : public FSEventHandler
	{
	private:
		FS_VARIABLE_GET(FSApplication*, Application);
		FS_VARIABLE_SETGET(FSGUI_Window*, DesktopWindow);
		FS_VARIABLE_SETGET(FSGUI_Window*, Focus);
		FS_VARIABLE_SETGET(bool, Sizing);
		FS_VARIABLE_SETGET(FSGUI_SIZEDIR, SizeDir);
		FS_VARIABLE_SETGET(bool, Docking);
		FS_VARIABLE_SETGET(FSGUI_LAYOUTSTYLE, OldLayoutStyle);
		FS_VARIABLE_SETGET(rect<s32>, OldWindowSize);

		list<IGUIElement*> m_RenderLastList;
		FS_VARIABLE_SETGET(bool, UseRenderLast);
		FS_VARIABLE_SETGET(FSGUI_Window*, TempRenderLast);

		FS_VARIABLE_SETGET(bool, Dragging);
		FS_VARIABLE_GET(FSGUI_DRAGDROPINFO*, DragDropInfo);

	public:	// public access

		void beginDragging(FSGUI_DRAGDROPINFO* ddi);
		void endDragging(int mousex, int mousey);
		void setDragDropInfo(FSGUI_DRAGDROPINFO* ddi);

		bool getHoldMouse()
		{
			if (getSizing()) return true;
			if (getDocking()) return true;
			if (getDragging()) return true;
			return false;
		}

		// class constructor / destructor
		FSGUI_Desktop() :FSEventHandler() { /* do nothing constructor */ FS_LOG(FSL_DEBUG, "FSGUI_Desktop::FSGUI_Desktop()"); }
		virtual ~FSGUI_Desktop() { /* do nothing destructor */  FS_LOG(FSL_DEBUG, "FSGUI_Desktop::~FSGUI_Desktop()"); }

		// set all variables to a known value
		virtual void initialize();

		// allows for better error handling
		virtual bool create(FSApplication* app);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

		void draw();

		void addToRenderLastList(IGUIElement* w);

		void beginDocking();
		void endDocking();
		int getDockingWidth(FSGUI_Window* w, FSGUI_LAYOUTSTYLE ls);
		int getDockingHeight(FSGUI_Window* w, FSGUI_LAYOUTSTYLE ls);

		FSGUI_Window* getFSGUI_WindowAt(int x, int y);
		virtual int getBorderSize(long flags);

		//		SColor getBorderColor()			{ return SColor(255, 25, 25, 25);	}
		//		SColor getSizingBorderColor()	{ return SColor(255, 250, 225, 250);		}
		//		SColor getDisabledBorderColor() { return SColor(255, 225, 255, 225);		}

	public:
		// unsafe object pointers
		IrrlichtDevice*		getDevice();
		IGUIEnvironment*	getGui();
		ISceneManager*		getSmgr();
		IVideoDriver*		getDriver();
		ICursorControl*		getCursorControl();

		// event overrides
		// simple default event handler functions
		virtual bool OnEvent(const SEvent & e);
		virtual bool onScreenResized(const SEvent & e);
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
		virtual bool OnMenuItemSelected(const SEvent & e) { return false; }
		virtual bool onComboboxChanged(const SEvent & e) { return false; }
		virtual bool onSpinboxChanged(const SEvent & e) { return false; }
		virtual bool onTableChanged(const SEvent & e) { return false; }
		virtual bool onTableHeaderChanged(const SEvent & e) { return false; }
		virtual bool onTableSelectedAgain(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeDeselect(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeSelect(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeExpand(const SEvent & e) { return false; }
		virtual bool onTreeviewNodeCollapse(const SEvent & e) { return false; }

		virtual bool onLMouseButtonDown(const SEvent & e);
		virtual bool onRMouseButtonDown(const SEvent & e) { return false; }
		virtual bool onMMouseButtonDown(const SEvent & e) { return false; }
		virtual bool onLMouseButtonUp(const SEvent & e);
		virtual bool onRMouseButtonUp(const SEvent & e) { return false; }
		virtual bool onMMouseButtonUp(const SEvent & e) { return false; }
		virtual bool onMouseMoved(const SEvent & e);
		virtual bool onMouseWheel(const SEvent & e) { return false; }
		virtual bool onLMouseDoubleClick(const SEvent & e) { return false; }
		virtual bool onRMouseDoubleClick(const SEvent & e) { return false; }
		virtual bool onMMouseDoubleClick(const SEvent & e) { return false; }
		virtual bool onLMouseTripleClick(const SEvent & e) { return false; }
		virtual bool onRMouseTripleClick(const SEvent & e) { return false; }
		virtual bool onMMouseTripleClick(const SEvent & e) { return false; }

	};

} // end namespace