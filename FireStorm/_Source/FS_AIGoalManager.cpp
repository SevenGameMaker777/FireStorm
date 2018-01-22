// include the needed header files
#include "FS_AIGoalManager.h"
#include "FSUtils.h"
#include "FSLevel.h"
#include "FSObject.h"
#include "FSObjectFactory.h"
#include "FSObjectManager.h"
#include "FSAI.h"

// use our own namespace so we dont clutter the global namespace
namespace FS
{
	////////////////////////////////////////////////////////////////////////////////////
	FSObject* FSAI_Goal::me() { return getGoalManager()->me(); }
	FSLevel* FSAI_Goal::getLevel() { return getGoalManager()->getLevel(); }
	////////////////////////////////////////////////////////////////////////////////////

	void FSAI_Goal_Idle::enter()
	{
		m_Timer = 0;
		me()->setVariable("Stop", "1");
	}

	void FSAI_Goal_Idle::update(const float &elapsedtime)
	{
		m_Timer += elapsedtime;
		if (m_Timer > m_HowLong) setFinished(true);
	}

	void FSAI_Goal_Idle::exit()
	{
		me()->setVariable("Stop", "1");
	}

	////////////////////////////////////////////////////////////////////////////////////
	void FSAI_Goal_Wander::enter()
	{
		setTimer(0);
		me()->setRotation(vector3df(0, getRandomFloat(0, 360), 0));
		me()->setVariable("MoveForward", "1");
	}

	void FSAI_Goal_Wander::update(const float &elapsedtime)
	{
		setTimer(getTimer() + elapsedtime);
		if (getTimer() > getHowLong()) setFinished(true);
	}

	void FSAI_Goal_Wander::exit()
	{
		me()->setVariable("Stop", "1");
	}

	////////////////////////////////////////////////////////////////////////////////////
	void FSAI_Goal_GoToObjectId::enter()
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->getObjectPointer(getTargetObject(), true);
		if (!target) { setFailed(true); return; }
		if (!me()->isFacing(target->getPosition()))
			getGoalManager()->addGoalFront(new FSAI_Goal_TurnAndFaceId(getGoalManager(), stringc("TurnAndFace"), false, getTargetObject()));
	}

	void FSAI_Goal_GoToObjectId::update(const float &elapsedtime)
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->getObjectPointer(getTargetObject(), true);
		if (!target) { setFailed(true); return; }
		if (target)
		{
			if (!me()->isFacing(target->getPosition(), 5.0f))
			{
				getGoalManager()->addGoalFront(new FSAI_Goal_TurnAndFaceId(getGoalManager(), stringc("TurnAndFace"), false, getTargetObject()));
				return;
			}

			if (vector3df(me()->getPosition() - target->getPosition()).getLength() <= getDistance())
			{
				setFinished(true);
			}
			else
			{
				me()->setVariable("MoveForward", "1");
			}
		}
		else printf("  goal NO TARGET %s\n", getName().c_str());
	}

	void FSAI_Goal_GoToObjectId::exit()
	{
		me()->setVariable("Stop", "1");
	}

	////////////////////////////////////////////////////////////////////////////////////
	void FSAI_Goal_GoToObjectName::enter()
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->findObjectByName(getTargetObject(), true);
		if (!target) { setFailed(true); return; }
		if (!me()->isFacing(target->getPosition()))
			getGoalManager()->addGoalFront(new FSAI_Goal_TurnAndFaceName(getGoalManager(), stringc("TurnAndFace"), false, getTargetObject()));
	}

	void FSAI_Goal_GoToObjectName::update(const float &elapsedtime)
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->findObjectByName(getTargetObject(), true);
		if (!target) { setFailed(true); return; }
		if (target)
		{
			if (!me()->isFacing(target->getPosition(), 5.0f))
			{
				getGoalManager()->addGoalFront(new FSAI_Goal_TurnAndFaceName(getGoalManager(), stringc("TurnAndFace"), false, getTargetObject()));
				return;
			}

			if (vector3df(me()->getPosition() - target->getPosition()).getLength() <= getDistance())
			{
				setFinished(true);
			}
			else
			{
				me()->setVariable("MoveForward", "1");
			}
		}
		else printf("  goal NO TARGET %s\n", getName().c_str());
	}

	void FSAI_Goal_GoToObjectName::exit()
	{
		me()->setVariable("Stop", "1");
	}

	////////////////////////////////////////////////////////////////////////////////////
	void FSAI_Goal_TurnAndFaceId::enter()
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->getObjectPointer(m_TargetObject, true);
		if (!target) { setFailed(true); return; }
		if (me()->isFacing(target->getPosition(), 5.0f)) setFinished(true);
	}

	void FSAI_Goal_TurnAndFaceId::update(const float &elapsedtime)
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->getObjectPointer(m_TargetObject, true);
		if (!target) { setFailed(true); return; }
		if (me()->isFacing(target->getPosition(), 5.0f))
		{
			setFinished(true);
		}
		else
		{
			vector3df nodePos = me()->getPosition();
			vector3df targetPos = target->getPosition();
			vector3df diff = targetPos - nodePos;
			vector3df lookat = diff.getHorizontalAngle();
			lookat = me()->getRotation() - lookat;
			if (lookat.Y < 0) lookat.Y = lookat.Y + 360;
			if (lookat.Y > 360) lookat.Y = 360 - lookat.Y;
			if (lookat.Y > 180)
				me()->setVariable("MoveTurnLeft", "1");
			else
				me()->setVariable("MoveTurnRight", "1");
		}
	}

	void FSAI_Goal_TurnAndFaceId::exit()
	{
		me()->setVariable("Stop", "1");
	}

	////////////////////////////////////////////////////////////////////////////////////
	void FSAI_Goal_TurnAndFaceName::enter()
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->findObjectByName(m_TargetObject, true);
		if (!target) { setFailed(true); return; }
		if (me()->isFacing(target->getPosition(), 5.0f)) setFinished(true);
	}

	void FSAI_Goal_TurnAndFaceName::update(const float &elapsedtime)
	{
		FSObject* target = getLevel()->getObjectFactory()->getObjectManager()->findObjectByName(m_TargetObject, true);
		if (!target) { setFailed(true); return; }
		if (me()->isFacing(target->getPosition(), 5.0f))
		{
			setFinished(true);
		}
		else
		{
			vector3df nodePos = me()->getPosition();
			vector3df targetPos = target->getPosition();
			vector3df diff = targetPos - nodePos;
			vector3df lookat = diff.getHorizontalAngle();
			lookat = me()->getRotation() - lookat;
			if (lookat.Y < 0) lookat.Y = lookat.Y + 360;
			if (lookat.Y > 360) lookat.Y = 360 - lookat.Y;
			if (lookat.Y > 180)
				me()->setVariable("MoveTurnLeft", "1");
			else
				me()->setVariable("MoveTurnRight", "1");
		}
	}

	void FSAI_Goal_TurnAndFaceName::exit()
	{
		me()->setVariable("Stop", "1");
	}

	////////////////////////////////////////////////////////////////////////////////////

	void FSAI_Goal_FollowObjectId::enter()
	{
		FSObject* target = getLevel()->getObjectPointer(getTargetObject(), true);
		if (!target) { setFailed(true); return; }
		if (!me()->isFacing(target->getPosition()))
					getGoalManager()->addGoalFront(new FSAI_Goal_TurnAndFaceId(getGoalManager(), stringc("TurnAndFace"), false, target->getId()));
	}



	void FSAI_Goal_FollowObjectId::update(const float &elapsedtime)
	{
		FSObject* target = getLevel()->getObjectPointer(getTargetObject(), true);
		if (target)
		{
			me()->lookAt(target->getPosition());
			if (vector3df(me()->getPosition() - target->getPosition()).getLength() <= getDistance())
			{
				me()->setVariable("Stop", "1");
				getGoalManager()->addGoalFront(new FSAI_Goal_Idle(getGoalManager(), "Idle", false, 5));
			}
			else
				me()->setVariable("MoveForward", "1");
		}
	}

	void FSAI_Goal_FollowObjectId::exit()
	{
		me()->setVariable("Stop", "1");
	}



	////////////////////////////////////////////////////////////////////////////////////
	FSObject* FSAI_GoalManager::me() { return getAI()->me(); }
	FSLevel* FSAI_GoalManager::getLevel() { return getAI()->getLevel(); }

	void FSAI_GoalManager::initialize()
	{
		// log this event
		FS_LOG(FSLOGTYPE::FSL_DEBUG, "FSAI_GoalManager::initialize()");

		// set all variables to a known value
		m_Goals.clear();
		m_CurrentGoal = 0;
		m_AI = 0;
	}

	bool FSAI_GoalManager::create(FSAI* ai)
	{
		// log this event
		FS_LOG(FSLOGTYPE::FSL_DEBUG, "FSAI_Manager::create()");

		// remember this
		m_AI = ai;

		// everything went fine
		return true;
	}

	bool FSAI_GoalManager::cleanup()
	{
		// log this event
		FS_LOG(FSLOGTYPE::FSL_DEBUG, "FSAI_Manager::cleanup()");

		// destory all of the remaining goals
		destroyAllGoals();

		// forget this
		m_AI = 0;

		// always return false from a cleanup function
		return false;
	}

	void FSAI_GoalManager::update(const float &elapsedtime)
	{
//		printf("goal manager update : \n");

		if (m_CurrentGoal)
		{
			if (m_CurrentGoal->getFinished())
			{
				if (m_CurrentGoal->getRecycle()) recycleGoal(m_CurrentGoal);
				else destroyGoal(m_CurrentGoal);
			}
		}

		if (!m_Goals.empty())
		{
			// get an iterator for the list
			list<FSAI_Goal*>::Iterator it = m_Goals.begin();
			if (m_CurrentGoal != (*it))
			{
				setCurrentGoal(*it);
			}
		}

		if (m_CurrentGoal) m_CurrentGoal->update(elapsedtime);
	}

	void FSAI_GoalManager::setCurrentGoal(FSAI_Goal* goal)
	{
//		if (m_CurrentGoal)
//			printf("set current goal from %s to %s\n", m_CurrentGoal->getName().c_str(), goal->getName().c_str());
//		else
//			printf("set current goal from NULL to %s\n", goal->getName().c_str());
		if (m_CurrentGoal) m_CurrentGoal->exit();
		m_CurrentGoal = goal;
//		if (m_CurrentGoal)
//			printf("setting goal!!!!!!!!!!!!! %s\n", m_CurrentGoal->getName().c_str());
//		else
//			printf("setting current goal to NULL\n");
		if (m_CurrentGoal) m_CurrentGoal->enter();
		createTextAnim(getLevel()->getGui(), getLevel()->getSmgr(), m_CurrentGoal->me()->getPrimarySceneNode(), stringw(m_CurrentGoal->getName()));
	}

	void FSAI_GoalManager::addGoalBack(FSAI_Goal* goal)
	{
		m_Goals.push_back(goal);
	}

	void FSAI_GoalManager::addGoalFront(FSAI_Goal* goal)
	{
//		printf("adding goal to front %s\n",goal->getName().c_str());
		m_Goals.push_front(goal);
		//setCurrentGoal(goal);
	}

	void FSAI_GoalManager::recycleGoal(FSAI_Goal* goal)
	{
		goal->setFinished(false);

		// this is odd. why are we recycling a goal that doesnt exist in the list?
		// we'll just add it to the list and then bail
		if (m_Goals.empty()) { addGoalBack(goal); return; }

		// get an iterator for the list
		list<FSAI_Goal*>::Iterator it = m_Goals.begin();

		// search the list 
		for (; it != m_Goals.end();)
		{
			// if this is our object
			if ((*it) == goal)
			{
				// if this is the active goal, forget it
				if (m_CurrentGoal == (*it))	m_CurrentGoal = 0;
				(*it)->exit();
				m_Goals.erase(it);
				continue;
			}
			else it++;
		}

		// add the goal back to the list
		addGoalBack(goal);
	}

	void FSAI_GoalManager::destroyAllGoals()
	{
		// make sure the list is not empty
		if (m_Goals.empty()) return;

		// get an iterator for the list
		list<FSAI_Goal*>::Iterator it = m_Goals.begin();

		// forget the active goal
		m_CurrentGoal = 0;

		// search the list 
		for (; it != m_Goals.end(); )
		{
			//(*it)->exit();
			delete (*it);
			it = m_Goals.erase(it);
		}
	}

	void FSAI_GoalManager::destroyGoal(FSAI_Goal* goal)
	{
		// make sure the list is not empty
		if (m_Goals.empty()) return;

		// get an iterator for the list
		list<FSAI_Goal*>::Iterator it = m_Goals.begin();

		// search the list 
		for (; it != m_Goals.end();)
		{
			// if this is our object
			if ((*it) == goal)
			{
				// if this is the active goal, forget it
				if (m_CurrentGoal == (*it))	m_CurrentGoal = 0;
				(*it)->exit();
				delete (*it);
				m_Goals.erase(it);
				return;
			}
			else it++;
		}
	}

} // end namespace