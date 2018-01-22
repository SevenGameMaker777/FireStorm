// include the needed header files
#include "FSApplication.h"
#include "FSEngine.h"
#include "FSLevelManager.h"
#include "FSLevel.h"
#include "FSPhysX.h"

// unclutter the globalnamespace
namespace FS
{
	// variable access
	FSEngine*			FSApplication::getEngine() { return m_Engine; }
	IrrlichtDevice*		FSApplication::getDevice() { return getEngine()->getDevice(); }
	IVideoDriver*		FSApplication::getDriver() { return getEngine()->getDriver(); }
	ISceneManager*		FSApplication::getSmgr() { return getEngine()->getSmgr(); }
	IGUIEnvironment*	FSApplication::getGui() { return getEngine()->getGui(); }
	int					FSApplication::getMode() { return m_Mode; }
	FSLevelManager*		FSApplication::getLevelManager() { return m_LevelManager; }
	FSGUI_Desktop*		FSApplication::getDesktop() { return m_Desktop;  }
	FSPhysXManager*		FSApplication::getPhysXManager() { return m_PhysXManager; }

	// set all variables to a knwon value
	void FSApplication::initialize()
	{
		// log this activity
		FS_LOG(FSL_DEBUG, "FSApplication::initialize()");

		// set all variables to a known value
		m_Mode = MODE_INIT;
		m_Engine = 0;
		m_LevelManager = 0;
		m_Desktop = 0;
		m_PhysXManager = 0;

		// setup the eventhandler id and name
		FSEventHandler::setEventHandlerId(ID_APPLICATION);
		FSEventHandler::setEventHandlerName("APPLICATION");
	}

	// dual creation allows for better error handling. return true for success, return false for failure
	bool FSApplication::create()
	{
		// log this activity
		FS_LOG(FSL_DEBUG, "FSApplication::create()");

		// setup the directory structure
		int drive;
		char path[_MAX_PATH];
		drive = _getdrive();
		if (_getdcwd(drive, path, _MAX_PATH) != NULL) setWorkingDirectory(path);

		// instantiate, initialize and attempt to create
		m_Engine = new FSEngine();
		m_Engine->initialize();
		if (!m_Engine->create(this, FS_DEFAULT, 0)) { FS_LOG(FSL_DEBUG, " Engine creation failed"); return false; }

		// instantiate, initialize and create the level manager
		m_PhysXManager = new FSPhysXManager();
		m_PhysXManager->initialize();
		if (!m_PhysXManager->create(getDevice(), getDriver())) { FS_LOG(FSL_ERROR, "unable to create FSPhysXManager"); return false; }

		// instantiate, initialize and attempt to create
		m_Desktop = new FSGUI_Desktop();
		m_Desktop->initialize();
		if (!m_Desktop->create(this)) { FS_LOG(FSL_DEBUG, " Desktop creation failed"); return false; }

		// instantiate, initialize and create the level manager
		m_LevelManager = new FSLevelManager();
		m_LevelManager->initialize();
		if (!m_LevelManager->create(this)) { FS_LOG(FSL_ERROR, "unable to create FSLevelManager"); return false; }

		// set the application mode
		m_Mode = MODE_RUN;

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made
	bool FSApplication::cleanup()
	{
		// log this activity
		FS_LOG(FSL_DEBUG, "FSApplication::cleanup()");

		// safely cleanup and delete the levels
		if (m_LevelManager)
		{
			m_LevelManager->cleanup();
			delete(m_LevelManager);
			m_LevelManager = 0;
		}

		// safely cleanup and delete PhysX
		if (m_PhysXManager)
		{
			m_PhysXManager->cleanup();
			delete(m_PhysXManager);
			m_PhysXManager = 0;
		}


		// safely cleanup and delete 
		if (m_Desktop) { m_Desktop->cleanup(); delete(m_Desktop); m_Desktop = 0; }

		// safely cleanup and delete 
		if (m_Engine) { m_Engine->cleanup(); delete(m_Engine); m_Engine = 0; }

		// always return false from a cleanup function
		return false;
	}

	// called each frame before beginScene
	void FSApplication::preFrame(const float & elapsedtime)
	{
		// let the level manager handle the active level 
		if (m_LevelManager) m_LevelManager->preFrame(elapsedtime, true);

		// let the engine render the smgr
		if (m_Engine) m_Engine->preFrame(elapsedtime);
	}

	// called each frame after beginScene but before endscene
	void FSApplication::frame(const float & elapsedtime)
	{
		// let the level manager handle the active level 
		if (m_LevelManager) m_LevelManager->frame(elapsedtime, true);

		// let the engine render the smgr
		if (m_Engine) m_Engine->frame(elapsedtime);

		// let the desktop render if it needs to
		if (getDesktop()) getDesktop()->draw();
	}

	// called each frame after smgr render but before endscene
	void FSApplication::postFrame(const float & elapsedtime)
	{
		// let the level manager handle the active level 
		if (m_LevelManager) m_LevelManager->postFrame(elapsedtime, true);

		// let the engine render the smgr
		if (m_Engine) m_Engine->postFrame(elapsedtime);
	}

	// the main message flow control
	void FSApplication::run()
	{
		// notify the application
		SEvent e;
		e.EventType = EET_USER_EVENT;
		e.UserEvent.UserData1 = EVT_SCREENRESIZED;
		getDevice()->postEventFromUser(e);

		// Work out a frame delta time.
		u32 then = getDevice()->getTimer()->getTime();
		f32 frameDeltaTime = 0;

		// pump the main message que
		while (getEngine() && getDevice() && getDevice()->run() && (getMode() != MODE_QUIT))
		{
			if (getDevice()->isWindowActive())
			{
				preFrame(frameDeltaTime);
				frame(frameDeltaTime);
				postFrame(frameDeltaTime);
				u32 now = getDevice()->getTimer()->getTime();
				frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
				then = now;
			}
		}
	}

	// add a level to the level manager
	bool FSApplication::addLevel(FSLevel* level, bool set_as_active_level)
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSApplication::addLevel()");

		// let the level manager validate and add the level
		return m_LevelManager->addLevel(level, set_as_active_level);
	}

	// set the next active level
	void FSApplication::setActiveLevel(int id)
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSApplication::setActiveLevel(%d)", id);

		// let the manager set the active level
		m_LevelManager->setNextLevel(id);
	}

	FSLevel* FSApplication::getActiveLevel()
	{
		// get a pointer from the level manager
		return m_LevelManager->getActiveLevel();
	}

	// register a directory name 
	void FSApplication::registerDirectory(stringc dirName, stringc path)
	{
		// if the directory exists, delete it
		map<stringc, stringc>::Node *node = m_Directories.find(dirName);
		if (node) m_Directories.remove(node);

		// add the directory to the map
		m_Directories.insert(dirName, path);
	}

	// get the path of a named directory
	stringc FSApplication::getDirectory(stringc dirName)
	{
		stringc path("./");
		map<stringc, stringc>::Node *node = m_Directories.find(dirName);
		if (node) path = node->getValue();
		return path;
	}

	// setup the directory paths
	void FSApplication::setWorkingDirectory(stringc path)
	{
		// clear the list
		m_Directories.clear();

		// replace all of the slashes to the needed style
		path.replace("\\", "/");

		// register the directory names
		FS_LOG(FSL_DEBUG, "%s", "setting the working directory structure");
		registerDirectory("WorkingDirectory", path);
		registerDirectory("GameDataDirectory", path + stringc("/_Data/GameData/"));
		registerDirectory("GameSaveDirectory", path + stringc("/_Data/SaveGames/"));
		registerDirectory("MediaDirectory", path + stringc("/Media/"));
		registerDirectory("ObjectsDirectory", path + stringc("/_Data/FSObjects/"));
		registerDirectory("PrefabDirectory", path + stringc("/_Data/Prefabs/"));
		registerDirectory("ActionTableDirectory", path + stringc("/_Data/ActionTables/"));
		registerDirectory("SoundDirectory", path + stringc("/media/Sound/"));
		registerDirectory("SkyboxThemeDirectory", path + stringc("/media/_assets/Skyboxes/"));
		registerDirectory("ObjectDataDirectory", path + stringc("/_data/objectdata/"));

		FS_LOG(FSL_INFO, "-Working Directory      %s", getDirectory("WorkingDirectory").c_str());
		FS_LOG(FSL_INFO, "-Game Data Directory    %s", getDirectory("GameDataDirectory").c_str());
		FS_LOG(FSL_INFO, "-Save Data Directory    %s", getDirectory("GameSaveDirectory").c_str());
		FS_LOG(FSL_INFO, "-Media Directory        %s", getDirectory("MediaDirectory").c_str());
		FS_LOG(FSL_INFO, "-Objects Directory      %s", getDirectory("ObjectsDirectory").c_str());
		FS_LOG(FSL_INFO, "-Prefab Directory       %s", getDirectory("PrefabDirectory").c_str());
		FS_LOG(FSL_INFO, "-Action Table Directory %s", getDirectory("ActionTableDirectory").c_str());
		FS_LOG(FSL_INFO, "-SkyboxThemeDirectory   %s", getDirectory("SkyboxThemeDirectory").c_str());
		FS_LOG(FSL_INFO, "-ObjectDataDirectory    %s", getDirectory("ObjectDataDirectory").c_str());
	}

	void FSApplication::changeDirectory(stringc dir)
	{
		_chdir(dir.c_str());
	}

	bool FSApplication::onKeyboardEvent(const SEvent& e)
	{
		return FSEventHandler::onKeyboardEvent(e);
	}

	bool FSApplication::onUserEvent(const SEvent& e)
	{
		if (getEngine()) getEngine()->onUserEvent(e);
		if (getActiveLevel()) getActiveLevel()->onUserEvent(e);
		if (getDesktop()) getDesktop()->onUserEvent(e);
		return FSEventHandler::onUserEvent(e);
	}

	bool FSApplication::onLogTextEvent(const SEvent& e)
	{
		return FSEventHandler::onLogTextEvent(e);
	}

	bool FSApplication::onGUIEvent(const SEvent& e)
	{
		if (getDesktop()) getDesktop()->onGUIEvent(e);

		return FSEventHandler::onGUIEvent(e);
	}

	bool FSApplication::onMouseEvent(const SEvent& e)
	{
//		if (getEngine()) getEngine()->onMouseEvent(e);
		if (getDesktop()) getDesktop()->onMouseEvent(e);
//		if (getActiveLevel()) getActiveLevel()->onMouseEvent(e);
		return FSEventHandler::onMouseEvent(e);
	}

	// someone resized the main window
	bool FSApplication::onScreenResized(const SEvent & e)
	{
		if (getDesktop()) getDesktop()->onScreenResized(e);
		if (getActiveLevel()) getActiveLevel()->onScreenResized(e);

		return FSEventHandler::onScreenResized(e);
	}

} // end namespace
