#pragma once

#include "FSO_Generic.h"

#include "FSLevel.h"

namespace FS
{
/*
	IAnimatedMesh* mesh = getSmgr()->addHillPlaneMesh("myHill",
		core::dimension2d<f32>(20, 20),
		core::dimension2d<u32>(40, 40), 0, 0,
		core::dimension2d<f32>(0, 0),
		core::dimension2d<f32>(10, 10));
	ISceneNode* node = getSmgr()->addWaterSurfaceSceneNode(mesh->getMesh(0), 3.0f, 300.0f, 30.0f);
	node->setPosition(core::vector3df(0, 7, 0));
	node->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH("stones.jpg")));
	node->setMaterialTexture(1, getDriver()->getTexture(MEDIAPATH("water.jpg")));
	node->setMaterialType(video::EMT_REFLECTION_2_LAYER);
*/

	/*
	a simple cube object
	adds texturefilename to supply texture to the scenenode
	*/
	class FSO_Water : public FSObject
	{
	private:
		// object variables
		ISceneNode* m_Node;			// the primary scenenode
		stringc m_Texture1Filename;	// the texture to apply to the cube
		stringc m_Texture2Filename;	// the texture to apply to the cube

	public:
		// simple setters and getters 
		stringc getTexture1Filename() { return m_Texture1Filename; }	// create a simple getter
		virtual void setTexture1Filename(stringc fn) { m_Texture1Filename = fn; createSceneNodes(); }	// create a simple setter
		virtual void setTexture1FilenameValue(stringc fn) { m_Texture1Filename = fn; }	// create a simple setter

		stringc getTexture2Filename() { return m_Texture2Filename; }	// create a simple getter
		virtual void setTexture2Filename(stringc fn) { m_Texture2Filename = fn; createSceneNodes(); }	// create a simple setter
		virtual void setTexture2FilenameValue(stringc fn) { m_Texture2Filename = fn; }	// create a simple setter

	public:
		// object properties
		ADD_PROPERTY(Texture1Filename, stringcToStringc, stringcToStringc);										// add the variable to the property list
		ADD_PROPERTY(Texture2Filename, stringcToStringc, stringcToStringc);										// add the variable to the property list

	public:
		// all obejcts must override this function
		virtual ISceneNode* getPrimarySceneNode() { return m_Node; }

		// set all variables to a known value
		virtual void FSO_Water::initialize()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Water::initialize()");

			// call the base class
			FSObject::initialize();

			// set these to a knwon value
			m_Node = 0;
			m_Texture1Filename = "stones.jpg";
			m_Texture2Filename = "water.jpg";
		}

		// dual creation allows for better error handling
		virtual bool FSO_Water::create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Water::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_WATER", "A simple water mesh", 1.00f, "Seven");

			// Register class properties
			REG_PROPERTY(FSO_Water, "Texture1Filename", &FSO_Water::fsSetTexture1Filename, &FSO_Water::fsGetTexture1Filename, "FS_PCHAR", "texture filename");
			REG_PROPERTY(FSO_Water, "Texture2Filename", &FSO_Water::fsSetTexture2Filename, &FSO_Water::fsGetTexture2Filename, "FS_PCHAR", "texture filename");

			// everything went fine
			return true;
		}

		virtual bool FSO_Water::reCreate()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Water::reCreate()");

			// call the base class
			FSObject::reCreate();

			// recursively set the ids for this object
			setId(getId());

			// everything went fine
			return true;
		}

		virtual void FSO_Water::createSceneNodes()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Water::createSceneNodes()");

			// call the base class
			FSObject::createSceneNodes();

			IAnimatedMesh* mesh = getSmgr()->addHillPlaneMesh(stringc( stringc("myHill")+stringc(getId())),
				core::dimension2d<f32>(20, 20),	
				core::dimension2d<u32>(40, 40), 0, 0,
				core::dimension2d<f32>(0, 0),
				core::dimension2d<f32>(10, 10));
			m_Node = getSmgr()->addWaterSurfaceSceneNode(mesh->getMesh(0), 3.0f, 300.0f, 30.0f);
			m_Node->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(getTexture1Filename())));
			m_Node->setMaterialTexture(1, getDriver()->getTexture(MEDIAPATH(getTexture2Filename())));
			m_Node->setMaterialType(video::EMT_REFLECTION_2_LAYER);

			// set the node material parameters
			m_Node->setMaterialFlag(video::EMF_LIGHTING, getLevel()->getUseLight());
			m_Node->setMaterialFlag(video::EMF_FOG_ENABLE, getLevel()->getUseFog());
		}

		virtual void FSO_Water::destroyAllSceneNodes()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Water::destroyallscenenodes()");

			// remove the cube node
			if (m_Node) m_Node->remove(); m_Node = 0;

			// call the base class
			FSObject::destroyAllSceneNodes();
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Water::serialize()");

			// call the base class
			FSObject::serializeAttributes(out, options);

			// add this objects properties
			out->addString("Texture1Filename", m_Texture1Filename.c_str());
			out->addString("Texture2Filename", m_Texture2Filename.c_str());
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Water::deserialize()");

			// call the base class
			FSObject::deserializeAttributes(in, options);

			// get this objects' properties
			m_Texture1Filename = in->getAttributeAsString("Texture1Filename");
			m_Texture2Filename = in->getAttributeAsString("Texture2Filename");
		}

	};

} // end namespace