// FSO_Generic.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FSO_Generic.h"


/////////////////////////////////////////////////////////////////////
//	these functions MUST be implemented in every game object DLL file
// so that the ObjectFactory can locate and create the objects
/////////////////////////////////////////////////////////////////////

#include "FSUtils.h"
#include "FSO_Cube.h"
#include "FSO_Light.h"
#include "FSO_Static.h"
#include "FSO_AnimatedMesh.h"
#include "FSO_Quakemap.h"
#include "FSO_Terrain.h"
#include "FSO_Terrain_triplaner.h"
#include "FSO_Skybox.h"
#include "FSO_Water.h"
#include "FSO_ParticleSystem.h"
#include "FSO_Plane.h"

	// simple test to determine if dll holds an Object class
	bool queryIsObject() { return true; }

	// simple function to determine if this DLL holds a specific type of Object
	bool queryIsObjectType(stringc objecttype)
	{
		if (objecttype == "FSO_CUBE") return true;
		if (objecttype == "FSO_LIGHT") return true;
		if (objecttype == "FSO_STATIC") return true;
		if (objecttype == "FSO_ANIMATEDMESH") return true;
		if (objecttype == "FSO_TERRAIN") return true;
		if (objecttype == "FSO_TERRAIN_TRIPLANER") return true;
		if (objecttype == "FSO_SKYBOX") return true;
		if (objecttype == "FSO_QUAKEMAP") return true;
		if (objecttype == "FSO_WATER") return true;
		if (objecttype == "FSO_PARTICLESYSTEM") return true;
		if (objecttype == "FSO_PLANE") return true;
		return false;
	}

	// simple function to determine the types of Objects that this dll holds
	void queryAllObjectTypes(list<stringc> *list)
	{
		list->push_back(stringc("FSO_CUBE"));
		list->push_back(stringc("FSO_LIGHT"));
		list->push_back(stringc("FSO_STATIC"));
		list->push_back(stringc("FSO_ANIMATEDMESH"));
		list->push_back(stringc("FSO_TERRAIN"));
		list->push_back(stringc("FSO_TERRAIN_TRIPLANER"));
		list->push_back(stringc("FSO_SKYBOX"));
		list->push_back(stringc("FSO_QUAKEMAP"));
		list->push_back(stringc("FSO_WATER"));
		list->push_back(stringc("FSO_PARTICLESYSTEM"));
		list->push_back(stringc("FSO_PANE"));
	}

	// create a reference of the object type
	void* createObject(stringc objecttype)
	{
		FSObject* object = NULL;

		// attempt to instantiate the object
		if (objecttype == "FSO_CUBE") object = new FSO_Cube();
		if (objecttype == "FSO_LIGHT") object = new FSO_Light();
		if (objecttype == "FSO_STATIC") object = new FSO_Static();
		if (objecttype == "FSO_ANIMATEDMESH") object = new FSO_AnimatedMesh();
		if (objecttype == "FSO_TERRAIN") object = new FSO_Terrain();
		if (objecttype == "FSO_TERRAIN_TRIPLANER") object = new FSO_Terrain_TriPlaner();
		if (objecttype == "FSO_SKYBOX") object = new FSO_Skybox();
		if (objecttype == "FSO_QUAKEMAP") object = new FSO_Quakemap();
		if (objecttype == "FSO_WATER") object = new FSO_Water();
		if (objecttype == "FSO_PARTICLESYSTEM") object = new FSO_ParticleSystem();
		if (objecttype == "FSO_PLANE") object = new FSO_Plane();

		// something went wrong
		return object;
	}

	stringc queryObjectDescription(stringc objecttype)
	{
		stringc info;

		if (objecttype == "FSO_CUBE") info = "a simple little box";
		if (objecttype == "FSO_LIGHT") info = "a simple little light";
		if (objecttype == "FSO_STATIC") info = "a simple little model";
		if (objecttype == "FSO_ANIMATEDMESH") info = "a simple little animated model";
		if (objecttype == "FSO_TERRAIN") info = "a simple little animated model";
		if (objecttype == "FSO_TERRAIN_TRIPLANER") info = "a simple little animated model";
		if (objecttype == "FSO_SKYBOX") info = "a simple little animated model";
		if (objecttype == "FSO_QUAKEMAP") info = "a simple little animated model";
		if (objecttype == "FSO_WATER") info = "a simple little water mesh";
		if (objecttype == "FSO_PARTICLESYSTEM") info = "a simple little particle system";
		if (objecttype == "FSO_PLANE") info = "a simple little plane";

		// return the info
		return info;
	}


/////////////////////////////////////////////////////////////////////
//	end - these functions must be implemented in every dll file
/////////////////////////////////////////////////////////////////////
