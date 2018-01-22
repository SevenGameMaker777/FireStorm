#include "FSutils.h"
#include "FSLogger.h"
#include "FSPropertyManager.h"
#include "FSObject.h"

namespace FS
{
	varData::varData() { initialize(); }
	varData::~varData() { cleanup(); }

	void varData::initialize()
	{
		vName = FS_DEFAULT;	// variable name			example	m_Id
		vType = FS_DEFAULT;	// variable type			example	int
		vControl = FS_DEFAULT;	// variable control type	example D_INT
		vAccess = 0;			// variable access			example true
		vCallback = 0;			// variable callback		used to validate data
		vDescription = FS_DEFAULT;	// description of variable	example this is the unique id of the object
		vComm = FS_DEFAULT;	// communication to editor	example value clipped to 40
		vMin = FS_DEFAULT;	// minimum value
		vMax = FS_DEFAULT;	// maximum value
	}

	bool varData::cleanup()
	{
		if (vCallback)
		{
			delete (CCallback<FSObject>*) vCallback;
			vCallback = NULL;
		}
		return false;
	}

	void varData::setComm(stringc c)
	{
		vComm = c;
	}

	void varData::clearComm()
	{
		vComm = stringc("");
	}

	void FSPropertyManager::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSPropertyManager::Initialize()");
	}

	bool FSPropertyManager::create()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSPropertyManager::create()");

		// everything went fine
		return true;
	}

	bool FSPropertyManager::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSPropertyManager::cleanup()");

		list<varData*>::Iterator it = m_List.begin();
		for (; it != m_List.end();)
		{
			delete (*it);
			it = m_List.erase(it);
		}

		// always return false from a cleanup function
		return false;
	}

	stringc FSPropertyManager::getVariable(stringc variable)
	{
		list<varData*>::Iterator it = m_List.begin();
		for (; it != m_List.end(); it++)
		{
			if (variable == (*it)->vName)
				return ((CCallback<FSObject> *)(*it)->vCallback)->getProperty();
		}
		return stringc(FS_DEFAULT);
	}

	stringc FSPropertyManager::getVariable(int index)
	{
		int i = 0;
		list<varData*>::Iterator it = m_List.begin();
		for (; it != m_List.end(); it++)
		{
			if (index == i)
			{
				return ((CCallback<FSObject> *)(*it)->vCallback)->getProperty();
			}
			i++;
			it++;
		}
		return stringc(FS_DEFAULT);
	}

	bool FSPropertyManager::setVariable(stringc variable, stringc value)
	{
		list<varData*>::Iterator it = m_List.begin();
		for (; it != m_List.end(); it++)
		{
			if (variable == (*it)->vName)
			{
				if ((*it)->vAccess)
				{
					((CCallback<FSObject> *)(*it)->vCallback)->setProperty(value);
					return true;
				}
				else return false;
			}
		}
		return false;
	}

	void FSPropertyManager::add(varData* data)
	{
		m_List.push_back(data);
	}

	void FSPropertyManager::remove(varData* data)
	{
		list<varData*>::Iterator it = m_List.begin();
		for (; it != m_List.end();)
		{
			if ((*it) == data)
			{
				delete (*it);
				it = m_List.erase(it);
			}
			else it++;
		}
	}

	varData* FSPropertyManager::getVarData(stringc variable)
	{
		list<varData*>::Iterator it = m_List.begin();
		for (; it != m_List.end(); it++)
		{
			if ((*it)->vName == variable) return (*it);
		}
		// unable to locate requested variable	
		return NULL;
	}

} // end namespace