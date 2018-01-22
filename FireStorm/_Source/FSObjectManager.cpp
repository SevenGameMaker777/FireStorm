// include the needed header files
#include "FSObjectManager.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"

// unclutter the global namespace
namespace FS
{
	void FSObjectManager::setId(int id) { m_Id = id; }	// simple id variable
	int  FSObjectManager::getId() { return m_Id; }	// simple id variable

	void FSObjectManager::initialize()
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSObjectManager::initialize()");

		// set all variables to a known value
		m_Id = ID_OBJECTMANAGER;
		m_Objects.clear();
	}

	bool FSObjectManager::create(FSLevel* level)
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSObjectManager::create()");

		// remember this
		m_Level = level;

		// everything went fine
		return true;
	}

	bool FSObjectManager::cleanup()
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSObjectManager::cleanup()");

		// safely destroy all of the Objects
		destroyAllObjects();

		// forget this
		m_Level = 0;

		// always return false from a cleanup function
		return false;
	}

	void FSObjectManager::preFrame(const float & elapsedtime, bool forceupdate)
	{
		// if the list is empty then bail
		if (m_Objects.empty()) return;

		// call the Object frame functions

			// get a list iterator
			list<FSObject*>::Iterator it;

			// run through the list
			for (it = m_Objects.begin(); it != m_Objects.end();)
			{
#if(1)
				// if the object is dead
				if ((*it)->getDead()!=0)
				{
					int id = (*it)->getId();

					// clean it up and delete it
					(*it)->cleanup();
					delete(*it);

					// notify the application
					SEvent e;
					e.EventType = EET_USER_EVENT;
					e.UserEvent.UserData1 = EVT_OBJECT_DESTROYED;
					e.UserEvent.UserData2 = id;
					getLevel()->getDevice()->postEventFromUser(e);

					// erase the dead object from the list
					it = m_Objects.erase(it);
				}
				else
#endif
				{
					// it is a live object so parse it
					(*it)->preFrame(elapsedtime, forceupdate);

					// increment the iterator
					it++;
				}
			}
	}

	void FSObjectManager::frame(const float & elapsedtime, bool forceupdate)
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// call the Object frame functions

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			// parse it
			(*it)->frame(elapsedtime, forceupdate);

			// increment the iterator
			it++;
		}
	}

	void FSObjectManager::postFrame(const float & elapsedtime, bool forceupdate)
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// call the Object frame functions

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			// parse it	
			(*it)->postFrame(elapsedtime, forceupdate);

			// increment the iterator
			it++;
		}
	}

	void FSObjectManager::reCreateAllObjects()
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->reCreate();
		}
	}
	void FSObjectManager::createSceneNodes()
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->createSceneNodes();
		}
	}

	void FSObjectManager::destroyAllSceneNodes()
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->destroyAllSceneNodes();
		}
	}

	void FSObjectManager::destroyPhysXObject()
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->destroyPhysXObject();
		}
	}

	void FSObjectManager::createPhysXObject()
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->createPhysXObject();
		}
	}

	// class functions
	bool FSObjectManager::addObject(FSObject* Object)
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSObjectManager::addObject()");

		if (!Object)
		{
			// something went wrong
			return false;
		}

		// add the Object to the list
		m_Objects.push_back(Object);

		// notify the application
		SEvent e;
		e.EventType = EET_USER_EVENT;
		e.UserEvent.UserData1 = EVT_OBJECT_ADDED;
		e.UserEvent.UserData2 = Object->getId();
		getLevel()->getDevice()->postEventFromUser(e);

		// everything went fine
		return true;
	}

	void FSObjectManager::destroyAllObjects()
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSObjectManager::destroyAllObjects()");

		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			int id = (*it)->getId();
			(*it)->cleanup();
			delete(*it);

			// notify the application
			SEvent e;
			e.EventType = EET_USER_EVENT;
			e.UserEvent.UserData1 = EVT_OBJECT_DESTROYED;
			e.UserEvent.UserData2 = id;
			getLevel()->getDevice()->postEventFromUser(e);

			it = m_Objects.erase(it);
		}
	}

	void FSObjectManager::destroyObject(int id)
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSObjectManager::destroyObject()");

		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			int id = (*it)->getId();
			(*it)->cleanup();
			delete(*it);

			// notify the application
			SEvent e;
			e.EventType = EET_USER_EVENT;
			e.UserEvent.UserData1 = EVT_OBJECT_DESTROYED;
			e.UserEvent.UserData2 = id;
			getLevel()->getDevice()->postEventFromUser(e);

			it = m_Objects.erase(it);
		}
	}

	FSObject* FSObjectManager::getObjectPointer(int id, bool deep)
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return NULL;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			if ((*it)->getId() == id) return (*it);
			it++;
		}

		// we did not find the object
		return NULL;
	}


	// run through the list and return a pointer to each object
	// use like this
	// FSObject* obj = getNextObject(true);
	// while (obj)
	// {
	//     do whatever with the object (except delete it!)
	//		
	//	   obj = getNextObject(false);
	// }
	FSObject* FSObjectManager::getNextObject(bool newsearch)
	{
		if (m_Objects.empty()) return NULL;

		if (newsearch)
		{
			m_Iterator = m_Objects.begin();
			return (*m_Iterator);
		}

		m_Iterator++;
		if (m_Iterator != m_Objects.end())	return (*m_Iterator);
		else return NULL;
		return NULL;
	}

	FSObject* FSObjectManager::findObjectByName(stringc name, bool deep)
	{
		list<FSObject*>::Iterator it = m_Objects.begin();
		for (; it != m_Objects.end(); it++)
		{
			if ((*it)->getName() == name) return (*it);
		}
		return NULL;
	}

	void FSObjectManager::setUseFog(bool v)
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->setUseFog(v);
			it++;
		}
	}

	void FSObjectManager::setUseLight(bool v)
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->setUseLight(v);
			it++;
		}
	}

	void FSObjectManager::setUseEditorVisuals(bool v)
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->setUseEditorVisuals(v);
			it++;
		}
	}

	void FSObjectManager::setUseShadows(bool v)
	{
		// if the list is emtpy then bail
		if (m_Objects.empty()) return;

		// get a list iterator
		list<FSObject*>::Iterator it;

		// run through the list
		for (it = m_Objects.begin(); it != m_Objects.end();)
		{
			(*it)->setVariable("Shadow", stringc(v));
			it++;
		}
	}

	void FSObjectManager::removeObject(FSObject* obj, bool destroy)
	{
		list<FSObject*>::Iterator it = m_Objects.begin();
		for (; it != m_Objects.end(); it++)
		{
			if ((*it) == obj)
			{
				if (destroy)
				{
					(*it)->cleanup();
					delete(*it);
				}
				m_Objects.erase(it);
				return;
			}
		}
	}

} // end namespace

