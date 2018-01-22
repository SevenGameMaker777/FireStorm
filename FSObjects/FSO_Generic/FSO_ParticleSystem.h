#pragma once

#include "FSO_Generic.h"

#include "FSLevel.h"

namespace FS
{
	/*
	a particle system object
	*/
	class FSO_ParticleSystem : public FSObject
	{
	private:
		// object variables
		IParticleSystemSceneNode* m_Node;			// the primary scenenode
		IParticleEmitter* m_EM;
		IParticleAffector* m_PAF;
		stringc m_TextureFilename;	// the texture to apply to the cube

		//! box particle emitter.
		aabbox3df m_Box;
		vector3df m_Direction;
		u32 m_MinParticlesPerSecond;
		u32 m_MaxParticlesPerSecond;
		SColor m_MinStartColor;
		SColor m_MaxStartColor;
		u32 m_LifeTimeMin;
		u32 m_LifeTimeMax;
		s32 m_MaxAngleDegrees;
		dimension2df m_MinStartSize;
		dimension2df m_MaxStartSize;

	public:
		// simple setters and getters 
		stringc			getTextureFilename() { return m_TextureFilename; }	// create a simple getter
		aabbox3df		getBox() { return m_Box; }
		vector3df		getDirection()				{ return m_Direction; }
		u32				getMinParticlesPerSecond()	{ return m_MinParticlesPerSecond; }
		u32				getMaxParticlesPerSecond()	{ return m_MaxParticlesPerSecond;}
		SColor			getMinStartColor()			{ return m_MinStartColor; }
		SColor			getMaxStartColor()			{ return m_MaxStartColor; }
		u32				getLifeTimeMin()			{ return m_LifeTimeMin; }
		u32				getLifeTimeMax()			{ return m_LifeTimeMax; }
		s32				getMaxAngleDegrees()		{ return m_MaxAngleDegrees; }
		dimension2df	getMinStartSize()			{ return m_MinStartSize; }
		dimension2df	getMaxStartSize()			{ return m_MaxStartSize; }

		virtual void setTextureFilename(stringc fn) { m_TextureFilename = fn; createSceneNodes(); }	// create a simple setter
		virtual void setTextureFilenameValue(stringc fn) { m_TextureFilename = fn; }	// create a simple setter
		void	setBox(aabbox3df d)				{ m_Box = d; createBoxEmitter();	}
		void 	setDirection(vector3df d)		{ m_Direction = d; createBoxEmitter();	}
		void	setMinParticlesPerSecond(u32 d) { m_MinParticlesPerSecond = d; createBoxEmitter(); }
		void	setMaxParticlesPerSecond(u32 d) { m_MaxParticlesPerSecond = d; createBoxEmitter(); }
		void	setMinStartColor(SColor d)		{ m_MinStartColor = d; createBoxEmitter(); }
		void	setMaxStartColor(SColor d)		{ m_MaxStartColor = d; createBoxEmitter(); }
		void	setLifeTimeMin(u32 d)			{ m_LifeTimeMin = d; createBoxEmitter();	}
		void	setLifeTimeMax(u32 d)			{ m_LifeTimeMax = d; createBoxEmitter();	}
		void	setMaxAngleDegrees(s32 d)		{ m_MaxAngleDegrees = d; createBoxEmitter();	}
		void	setMinStartSize(dimension2df d) { m_MinStartSize = d; createBoxEmitter(); }
		void	setMaxStartSize(dimension2df d) { m_MaxStartSize = d; createBoxEmitter(); }
	public:

		// object properties
		ADD_PROPERTY(TextureFilename, stringcToStringc, stringcToStringc);	
		ADD_PROPERTY(Box, stringcToaabbox3df, aabbox3dfToStringc);
		ADD_PROPERTY(Direction, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(MinParticlesPerSecond, stringcToInt, intToStringc);
		ADD_PROPERTY(MaxParticlesPerSecond, stringcToInt, intToStringc);
		ADD_PROPERTY(MinStartColor, stringcToSColor, SColorToStringc);
		ADD_PROPERTY(MaxStartColor, stringcToSColor, SColorToStringc);
		ADD_PROPERTY(LifeTimeMin, stringcToInt, intToStringc);
		ADD_PROPERTY(LifeTimeMax, stringcToInt, intToStringc);
		ADD_PROPERTY(MaxAngleDegrees, stringcToInt, intToStringc);
		ADD_PROPERTY(MinStartSize, stringcToDimension2df, dimension2dfToStringc);
		ADD_PROPERTY(MaxStartSize, stringcToDimension2df, dimension2dfToStringc);

	public:
		// all obejcts must override this function
		virtual ISceneNode* getPrimarySceneNode() { return m_Node; }

		// set all variables to a known value
		virtual void FSO_ParticleSystem::initialize()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_ParticleSystem::initialize()");

			// call the base class
			FSObject::initialize();

			// set these to a knwon value
			m_Node = 0;
			m_EM = 0;
			m_PAF = 0;
			m_TextureFilename = "wall.bmp";

			m_Box = aabbox3d<f32>(-7, 0, -7, 7, 1, 7); // emitter size
			m_Direction = vector3df(0.0f, 0.06f, 0.0f);   // initial direction
			m_MinParticlesPerSecond = 80;
			m_MaxParticlesPerSecond = 100;                        // emit rate
			m_MinStartColor = SColor(0, 255, 255, 255);   // darkest color
			m_MaxStartColor = SColor(0, 255, 255, 255);   // brightest color
			m_LifeTimeMin = 800;						// min age
			m_LifeTimeMax = 2000;						// max age
			m_MaxAngleDegrees = 0;							// angle
			m_MinStartSize = dimension2df(10.f, 10.f);	// min size
			m_MaxStartSize = dimension2df(20.f, 20.f);	// max size
		}

		// dual creation allows for better error handling
		virtual bool FSO_ParticleSystem::create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_ParticleSystem::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_PARTICLESYSTEM", "A simple particle system", 1.00f, "Seven");

			aabbox3df Box;
			vector3df Direction;
			u32 MinParticlesPerSecond;
			u32 MaxParticlesPerSecond;
			SColor MinStartColor;
			SColor MaxStartColor;
			u32 LifeTimeMin;
			u32 LifeTimeMax;
			s32 MaxAngleDegrees;
			dimension2df MinStartSize;
			dimension2df MaxStartSize;

			// Register class properties
			REG_PROPERTY(FSO_ParticleSystem, "TextureFilename", &FSO_ParticleSystem::fsSetTextureFilename, &FSO_ParticleSystem::fsGetTextureFilename, "FS_PCHAR", "texture filename");
			REG_PROPERTY(FSO_ParticleSystem, "Box", &FSO_ParticleSystem::fsSetBox, &FSO_ParticleSystem::fsGetBox, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "Direction", &FSO_ParticleSystem::fsSetDirection, &FSO_ParticleSystem::fsGetDirection, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "MinParticlesPerSecond", &FSO_ParticleSystem::fsSetMinParticlesPerSecond, &FSO_ParticleSystem::fsGetMinParticlesPerSecond, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "MaxParticlesPerSecond", &FSO_ParticleSystem::fsSetMaxParticlesPerSecond, &FSO_ParticleSystem::fsGetMaxParticlesPerSecond, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "MinStartColor", &FSO_ParticleSystem::fsSetMinStartColor, &FSO_ParticleSystem::fsGetMinStartColor, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "MaxStartColor", &FSO_ParticleSystem::fsSetMaxStartColor, &FSO_ParticleSystem::fsGetMaxStartColor, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "LifeTimeMin", &FSO_ParticleSystem::fsSetLifeTimeMin, &FSO_ParticleSystem::fsGetLifeTimeMin, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "LifeTimeMax", &FSO_ParticleSystem::fsSetLifeTimeMax, &FSO_ParticleSystem::fsGetLifeTimeMax, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "MaxAngleDegrees", &FSO_ParticleSystem::fsSetMaxAngleDegrees, &FSO_ParticleSystem::fsGetMaxAngleDegrees, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "MinStartSize", &FSO_ParticleSystem::fsSetMinStartSize, &FSO_ParticleSystem::fsGetMinStartSize, "FS_PCHAR", "box emitter param");
			REG_PROPERTY(FSO_ParticleSystem, "MaxStartSize", &FSO_ParticleSystem::fsSetMaxStartSize, &FSO_ParticleSystem::fsGetMaxStartSize, "FS_PCHAR", "box emitter param");

			// everything went fine
			return true;
		}

		virtual bool FSO_ParticleSystem::reCreate()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_ParticleSystem::reCreate()");

			// call the base class
			FSObject::reCreate();

			// recursively set the ids for this object
			setId(getId());

			// everything went fine
			return true;
		}

		void createBoxEmitter()
		{
			if (!m_Node) return;

			scene::IParticleEmitter* em = m_Node->createBoxEmitter(
				getBox(), // emitter size
				getDirection(),   // initial direction
				getMinParticlesPerSecond(), getMaxParticlesPerSecond(),  // emit rate
				getMinStartColor(),       // darkest color
				getMaxStartColor(),       // brightest color
				getLifeTimeMin(), getLifeTimeMax(), getMaxAngleDegrees(), // min and max age, angle
				getMinStartSize(),         // min size
				getMaxStartSize());        // max size

			m_Node->setEmitter(em); // this grabs the emitter
			em->drop(); // so we can drop it here without deleting it

			scene::IParticleAffector* paf = m_Node->createFadeOutParticleAffector();

			m_Node->addAffector(paf); // same goes for the affector
			paf->drop();
		}

		virtual void FSO_ParticleSystem::createSceneNodes()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_ParticleSystem::createSceneNodes()");

			// call the base class
			FSObject::createSceneNodes();

			// create a particle system
			m_Node = getSmgr()->addParticleSystemSceneNode(false);
			createBoxEmitter();

			m_Node->setMaterialFlag(video::EMF_LIGHTING, false);
			m_Node->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
			m_Node->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(getTextureFilename().c_str())));
			m_Node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		}

		virtual void FSO_ParticleSystem::destroyAllSceneNodes()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_ParticleSystem::destroyallscenenodes()");

			// remove the cube node
			if (m_Node) m_Node->remove(); m_Node = 0;

			// call the base class
			FSObject::destroyAllSceneNodes();
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_ParticleSystem::serialize()");

			// call the base class
			FSObject::serializeAttributes(out, options);

			// add this objects properties
			out->addBox3d("Box", getBox());
			out->addVector3d("Direction", getDirection());
			out->addInt("MinParticlesPerSecond", getMinParticlesPerSecond());
			out->addInt("MaxParticlesPerSecond", getMaxParticlesPerSecond());
			out->addColor("MinStartColor", getMinStartColor());
			out->addColor("MaxStartColor", getMaxStartColor());
			out->addInt("LifeTimeMin", getLifeTimeMin());
			out->addInt("LifeTimeMax", getLifeTimeMax());
			out->addInt("MaxAngleDegrees", getMaxAngleDegrees());
			out->addVector3d("MinStartSize", vector3df(getMinStartSize().Width,0, getMinStartSize().Height));
			out->addVector3d("MaxStartSize", vector3df(getMaxStartSize().Width,0, getMaxStartSize().Height));
			out->addString("TextureFilename", m_TextureFilename.c_str());
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_ParticleSystem::deserialize()");

			// call the base class
			FSObject::deserializeAttributes(in, options);

			// get this objects' properties
			aabbox3df		m_Box					= in->getAttributeAsBox3d("Box");
			vector3df		m_Direction				= in->getAttributeAsVector3d("Direction");
			u32				m_MinParticlesPerSecond = in->getAttributeAsInt("MinParticlesPerSecond");
			u32				m_MaxParticlesPerSecond = in->getAttributeAsInt("MaxParticlesPerSecond");
			SColor			m_MinStartColor			= in->getAttributeAsColor("MinStartColor");
			SColor			m_MaxStartColor			= in->getAttributeAsColor("MaxStartColor");
			u32				m_LifeTimeMin			= in->getAttributeAsInt("LifeTimeMin");
			u32				m_LifeTimeMax			= in->getAttributeAsInt("LifeTimeMax");
			s32				m_MaxAngleDegrees		= in->getAttributeAsInt("MaxAngleDegrees");
			vector3df		d = in->getAttributeAsVector3d("MinStartSize");
			m_MinStartSize = dimension2df(d.X, d.Z);
			vector3df		d2 = in->getAttributeAsVector3d("MaxStartSize");
			m_MaxStartSize = dimension2df(d2.X, d2.Z);
			m_TextureFilename = in->getAttributeAsString("TextureFilename");
		}

	};

} // end namespace
