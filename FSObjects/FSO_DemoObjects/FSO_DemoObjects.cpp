// FSO_Generic.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FSO_DemoObjects.h"


/////////////////////////////////////////////////////////////////////
//	these functions MUST be implemented in every game object DLL file
// so that the ObjectFactory can locate and create the objects
/////////////////////////////////////////////////////////////////////

#include "FSUtils.h"
#include "FSO_Character.h"
#include "FSO_MainCharacter.h"
#include "FSO_Character_Goblin.h"
#include "FSO_Fire.h"
#include "FSO_Torch.h"

extern "C"
{
	FSO_DEMOOBJECTS_API bool	queryIsObject();
	FSO_DEMOOBJECTS_API bool	queryIsObjectType(stringc objecttype);
	FSO_DEMOOBJECTS_API void*	createObject(stringc objecttype);
	FSO_DEMOOBJECTS_API void 	queryAllObjectTypes(list<stringc> *list);
	FSO_DEMOOBJECTS_API stringc	queryObjectDescription(stringc objecttype);
}

// simple test to determine if dll holds an Object class
bool queryIsObject() { return true; }

// simple function to determine if this DLL holds a specific type of Object
bool queryIsObjectType(stringc objecttype)
{
	if (objecttype == "FSO_CHARACTER") return true;
	if (objecttype == "FSO_MAINCHARACTER") return true;
	if (objecttype == "FSO_CHARACTER_GOBLIN") return true;
	if (objecttype == "FSO_FIRE") return true;
	if (objecttype == "FSO_TORCH") return true;
	return false;
}

// simple function to determine the types of Objects that this dll holds
void queryAllObjectTypes(list<stringc> *list)
{
	list->push_back(stringc("FSO_CHARACTER"));
	list->push_back(stringc("FSO_MAINCHARACTER"));
	list->push_back(stringc("FSO_CHARACTER_GOBLIN"));
	list->push_back(stringc("FSO_FIRE"));
	list->push_back(stringc("FSO_TORCH"));
}

// create a reference of the object type
void* createObject(stringc objecttype)
{
	FSObject* object = NULL;

	// attempt to instantiate the object
	if (objecttype == "FSO_CHARACTER") object = new FSO_Character();
	if (objecttype == "FSO_MAINCHARACTER") object = new FSO_MainCharacter();
	if (objecttype == "FSO_CHARACTER_GOBLIN") object = new FSO_Character_Goblin();
	if (objecttype == "FSO_FIRE") object = new FSO_Fire();
	if (objecttype == "FSO_TORCH") object = new FSO_Torch();

	// something went wrong
	return object;
}

stringc queryObjectDescription(stringc objecttype)
{
	stringc info;

	if (objecttype == "FSO_CHARACTER") info = "a simple little character";
	if (objecttype == "FSO_MAINCHARACTER") info = "a simple little character";
	if (objecttype == "FSO_CHARACTER_GOBLIN") info = "a simple little character";
	if (objecttype == "FSO_FIRE") info = "a simple little fire";
	if (objecttype == "FSO_TORCH") info = "a simple little torch";

	// return the info
	return info;
}

/////////////////////////////////////////////////////////////////////
//	end - these functions must be implemented in every dll file
/////////////////////////////////////////////////////////////////////
