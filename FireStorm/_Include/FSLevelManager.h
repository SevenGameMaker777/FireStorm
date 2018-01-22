// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"

// unclutter the global namespace
namespace FS
{
	// forward declarations
	typedef class FSApplication FSApplicaiton;
	typedef class FSLevel FSLevel;

	/*
	*/
	class FSLevelManager
	{
	private:
		// class variables
		int m_Id;					// simple id variable
		FSApplication* m_App;		// pointer to the owner class
		list<FSLevel*> m_Levels;	// list of available levels
		FSLevel* m_ActiveLevel;		// pointer to the currently active level
		int m_ActiveLevelId;		// id of the active level
		int m_NextLevelId;			// id of the next level

	public:
		// getters
		int getId();		// simple id variable

	public:
		// setters
		void setId(int id);	// simple id variable

	public:
		FSLevelManager() {}	// do nothing constructor
		virtual ~FSLevelManager() {}	// do nothing destructor

										// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(FSApplication* app);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

	public:
		// class frame functions
		virtual void preFrame(const float & elapsedtime, bool forceupdate);	// called before begin frame
		virtual void frame(const float & elapsedtime, bool forceupdate);		// called between begin frame and end frame
		virtual void postFrame(const float & elapsedtime, bool forceupdate);	// called after endframe

																				// class functions

																				// add a level to the list
		virtual bool addLevel(FSLevel* level, bool make_active);

		// destroy all of the levels
		virtual void destroyAllLevels();

		// destroy a single level
		virtual void destroyLevel(int id);

		// get a pointer to a level
		FSLevel* getLevel(int id);

		// get a pointer to the active level
		FSLevel* getActiveLevel();

		// set the next level
		virtual void setNextLevel(int id);

		// force the level change
		virtual void forceLevelChange();
	};

} // end namepsace

