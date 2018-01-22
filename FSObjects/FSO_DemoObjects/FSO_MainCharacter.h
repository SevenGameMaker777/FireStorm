#pragma once

#include "FSUtils.h"
#include "FSLevel.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSObject_Character.h"
#include "FSActionTable.h"
#include "FSPhysX.h"

namespace FS
{
	typedef class FSLevel FSLevel;

	class FSO_MainCharacter : public FSObject_Character
	{
	private:
		FS_VARIABLE_SETGET(bool, CameraFollow);
		FS_VARIABLE_SETGET(int, FollowUp);
		FS_VARIABLE_SETGET(int, FollowBack);
		FS_VARIABLE_SETGET(int, FollowMaxUp);
		FS_VARIABLE_SETGET(int, FollowMaxDown);

		FS_VARIABLE_SETGET(bool, UseLightSource);
		FS_VARIABLE_SETGET(int, LightSource);
		FS_VARIABLE_SETGET(float, JumpTimer);

	public:
		ADD_PROPERTY(UseLightSource, stringcToInt, intToStringc);
		ADD_PROPERTY(CameraFollow, stringcToInt, intToStringc);

		virtual void setDead(bool value) {}

		// set all variables to a known value
		virtual void initialize()
		{
			// call the base class
			FSObject_Character::initialize();

			setIsDebugObject(true);
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// call the base class
			FSObject_Character::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_MAINCHARACTER", "A simple character with physx attached", 1.00f, "Seven");

			REG_PROPERTY(FSO_MainCharacter, "CameraFollow", &FSO_MainCharacter::fsSetCameraFollow, &FSO_MainCharacter::fsGetCameraFollow, "FS_BOOL", "");
			REG_PROPERTY(FSO_MainCharacter, "UseLightSource", &FSO_MainCharacter::fsSetUseLightSource, &FSO_MainCharacter::fsGetUseLightSource, "FS_BOOL", "");

			setObjectType(FSOT_CHARACTER);

			setPositionOffsetValue(vector3df(0, -38, 0));
			setMass(10);
			setMoveSpeed(25);
			setTurnSpeed(20);
			setHealth(99999);
			setActorFilename("_Assets\\Models\\RoyalKnight\\RoyalKnight.b3d");
			setActionTableFilename("royalknight.actiontable");

			m_CameraFollow = true;
			m_FollowUp = 200;
			m_FollowBack = -200;
			m_FollowMaxUp = 300;
			m_FollowMaxDown = 10;

			m_UseLightSource = 1;
			m_LightSource = 0;
			m_JumpTimer = 0;

			return true;
		}

		virtual bool reCreate()
		{
			// call the base class
			FSObject_Character::reCreate();

			return true;
		}

		// frame functions 
		virtual void preFrame(const float &elapsedtime, bool forceupdate)
		{
			if (getHealth() <= 0) setDead(true);
			else
			{
				bool action = false;

				if (getLevel()->IsKeyDown(KEY_LSHIFT))	setVariable("MoveRun", "1");		else setVariable("MoveRun", "0");
				if (getLevel()->IsKeyDown(KEY_KEY_A))	setVariable("MoveTurnRight", "1");	else setVariable("MoveTurnRight", "0");
				if (getLevel()->IsKeyDown(KEY_KEY_D))	setVariable("MoveTurnLeft", "1");	else setVariable("MoveTurnLeft", "0");
				if (getLevel()->IsKeyDown(KEY_KEY_W))	setVariable("MoveForward", "1");	else setVariable("MoveForward", "0");
				if (getLevel()->IsKeyDown(KEY_KEY_S))	setVariable("MoveBackward", "1");	else setVariable("MoveBackward", "0");

				if (getLevel()->IsKeyDown(KEY_SPACE))
				{
					if (getJump() == false)
					{
						FSPhysXObject_Character* cobj = (FSPhysXObject_Character*)getPhysXObject();
						if (!cobj->getJumping())
						{
							setJump(true);
							cobj->m_JumpFinished = false;
						}
					}
				}
				if (getJump())
				{
					//action = true;
					FSPhysXObject_Character* cobj = (FSPhysXObject_Character*)getPhysXObject();
					if ((cobj->getJumpFinished()))
					{
						setJump(false);
						m_JumpTimer = 0;
					}
					else
					{
						setAnimation("JUMP");
						m_JumpTimer += elapsedtime;
						if (m_JumpTimer > 0.1f)	cobj->jump();
					}

				}

				//if (getJump()) { action = true;  }
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

				action = getMoveForward() || getMoveBackward() || getMoveTurnLeft() || getMoveTurnRight() || getMoveStrafeLeft() || getMoveStrafeRight() || getJump();
				if (!action) { setAnimation("IDLE_1");  stop(); }

				if (getPhysXObject()) getPhysXObject()->frame(elapsedtime);

				if (getCameraFollow())
				{
					vector3df pos = getPosition();
					pos += getUp(getPrimarySceneNode()) * getFollowUp();
					pos += getIn(getPrimarySceneNode()) * getFollowBack();
					getLevel()->getCamera()->setPosition(pos);
					getLevel()->getCamera()->setTarget(getPosition());
				}
				if (!getLightSource())
				{
					FSObject* light = getLevel()->getObjectFactory()->createObjectByType("FSO_LIGHT", true, getId() + 1, "Light Source", true);
					light->setVariable("Radius", "400");
					light->setVariable("Diffuse", SColorToStringc(SColor(255, 255, 255, 255)));
					light->setVariable("Ambient", SColorToStringc(SColor(255, 255, 255, 255)));
					light->setIsDebugObject(true);
					m_LightSource = light->getId();
				}
				if (getLightSource())
				{
					FSObject* light = getLevel()->getObjectPointer(getLightSource(), true);
					if (light)
					{
						if (getUseLightSource())
						{
							light->getPrimarySceneNode()->setVisible(true);
							vector3df pos = getPosition();
							pos += getUp(getPrimarySceneNode()) * 100;
							pos += getIn(getPrimarySceneNode()) * 300;
							light->setPosition(pos);
						}
						else
						{
							light->getPrimarySceneNode()->setVisible(false);
						}
					}
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

			getChildren()->preFrame(elapsedtime,true);
		}

		virtual bool onMouseWheel(const SEvent & e)
		{
			if (e.MouseInput.Wheel < 0)
			{
				m_FollowUp += 10;
				if (m_FollowUp > m_FollowMaxUp) m_FollowUp = m_FollowMaxUp;
			}
			else
			{
				m_FollowUp -= 10;
				if (m_FollowUp < m_FollowMaxDown) m_FollowUp = m_FollowMaxDown;
			}

			return true;
		}

		// receive a message from the system
		virtual bool receiveMessage(FSObjectMessage* m)
		{
			if (m->getSource() != getId()) return false;

			FSObject* obj = getLevel()->getObjectPointer(m->getDest(), true);
			switch (m->getType())
			{
			case MESSAGE_TRIGGER_ENTER: printf("enter trigger from %s\n", obj->getName()); break;
			case MESSAGE_TRIGGER_CONTACT: printf("inside trigger from %s\n", obj->getName()); break;
			case MESSAGE_TRIGGER_EXIT: printf("exit trigger from %s\n", obj->getName()); break;
			case MESSAGE_COLLISION: printf("collision with %s\n", obj->getName()); break;
			default:printf("UNKNONW %d\n", m->getType()); break;
			}
			return true;
		}

	};

} // end namespace#pragma once

#pragma once
