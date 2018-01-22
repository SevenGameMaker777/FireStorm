#pragma once

// this stuff allows us to load objects dynamically by knowing what DLL
// the code resides in. 

// include the needed headers
#include "FSUtils.h"

namespace FS
{
	// structure to hold object dll-class name information
	struct  FSObjectData
	{
		stringc DLLName;		// name of dll that object is in
		stringc ObjectName;	// name of object class
		stringc Info;
	};

	// this class creates and maintains a list of available Object derived 
	// classes by DLL and classname, that the ObjectFactory can use when 
	// creating new objects
	class  FSClassManager
	{
	public:

		// the list of ObjectData pointers
		list<FSObjectData*> m_List;
		list<FSObjectData*>::Iterator m_Iterator;

		// search path for dll's
		stringc m_ObjectDirectory;
		stringc getObjectDirectory() { return m_ObjectDirectory; }
		void setObjectDirectory(stringc path) { m_ObjectDirectory = path; }

		FSClassManager();				// class constructor
		virtual ~FSClassManager();		// class destructor
		virtual void initialize();		// set all variables to a known value
		virtual bool create();			// dual creation allows for better error handling
		virtual bool cleanup();			// cleanup whatever memory mess we made

		// search the directory for Object derived classes
		virtual bool buildList(stringc directory);

		// helper function for searching through DLL files for Object derived classes
		virtual void findObjects(stringc filename);

		// knowing the dll name and class name, return the correct ObjectData pointer
		virtual FSObjectData* findClassInfo(stringc dllname, stringc classname);

		// knowing only the class name, return the correct ObjectData pointer
		virtual FSObjectData* findClassInfo(stringc classname);

		// simple method for printing out the available text
		virtual void logInfo();

		// get a list of objects
		FSObjectData* getNextObject(bool newsearch);

	};

} // end namespace
