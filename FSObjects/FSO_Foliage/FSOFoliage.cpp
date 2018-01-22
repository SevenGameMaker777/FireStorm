// FSOFoliage.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FSOFoliage.h"

/////////////////////////////////////////////////////////////////////
//	these functions MUST be implemented in every game object DLL file
// so that the ObjectFactory can locate and create the objects
/////////////////////////////////////////////////////////////////////

#include "FSUtils.h"
#include "FSO_Tree.h"
/*
#include "FSO_Light.h"
#include "FSO_Static.h"
#include "FSO_AnimatedMesh.h"
#include "FSO_Quakemap.h"
#include "FSO_Terrain.h"
#include "FSO_Skybox.h"
#include "FSO_Water.h"
*/

// simple test to determine if dll holds an Object class
bool queryIsObject() { return true; }

// simple function to determine if this DLL holds a specific type of Object
bool queryIsObjectType(stringc objecttype)
{
	if (objecttype == "FSO_TREE") return true;
	return false;
}

// simple function to determine the types of Objects that this dll holds
void queryAllObjectTypes(list<stringc> *list)
{
	list->push_back(stringc("FSO_TREE"));
}

// create a reference of the object type
void* createObject(stringc objecttype)
{
	FSObject* object = NULL;

	// attempt to instantiate the object
	if (objecttype == "FSO_TREE") object = new FSO_Tree();
	
	// something went wrong
	return object;
}

stringc queryObjectDescription(stringc objecttype)
{
	stringc info("unknown description");

	if (objecttype == "FSO_TREE") info = "a simple little tree";

	// return the info
	return info;
}


/////////////////////////////////////////////////////////////////////
//	end - these functions must be implemented in every dll file
/////////////////////////////////////////////////////////////////////
