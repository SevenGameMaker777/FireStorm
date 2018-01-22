#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"
#include "FSPhysX.h"
#include "FSO_Static.h"

namespace FS
{
	class FSO_Torch : public FSO_Static
	{
	private:
		bool m_Lit;
	public:
		// object properties
		ADD_PROPERTY(Lit, stringcToBool, boolToStringc); // add the variable to the property list
	public:
		virtual bool getLit()		{ return m_Lit; }
		virtual void setLit(bool v) 
		{ 
			m_Lit = v;
			reCreate();
		}

		FSO_Torch() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Torch::FSO_Torch()"); }
		virtual ~FSO_Torch() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Torch::~FSO_Torch()"); }

		virtual void initialize()
		{
			FSO_Static::initialize();

			m_Lit = true;
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Torch::create()");

			// call the base class
			FSO_Static::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_TORCH", "A simple torch", 1.00f, "Seven");

			// Register class properties
			REG_PROPERTY(FSO_Torch, "Lit", &FSO_Torch::fsSetLit, &FSO_Torch::fsGetLit, "FS_BOOL", "Torch lit or unlit");

			// everything went fine
			return true;
		}

		virtual bool reCreate()
		{
			FSO_Static::reCreate();

			if (getLit())
			{
				// add a fire to the torch
				FSObject* fire = getLevel()->getObjectFactory()->createObjectByType("FSO_FIRE", true, 0, "TorchFire", false);
				if (fire)
				{
					addChild(fire);

					// add a fire to the torch
					fire->setPosition(vector3df(0, getScale().Y * 120, 0));
				}
			}

			return true;
		}
	};

} // end namespace
