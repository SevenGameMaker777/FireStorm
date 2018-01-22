// FSMessageManager.cpp: implementation of the FSMessageManager class.
//
//////////////////////////////////////////////////////////////////////

#include "FSLogger.h"
#include "FSMessageManager.h"
#include "FSObjectManager.h"
#include "FSApplication.h"
#include "FSLevelManager.h"
#include "FSLevel.h"
#include "FSObject.h"

////////////////////////////////////////////////////////////////////////////////////////
// message list
////////////////////////////////////////////////////////////////////////////////////////
namespace FS
{

	MessageList::MessageList()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "MessageList::MessageList()");
	}

	MessageList::~MessageList()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "MessageList::~messageList()");
	}

	void MessageList::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "MessageList::initialize()");

		m_List.clear();
	}

	bool MessageList::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "MessageList::cleanup()");

		// run through the list of messages and delete them
		for (m_Iterator = m_List.begin(); m_Iterator != m_List.end(); ++m_Iterator)
		{
			delete(*m_Iterator);
			m_Iterator = m_List.erase(m_Iterator);
		}

		// clear the list once and for all
		m_List.clear();

		// always return false from a cleanup function
		return false;
	}

	bool MessageList::create()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "MessageList::create()");

		// everythign went fine
		return true;
	}

	bool MessageList::add(FSObjectMessage* data)
	{
		m_List.push_front(data);

		return true;
	}

	void MessageList::logInfo()
	{
		for (m_Iterator = m_List.begin(); m_Iterator != m_List.end(); ++m_Iterator)
		{
			// Log the messages here
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////
	// message class
	///////////////////////////////////////////////////////////////////////////

	FSObjectMessage::FSObjectMessage()
	{
	}

	FSObjectMessage::~FSObjectMessage()
	{
	}

	bool FSObjectMessage::create(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data)
	{
		setType(type);
		setType2(type2);
		setSource(source);
		setDest(dest);
		setRTime(rtime);
		setDTime(dtime);
		setData(data);

		// everything went fine
		return true;
	}

	///////////////////////////////////////////////////////////////////////////
	// end message class
	///////////////////////////////////////////////////////////////////////////


	FSMessageManager::FSMessageManager()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSMessageManager::FSMessageManager()");
	}

	FSMessageManager::~FSMessageManager()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSMessageManager::~FSMessageManager()");
	}

	void FSMessageManager::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSMessageManager::initialize()");

		// set to a known value
		m_ObjectManager = 0;
		m_Application = 0;
		m_Level = 0;
		MessageCallBack = NULL;
	}

	bool FSMessageManager::create(FSObjectManager* m, FSApplication* app, FSLevel* level)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSMessageManager::create()");

		// verify the manager is valid
		if (!m) return false;

		// remember this
		m_ObjectManager = m;
		m_Application = app;
		m_Level = level;

		// everything went fine
		return true;
	}

	bool FSMessageManager::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSMessageManager::cleanup()");

		// clear the list
		m_List.cleanup();

		// forget this
		MessageCallBack = NULL;
		m_ObjectManager = 0	;
		m_Application = 0;
		m_Level = 0;

		// always return false from a cleanup function
		return false;
	}

	void FSMessageManager::clear()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSMessageManager::clear()");

		// clear the list
		m_List.cleanup();
	}

	void FSMessageManager::addMessage(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data)
	{
		FSObjectMessage* m = new FSObjectMessage();
		m->create(type, type2, source, dest, rtime, dtime, data);
		m_List.add(m);
	}

	void FSMessageManager::addMessage(FSObjectMessage* msg)
	{
		m_List.add(msg);
	}

	// do what this class does each frame
	void FSMessageManager::frame(float elaspedtime)
	{
		for (m_List.m_Iterator = m_List.m_List.begin(); m_List.m_Iterator != m_List.m_List.end(); )
		{
			if ((*m_List.m_Iterator)->getDTime() >= 0)
			{
				dispatchMessage((*m_List.m_Iterator));
				delete(*m_List.m_Iterator);
				m_List.m_Iterator = m_List.m_List.erase(m_List.m_Iterator);
			}
			else
			{
				(*m_List.m_Iterator)->setDTime((*m_List.m_Iterator)->getDTime() + 1);
				++m_List.m_Iterator;
			}
		}
	}

	// send the message 
	void FSMessageManager::dispatchMessage(FSObjectMessage* message)
	{
		// if we have a callback, send it there too
		if (MessageCallBack)
			if (message->getDest() == m_MessageCallBackId)
				MessageCallBack(message);

		// if this is meant for the app level strcuture, send it there
		if (message->getDest() == FSID_APP_MESSAGE)
		{
			//			getApplication()->receiveMessage(message);
		}
		else
			// if this is meant for the level strcuture, send it there
			if (message->getDest() == FSID_LEVEL_MESSAGE)
			{
				//			getLevel()->receiveMessage(message);
			}
			else
			{
				// it must be for an object, so get a pointer to the object
				FSObject* obj = m_ObjectManager->getObjectPointer(message->getDest(), true);
				if (obj)
				{
					// if the object exists, send the message
					obj->receiveMessage(message);
				}
			}
	}

	void FSMessageManager::setMessageCallBack(MESSAGECALLBACKFUNC NewCallBack, int id)
	{
		MessageCallBack = NewCallBack;
		m_MessageCallBackId = id;
	}

} // end namespace
