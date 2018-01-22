#pragma once

#include "FSUtils.h"
#include "FSObjectManager.h"
#include "FSMessageManager.h"
#include "FSClassManager.h"
#include "FSLogger.h"


namespace FS
{
	typedef class FSLevel FSLevel;
	typedef class FSObject FSObject;

	class FSObjectFactory
	{
	public:

		FSObjectFactory() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSObjectFactory::FSObjectFactory()"); }
		virtual ~FSObjectFactory() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSObjectFactory::~FSObjectFactory()"); }

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

		// frame functions 
		virtual void preFrame(const float &elapsedtime, bool forceupdate);
		virtual void frame(const float &elapsedtime, bool forceupdate);
		virtual void postFrame(const float &elapsedtime, bool forceupdate);

		// clear out the objects
		virtual void clear();

		// set up light / fog / editor use
		virtual void setUseLight(bool value);
		virtual void setUseFog(bool value);
		virtual void setUseEditorVisuals(bool showValue);

		// simple check to see if an object class has been registered
		virtual bool objectTypeExists(stringc ot);

		// complex methods for loading registered object types
		FSObject* createObjectByType(stringc type, bool recreate, int id, stringc name, bool addtolevel);
		FSObject* createObjectByTypeAndDLL(stringc dllname, stringc objtype, bool recreate, int id, stringc name, bool addtolevel);

		// save a single object to the prefab format
		virtual void savePrefab(FSObject* obj, stringc filename);
		virtual FSObject*  loadPrefab(stringc filename);

		// unsafe method to retieve pointers
		FSLevel*			getLevel() { return m_Level; }
		FSObjectManager*	getObjectManager() { return m_ObjectManager; }
		FSMessageManager*	getMessageManager() { return m_MessageManager; }
		FSClassManager*		getClassManager() { return m_ClassManager; }
		FSObject*			getObjectPointer(int id, bool deep);
		int					getUniqueId();

	private:
		int						m_UniqueId;				// simple unique id variable
		FSLevel*				m_Level;				// the level this belongs to
		FSObjectManager*		m_ObjectManager;		// list of objects in the manager
		FSMessageManager*		m_MessageManager;		// routes messages through the application
		FSClassManager*			m_ClassManager;			// list of available obejct classes

	};

} // end namespace

