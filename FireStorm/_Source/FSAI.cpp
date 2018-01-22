// include the needed header files
#include "FSAI.h"
#include "FSLevel.h"
#include "FSUtils.h"
#include "FSLogger.h"



// use our own namespace so we dont clutter the global namespace

namespace FS
{
	void FSAI::initialize()
	{
		// log this event
		FS_LOG(FSLOGTYPE::FSL_DEBUG, "FSAI::initialize()");

		// set all variables to a known value
		m_ThisId = 0;
		m_This = 0;
		m_Level = 0;
		m_GoalManager = 0;
	}



	bool FSAI::create(int thisId, FSLevel* level)
	{
		// log this event
		FS_LOG(FSLOGTYPE::FSL_DEBUG, "FSAI::create()");

		// remember this
		m_ThisId = thisId;
		m_Level = level;

		m_GoalManager = new FSAI_GoalManager();
		m_GoalManager->initialize();
		m_GoalManager->create(this);

		// everything went fine
		return true;
	}



	bool FSAI::cleanup()
	{
		// log this event
		FS_LOG(FSLOGTYPE::FSL_DEBUG, "FSAI::cleanup()");

		// safely cleanup and delete the brain
		if (m_GoalManager) { m_GoalManager->cleanup(); delete(m_GoalManager); m_GoalManager = 0; }

		m_Level = 0;
		m_This = 0;

		// always return false from a cleanup function
		return false;
	}



	void FSAI::preFrame(const float &elapsedtime)
	{
		m_This = getLevel()->getObjectPointer(m_ThisId,true);

		if (!m_This)
		{
			FS_LOG(FSL_WARNING, "WARNING! FSAI::preFrame() object is not valid!!");
			return;
		}

		if (m_GoalManager) m_GoalManager->update(elapsedtime);
	}



	void FSAI::frame(const float &elapsedtime) {}

	void FSAI::postFrame(const float &elapsedtime) {}

} // end namespace