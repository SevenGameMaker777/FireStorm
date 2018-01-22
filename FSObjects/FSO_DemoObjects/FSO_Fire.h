#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"
#include "FSPhysX.h"

namespace FS
{
	class FSO_Fire : public FSObject
	{
	private:
		FS_VARIABLE_GET(IParticleSystemSceneNode*, Ps);
		FS_VARIABLE_GET(IParticleEmitter*, Em);
		FS_VARIABLE_GET(IParticleAffector*, Paf);
		FS_VARIABLE_GET(float, Timer);
		FS_VARIABLE_GET(bool, Dying);

	public:

	public:
		virtual ISceneNode* getPrimarySceneNode() { return m_Ps; }

		virtual void destroyAllSceneNodes()
		{
			// call the base class
			FSObject::destroyAllSceneNodes();
	
			// remove the node
			if (m_Ps) m_Ps->remove(); m_Ps = 0;
		}

		FSO_Fire() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Fire::FSO_Fire()"); }
		virtual ~FSO_Fire() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Fire::~FSO_Fire()"); }

		// set all variables to a known value
		virtual void initialize()
		{
			FSObject::initialize();

			m_Dying = 0;
			m_Ps = 0;
			m_Em = 0;
			m_Paf = 0;
			setBBOffsetValue(vector3df(20, 40, 20));
			//setStatic(true);
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Fire::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_FIRE", "A simple light source", 1.00f, "Seven");

			setObjectType(FSOT_OBJECT);

			// everything went fine
			return true;
		}

		virtual bool reCreate()
		{
			FSObject::reCreate();

			// add a light to the fire
			FSObject* light = getLevel()->getObjectFactory()->createObjectByType("FSO_LIGHT", true, 0, "TorchLight", false);
			if (light)
			{
				light->setScale(vector3df(400, 1, 1));
				light->setPosition(vector3df(0, getScale().Y * 60, 0));
				light->setVariable("Diffuse", SColorfToStringc(SColorf(100, 50, 50, 255)));
				addChild(light);
			}

			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();

			m_Ps = getSmgr()->addParticleSystemSceneNode(false);
			m_Em = m_Ps->createBoxEmitter(
				aabbox3d<f32>(-getScale().X, -getScale().Y, -getScale().Z, getScale().X, getScale().Y, getScale().Z),
				vector3df(0.0f, 0.1f, 0.0f), 50, 200, SColor(0, 255, 0, 0),
				SColor(0, 255, 255, 255), 650, 650, 10, dimension2df(20.f, 20.f), dimension2df(20.0f, 20.0f));
			m_Ps->setEmitter(m_Em);
			m_Em->drop();

			m_Paf = m_Ps->createFadeOutParticleAffector();
			m_Ps->addAffector(m_Paf);
			m_Paf->drop();

			m_Ps->setPosition(getPosition());
			m_Ps->setScale(getScale()); // vector3df(1.0f, 0.1f, 1.0f));
			m_Ps->setMaterialFlag(EMF_LIGHTING, false);
			m_Ps->setMaterialFlag(EMF_ZWRITE_ENABLE, false);
			m_Ps->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH("fireball.bmp")));
			m_Ps->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);


			setId(getId());
		}

		// frame functions 
		void FSO_Fire::preFrame(const float &elapsedtime, bool forceupdate)
		{
			if (m_Dying)
			{
				m_Timer -= elapsedtime;
				if (m_Timer <= 0) setDead(true);
				else
				{
					if (m_Ps)
					{
						m_Ps->getEmitter()->setMaxParticlesPerSecond(0.01);
						m_Ps->getEmitter()->setMinStartColor(SColor(50, 50, 50, 50));
						m_Ps->getEmitter()->setMaxStartColor(SColor(100, 100, 100, 100));
					}
				}
			}

			// call the base class
			FSObject::preFrame(elapsedtime, forceupdate);
		}

		void FSO_Fire::setDying(bool value)
		{
			m_Dying = value;
		}

		void FSO_Fire::setDead(bool value)
		{
			if (m_Dying) FSObject::setDead(value);
			else setDying(true);
		}

		virtual void createPhysXObject()
		{
			FSObject::createPhysXObject();

			if (!m_Ps) return;

			IPhysXObjectData data;
			data.userdata = getId();
			data.type = POT_TRIGGER;
			data.node = m_Ps;
			data.mesh = 0;
			data.position = getPosition();
			data.rotation = getRotation();
			data.scale = getScale();
			data.bboffset = getBBOffset();
			data.mass = 0;
			data.dynamic = false;
			data.istrigger = true;
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Fire::serialize()");

			FSObject::serializeAttributes(out, options);
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Fire::deserialize()");

			FSObject::deserializeAttributes(in, options);
		}

	};

} // end namespace
