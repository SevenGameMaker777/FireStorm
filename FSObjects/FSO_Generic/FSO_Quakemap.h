#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"

namespace FS
{

	class FSO_Quakemap : public FSObject
	{
	private:
		IMeshSceneNode* m_Node;
		stringc m_QuakeMapFilename;
	public:
		void setQuakeMapFilename(stringc fn) { m_QuakeMapFilename = fn; reCreate(); }
		stringc getQuakeMapFilename() { return m_QuakeMapFilename; }

		ADD_PROPERTY(QuakeMapFilename, stringcToStringc, stringcToStringc);

	public:
		virtual ISceneNode* getPrimarySceneNode() { return m_Node; }
		virtual void destroyAllSceneNodes()	{ if (m_Node) m_Node->remove(); m_Node = 0;	}

		FSO_Quakemap() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Quakemap::FSO_Quakemap()"); }
		virtual ~FSO_Quakemap() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Quakemap::~FSO_Quakemap()"); }

		// set all variables to a known value
		virtual void initialize()
		{
			FSObject::initialize();

			m_Node = 0;
			m_QuakeMapFilename = FS_DEFAULT;
			//setStatic(true);
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Quakemap::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_QUAKEMAP", "A simple quakemap with physx attached", 1.00f, "Seven");

			// load the pk3 file
			getDevice()->getFileSystem()->addFileArchive(MEDIAPATH("map-20kdm2.pk3"));

			// set a default editor mesh to display
			m_QuakeMapFilename = "20kdm2.bsp";

			// register the filename as an editable property
			REG_PROPERTY(FSO_Quakemap, "QuakeMapFilename", &FSO_Quakemap::fsSetQuakeMapFilename, &FSO_Quakemap::fsGetQuakeMapFilename, "FS_FILE", "quake map filename");

			// set the object type
			setObjectType(FSOT_STATIC);

			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();

			// attempt to load the mesh
			IAnimatedMesh* mesh = getSmgr()->getMesh(MEDIAPATH(getQuakeMapFilename()));

			// if we failed to load the mesh
			if (!mesh)
			{
				// let the user know about the failure
				FS_LOG(FSL_WARNING, "%d %s FSO_Quake3Map::createSceneNodes() failed to load mesh(%s)", getId(), getName().c_str(), getQuakeMapFilename().c_str());

				// bail out of here
				return;
			}

			// if we loaded a mesh, create a scenenode with it
			m_Node = getSmgr()->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);

			// if we failed to create the scenenode
			if (!m_Node)
			{
				// let the user know about the failure
				FS_LOG(FSL_WARNING, "%d %s FSO_Quake3Map::createSceneNodes() failed to create animatedmeshscenenode(%s)", getId(), getName().c_str(), getQuakeMapFilename().c_str());

				// bail out of here
				return;
			}

			// set a few material variables for this model
			m_Node->setMaterialFlag(EMF_LIGHTING, getLevel()->getUseLight());
			m_Node->setMaterialFlag(EMF_FOG_ENABLE, getLevel()->getUseFog());

			setId(getId());
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Quakemap::serialize()");

			FSObject::serializeAttributes(out, options);
			out->addString("QuakeMapFileName", m_QuakeMapFilename.c_str());
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Quakemap::deserialize()");

			FSObject::deserializeAttributes(in, options);
			m_QuakeMapFilename = in->getAttributeAsString("QuakeMapFileName");
		}

		virtual void createPhysXObject()
		{
			FSObject::createPhysXObject();

			if (!m_Node) return;

			IPhysXObjectData data;
			data.userdata = getId();
			data.type = POT_TREE;
			data.node = m_Node;
			data.mesh = m_Node->getMesh();
			data.position = getPosition();
			data.rotation = getRotation();
			data.scale = getScale();
			data.mass = 0;
			data.dynamic = false;
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}
	};

} // end namespace#pragma once
