// include this file only once
#pragma once

// include the needed header files
#include <Windows.h>
#include "irrlicht.h"
#include <stdio.h>  
#include <direct.h>  
#include <stdlib.h>  
#include <ctype.h> 

#include <irrKlang.h>

// link to the needed libraries
#ifdef _DEBUG
#pragma comment(lib,"irrlicht_debug.lib")
#pragma comment(lib,"irrklang.lib")
#else
#pragma comment(lib,"irrlicht.lib")
#pragma comment(lib,"irrklang.lib")
#endif

// use these namespaces to make typeing easier
using namespace irr;
using namespace scene;
using namespace gui;
using namespace core;
using namespace io;
using namespace video;
using namespace irrklang;

#include "PxPhysicsAPI.h"
#pragma comment(lib, "PhysX3CHECKED_x86.lib")	
#pragma comment(lib, "PhysX3CommonCHECKED_x86.lib") 
#pragma comment(lib, "PhysX3ExtensionsCHECKED.lib")
//#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PhysX3CookingCHECKED_x86.lib")
#pragma comment(lib,"PhysX3CharacterKinematicCHECKED_x86.lib")
using namespace physx;


// simple mode values. (set application mode to MODE_QUIT to auto exit program)
#define MODE_INIT	0
#define MODE_RUN	1
#define MODE_QUIT	2

#define FSID_MINIMIZE 2000
//#define EET_PROPERTYGRID_MINIMIZE 2001
#define EET_PROPERTYGRID_PROPERTYCHANGED 2008
#define WS_TITLEBAR 0x00000001
#define WS_SYSTEMBAR 0x00000010


#define FSID_EXPANDALL					2001
#define FSID_MINIMIZEALL				2002
#define FSID_TOP						2003

#define FSID_PROPERTYGRID_MINIMIZEALL	2004
#define FSID_PROPERTYGRID_EXPANDALL		2005
#define FSID_PROPERTYGRID_TOP			2006
#define EET_PROPERTYGRID_MINIMIZE		2007


// define a standardized default string value
#define FS_DEFAULT "FS_DEFAULT"

// simple defines to make working with directories easier
#define WORKINGPATH(x) stringc(getDirectory("WorkingDirectory") + stringc(x)).c_str() 
#define GAMEDATAPATH(x) stringc(getDirectory("GameDataDirectory") + stringc(x)).c_str() 
#define GAMESAVEPATH(x) stringc(getDirectory("GameSaveDirectory") + stringc(x)).c_str() 
#define MEDIAPATH(x) stringc(getDirectory("MediaDirectory") + stringc(x)).c_str() 
#define OBJECTPATH(x) stringc(getDirectory("ObjectsDirectory") + stringc(x)).c_str() 
#define PREFABPATH(x) stringc(getDirectory("PrefabDirectory") + stringc(x)).c_str() 
#define ACTIONTABLEPATH(x) stringc(getDirectory("ActionTableDirectory") + stringc(x)).c_str() 
#define SOUNDPATH(x) stringc(getDirectory("SoundDirectory") + stringc(x)).c_str() 
#define OBJECTDATAPATH(x) stringc(getDirectory("ObjectDataDirectory") + stringc(x)).c_str() 

#define SHORTWORKINGPATH(x) stringc(x.make_lower()).remove( (getDirectory("MediaDirectory").make_lower()).c_str()).c_str()
#define SHORTGAMEDATAPATH(x) stringc(x.make_lower()).remove( (getDirectory("GameDataDirectory").make_lower()).c_str()).c_str()
#define SHORTGAMESAVEPATH(x) stringc(x.make_lower()).remove( (getDirectory("GameSaveDirectory").make_lower()).c_str()).c_str()
#define SHORTMEDIAPATH(x) stringc(x.make_lower()).remove( (getDirectory("MediaDirectory").make_lower()).c_str()).c_str()
#define SHORTOBJECTPATH(x) stringc(x.make_lower()).remove( (getDirectory("ObjectDirectory").make_lower()).c_str()).c_str()
#define SHORTPREFABPATH(x) stringc(x.make_lower()).remove( (getDirectory("PrefabDirectory").make_lower()).c_str()).c_str()
#define SHORTACTIONTABLEPATH(x) stringc(x.make_lower()).remove( (getDirectory("ActionTableDirectory").make_lower()).c_str()).c_str()
#define SHORTSOUNDPATH(x) stringc(x.make_lower()).remove( (getDirectory("SoundDirectory").make_lower()).c_str()).c_str()
#define SHORTOBJECTDATAPATH(x) stringc(x.make_lower()).remove( (getDirectory("ObjectDataDirectory").make_lower()).c_str()).c_str()

#define FSID_APP_MESSAGE		1
#define FSID_LEVEL_MESSAGE		2

// define event message id's
#define EVT_SCREENRESIZED		1
#define EVT_LEVEL_LOAD			2
#define EVT_LEVEL_UNLOAD		3
#define EVT_LEVEL_DELETED		4
#define EVT_LEVEL_REMOVED		5
#define EVT_LEVEL_ADDED			6
#define EVT_LEVEL_CHANGED		7
#define EVT_OBJECT_DESTROYED	8
#define EVT_OBJECT_REMOVED		9
#define EVT_OBJECT_ADDED		10
#define EVT_OBJECT_SELECTED		11

// define some object materials
#define FSOM_WOOD		0x00000001
#define FSOM_METAL		0x00000010
#define FSOM_CLOTH		0x00000100
#define FSOM_PLASTIC	0x00001000
#define	FSOM_CHARACTER	0x00010000

#define FSOT_NONE		0x00000000
#define FSOT_ALL		0x11111111
#define FSOT_OBJECT		0x00000001
#define FSOT_TERRAIN	0x00000010
#define FSOT_STATIC		0x00000100
#define FSOT_SKYBOX		0x00001000
#define FSOT_LIGHT		0x00010000
#define FSOT_TRIGGER	0x00100000
#define FSOT_WATER		0x01000000
#define FSOT_CHARACTER	0x10000000

// simple id values for eventhandler messages
#define ID_APPLICATION		1
#define ID_ENGINE			2
#define ID_LEVELMANAGER		3
#define ID_LEVEL			4
#define ID_OBJECT			5
#define ID_OBJECTMANAGER	6
#define ID_DESKTOP			7
#define ID_CURSORTEXT		8



#define MODE_NONE				0x00000000
#define MODE_TRANSLATE			0x00000010
#define MODE_ROTATE				0x00000100
#define MODE_SCALE				0x00001000
#define MODE_BBOX				0x00010000
#define MODE_POSITIONOFFSET		0x00100000
#define MODE_ROTATIONOFFSET		0x01000000
#define MODE_SCALEOFFSET		0x10000000

struct FS_FogParams
{
public:
	SColor m_Color;
	float m_Start;
	float m_End;
	FS_FogParams() :m_Color(255, 0, 0, 0), m_Start(0), m_End(500) {}
	FS_FogParams(SColor c, float start, float end) :m_Color(c), m_Start(start), m_End(end) {}
};

// unclutter the global namespace
namespace FS
{
	extern stringc getEditorSelectionString(long e);

	void createTextAnim(IGUIEnvironment* gui, ISceneManager* smgr, ISceneNode* node, stringw text = L"", SColor color = SColor(255, 255, 0, 0), u32 duration = 2000, dimension2d<f32> size = dimension2d<f32>(18, 10));

	extern int getRandomInteger(int min, int max);
	extern float getRandomFloat(float min, float max);

	enum FilePickerType { FPT_STATICMODEL, FPT_ANIMATEDMODEL, FPT_TEXTURE, FPT_SKYBOXTHEME, FPT_PREFAB, FPT_NONE };
	extern bool isFileType(FilePickerType type, stringc s);

	vector3df getIn(ISceneNode* node);

	extern vector3df	stringcToVector3df(stringc v);
	extern stringc		stringcToStringc(stringc v);
	extern int			stringcToInt(stringc v);
	extern bool			stringcToBool(stringc v);
	extern float		stringcToFloat(stringc v);
	extern float		stringwToFloat(stringw v);
	extern SColorf		stringcToSColorf(stringc v);
	extern SColor		stringcToSColor(stringc v);
	extern dimension2df stringcToDimension2df(stringc v);
	extern dimension2du stringcToDimension2du(stringc v);
	extern aabbox3df stringcToaabbox3df(stringc v);


	extern stringc intToStringc(int v);
	extern stringc boolToStringc(bool v);
	extern stringc floatToStringc(float v);
	extern stringw floatToStringw(float v);
	extern stringc vector3dfToStringc(vector3df v);
	extern stringc SColorfToStringc(SColorf v);
	extern stringc SColorToStringc(SColor v);
	extern stringc dimension2dfToStringc(dimension2df v);
	extern stringc dimension2duToStringc(dimension2du v);
	extern stringc aabbox3dfToStringc(aabbox3df v);


} // end namespace

