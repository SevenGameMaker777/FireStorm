#pragma once

// include the needed headers
#include "FSUtils.h"

namespace FS
{
	// class forward declaration
	typedef class FSObjectManager FSObjectManager;
	typedef class FSApplication FSApplication;
	typedef class FSLevel FSLevel;

	enum FSMT
	{
		STATE_Global = 0,				//	0	 STATE_Global 
		MSG_NULL,					//	1
		MSG_RESERVED_Enter,			//	2	 Don't send this message - the system sends this when a state is first entered - Use OnInitialize to listen for it
		MSG_RESERVED_Exit,			//	3	 Don't send this message - the system sends this when a state is exited - Use OnExit to listen for it
		MSG_RESERVED_Update,		//	4	 Don't send this message - the system sends this when a game tick goes by - Use OnUpdate to listen for it
		MSG_Timeout,				//	5
		MSG_ChangeState,			//	6
		MESSAGE_TRIGGER_ENTER,
		MESSAGE_TRIGGER_EXIT,
		MESSAGE_TRIGGER_CONTACT,
		MESSAGE_TOUCH,
		MESSAGE_COLLISION,
		LEVELTEXT,
		WINDOW_RESIZE,
		NOMESSAGE,
		FSFLOAT,
		FSINT,
		FSVECTOR3DF,
		DAMAGE_FIRE,
		DAMAGE_COLD,
		DAMAGE_PHYSICAL,
		DAMAGE_ACID
	};

	// a simple message class
	class FSObjectMessage
	{
	private:
		FSMT	m_Type;			// message type
		FSMT	m_Type2;		// message type
		int		m_Source;		// who initiated this message
		int		m_Dest;			// who is the message going to
		float	m_RTime;		// what time was the message sent	
		float	m_DTime;		// what time to deliver the message
		stringc m_Data;			// the message

	public:
		FSMT	getType()	{	return m_Type; 		}	// message type
		FSMT	getType2()	{	return m_Type2;		}	// message type
		int		getSource() {	return m_Source;	}	// who initiated this message
		int		getDest()	{	return m_Dest;		}	// who is the message going to
		float	getRTime()	{	return m_RTime;		}	// what time was the message sent	
		float	getDTime()	{	return m_DTime;		}	// what time to deliver the message
		stringc getData()	{	return m_Data;		}	// the message

		void setType(FSMT v)	{	m_Type = v;		}	// message type
		void setType2(FSMT v)	{	m_Type2 = v;	}	// message type
		void setSource(int v)	{	m_Source = v;	}	// who initiated this message
		void setDest(int v)		{	m_Dest = v;		}	// who is the message going to
		void setRTime(float v)	{	m_RTime = v;	}	// what time was the message sent	
		void setDTime(float v)	{	m_DTime = v;	}	// what time to deliver the message
		void setData(stringc v) {	m_Data = v;		}	// the message

														// class constructor
		FSObjectMessage();
		FSObjectMessage(FSObjectMessage* m) :
			m_Type(m->getType()), m_Type2(m->getType2()), m_Source(m->getSource()), m_Dest(m->getDest()), m_RTime(m->getRTime()), m_DTime(m->getDTime()), m_Data(m->getData()) {}
		FSObjectMessage(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data) :
			m_Type(type), m_Type2(type2), m_Source(source), m_Dest(dest), m_RTime(rtime), m_DTime(dtime), m_Data(data) {}

		// class destructor
		virtual ~FSObjectMessage();

		// dual creation allows for better error handling
		virtual bool create(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data);
	};

	// a simple state machine definition
	#define FSBeginStateMachine   if( STATE_Global == getState() ) { if(0) {
	#define FSState(a)            return( true ); } } else if( a == getState() ) { if(0) {
	#define FSOnEnter             return( true ); } else if( MSG_RESERVED_Enter == msg->getType() ) { 
	#define FSOnExit              return( true ); } else if( MSG_RESERVED_Exit == msg->getType() ) { 
	#define FSOnUpdate            return( true ); } else if( MSG_RESERVED_Update == msg->getType() ) { 
	#define FSOnMsg(a)            return( true ); } else if( a == msg->getType() ) {
	#define FSSetState(a)         { setNextState(a); setForceStateChange(true); }
	#define FSEndStateMachine     return( true ); } } else { /*Logf("Invalid State" );*/ return( false );}  return( false );

	// simple state machine
	class FSStateMachine
	{
	public:

	private:
		////////////////////////////////////////////////////////////////////////////////////
		// State machine info
		unsigned int state;			// the current state of the state machine
		unsigned int next_state;	// the next state when a state change was requested
		bool force_state_change;	// whether a state change has been requested
									///////////////////////////////////////////////////////////////////////////////////
	public:
		unsigned int getState() { return state; }				// the current state of the state machine
		unsigned int getNextState() { return next_state; }			// the next state when a state change was requested
		bool getForceStateChange() { return force_state_change; }	// whether a state change has been requested

		void setState(unsigned int s) { state = s; }										// the current state of the state machine
		void setNextState(unsigned int s) { next_state = s; setForceStateChange(true); }	// the next state when a state change was requested
		void setForceStateChange(bool v) { force_state_change = v; }							// whether a state change has been requested

		FSStateMachine() { /* do nothing contructor */ };
		virtual ~FSStateMachine() { /* do nothing destrcutor */ };

		// set all variables to a knwon value
		virtual void initialize()
		{
			state = 0;
			next_state = 0;
			force_state_change = 0;
			setState(STATE_Global);

			// start the state machine
			FSObjectMessage msg;
			msg.create(MSG_RESERVED_Enter, NOMESSAGE, 0, 0, 0, 0, "");
			processStateMachine(&msg);
		};

		// step the state machine
		virtual bool processStateMachine(FSObjectMessage* msg) { return true; }

		// set the new state
		virtual void setCurrentState(int state) { FSSetState(state); };
	};

	class MessageList
	{
	public:
		core::list<FSObjectMessage*> m_List;				// list of messages
		core::list<FSObjectMessage*>::Iterator m_Iterator;	// message list iterator

		MessageList();
		~MessageList();
		void initialize();
		bool cleanup();
		bool create();
		bool add(FSObjectMessage* data);
		void logInfo();
	};

	// callback function for the messages
	typedef void(*MESSAGECALLBACKFUNC)(FSObjectMessage* message);

	// simple message manager class
	// messages are time based and are stored until the appropriate time.
	// a good example of using messages is for a missile to send itself a timed message
	// to explode in 10 seconds when it is fired. the message hangs around and gets delivered 
	// when the time is right. if the missle already exploded then the message will get 
	// dropped (since the reciever is not a valid object anymore), otherwise the missile will 
	// receive the message to explode and will react accordingly.
	class FSMessageManager
	{
	public:

		MessageList			m_List;				// the list of Message pointers
		FSObjectManager*	m_ObjectManager;	// list of created objects
		FSApplication*		m_Application;		// the app
		FSLevel*			m_Level;			// the level

		FSMessageManager();							// class constructor
		virtual ~FSMessageManager();				// class destructor
		virtual void initialize();					// set all variables to a known value

		// dual creation allows for better error handling
		virtual bool create(FSObjectManager* m, FSApplication* app, FSLevel* level);

		virtual bool cleanup();							// cleanup whatever memory mess we made

		virtual void preFrame(float elaspedtime) {}		// do what this class does each frame
		virtual void frame(float elaspedtime);			// do what this class does each frame
		virtual void postFrame(float elaspedtime) {}	// do what this class does each frame

														// create and add a message to the list
		virtual void addMessage(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data);
		virtual void addMessage(FSObjectMessage* msg);

		// send the message 
		virtual void dispatchMessage(FSObjectMessage* message);

		// clear out existing messages
		virtual void clear();

		// use a callback if you like.
		MESSAGECALLBACKFUNC MessageCallBack;
		int m_MessageCallBackId;
		void setMessageCallBack(MESSAGECALLBACKFUNC NewCallBack, int id);

	};

} // end namespace