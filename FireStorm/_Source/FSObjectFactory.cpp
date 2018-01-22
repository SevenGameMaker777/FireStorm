#include "FSLogger.h"
#include "FSObjectFactory.h"
#include "FSObjectManager.h"
#include "FSObject.h"
#include "FSLevel.h"
#include "FSApplication.h"

namespace FS
{
	void FSObjectFactory::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%s", "FSObjectFactory::initialize()");

		// set all variables toa known value
		m_Level = 0;
		m_ObjectManager = NULL;
		m_MessageManager = NULL;
		m_ClassManager = NULL;
		m_UniqueId = 1;
	}

	bool FSObjectFactory::create(FSLevel* level)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%s", "FSObjectFactory::create()");

		// remember this
		m_Level = level;

		// instantiate, initialize and create the objectfactory
		m_ClassManager = new FSClassManager();
		m_ClassManager->initialize();
		if (!m_ClassManager->create()) { FS_LOG(FSL_DEBUG, "%s", "unable to create classManager"); return false; }
		m_ClassManager->buildList(getLevel()->getDirectory("ObjectsDirectory"));

		// instantiate, initialize and create the objectfactory
		m_ObjectManager = new FSObjectManager();
		m_ObjectManager->initialize();
		if (!m_ObjectManager->create(level)) { FS_LOG(FSL_DEBUG, "%s", "unable to create objectManager"); return false; }

		// instantiate, initialize and create the MessageManager
		m_MessageManager = new FSMessageManager();
		m_MessageManager->initialize();
		if (!m_MessageManager->create(getObjectManager(), getLevel()->getApp(), getLevel())) { FS_LOG(FSL_DEBUG, "%s", "unable to create MessageManager"); return false; }

		// everything went fine
		return true;
	}

	bool FSObjectFactory::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%s", "FSObjectFactory::cleanup()");

		// destroy the class manager
		if (m_ClassManager) { m_ClassManager->cleanup(); delete(m_ClassManager); m_ClassManager = NULL; }

		// destroy the object manager
		if (m_ObjectManager) { m_ObjectManager->cleanup(); delete(m_ObjectManager); m_ObjectManager = NULL; }

		// destroy the Message manager
		if (m_MessageManager) { m_MessageManager->cleanup(); delete(m_MessageManager); m_MessageManager = NULL; }

		// forget this
		m_Level = 0;

		// always return false from a cleanup function
		return false;
	}

	// called before each frame
	void FSObjectFactory::preFrame(const float &elapsedtime, bool forceupdate)
	{
		// let the MessageManager do it's thing
		if (getMessageManager()) getMessageManager()->preFrame(elapsedtime);

		// let the ObjectManager do it's thing
		if (getObjectManager())
		{
			getObjectManager()->preFrame(elapsedtime, forceupdate);
		}
	}

	// called during each frame
	void FSObjectFactory::frame(const float &elapsedtime, bool forceupdate)
	{
		// let the ObjectManager do it's thing
		if (getObjectManager()) getObjectManager()->frame(elapsedtime, forceupdate);

		// let the MessageManager do it's thing
		if (getMessageManager()) getMessageManager()->frame(elapsedtime);
	}

	// called after each frame
	void FSObjectFactory::postFrame(const float &elapsedtime, bool forceupdate)
	{
		// let the MessageManager do it's thing
		if (getMessageManager()) getMessageManager()->postFrame(elapsedtime);

		// let the ObjectManager do it's thing
		if (getObjectManager()) getObjectManager()->postFrame(elapsedtime, forceupdate);
	}

	void FSObjectFactory::clear()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObjectFactory::clear()");

		// let the MessageManager do it's thing
		if (getMessageManager()) getMessageManager()->clear();

		// let the ObjectManager do it's thing
		if (getObjectManager()) getObjectManager()->destroyAllObjects();
	}

	// get a pointer to an object in the list
	FSObject* FSObjectFactory::getObjectPointer(int id, bool deep)
	{
		return m_ObjectManager->getObjectPointer(id, deep);
	}

	// setup light use
	void FSObjectFactory::setUseLight(bool value)
	{
		if (getObjectManager())	getObjectManager()->setUseLight(value);
	}

	// setup fog use
	void FSObjectFactory::setUseFog(bool value)
	{
		if (getObjectManager())	getObjectManager()->setUseFog(value);
	}

	int FSObjectFactory::getUniqueId()
	{
		m_UniqueId++;
		return m_UniqueId;
	}

	FSObject* FSObjectFactory::createObjectByType(stringc type, bool recreate, int id, stringc name, bool addtolevel)
	{
		if (!m_ClassManager) { FS_LOG(FSL_WARNING, "FSObjectFactory::createObjectByType() - ClassManager is not valid"); return NULL; }
		if (!m_ObjectManager) { FS_LOG(FSL_WARNING, "FSObjectFactory::createObjectByType() - ObjectManager is not valid");  return NULL; }

		FSObjectData* data = m_ClassManager->findClassInfo(type);
		if (!data) { FS_LOG(FSL_WARNING, "FSObjectFactory::AddObjectByType() - unable to locate class definition"); return NULL; }

		return createObjectByTypeAndDLL(data->DLLName, data->ObjectName, recreate, id, name, addtolevel);
	}

	// create an object from class name
	FSObject* FSObjectFactory::createObjectByTypeAndDLL(stringc dllname, stringc objtype, bool recreate, int id, stringc name, bool addtolevel)
	{
		_chdir(getLevel()->getApp()->getDirectory("workingDirectory").c_str());

		// make sure our pointers are valid
		if (!m_ClassManager) { FS_LOG(FSL_WARNING, "FSObjectFactory::createObjectByTypeAndDLL() - ClassManager is not valid"); return NULL; }
		if (!m_ObjectManager) { FS_LOG(FSL_WARNING, "FSObjectFactory::createObjectByTypeAndDLL() - ObjectManager is not valid"); return NULL; }

		// get the data structure for this object definition
		FSObjectData* data = m_ClassManager->findClassInfo(dllname, objtype);
		if (!data) { FS_LOG(FSL_WARNING, "FSObjectFactory::createObjectByTypeAndDLL() - unable to locate class definition"); return NULL; }

		// get the dll path name to load
		stringc filename = m_ClassManager->getObjectDirectory() + data->DLLName;

		// attempt to load the dll
		HMODULE hwnd = LoadLibrary(stringc(filename).c_str());
		if (!hwnd) { FS_LOG(FSL_WARNING, "FSObjectFactory::createObjectByTypeAndDLL() - unable to load DLL file"); return NULL; }

		// find the object create function 
		typedef void* (*createOBJECT)(stringc objtype);
		createOBJECT createProc = (createOBJECT)GetProcAddress(hwnd, "createObject");

		// create the object
		FSObject* theObj = (FSObject*)createProc(data->ObjectName);
		if (!theObj) { FS_LOG(FSL_WARNING, "FSObjectFactory::createObjectByTypeAndDLL() - object creation failed"); return NULL; }

		// let the object manager set the object's ID. we can assure unique ID's this way
		if (id == 0)
		{
			int idMax = 1;
			FSObject* o = getObjectManager()->getNextObject(true);
			while (o)
			{
				if (o->getId() >= idMax) idMax = o->getId();
				o = getObjectManager()->getNextObject(false);
			}
			m_UniqueId = idMax + 1;
			id = getUniqueId();
		}

		// initiliaze and create the object
		theObj->initialize();
		theObj->create(getLevel(), id, name);
		if (recreate) theObj->reCreate();

		// add it to the object manager
		if (addtolevel) m_ObjectManager->addObject(theObj);

		// object created and added to list successfully
		return theObj;
	}

	void FSObjectFactory::savePrefab(FSObject* obj, stringc filename)
	{
		if (!obj) return;

		FS_LOG(FSL_DEBUG, "%s %s", "Saving prefab file", filename);

		// create a temporary device
		IrrlichtDevice* device = createDevice(EDT_NULL);

		if (device)
		{
			stringc fn = getLevel()->getDirectory("PrefabDirectory");
			fn += filename;

			// create the xml writer
			IXMLWriter* writer = device->getFileSystem()->createXMLWriter(fn);
			if (writer)
			{
				// write the xml header
				writer->writeXMLHeader();

				// write the node type
				stringw name("FSOBJECT");
				writer->writeElement(name.c_str(), false, L"TYPE", stringw(obj->getInfo()->getName()).c_str());
				writer->writeLineBreak();

				// let the object serialize itself into our attributes structure
				IAttributes* attr = getLevel()->getDevice()->getFileSystem()->createEmptyAttributes(getLevel()->getDriver());
				if (attr)
				{
					SAttributeReadWriteOptions options;
					obj->serializeAttributes(attr, &options);

					// if there are attributes
					if (attr->getAttributeCount() != 0)
					{
						// write the attributes to the xml file
						attr->write(writer);

						// make the file pretty
						writer->writeLineBreak();
					}

					// drop the pointer
					attr->drop();
				}

				// finish writing the xml header / footer
				writer->writeClosingTag(name.c_str());
				writer->writeLineBreak();
				writer->writeLineBreak();

				// drop the pointer
				writer->drop();
			}

			// drop the temporary device
			device->drop();
		}
	}

	FSObject*  FSObjectFactory::loadPrefab(stringc filename)
	{
		FSObject* obj = 0;

		// create a temporary device
		IrrlichtDevice* device = createDevice(EDT_NULL);

		if (device)
		{
			stringc fn = getLevel()->getApp()->getDirectory("PrefabDirectory") + filename;// +".prefab";
			FS_LOG(FSL_DEBUG, "loading prefab %s", fn.c_str());

			// attempt to open the file
			IXMLReader* reader = device->getFileSystem()->createXMLReader(fn);
			if (reader)
			{
				FS_LOG(FSL_DEBUG, " reader created....");
				// read file
				while (reader->read())
				{
					// based on the node type
					switch (reader->getNodeType())
					{
					case io::EXN_ELEMENT:
					{
						// get the node name
						stringw name = reader->getNodeName();

						// if this is an object definition
						if (stringw("FSOBJECT") == name)
						{
							// get the object type
							stringw type = reader->getAttributeValueSafe(L"TYPE");

							// attempt to create the object
							obj = createObjectByType(stringc(type), false, 0, stringc(type), true);
							if (obj)
							{
								FS_LOG(FSL_DEBUG, " object created....");

								// load the attributes from the file
								IAttributes* attr = getLevel()->getDevice()->getFileSystem()->createEmptyAttributes(getLevel()->getDriver());
								attr->read(reader, false);

								// let the object deserialize from the attributes
								obj->deserializeAttributes(attr);

								// update the id
								obj->setId(getUniqueId());

								// recreate the object using the new attribtues
								obj->reCreate();

								// drop the pointer
								attr->drop();
							}
						}
					}
					}
				}
			} // reader

			  // drop the temporary device
			device->drop();
		} // device

		return obj;
	}

	bool FSObjectFactory::objectTypeExists(stringc ot)
	{
		if (m_ClassManager->findClassInfo(ot))
			return true; else return false;
	}

	void FSObjectFactory::setUseEditorVisuals(bool showValue)
	{
		if (getObjectManager()) getObjectManager()->setUseEditorVisuals(showValue);
	}

} // end namespace