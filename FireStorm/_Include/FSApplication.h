// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FSLogger.h"
#include "FSEventHandler.h"
#include "FSGUI_Desktop.h"

// unclutter the global namespace
namespace FS
{
	// forward declarations
	typedef class FSEngine FSEngine;
	typedef class FSLevelManager FSLevelManager;
	typedef class FSLevel FSLevel;
	typedef class FSPhysXManager FSPhysXManager;

	/*
	*/
	class FSApplication : public FSEventHandler
	{
	private:
		// class variables
		FSEngine*				m_Engine;		// irrlicht wrapper
		map<stringc, stringc>	m_Directories;	// game resource directory structure
		int						m_Mode;			// game mode (set to MODE_QUIT to safely exit the program)
		FSLevelManager*			m_LevelManager;	// list of available levels
		FSGUI_Desktop*			m_Desktop;		// pointer to graphical desktop
		FSPhysXManager*			m_PhysXManager;	// physx wrapper manager

	public:
		// setter functions
		void setMode(int v) { m_Mode = v; }
	public:
		// getter functions
		FSEngine*			getEngine();
		IrrlichtDevice*		getDevice();
		IVideoDriver*		getDriver();
		ISceneManager*		getSmgr();
		IGUIEnvironment*	getGui();
		int					getMode();
		FSLevelManager*		getLevelManager();	// list of available levels
		FSGUI_Desktop*		getDesktop();		// pointer to graphical desktop
		FSPhysXManager*		getPhysXManager();	// physx wrapper manager


	public:
		// class constructor / destructor (use initialize(), create() and cleanup() )
		FSApplication() { FS_LOG(FSL_DEBUG, "FSApplication::FSApplication()"); }
		~FSApplication() { FS_LOG(FSL_DEBUG, "FSApplication::~FSApplication()"); }

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create();

		// cleanup whatever memory mess we made
		virtual bool cleanup();

		// class frame functions
		virtual void preFrame(const float & elapsedtime);
		virtual void frame(const float & elapsedtime);
		virtual void postFrame(const float & elapsedtime);

		// the primary message pump and program flow control
		void run();

		// add a level to the level manager, true if this should be made the active level
		virtual bool addLevel(FSLevel* level, bool set_as_active_level);

		// set the next active level
		virtual void setActiveLevel(int id);

		// get a pointer to the active level	
		FSLevel* getActiveLevel();

	public:
		// register a directory name 
		void registerDirectory(stringc dirName, stringc path);

		// get the path of a named directory
		stringc getDirectory(stringc dirName);

		// setup the directory paths
		void setWorkingDirectory(stringc path);

		// change directories
		void changeDirectory(stringc dir);

	public:
		// event overrides
		virtual bool onGUIEvent(const SEvent& e);
		virtual bool onKeyboardEvent(const SEvent& e);
		virtual bool onMouseEvent(const SEvent & e);	// override to pass events to engine and level
		virtual bool onUserEvent(const SEvent & e);	// override to pass events to engine and level
		virtual bool onLogTextEvent(const SEvent & e);	// override to pass events to engine and level
		virtual bool onScreenResized(const SEvent & e); // someone resized the main window
	private:
		// class private functions
	};

} // end namespace
