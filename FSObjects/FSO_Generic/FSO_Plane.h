#pragma once

#include "FSO_Generic.h"
#include "FSLevel.h"
#include "FSO_Cube.h"
#include "FSPhysX.h"

namespace FS
{
	/*
		a simple test of the ability to derive objects from other objects 
	*/
	class FSO_Plane : public FSO_Cube
	{
	private:
		// no new variables, just a special case FSO_Cube derived object

	public:
		virtual void FSO_Plane::initialize()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Plane::initialize()");

			// call the base class
			FSO_Cube::initialize();

			// use a different texture than the base cube class so we can be sure this works
			setTextureFilenameValue("wall.jpg");
		}

		// dual creation allows for better error handling
		virtual bool FSO_Plane::create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_PLANE", "A simple plane with physx attached", 1.00f, "Seven");

			// everything went fine
			return true;
		}

		virtual void createPhysXObject()
		{
			// call the original base class, note we dont call the cube class since it create sa different physx object
			FSObject::createPhysXObject();

			// fill out the physx creation structure
			IPhysXObjectData data;
			data.userdata	= getId();
			data.type		= POT_PRIMITIVE_PLANE;
			data.position	= getPosition();
			data.rotation	= getRotation();
			data.scale		= getScale();
			data.mass		= 0;
			data.dynamic	= false;

			// set the newly created physx object
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}
	};

} // end namespace