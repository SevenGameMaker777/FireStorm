// include this file only once

#pragma once



// include the needed header files

#include "FSUtils.h"



// use our own namespace so we dont clutter the global namespace

namespace FS

{

	typedef class FSLevel FSLevel;

	typedef class FSObject FSObject;

	typedef class FSAI FSAI;

	typedef class FSAI_GoalManager FSAI_GoalManager;



	class FSAI_Goal

	{

	public:

		FSAI_Goal(FSAI_GoalManager* gm, stringc name, bool recycle) :

			m_GoalManager(gm), m_Name(name), m_Recycle(recycle), m_Finished(false), m_Failed(false) {}

		virtual void enter() {}

		virtual void update(const float &elapsedtime) {}

		virtual void exit() {}

		FSAI_GoalManager* m_GoalManager;
		FSAI_GoalManager* getGoalManager() { return m_GoalManager; }

		stringc m_Name;
		stringc getName() { return m_Name; }

		bool m_Finished;
		bool getFinished() { return m_Finished; }
		void setFinished(bool v) { m_Finished = v; }

		bool m_Failed;
		void setFailed(bool v) { m_Failed = v; }
		bool getFailed() { return m_Failed; }

		bool m_Recycle;
		void setRecycle(bool v) { m_Recycle = v; }
		bool getRecycle() { return m_Recycle; }

		virtual stringc getDescription() { return stringc("Base goal"); }

		FSLevel* getLevel();



		// warning this is only valid for one call!!!!!!

		FSObject* me();

	};



	class FSAI_Goal_Idle : public FSAI_Goal

	{

	public:

		FSAI_Goal_Idle(FSAI_GoalManager* gm, stringc name, bool recycle, float howlong)

			: FSAI_Goal(gm, name, recycle), m_HowLong(howlong) {}

		virtual void update(const float &elapsedtime);

		virtual void enter();

		virtual void exit();

		virtual stringc getDescription() { return stringc("Idle - "); }

		float m_Timer;
		float getTimer() { return m_Timer; }
		void setTimer(float v) { m_Timer = v; }

		float m_HowLong;
		float getHowLong() { return m_HowLong; }
		void setHowLong(float v) { m_HowLong = v; }

	};



	class FSAI_Goal_Wander : public FSAI_Goal_Idle

	{

	public:

		FSAI_Goal_Wander(FSAI_GoalManager* gm, stringc name, bool recycle, float howlong)

			: FSAI_Goal_Idle(gm, name, recycle, howlong) {}

		virtual void update(const float &elapsedtime);

		virtual void enter();

		virtual void exit();

		virtual stringc getDescription() { return stringc("Wander"); }

	};



	class FSAI_Goal_GoToObjectId : public FSAI_Goal

	{

	public:

		FSAI_Goal_GoToObjectId(FSAI_GoalManager* gm, stringc name, bool recycle, int targetobject, float distance)

			: FSAI_Goal(gm, name, recycle), m_TargetObject(targetobject), m_Distance(distance) {}

		virtual void update(const float &elapsedtime);

		virtual void enter();

		virtual void exit();

		float m_Distance;
		float getDistance() { return m_Distance; }
		void setDistance(float v) { m_Distance = v; }

		int m_TargetObject;
		int getTargetObject() { return m_TargetObject; }
		void setTargetObject(int v) { m_TargetObject = v; }

		virtual stringc getDescription() { return stringc(" gotoobjectid ") + stringc(m_TargetObject); }

	};



	class FSAI_Goal_GoToObjectName : public FSAI_Goal

	{

	public:

		FSAI_Goal_GoToObjectName(FSAI_GoalManager* gm, stringc name, bool recycle, stringc targetobject, float distance)

			: FSAI_Goal(gm, name, recycle), m_TargetObject(targetobject), m_Distance(distance) {}

		virtual void update(const float &elapsedtime);

		virtual void enter();

		virtual void exit();

		virtual stringc getDescription() { return stringc(" gotoobjectname") + stringc(m_TargetObject); }

		float m_Distance;
		float getDistance() { return m_Distance; }
		void setDistance(float v) { m_Distance = v; }

		stringc m_TargetObject;
		stringc getTargetObject() { return m_TargetObject; }
		void setTargetObject(stringc v) { m_TargetObject = v; }

	};



	class FSAI_Goal_GoToLocation : public FSAI_Goal

	{

	public:

		FSAI_Goal_GoToLocation(FSAI_GoalManager* gm, stringc name, bool recycle, vector3df targetlocation, float distance)

			: FSAI_Goal(gm, name, recycle), m_TargetLocation(targetlocation), m_Distance(distance) {}

		virtual void update(const float &elapsedtime);

		virtual void exit();

		virtual stringc getDescription() { return stringc(" gotoLocation") + vector3dfToStringc(m_TargetLocation); }

		float m_Distance;
		float getDistance() { return m_Distance; }
		void setDistance(float v) { m_Distance = v; }

		vector3df m_TargetLocation;
		vector3df getTargetLocation() { return m_TargetLocation; }
		void setTargetLocation(vector3df v) { m_TargetLocation = v; }

	};



	class FSAI_Goal_TurnAndFaceId : public FSAI_Goal
	{
	public:
		FSAI_Goal_TurnAndFaceId(FSAI_GoalManager* gm, stringc name, bool recycle, int targetobject)
			: FSAI_Goal(gm, name, recycle), m_TargetObject(targetobject) {}
		virtual void update(const float &elapsedtime);
		virtual void enter();
		virtual void exit();
		virtual stringc getDescription() { return stringc(" turnandface ") + stringc(m_TargetObject); }
		int m_TargetObject;
		int getTargetObject() { return m_TargetObject; }
		void setTargetObject(int v) { m_TargetObject = v; }
	};

	class FSAI_Goal_TurnAndFaceName : public FSAI_Goal
	{
	public:
		FSAI_Goal_TurnAndFaceName(FSAI_GoalManager* gm, stringc name, bool recycle, stringc targetobject)
			: FSAI_Goal(gm, name, recycle), m_TargetObject(targetobject) {}
		virtual void update(const float &elapsedtime);
		virtual void enter();
		virtual void exit();
		virtual stringc getDescription() { return stringc(" turnandface ") + stringc(m_TargetObject); }
		stringc m_TargetObject;
		stringc getTargetObject() { return m_TargetObject; }
		void setTargetObject(stringc v) { m_TargetObject = v; }
	};



	class FSAI_Goal_FollowObjectId : public FSAI_Goal

	{

	public:

		FSAI_Goal_FollowObjectId(FSAI_GoalManager* gm, stringc name, bool recycle, int targetobject, float distance)

			: FSAI_Goal(gm, name, recycle), m_TargetObject(targetobject), m_Distance(distance) {}

		virtual void update(const float &elapsedtime);

		virtual void enter();

		virtual void exit();

		virtual stringc getDescription() { return stringc(" followobjectid ") + stringc(m_TargetObject); }

		float m_Distance;
		float getDistance() { return m_Distance; }
		void setDistance(float v) { m_Distance = v; }

		int m_TargetObject;
		int getTargetObject() { return m_TargetObject; }
		void setTargetObject(int v) { m_TargetObject = v; }

	};



	class FSAI_GoalManager
	{
	public:

		FSAI_GoalManager() { /* do nothing constructor	*/ }

		virtual ~FSAI_GoalManager() { /* do nothing destructor	*/ }

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(FSAI* ai);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

		virtual void update(const float &elapsedtime);

		FSAI_Goal* m_CurrentGoal;
		FSAI_Goal* getCurrentGoal() { return m_CurrentGoal; }
		void setCurrentGoal(FSAI_Goal* g);

		list<FSAI_Goal*> m_Goals;

		void addGoalFront(FSAI_Goal* goal);

		void addGoalBack(FSAI_Goal* goal);

		void destroyGoal(FSAI_Goal* goal);

		void FSAI_GoalManager::destroyAllGoals();

		void recycleGoal(FSAI_Goal* goal);

		FSAI* m_AI;
		FSAI* getAI() { return m_AI; }

		// warning this is only valid for one call!!!!!!

		FSObject* me();

		FSLevel* getLevel();
	};



} // end namespace