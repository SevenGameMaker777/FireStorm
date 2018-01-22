#include "FSObject.h"
#include "FSLevel.h"
#include "FSLogger.h"
#include "FSUtils.h"
#include "FSObject_Character.h"
#include "FSPhysX.h"

// unclutter the global namespace
namespace FS
{
	// mandatory overrides
	ISceneNode* FSObject_Character::getPrimarySceneNode()
	{
		return m_Node;
	}

	void FSObject_Character::destroyAllSceneNodes()
	{
		// call the base class
		FSObject::destroyAllSceneNodes();

		// remove the cube node
		if (m_Node) m_Node->remove(); m_Node = 0;
	}

	// set all variables to a known value
	void FSObject_Character::initialize()
	{
		FS_LOG(FSL_ERROR, "FSObject_Character::initialize()");
		// call the base class
		FSObject::initialize();

		m_Brain = 0;
		m_Node = 0;
		m_ActorFilename = FS_DEFAULT;
		m_ActionTableFilename = FS_DEFAULT;
		setObjectType(FSOT_CHARACTER);
//		setMass(10);
		m_ActionTable = 0;
		m_MoveSpeed = 25;
		m_TurnSpeed = 30;
		m_MoveRun = false;
		m_RunSpeed = 2;
		m_AnimationSpeed = 25;
		m_MoveForward = false;
		m_MoveBackward = false;
		m_MoveStrafeLeft = false;
		m_MoveStrafeRight = false;
		m_MoveTurnLeft = false;
		m_MoveTurnRight = false;
		m_AutoAI = true;
		m_Jump = false;
		m_Shadow = true;
	}

	// dual creation allows for better error handling
	bool FSObject_Character::create(FSLevel* level, int id, stringc name)
	{
		// call the base class
		FSObject::create(level, id, name);

		// update the info structure
		getInfo()->create(2, "FSOBJECT_CHARACTER", "A simple character with physx attached", 1.00f, "Seven");

		REG_PROPERTY(FSObject_Character, "ActorFilename", &FSObject_Character::fsSetActorFilename, &FSObject_Character::fsGetActorFilename, "FS_PCHAR", "actor filename variable");
		REG_PROPERTY(FSObject_Character, "Shadow", &FSObject_Character::fsSetShadow, &FSObject_Character::fsGetShadow, "FS_BOOL", "does node cast Shadow");
		REG_PROPERTY(FSObject_Character, "ActionTableFilename", &FSObject_Character::fsSetActionTableFilename, &FSObject_Character::fsGetActionTableFilename, "FS_BOOL", "actiontable filename variable");

		REG_PROPERTY(FSObject_Character, "AutoAI", &FSObject_Character::fsSetAutoAI, &FSObject_Character::fsGetAutoAI, "FS_BOOL", "AI variable");

		REG_PROPERTY(FSObject_Character, "Stop", &FSObject_Character::fsSetMoveStop, &FSObject_Character::fsGetMoveStop, "FS_BOOL", "stop movement variable");
		REG_PROPERTY(FSObject_Character, "Jump", &FSObject_Character::fsSetJump, &FSObject_Character::fsGetJump, "FS_BOOL", "movement variable");
		REG_PROPERTY(FSObject_Character, "MoveForward", &FSObject_Character::fsSetMoveForward, &FSObject_Character::fsGetMoveForward, "FS_BOOL", "movement variable");
		REG_PROPERTY(FSObject_Character, "MoveBackward", &FSObject_Character::fsSetMoveBackward, &FSObject_Character::fsGetMoveBackward, "FS_BOOL", "movement variable");
		REG_PROPERTY(FSObject_Character, "MoveStrafeLeft", &FSObject_Character::fsSetMoveStrafeLeft, &FSObject_Character::fsGetMoveStrafeLeft, "FS_BOOL", "movement variable");
		REG_PROPERTY(FSObject_Character, "MoveStrafeRight", &FSObject_Character::fsSetMoveStrafeRight, &FSObject_Character::fsGetMoveStrafeRight, "FS_BOOL", "movement variable");
		REG_PROPERTY(FSObject_Character, "MoveTurnLeft", &FSObject_Character::fsSetMoveTurnLeft, &FSObject_Character::fsGetMoveTurnLeft, "FS_BOOL", "movement variable");
		REG_PROPERTY(FSObject_Character, "MoveTurnRight", &FSObject_Character::fsSetMoveTurnRight, &FSObject_Character::fsGetMoveTurnRight, "FS_BOOL", "movement variable");
		REG_PROPERTY(FSObject_Character, "MoveRun", &FSObject_Character::fsSetMoveRun, &FSObject_Character::fsGetMoveRun, "FS_BOOL", "movement variable");

		REG_PROPERTY(FSObject_Character, "MoveSpeed", &FSObject_Character::fsSetMoveSpeed, &FSObject_Character::fsGetMoveSpeed, "FS_FLOAT", "movement variable");
		REG_PROPERTY(FSObject_Character, "TurnSpeed", &FSObject_Character::fsSetTurnSpeed, &FSObject_Character::fsGetTurnSpeed, "FS_FLOAT", "movement variable");
		REG_PROPERTY(FSObject_Character, "RunSpeed", &FSObject_Character::fsSetRunSpeed, &FSObject_Character::fsGetRunSpeed, "FS_FLOAT", "movement variable");
		REG_PROPERTY(FSObject_Character, "AnimationSpeed", &FSObject_Character::fsSetAnimationSpeed, &FSObject_Character::fsGetAnimationSpeed, "FS_FLOAT", "movement variable");

		// create the action table
		m_ActionTable = new FSActionTable();
		m_ActionTable->initialize();
		m_ActionTable->create();

		if (getActionTable()) getActionTable()->loadFromDisk(getDirectory("ActionTableDirectory") + getActionTableFilename());

		m_Brain = new FSAI();
		m_Brain->initialize();
		m_Brain->create(getId(), getLevel());

		// everything went fine
		return true;
	}

	bool FSObject_Character::cleanup()
	{
		// safely destroy the action table
		if (m_ActionTable) { m_ActionTable->cleanup(); delete(m_ActionTable); m_ActionTable = 0; }

		// safely delete the brain
		if (m_Brain) { m_Brain->cleanup(); delete(m_Brain); m_Brain = 0;}

		// call the base class
		return FSObject::cleanup();
	}

	// frame functions 
	void FSObject_Character::preFrame(const float &elapsedtime)
	{
		if (getHealth() <= 0) setDead(true);
		else
		{
			bool action = false;
			if (getMoveForward()) { action = true; getPhysXObject()->addForce(getIn(getPrimarySceneNode()), getMovingSpeed() * getRunSpeed()); }
			if (getMoveBackward()) { action = true; if (getPhysXObject()) getPhysXObject()->addForce(-getIn(getPrimarySceneNode()), getMovingSpeed() * getRunSpeed()); }
			if (getMoveTurnLeft()) { action = true; rotate(false, elapsedtime); }
			if (getMoveTurnRight()) { action = true; rotate(true, elapsedtime); }
			if (getMoveStrafeLeft()) { action = true; if (getPhysXObject()) getPhysXObject()->addForce(-getLeft(getPrimarySceneNode()), getMovingSpeed()); }
			if (getMoveStrafeRight()) { action = true; if (getPhysXObject()) getPhysXObject()->addForce(getLeft(getPrimarySceneNode()), getMovingSpeed()); }
			if (!action) stop();
			if (getPhysXObject()) getPhysXObject()->frame(elapsedtime);
		
		
			//if (getJump()) { action = true;  }
			setJump(false);
			if ((!action) && (getMoveForward())) { if (!getJump()) { if (getMoveRun()) setAnimation("RUN"); else setAnimation("WALK"); } if (getPhysXObject()) getPhysXObject()->addForce(getIn(getPrimarySceneNode()), getMovingSpeed() * getRunSpeed()); }
			if ((!action) && (getMoveBackward())) {
				if (!getJump()) {
					if (getMoveRun()) setAnimation("RUN"); else setAnimation("WALK");
				} if (getPhysXObject()) getPhysXObject()->addForce(-getIn(getPrimarySceneNode()), getMovingSpeed() * getRunSpeed());
			}
			if ((!action) && (getMoveTurnLeft())) {
				if (!getJump()) {
					if (getMoveRun()) setAnimation("RUN"); else setAnimation("WALK");
				} rotate(false, elapsedtime);
			}
			if ((!action) && (getMoveTurnRight())) {
				if (!getJump()) {
					if (getMoveRun()) setAnimation("RUN"); else setAnimation("WALK");
				} rotate(true, elapsedtime);
			}
			if ((!action) && (getMoveStrafeLeft())) {
				if (!getJump()) {
					if (getMoveRun()) setAnimation("RUN"); else setAnimation("WALK");
				} if (getPhysXObject()) getPhysXObject()->addForce(-getLeft(getPrimarySceneNode()), getMovingSpeed());
			}
			if ((!action) && (getMoveStrafeRight())) {
				if (!getJump()) {
					if (getMoveRun()) setAnimation("RUN"); else setAnimation("WALK");
				} if (getPhysXObject()) getPhysXObject()->addForce(getLeft(getPrimarySceneNode()), getMovingSpeed());
			}

		}

		if ((getPhysXObject()) && (getPrimarySceneNode()))
		{
			setPositionValue(getPhysXObject()->getPosition());
			vector3df up = getUp(getPrimarySceneNode()) * getPositionOffset().Y;
			vector3df left = getLeft(getPrimarySceneNode()) * getPositionOffset().X;
			vector3df in = getIn(getPrimarySceneNode()) * getPositionOffset().Z;
			vector3df posoffset = (up + in + left);
			setPrimarySceneNodePositionAndRotation(getPosition() + posoffset, getRotation() + getRotationOffset());
		}
		else
			setPrimarySceneNodePositionAndRotation(getPosition(), getRotation());
	}

	void FSObject_Character::createPhysXObject()
	{
		// call the base class
		FSObject::createPhysXObject();

		if (!m_Node) return;

		IPhysXObjectData data;
		data.userdata = getId();
		data.type = POT_CHARACTER;
		data.position = getPosition();
		data.rotation = getRotation();
		data.scale = getScale();
		data.bboffset = getBBOffset();
//		data.mass = getMass();
		data.node = m_Node;
		data.mesh = m_Node->getMesh();
		data.dynamic = true;
		setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
	}

	void FSObject_Character::createSceneNodes()
	{
		// call the base class
		FSObject::createSceneNodes();

		// create a simple cube node
		scene::IAnimatedMesh* mesh = getSmgr()->getMesh(getDirectory("MediaDirectory") + getActorFilename());
		if (mesh)
		{
			m_Node = getSmgr()->addAnimatedMeshSceneNode(mesh, 0, getId(), getPosition(), getRotation(), getScale());
			m_Node->setAnimationSpeed(getAnimationSpeed());
			m_Node->setMaterialFlag(video::EMF_LIGHTING, getLevel()->getUseLight());
			m_Node->setMaterialFlag(video::EMF_FOG_ENABLE, getLevel()->getUseFog());
		}
		if ((getLevel()->getUseShadows()) && getShadow()) m_Node->addShadowVolumeSceneNode();
	}


	void FSObject_Character::setActorFilename(stringc fn)
	{
		m_ActorFilename = fn;
		reCreate();
	}

	void FSObject_Character::setActionTableFilename(stringc fn)
	{
		m_ActionTableFilename = fn;

		if (getActionTable())
		{
			getActionTable()->cleanup();
			getActionTable()->loadFromDisk(getDirectory("ActionTableDirectory") + getActionTableFilename());
		}
	}


	void FSObject_Character::setAnimation(stringc name, bool loop)
	{
		//printf("setting animation to %s\n", name.c_str());
		if (name == getAnimationName()) return;
		m_AnimationName = name;
		if (!getActionTable()) { FS_LOG(FSL_WARNING, "FSActionTable::setAnimation() action table not valid"); return; }

		FSActionTableEntry* e = getActionTable()->getEntry(name);
		if (!e) { FS_LOG(FSL_WARNING, "FSActionTable::setAnimation() cannot locate actiontable entry"); return; }
		if (!m_Node) { FS_LOG(FSL_WARNING, "FSActionTable::setAnimation() node is not valid"); return; }

		m_Node->setFrameLoop(e->m_AnimationStart, e->m_AnimationEnd);
		m_Node->setLoopMode(loop);
	}

	void FSObject_Character::stop()
	{
		m_MoveForward = false;
		m_MoveBackward = false;
		m_MoveStrafeLeft = false;
		m_MoveStrafeRight = false;
		m_MoveTurnLeft = false;
		m_MoveTurnRight = false;
		m_MoveRun = false;
	}

	float FSObject_Character::getMovingSpeed()
	{
		float speed = getMoveSpeed();
		if (getMoveRun()) speed *= getRunSpeed();
		return speed;
	}

	void FSObject_Character::rotate(bool left, const float &elapsedtime)
	{
		vector3df rot = getRotation();
		rot.X = 0.5;
		rot.Z = 0.5;
		vector3df old = getRotation();
		if (left) rot.Y -= getTurnSpeed() * elapsedtime; else rot.Y += getTurnSpeed() * elapsedtime;
		setRotation(rot);
	}

	void FSObject_Character::setRotation(vector3df rot)
	{
		FSObject::setRotation(rot);
	}

	//! Writes attributes of the object.
	void FSObject_Character::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
	{
		FSObject::serializeAttributes(out, options);

		out->addString("ActorFilename", m_ActorFilename.c_str());
		out->addString("ActionTableFilename", m_ActionTableFilename.c_str());
		out->addBool("AutoAI", m_AutoAI);
		out->addFloat("MoveSpeed", m_MoveSpeed);
		out->addFloat("TurnSpeed", m_TurnSpeed);
		out->addFloat("RunSpeed", m_RunSpeed);
		out->addFloat("AnimationSpeed", m_AnimationSpeed);
		out->addBool("Shadow", m_Shadow);
	}

	//! Reads attributes of the object.
	void FSObject_Character::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{
		FSObject::deserializeAttributes(in, options);

		m_ActorFilename = in->getAttributeAsString("ActorFilename");
		m_ActionTableFilename = in->getAttributeAsString("ActionTableFilename");
		m_AutoAI = in->getAttributeAsBool("AutoAI");
		m_MoveSpeed = in->getAttributeAsFloat("MoveSpeed");
		m_TurnSpeed = in->getAttributeAsFloat("TurnSpeed");
		m_RunSpeed = in->getAttributeAsFloat("RunSpeed");
		m_AnimationSpeed = in->getAttributeAsFloat("AnimationSpeed");
		m_Shadow = in->getAttributeAsBool("Shadow");
	}


} // end namespace
