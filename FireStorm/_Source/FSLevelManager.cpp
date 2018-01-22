// include the needed header files
#include "FSLevelManager.h"
#include "FSLevel.h"
#include "FSLogger.h"
#include "FSApplication.h"

// unclutter the global namespace
namespace FS
{
	void FSLevelManager::setId(int id) { m_Id = id; }	// simple id variable
	int  FSLevelManager::getId() { return m_Id; }	// simple id variable

	void FSLevelManager::initialize()
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSLevelManager::initialize()");

		// set all variables to a known value
		m_Id = ID_LEVELMANAGER;
		m_App = 0;
		m_Levels.clear();
		m_NextLevelId = 0;
		m_ActiveLevelId = 0;
		m_ActiveLevel = 0;
	}

	bool FSLevelManager::create(FSApplication* app)
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSLevelManager::create()");

		// remmber this
		m_App = app;

		// everything went fine
		return true;
	}

	bool FSLevelManager::cleanup()
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSLevelManager::cleanup()");

		// safely destroy all of the levels
		destroyAllLevels();

		// forget this 
		m_App = 0;

		// always return false from a cleanup function
		return false;
	}

	void FSLevelManager::preFrame(const float & elapsedtime, bool forceupdate)
	{
		if (m_NextLevelId != m_ActiveLevelId)
		{
			int ID = m_ActiveLevelId;
			int ID2 = m_NextLevelId;
			FS_LOG(FSL_ERROR, "%s %d %d", "Level Changed From ", ID, ID2);

			if (getActiveLevel())
			{
				ID = getActiveLevel()->getId();

				// notify the system that the level is being unloaded
				SEvent e;
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_LEVEL_UNLOAD;
				e.UserEvent.UserData2 = getActiveLevel()->getId();
				m_App->getDevice()->postEventFromUser(e);

				getActiveLevel()->unload();
			}

			m_ActiveLevel = getLevel(m_NextLevelId);
			m_ActiveLevelId = m_NextLevelId;

			if (getActiveLevel())
			{
				SEvent e;

				// notify the system that the level is being unloaded
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_LEVEL_LOAD;
				e.UserEvent.UserData2 = getActiveLevel()->getId();
				m_App->getDevice()->postEventFromUser(e);

				getActiveLevel()->load();

				// notify the system that the level has changed
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_LEVEL_CHANGED;
				e.UserEvent.UserData2 = getActiveLevel()->getId();
				m_App->getDevice()->postEventFromUser(e);

			}
		}
		// call the active level frame functions
		if (m_ActiveLevel) m_ActiveLevel->preFrame(elapsedtime, forceupdate);
	}

	void FSLevelManager::frame(const float & elapsedtime, bool forceupdate)
	{
		// call the active level frame functions
		if (m_ActiveLevel) m_ActiveLevel->frame(elapsedtime, forceupdate);
	}

	void FSLevelManager::postFrame(const float & elapsedtime, bool forceupdate)
	{
		// call the active level frame functions
		if (m_ActiveLevel) m_ActiveLevel->postFrame(elapsedtime, forceupdate);
	}

	// class functions
	bool FSLevelManager::addLevel(FSLevel* level, bool make_active)
	{
		if (!level)
		{
			// something went wrong
			return false;
		}

		level->unload();

		// add the level to the list
		m_Levels.push_back(level);

		if (make_active) setNextLevel(level->getId());

		// everything went fine
		return true;
	}

	void FSLevelManager::destroyAllLevels()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSLevelManager::destroyAllLevels()");

		// get a list iterator
		list<FSLevel*>::Iterator it;

		// take care of the current level first
		if (getActiveLevel()) destroyLevel(getActiveLevel()->getId());
		m_ActiveLevel = 0;
		m_NextLevelId = 0;
		m_ActiveLevelId = 0;

		// run through the list
		for (it = m_Levels.begin(); it != m_Levels.end(); )
		{
			// notify the application
			SEvent e;
			e.EventType = EET_USER_EVENT;
			e.UserEvent.UserData1 = EVT_LEVEL_DELETED;
			e.UserEvent.UserData2 = (*it)->getId();

			m_App->getDevice()->postEventFromUser(e);

			// cleanup the level
			(*it)->cleanup();

			// delete the level
			delete(*it);

			// remove the level fromt he list
			it = m_Levels.erase(it);
		}

		// make sure the list is cleared
		m_Levels.clear();
	}

	void FSLevelManager::destroyLevel(int id)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%s %d", "FSLevelManager::destroyLevel()", id);

		// get a list iterator
		list<FSLevel*>::Iterator it;

		// run through the list
		for (it = m_Levels.begin(); it != m_Levels.end(); )
		{
			// if this is the right level
			if ((*it)->getId() == id)
			{
				if ((*it) == getActiveLevel())
				{
					// notify the application
					SEvent e;
					e.EventType = EET_USER_EVENT;
					e.UserEvent.UserData1 = EVT_LEVEL_UNLOAD;
					e.UserEvent.UserData2 = (*it)->getId();
					m_App->getDevice()->postEventFromUser(e);

					getActiveLevel()->unload();
					m_ActiveLevel = 0;
					m_NextLevelId = 0;
					m_ActiveLevelId = 0;
				}

				// notify the application
				SEvent e;
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_LEVEL_DELETED;
				e.UserEvent.UserData2 = (*it)->getId();

				m_App->getDevice()->postEventFromUser(e);

				// cleanup the level
				(*it)->cleanup();

				// delete the level
				delete(*it);

				// remove the level from the list
				it = m_Levels.erase(it);

				return;
			}
			else it++; // increment the iterator
		}
	}

	FSLevel* FSLevelManager::getLevel(int id)
	{
		// if the list is emtpy then bail
		if (m_Levels.empty()) return NULL;

		// get a list iterator
		list<FSLevel*>::Iterator it;

		// run through the list
		for (it = m_Levels.begin(); it != m_Levels.end();)
		{
			if ((*it)->getId() == id) return (*it);
			it++;
		}

		// we did not find the level
		return NULL;
	}

	FSLevel* FSLevelManager::getActiveLevel()
	{
		return m_ActiveLevel;
	}

	void FSLevelManager::setNextLevel(int id)
	{
		m_NextLevelId = id;
	}

	// force the level change
	void FSLevelManager::forceLevelChange()
	{
		if (m_NextLevelId != m_ActiveLevelId)
		{
			if (getActiveLevel())
			{
				// notify the system that the level is being unloaded
				SEvent e;
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_LEVEL_UNLOAD;
				e.UserEvent.UserData2 = getActiveLevel()->getId();
				m_App->getDevice()->postEventFromUser(e);

				getActiveLevel()->unload();
			}

			m_ActiveLevel = getLevel(m_NextLevelId);
			m_ActiveLevelId = m_NextLevelId;

			if (getActiveLevel())
			{
				SEvent e;

				// notify the system that the level is being unloaded
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_LEVEL_LOAD;
				e.UserEvent.UserData2 = getActiveLevel()->getId();
				m_App->getDevice()->postEventFromUser(e);

				getActiveLevel()->load();

				// notify the system that the level has changed
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_LEVEL_CHANGED;
				e.UserEvent.UserData2 = getActiveLevel()->getId();
				m_App->getDevice()->postEventFromUser(e);
			}
		}
	}

} // end namespace

