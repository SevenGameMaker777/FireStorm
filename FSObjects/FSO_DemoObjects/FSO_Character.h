#pragma once

#include "FSUtils.h"
#include "FSLevel.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSObject_Character.h"
#include "FSActionTable.h"
#include "FSUtils.h"

#define STATE_IDLE 1
#define STATE_WANDER 2
#define STATE_ATTACK 3
#define STATE_DYING 4
#define STATE_DEAD 5

namespace FS
{
	typedef class FSLevel FSLevel;

	class FSO_Character : public FSObject_Character, public FSStateMachine
	{
	private:

		FS_VARIABLE_SETGET(int, TargetId);
		FS_VARIABLE_SETGET(int, NextTargetId);
		FS_VARIABLE_SETGET(float, Timer);

		FS_VARIABLE_SETGET(float, WanderTimer);
		FS_VARIABLE_SETGET(float, IdleTimer);
		FS_VARIABLE_SETGET(float, DyingTimer);
		FS_VARIABLE_SETGET(float, VisionShort);
		FS_VARIABLE_SETGET(float, VisionMedium);
		FS_VARIABLE_SETGET(float, VisionLong);

	public:


	public:

		// set all variables to a known value
		virtual void initialize()
		{
			FS_LOG(FSL_ERROR, "FSO_Character::initialize()");
			// call the base class
			FSObject_Character::initialize();

			setMass(10);
			setMoveSpeed(50);
			setTurnSpeed(50);
//			setObjectMaterial(FSOM_CHARACTER);
			m_WanderTimer = getRandomFloat(2, 3);
			m_IdleTimer = getRandomFloat(10, 50);
			m_Timer = getRandomFloat(0, m_IdleTimer);
			m_DyingTimer = getRandomFloat(10, 20);
			m_TargetId = 0;
			m_NextTargetId = 0;
			m_VisionShort = 200;
			m_VisionMedium = 300;
			m_VisionLong = 400;

			FSStateMachine::initialize();
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// call the base class
			FSObject_Character::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_CHARACTER", "A simple character with physx attached", 1.00f, "Seven");

			setPositionOffsetValue(vector3df(0, -22, 0));
			setScaleValue(vector3df(0.5, 0.5, 0.5));
			setMass(10);
			setMoveSpeed(10);
			setTurnSpeed(50);
			setRunSpeed(2);
			setAnimationSpeed(25);
			setHealth(10);
			setActorFilename("_Assets/Models/guplin/Realm Crafter/gobinarc/goblin180.b3d");
			setActionTableFilename("guplin.actiontable");
			setAutoAI(true);

			return true;
		}

		virtual void preFrame(const float & elapsedtime)
		{
			FSObjectMessage msg;

			if (getAutoAI())
			{
				if (getNextState() != getState())
				{
					if (getForceStateChange())
					{
						msg.create(MSG_RESERVED_Exit, MSG_NULL, 0, 0, 0, 0, stringc(""));
						processStateMachine(&msg);

						setState(getNextState());
						setNextState(getState());
						setForceStateChange(false);

						msg.create(MSG_RESERVED_Enter, MSG_NULL, 0, 0, 0, 0, stringc(""));
						processStateMachine(&msg);
					}
				}
				msg.create(MSG_RESERVED_Update, MSG_NULL, 0, 0, 0, 0, stringc(""));
				processStateMachine(&msg);
			}

			FSObject_Character::preFrame(elapsedtime);
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			FSObject_Character::serializeAttributes(out, options);
			out->addInt("TargetId", m_TargetId);
			out->addInt("NextTargetId", m_NextTargetId);
			out->addFloat("Timer", m_Timer);
			out->addFloat("WanderTimer", m_WanderTimer);
			out->addFloat("IdleTimer", m_IdleTimer);
			out->addFloat("DyingTimer", m_DyingTimer);
			out->addFloat("VisionShort", m_VisionShort);
			out->addFloat("VisionMedium", m_VisionMedium);
			out->addFloat("VisionLong", m_VisionLong);
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			FSObject_Character::deserializeAttributes(in, options);
			m_TargetId = in->getAttributeAsInt("TargetId");
			m_NextTargetId = in->getAttributeAsInt("NextTargetId");
			m_Timer = in->getAttributeAsFloat("Timer");
			m_WanderTimer = in->getAttributeAsFloat("WanderTimer");
			m_IdleTimer = in->getAttributeAsFloat("IdleTimer");
			m_DyingTimer = in->getAttributeAsFloat("DyingTimer");
			m_VisionShort = in->getAttributeAsFloat("VisionShort");
			m_VisionMedium = in->getAttributeAsFloat("VisionMedium");
			m_VisionLong = in->getAttributeAsFloat("VisionLong");
		}

/*
		virtual bool processStateMachine(FSObjectMessage* msg)
		{
			FSBeginStateMachine
				FSOnEnter
				FSSetState(STATE_IDLE);

			// stand still and look around
			FSState(STATE_IDLE)
				FSOnEnter
				createTextAnim(getGui(), getSmgr(), getPrimarySceneNode(), L"State Idle", SColor(255, 0, 0, 255), 5000);
			getPrimarySceneNode()->getMaterial(0).EmissiveColor = SColor(255, 0, 0, 255);
			stop();
			setAnimation("IDLE_1");
			setTimer(0);
			FSOnUpdate
				setTimer(getTimer() + getLevel()->getElapsedTime());
			if (getTimer() > getIdleTimer())
				FSSetState(STATE_WANDER);
			if (getLevel()->getDistanceBetween(this, getLevel()->getObjectPointer(getTargetId(), true)) < getVisionLong())
				FSSetState(STATE_ATTACK);
			FSOnExit

				// wander ailessly around
				FSState(STATE_WANDER)
				FSOnEnter
				createTextAnim(getGui(), getSmgr(), getPrimarySceneNode(), L"State Wander", SColor(255, 0, 255, 0), 5000);
			getPrimarySceneNode()->getMaterial(0).EmissiveColor = SColor(255, 0, 255, 0);
			setTimer(0);
			setRotation(vector3df(0, getRandomFloat(0, 360), 0));
			setVariable("MoveForward", "1");
			setAnimation("WALK");
			FSOnUpdate
				setTimer(getTimer() + getLevel()->getElapsedTime());
			if (getTimer() > getWanderTimer()) FSSetState(STATE_IDLE);
			if (targetChanged()) changeTarget();
			if (getLevel()->getDistanceBetween(this, getLevel()->getObjectPointer(getTargetId(), true)) < getVisionShort()) FSSetState(STATE_ATTACK);
			FSOnExit

				// attack something
				FSState(STATE_ATTACK)
				FSOnEnter
				getPrimarySceneNode()->getMaterial(0).EmissiveColor = SColor(255, 255, 0, 0);
			createTextAnim(getGui(), getSmgr(), getPrimarySceneNode(), L"State Attack", SColor(255, 255, 0, 0), 5000);
			setTimer(0);
			FSOnUpdate
				FSObject* obj = getLevel()->getObjectPointer(getTargetId(), true);
			if (obj)
			{
//				lookAt(obj->getPosition());
				if (getLevel()->getDistanceBetween(this, obj) < getVisionShort())
				{
					stop();
					setAnimation("ATTACK1");
				}
				else
					if (getLevel()->getDistanceBetween(this, obj) < getVisionMedium())
					{
						setVariable("MoveForward", "1");
						setVariable("MoveRun", "0");
						setAnimation("WALK");
					}
					else
						if (getLevel()->getDistanceBetween(this, obj) < getVisionLong())
						{
							setVariable("MoveForward", "1");
							setVariable("MoveRun", "1");
							setAnimation("WALK");
						}
						else FSSetState(STATE_IDLE);
			}
			else FSSetState(STATE_IDLE);
			FSOnExit

				// uh-oh, something killed us. start the dying sequence
				FSState(STATE_DYING)
				FSOnEnter
				createTextAnim(getGui(), getSmgr(), getPrimarySceneNode(), L"State Dying", SColor(255, 255, 255, 255), 5000);
			stop();
			setAnimation("DEATH", false);
			setPositionOffset(vector3df(0, -50, -100));
			setTimer(0);
			FSOnUpdate
				setTimer(getTimer() + getLevel()->getElapsedTime());
			if (getTimer() > getDyingTimer()) FSSetState(STATE_DEAD);
			FSOnExit

				// now we are actually dead. auto delete ourselves
				FSState(STATE_DEAD)
				FSOnEnter
				setDead(true);
			FSOnUpdate
				FSOnExit

				FSEndStateMachine
		}

		virtual bool targetChanged()
		{
			return (m_NextTargetId != m_TargetId);
		}

		virtual void changeTarget()
		{
			m_TargetId = m_NextTargetId;
		}

		// override the base class to provide a dying animation before actual death
		virtual void setDead(bool value)
		{
			if (getState() == STATE_DEAD)
			{
				FSObject::setDead(value);
			}
			else
			{
				FSSetState(STATE_DYING);
			}
		}
*/
	};

} // end namespace#pragma once

