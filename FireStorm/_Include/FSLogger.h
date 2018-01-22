// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include <stdio.h>  
#include <stdarg.h>  

// simple macro to convert enum to string value
#define ENUMTOSTRING(x) case x : return #x; break;

// if we are in debug mode, send events and messages to the global logger
#define _FSDEBUG

#ifdef _FSDEBUG
#define FS_LOG getGlobalLogger()->log
#define FS_LOG_EVENT getGlobalLogger()->logEvent
// otherwise just let them sit
#else 
#define FS_LOG 
#define FS_LOG_EVENT 
#endif


namespace FS
{
	// simple enums to simplify the code reading
	enum FSLOGTYPE { FSL_INFO, FSL_WARNING, FSL_ERROR, FSL_DEBUG, FSL_EVENT, FSL_NONE };
	static const char * FSLOGTYPE_STRING[] = { "INFO", "WARNING", "ERROR", "DEBUG", "EVENT", " " };

	// a single log text structure
	struct LogText
	{
	public:
		FSLOGTYPE m_Type;
		stringc m_Text;
		LogText(FSLOGTYPE lt, stringc text) : m_Type(lt), m_Text(text) {}
	};

	// the list of messages
	extern list<LogText*> gTextStrings;

	// a simple class for logging messages
	class FSLogger
	{
	public:
		bool logMessages = true;

		FSLogger() { /* do nothing constructor */ }
		virtual ~FSLogger() { clear(); /* clear the list */ }

		// clear all messages
		// run through the list and remove all of the messages
		virtual void clear();

		// override this function to route messages as desired
		// base class just adds the message to the list and printf's them to the console
		virtual void print(FSLOGTYPE type, stringc text, bool addtolist = true, bool printtoconsole = true);

		// parse the text for the next string
		int getString(stringc &text, char *szTypes, int index);

		/*
		%i - integer
		%f - float
		%c - char
		%s - char*
		%v - vector3df
		*/
		void log(FSLOGTYPE type, char *szTypes, ...);

		// simple method to get event type in string form
		stringc getEventType(const SEvent & e);

		// simpler irrlicht SEvent logging
		virtual void logEvent(stringc text, SEvent e, int id, stringc name);

		// simple keycode to string converter // http://irrlicht.sourceforge.net/forum/viewtopic.php?t=34584
		stringc keyCodeToStringc(EKEY_CODE e);
	};

	// external reference to the global logger object
	extern void setGlobalLogger(FSLogger* l);
	extern FSLogger* getGlobalLogger();

} // end namespace