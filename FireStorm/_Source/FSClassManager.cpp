#include "FSLogger.h"
#include "FSClassManager.h"
#include "FSUtils.h"
#include <lm.h>

namespace FS
{
	FSClassManager::FSClassManager()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSClassManager::ClassManager()");
	}

	FSClassManager::~FSClassManager()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSClassManager::~ClassManager()");
	}

	void FSClassManager::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSClassManager::Initialize()");

		m_ObjectDirectory = "FSObjects\\";
	}

	bool FSClassManager::create()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSClassManager::create()");

		// set a default object directory
		setObjectDirectory("FSObjects\\");

		// everything went fine
		return true;
	}

	bool FSClassManager::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSClassManager::cleanup()");

		// delete the list of classes
		list<FSObjectData*>::Iterator it = m_List.begin();
		for (; it != m_List.end();)
		{
			delete (*it);
			it = m_List.erase(it);
		}

		// always return false from a cleanup function
		return false;
	}

	FSObjectData* FSClassManager::getNextObject(bool newsearch)
	{
		if (m_List.empty()) return NULL;

		if (newsearch)
		{
			m_Iterator = m_List.begin();
			return (*m_Iterator);
		}
		m_Iterator++;

		if (m_Iterator == m_List.end())	return NULL; else return (*m_Iterator);

		return NULL;
	}

	void FSClassManager::logInfo()
	{
		FS_LOG(FSL_DEBUG, "\nFSClassManager::LogInfo()");

		list<FSObjectData*>::Iterator it = m_List.begin();
		for (; it != m_List.end(); ++it)
		{
			FS_LOG(FSL_DEBUG, "   %s-%s", (*it)->DLLName.c_str(), (*it)->ObjectName.c_str());
		}

		FS_LOG(FSL_DEBUG, "");
	}

	FSObjectData* FSClassManager::findClassInfo(stringc dllname, stringc classname)
	{
		FS_LOG(FSL_DEBUG, "FSClassManager::FindClassInfo - ");
		FS_LOG(FSL_DEBUG, "  %s %s", dllname.c_str(), classname.c_str());

		list<FSObjectData*>::Iterator it = m_List.begin();
		for (; it != m_List.end(); ++it)
		{
			if (((*it)->DLLName == dllname) && ((*it)->ObjectName == classname))
				return (*it);
		}

		return NULL;
	}

	FSObjectData* FSClassManager::findClassInfo(stringc classname)
	{
		// change to all lower case
		//classname.make_lower();

		// log this event
		FS_LOG(FSL_DEBUG, "FSClassManager::FindClassInfo - %s", classname.c_str());

		// scan the list to find the definition
		list<FSObjectData*>::Iterator it = m_List.begin();
		for (; it != m_List.end(); ++it)
		{
			//		if ((*it)->ObjectName.make_lower() == classname)  return (*it);
			if ((*it)->ObjectName == classname)  return (*it);
		}

		return NULL;
	}

	bool FSClassManager::buildList(stringc directory)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSClassManager::BuildList - %s", directory.c_str());

		if (!m_List.empty())
		{
			// cleanup the old list if it exists
			list<FSObjectData*>::Iterator it = m_List.begin();
			for (; it != m_List.end();)
			{
				delete (*it);
				it = m_List.erase(it);
			}
		}

		// some nice variables to use
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		stringc dllsearch;

		// set the directories
		setObjectDirectory(directory);
		dllsearch = directory + "*.dll";

		// log this event
		FS_LOG(FSL_DEBUG, " Searching for  Objects in - %s", directory.c_str());

		// find the file	
		hFind = FindFirstFile(stringc(dllsearch).c_str(), &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			FS_LOG(FSL_DEBUG, " ++ NO OBJECTS FOUND IN DIRECTORY ++");
			return 0;
		}

		// search the file for  objects
		findObjects(FindFileData.cFileName);

		// continue doing this while we have a file to use
		while (FindNextFile(hFind, &FindFileData))
			findObjects(FindFileData.cFileName);

		// close it out
		FindClose(hFind);

		FS_LOG(FSL_DEBUG, " Object Search Completed");

		// everything went fine
		return true;
	}


	// Determine whether the specified DLL is a  Object DLL 
	// and if it is then find all the objects defined in the DLL
	void FSClassManager::findObjects(stringc filename)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "   Scanning - %s", filename.c_str());

		// temp variable
		stringc fullpath;

		// get the path name
		fullpath = getObjectDirectory() + filename;

		///////////////////////////////////////////////////////////
		// load the DLL file
		///////////////////////////////////////////////////////////
		HMODULE hwnd = LoadLibrary(stringc(fullpath).c_str());
		if (!hwnd)
		{
			FS_LOG(FSL_DEBUG, "Unable to load library - %s", fullpath.c_str());
			return;
		}

		///////////////////////////////////////////////////////////
		// Object compatible query 
		///////////////////////////////////////////////////////////
		typedef bool(*QUERY_IS_FSOBJECT)();
		QUERY_IS_FSOBJECT Proc = (QUERY_IS_FSOBJECT)GetProcAddress(hwnd, "queryIsObject");
		if (!Proc)
		{
			FS_LOG(FSL_DEBUG, "   DLL is not an Object DLL - %s", filename.c_str());
			return;
		}

		///////////////////////////////////////////////////////////
		// use the function to check if Object File
		///////////////////////////////////////////////////////////
		if (!Proc())
		{
			FS_LOG(FSL_DEBUG, "    DLL is not a valid  Object DLL - %s", filename.c_str());
			return;
		}

		///////////////////////////////////////////////////////////
		// Objecttype query 
		///////////////////////////////////////////////////////////

		list<stringc> thelist;
		typedef void(*QUERY_ALLOBJECTTYPE)(list<stringc>*);

		QUERY_ALLOBJECTTYPE Proc1 = (QUERY_ALLOBJECTTYPE)GetProcAddress(hwnd, "queryAllObjectTypes");
		if (!Proc1)
		{
			FS_LOG(FSL_DEBUG, "     DLL %s does not expose  objects", filename.c_str());
			return;
		}

		Proc1(&thelist);

		// we have alist of objects, now store them individually with info
		list<stringc>::Iterator it = thelist.begin();
		for (; it != thelist.end(); ++it)
		{
			FSObjectData* data = new FSObjectData;
			m_List.push_back(data);
			data->DLLName = filename;
			data->ObjectName = (*it).c_str();

			// description query 
			typedef stringc(*QUERY_OBJECTINFO)(stringc objtype);
			QUERY_OBJECTINFO Proc4 = (QUERY_OBJECTINFO)GetProcAddress(hwnd, "queryObjectDescription");
			if (!Proc4) { FS_LOG(FSL_DEBUG, "     unable to access object info %s", data->ObjectName.c_str()); }
			else
			{
				// use the function
				stringc info = Proc4(data->ObjectName);
				data->Info = info;
			}
			FS_LOG(FSL_DEBUG, "          %s %s", data->ObjectName.c_str(), data->Info.c_str());
		}
	}

} // end namespace