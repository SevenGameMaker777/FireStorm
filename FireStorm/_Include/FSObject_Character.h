#pragma once

#include "FSUtils.h"
#include "FSLevel.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSActionTable.h"
#include "FSAI.h"

namespace FS
{
	typedef class FSLevel FSLevel;

	class FSObject_Character : public FSObject
	{
	private:
		FSAI* m_Brain;

		stringc m_ActorFilename;
		stringc m_ActionTableFilename;
		FSActionTable* m_ActionTable;
		stringc m_AnimationName;
		bool m_AutoAI;

		bool m_MoveForward;
		bool m_MoveBackward;
		bool m_MoveStrafeLeft;
		bool m_MoveStrafeRight;
		bool m_MoveTurnLeft;
		bool m_MoveTurnRight;
		bool m_MoveRun;

		float m_MoveSpeed;
		float m_RunSpeed;
		float m_AnimationSpeed;
		float m_TurnSpeed;
		bool m_MoveStop;
		bool m_Jump;

		FS_VARIABLE_GET(bool, Shadow);

		// add some variables that can be edited during runtime
		ADD_PROPERTY(ActorFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(ActionTableFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(Shadow, stringcToBool, boolToStringc);

		ADD_PROPERTY(AutoAI, stringcToBool, boolToStringc);

		ADD_PROPERTY(MoveStop, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveForward, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveBackward, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveStrafeLeft, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveStrafeRight, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveTurnLeft, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveTurnRight, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveRun, stringcToBool, boolToStringc);
		ADD_PROPERTY(MoveSpeed, stringcToFloat, floatToStringc);
		ADD_PROPERTY(TurnSpeed, stringcToFloat, floatToStringc);
		ADD_PROPERTY(RunSpeed, stringcToFloat, floatToStringc);
		ADD_PROPERTY(AnimationSpeed, stringcToFloat, floatToStringc);
		ADD_PROPERTY(Jump, stringcToBool, boolToStringc);

	public:
		void setShadow(bool fn)	{ m_Shadow = fn; reCreate(); }

		FSAI*	getBrain() { return m_Brain; }
		virtual bool getAutoAI() { return m_AutoAI; }
		virtual void setAutoAI(bool v) { m_AutoAI = v; }

		stringc getActorFilename() { return m_ActorFilename; }
		virtual void setActorFilename(stringc fn);

		stringc getActionTableFilename() { return m_ActionTableFilename; }
		virtual void setActionTableFilename(stringc fn);

		stringc getAnimationName() { return m_AnimationName; }
		void	setAnimationName(stringc n) { m_AnimationName = n; }

		float	getMovingSpeed();

		void	setMoveStop(bool v) { m_MoveStop = v; if (v) stop(); }
		void	setMoveForward(bool v) { m_MoveForward = v; }
		void	setMoveBackward(bool v) { m_MoveBackward = v; }
		void	setMoveStrafeLeft(bool v) { m_MoveStrafeLeft = v; }
		void	setMoveStrafeRight(bool v) { m_MoveStrafeRight = v; }
		void	setMoveTurnLeft(bool v) { m_MoveTurnLeft = v; }
		void	setMoveTurnRight(bool v) { m_MoveTurnRight = v; }
		void	setMoveRun(bool v) { m_MoveRun = v; }
		void	setMoveSpeed(float v) { m_MoveSpeed = v; }
		void	setRunSpeed(float v) { m_RunSpeed = v; }
		void	setTurnSpeed(float v) { m_TurnSpeed = v; }
		void	setAnimationSpeed(float v) { m_AnimationSpeed = v; }
		void	setJump(bool v) { m_Jump = v; }

		bool	getMoveStop() { return m_MoveStop; }
		bool	getMoveForward() { return m_MoveForward; }
		bool	getMoveBackward() { return m_MoveBackward; }
		bool	getMoveStrafeLeft() { return m_MoveStrafeLeft; }
		bool	getMoveStrafeRight() { return m_MoveStrafeRight; }
		bool	getMoveTurnLeft() { return m_MoveTurnLeft; }
		bool	getMoveTurnRight() { return m_MoveTurnRight; }
		bool	getMoveRun() { return m_MoveRun; }
		float	getMoveSpeed() { return m_MoveSpeed; }
		float	getRunSpeed() { return m_RunSpeed; }
		float	getTurnSpeed() { return m_TurnSpeed; }
		float	getAnimationSpeed() { return m_AnimationSpeed; }
		bool	getJump() { return m_Jump; }

		FSActionTable* getActionTable() { return m_ActionTable; }

		virtual void setAnimation(stringc name, bool loop = true);
		virtual void stop();

		virtual void rotate(bool left, const float &elapsedtime);
		virtual void setRotation(vector3df rot);

	public:
		// a simple animated mesh scene node
		IAnimatedMeshSceneNode* m_Node;

		// mandatory overrides
		virtual ISceneNode* getPrimarySceneNode();

		// destroy all of the scenenodes
		virtual void destroyAllSceneNodes();

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name);

		//  cleanup whatever memory mess we made
		virtual bool cleanup();

		// create the scenenodes
		virtual void createSceneNodes();

		// frame functions 
		virtual void preFrame(const float &elapsedtime);

		// crate the physx representation
		virtual void createPhysXObject();

		/* serialization / deserialization*/
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0);
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0);
	};

} // end namespace#pragma once

