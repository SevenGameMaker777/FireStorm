#pragma once

#include "FSUtils.h"
#include "FSLevel.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSO_Character.h"
#include "FSActionTable.h"
#include "FSPhysX.h"

namespace FS
{
	typedef class FSLevel FSLevel;

	class FSO_Character_Goblin : public FSO_Character
	{
	public:

		// set all variables to a known value
		virtual void initialize()
		{
			FS_LOG(FSL_ERROR, "FSO_Character_Goblin::initialize()");
			// call the base class
			FSO_Character::initialize();

		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// call the base class
			FSO_Character::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_CHARACTER_GOBLIN", "A simple character with physx attached", 1.00f, "Seven");

			setPositionOffsetValue(vector3df(0, -22, 0));
			setScaleValue(vector3df(0.5, 0.5, 0.5));
			setMass(10);
			setMoveSpeed(10);
			setTurnSpeed(30);
			setRunSpeed(2);
			setAnimationSpeed(25);
			setHealth(10);
			setActorFilename("_Assets/Models/guplin/Realm Crafter/gobinarc/goblin180.b3d");
			setActionTableFilename("guplin.actiontable");
			setAutoAI(true);

			getBrain()->getGoalManager()->addGoalBack(new FSAI_Goal_GoToObjectName(getBrain()->getGoalManager(), "GotoMarker1", true, "MARKER1", 200));
			getBrain()->getGoalManager()->addGoalBack(new FSAI_Goal_GoToObjectName(getBrain()->getGoalManager(), "GotoMarker2", true, "MARKER2", 200));
			getBrain()->getGoalManager()->addGoalBack(new FSAI_Goal_GoToObjectName(getBrain()->getGoalManager(), "GotoMarker3", true, "MARKER3", 200));
			getBrain()->getGoalManager()->addGoalBack(new FSAI_Goal_GoToObjectName(getBrain()->getGoalManager(), "GotoMarker4", true, "MARKER4", 200));

			return true;
		}

		virtual void preFrame(const float & elapsedtime, bool forceupdate)
		{
			getBrain()->preFrame(elapsedtime);
			if (getHealth() <= 0) setDead(true);
			else
			{
				bool action = false;

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

			getChildren()->preFrame(elapsedtime, forceupdate);
		}
		virtual void frame(const float & elapsedtime, bool forceupdate)
		{
			FSO_Character::frame(elapsedtime, forceupdate);

			getBrain()->frame(elapsedtime);
		}
		virtual void postFrame(const float & elapsedtime, bool forceupdate)
		{
			FSO_Character::postFrame(elapsedtime, forceupdate);

			getBrain()->postFrame(elapsedtime);
		}

	};

} // end namespace
