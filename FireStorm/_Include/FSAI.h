// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FS_AIGoalManager.h"
#include "FSMessageManager.h"
#include "FS_BlackBoard.h"

// use our own namespace so we dont clutter the global namespace
namespace FS
{
	class FSAI
	{
	public:
		FSAI() { /* do nothing constructor	*/ }
		virtual ~FSAI() { /* do nothing destructor	*/ }

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(int thisId, FSLevel* level);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

		// do whatever this class does each frame
		virtual void preFrame(const float &elapsedtime);
		virtual void frame(const float &elapsedtime);
		virtual void postFrame(const float &elapsedtime);

		int m_ThisId;
		FSLevel* m_Level;
		FSLevel* getLevel() { return m_Level; }
		FSAI_GoalManager* m_GoalManager; 
		FSAI_GoalManager* getGoalManager() { return m_GoalManager; }

		// this is only valid for one call!!!!
		FSObject* m_This;
		FSObject* me() { return m_This; };
	};



} // end namespace