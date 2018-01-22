// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FSObject.h"

// unclutter the global namespace
namespace FS
{
	// forward declarations
	typedef class FSLevel FSLevel;

	/*
	*/
	class FSObjectManager
	{
	private:
		// class variables
		int m_Id;								// simple id variable
		list<FSObject*> m_Objects;				// list of available Objects
		FSLevel* m_Level;						// pointer to the owner class
		list<FSObject*>::Iterator m_Iterator;	// persistent object list iterator

	public:
		// getters
		int			getId();		// simple id variable
		FSLevel*	getLevel()		{ return m_Level; }

	public:
		// setters
		void setId(int id);	// simple id variable

	public:
		FSObjectManager() {}	// do nothing constructor
		virtual ~FSObjectManager() {}	// do nothing destructor

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

	public:
		// class frame functions
		virtual void preFrame(const float & elapsedtime, bool forceupdate);	// called before begin frame
		virtual void frame(const float & elapsedtime, bool forceupdate);		// called between begin frame and end frame
		virtual void postFrame(const float & elapsedtime, bool forceupdate);	// called after endframe

		// class functions

		// recreate everything
		virtual void reCreateAllObjects();
		virtual void createSceneNodes();
		virtual void destroyAllSceneNodes();
		virtual void createPhysXObject();
		virtual void destroyPhysXObject();

		// add a Object to the list
		virtual bool addObject(FSObject* Object);

		// destroy all of the Objects
		virtual void destroyAllObjects();

		// destroy a single Object
		virtual void destroyObject(int id);

		// get a pointer to a Object
		FSObject* getObjectPointer(int id, bool deep);

		// reentrant object pointers
		FSObject* getNextObject(bool newsearch);

		// get an object by it's name. returns first found
		FSObject* findObjectByName(stringc name, bool deep);

		void removeObject(FSObject* obj, bool destroy);

		virtual void setUseFog(bool v);
		virtual void setUseLight(bool v);
		virtual void setUseEditorVisuals(bool v);
		virtual void setUseShadows(bool v);		// default shadow rendering

	};

} // end namepsace

