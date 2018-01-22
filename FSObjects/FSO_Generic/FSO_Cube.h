#pragma once

#include "FSO_Generic.h"

#include "FSLevel.h"

namespace FS
{
	/*
	a simple cube object
	adds texturefilename to supply texture to the scenenode
	*/
	class FSO_Cube : public FSObject
	{
	private:
		// object variables
		IMeshSceneNode* m_Cube;			// the primary scenenode
		stringc m_TextureFilename;	// the texture to apply to the cube

	public:
		// simple setters and getters 
		stringc getTextureFilename() { return m_TextureFilename; }	// create a simple getter
		virtual void setTextureFilename(stringc fn) { m_TextureFilename = fn; createSceneNodes(); }	// create a simple setter
		virtual void setTextureFilenameValue(stringc fn) { m_TextureFilename = fn; }	// create a simple setter

	public:
		// object properties
		ADD_PROPERTY(TextureFilename, stringcToStringc, stringcToStringc);										// add the variable to the property list

	public:
		// all obejcts must override this function
		virtual ISceneNode* getPrimarySceneNode() { return m_Cube; }

		// set all variables to a known value
		virtual void FSO_Cube::initialize()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::initialize()");

			// call the base class
			FSObject::initialize();

			// set these to a knwon value
			m_Cube = 0;
			setScaleValue(vector3df(30, 30, 30));
			m_TextureFilename = "wall.bmp";
		}

		// dual creation allows for better error handling
		virtual bool FSO_Cube::create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_CUBE", "A simple cube with physx attached", 1.00f, "Seven");

			// Register class properties
			REG_PROPERTY(FSO_Cube, "TextureFilename", &FSO_Cube::fsSetTextureFilename, &FSO_Cube::fsGetTextureFilename, "FS_PCHAR", "texture filename");

			// everything went fine
			return true;
		}

		virtual bool FSO_Cube::reCreate()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::reCreate()");

			// call the base class
			FSObject::reCreate();

			// recursively set the ids for this object
			setId(getId());

			// everything went fine
			return true;
		}

		virtual void FSO_Cube::createSceneNodes()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::createSceneNodes()");

			// call the base class
			FSObject::createSceneNodes();

			// create a simple cube node
			m_Cube = getSmgr()->addCubeSceneNode(1, 0, getId());
			m_Cube->setScale(getScale()*getScaleOffset());

			// set the texture based on the property
			m_Cube->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(getTextureFilename())));

			// set up fog / light use
			m_Cube->setMaterialFlag(video::EMF_LIGHTING, getLevel()->getUseLight());
			m_Cube->setMaterialFlag(video::EMF_FOG_ENABLE, getLevel()->getUseFog());
		}

		virtual void FSO_Cube::destroyAllSceneNodes()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::destroyallscenenodes()");

			// remove the cube node
			if (m_Cube) m_Cube->remove(); m_Cube = 0;

			// call the base class
			FSObject::destroyAllSceneNodes();
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::serialize()");

			// call the base class
			FSObject::serializeAttributes(out, options);

			// add this objects properties
			out->addString("TextureFilename", m_TextureFilename.c_str());
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::deserialize()");

			// call the base class
			FSObject::deserializeAttributes(in, options);

			// get this objects' properties
			m_TextureFilename = in->getAttributeAsString("TextureFilename");
		}

		virtual void createPhysXObject()
		{
			// call the base class
			FSObject::createPhysXObject();

			// fill out the descriptor
			IPhysXObjectData data;
			data.userdata = getId();
			if (getMass() >0) data.type = POT_PRIMITIVE_BOX;
			else data.type = POT_PRIMITIVE_PLANE;
			data.position = getPosition();
			data.rotation = getRotation();
			data.scale = getScale();
			data.mass = getMass();
			if (getMass() > 0) data.dynamic = true; else data.dynamic = false;

			// set the newly created physx object
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}
	};

} // end namespace