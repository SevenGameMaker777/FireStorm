
#pragma once



#include "FSUtils.h"

#include "FSMessageManager.h"



	namespace FS

{

	typedef class FSLevel FSLevel;



	struct FSBlackBoardEntry : public FSObjectMessage

	{

	public:

		FSBlackBoardEntry(stringc name, bool onetimeevent, bool destroyafterfire, FSMT t1, FSMT t2,

			int source, int dest, stringc data) : FSObjectMessage(t1, t2, source, dest, 0, 0, data), m_Name(name),

			m_OneTimeEvent(onetimeevent), m_DestroyAfterFired(destroyafterfire), m_Value(false), m_HasFired(false) {}

		bool m_Value;
		bool getValue() { return m_Value; }
		void setValue(bool v) { m_Value = v; }

		stringc m_Name;
		stringc getName() { return m_Name; }

		bool m_HasFired;
		bool getHasFired() { return m_HasFired; }
		void setHasFired(bool v) { m_HasFired = v;  }

		bool m_OneTimeEvent;
		bool getOneTimeEvent() { return  m_OneTimeEvent; }

		bool m_DestroyAfterFired;
		bool getDestroyAfterFired() { return m_DestroyAfterFired; }
	};



	class FSBlackBoard

	{

	public:

		FSBlackBoard() {}

		virtual ~FSBlackBoard() {}



		// set all variables to a known value

		virtual void initialize();



		// dual creation allows for better error handling

		virtual bool create(FSLevel* level);



		// cleanup whatever memory mess we made

		virtual bool cleanup();



		// do whatever this class does each frame

		virtual void preFrame(const float &elapsedtime);



		// add an item to the BlackBoard

		virtual void addEvent(FSBlackBoardEntry* event);



		// set a value in the map

		virtual void setValue(int who, stringc name, bool value);

		virtual bool getValue(stringc name);



		// the list of entries

		map<stringc, FSBlackBoardEntry*> m_List;

		FSLevel* m_Level;
		FSLevel* getLevel() { return m_Level; }

	};



} // end namespace
