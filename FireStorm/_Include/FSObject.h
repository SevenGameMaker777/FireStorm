// process this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FSMessageManager.h"
#include "FSPropertyManager.h"

// defines to make code easier to read
#define ADD_PROPERTY(nametag,tostring,fromstring) stringc fsGet##nametag() { return fromstring(get##nametag()); }; void fsSet##nametag(stringc s) { set##nametag(tostring(s)); }; 
#define REG_PROPERTY(classname, propname, set, get, datatype, description) { registerProperty(propname, datatype, (void*)(new CCallback<classname>(this, set, get)), (set ? TRUE : FALSE), description, FS_DEFAULT, FS_DEFAULT); }

// unclutter the global namespace
namespace FS
{
	// forwarddeclarations
	typedef class FSLevel FSLevel;
	typedef class FSPhysXObject FSPhysXObject;

	// simple class to hold object class information. this information is readily available for users to store / retrieve
	class FSObjectInfo
	{
	private:
		// class variables
		int m_GUID;				// unique identifier
		stringc m_Name;			// object formal name
		stringc m_Description;	// small description of object
		float m_Version;		// version control
		stringc m_Author;		// author of the object
	public:
		// class constructor
		FSObjectInfo(int guid, stringc name, stringc desc, float version, stringc author) :
			m_GUID(guid), m_Name(name), m_Description(desc), m_Version(version), m_Author(author)
		{}

		// crete function to remember all of the passed in variables
		void create(int guid, stringc name, stringc desc, float version, stringc author)
		{
			m_GUID = guid;
			m_Name = name;
			m_Description = desc;
			m_Version = version;
			m_Author = author;
		}

	public:
		// class members
		int getGUID() { return m_GUID; }	// unique identifier
		stringc getName() { return m_Name; }	// object formal name
		stringc getDescription() { return m_Description; }	// small description of object
		float getVersion() { return m_Version; }	// version control
		stringc getAuthor() { return m_Author; }	// author of the object
	};

	/*
	the object class
	responsible for the creation, management and destruction of all object components
	derive your object from this class to add your own functionality
	*/
	class FSObject
	{
	public:
		// this must be overridden in all base classes
		virtual ISceneNode* getPrimarySceneNode() = 0;

	private:
		FSLevel*			m_Level;			// pointer back to the level
		FSObjectInfo*		m_Info;				// class information
		FSPropertyManager*	m_PropertyManager;	// property container
		FSObjectManager*	m_Children;			// children of this object
		FSObject*			m_Parent;			// parent of this object
		FSPhysXObject*		m_PhysXObject;		// physx object wrapper

		// base class variables
			int				m_Id;				// simple id	
			stringc			m_Name;				// simple name
			int				m_Dead;				// set to true to auto destroy the object
			vector3df		m_Position;			// position
			vector3df		m_PositionOffset;	// node to physx offset
			vector3df		m_Rotation;			// rotation
			vector3df		m_RotationOffset;	// node to physx offset
			vector3df		m_Scale;			// scale
			vector3df		m_ScaleOffset;		// node to physx offset
			vector3df		m_BBOffset;			// node to physx offset 
			float			m_Health;			// simple health - set to 0 to auto destroy object
			long			m_ObjectType;		// flag for physx collision
			long			m_ObjectMaterial;	// flag for physx interaction
			bool			m_IsDebugObject;	// set to true to prevent saving in level file
			float			m_Mass;				// mass of the object
			bool			m_Static;			// static objects dont get updated each frame
		// class construction / destruction
	public:
		FSObject();
		virtual ~FSObject();

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name);

		// recreate the object
		virtual bool reCreate();

		// clenaup whatever memory mess we made
		virtual bool cleanup();

		// create the scenenodes
		virtual void createSceneNodes();

		// destroy the scenenodes
		virtual void destroyAllSceneNodes();

		// test capability
		virtual void setDefaultParams() {}
		virtual void logInfo();

		// object type flags
		virtual void setObjectType(long t);				// set the object type (mostly for collision filtering)
		virtual	bool isObjectType(long flag) { if (flag == FSOT_ALL) return true;  return (bool)(m_ObjectType & flag); }
		virtual void setObjectTypeFlags(long flags)		{ m_ObjectType = flags;					}
		virtual void setObjectTypeFlag(long flag)		{ m_ObjectType |= flag;					}
		virtual void removeObjectTypeFlag(long flag)	{ m_ObjectType ^= flag;					}

		// frame functions. called each frame
	public:
		virtual void preFrame(const float &elapsedtime, bool forceupdate);
		virtual void frame(const float &elapsedtime, bool forceupdate);
		virtual void postFrame(const float &elapsedtime, bool forceupdate);


		// class functions
	public:
		FSLevel*			getLevel();							// pointer back to the level
		FSObjectInfo*		getInfo();							// class information
		FSPropertyManager*	getPropertyManager();				// property container

		virtual void	setId(int id);						// set the object id
		void			setIdRecursive(ISceneNode* node);	// set the object id recursively
		virtual void	setName(stringc v);					// set the object name
		virtual void	setDead(int v);						// set the object dead value
		virtual void	setPosition(vector3df v);			// set the object position
		virtual void	setPositionOffset(vector3df v);		// set the object positionoffset 
		virtual void	setRotation(vector3df v);			// set the object rotation
		virtual void	setRotationOffset(vector3df v);		// set the object rotation offset
		virtual void	setScale(vector3df v);				// set the object scale (reCreate())
		virtual void	setScaleOffset(vector3df v);		// set the object scaleoffset (reCreate())
		virtual void	setBBOffset(vector3df v);			// set the object bounding box offset (reCreate())
		virtual void	setHealth(float v);					// set the object health
		virtual void	setIsDebugObject(bool value);		// set the object debug value
		virtual void	setMass(float value);				// set the object mass
		virtual void	setStatic(bool v);					// static objects dont get updated each frame

		virtual void setPositionValue(vector3df v)			{ m_Position = v;		}	// set variable / prevents recreate
		virtual void setPositionOffsetValue(vector3df v)	{ m_PositionOffset = v; }	// set variable / prevents recreate
		virtual void setRotationValue(vector3df v)			{ m_Rotation = v;		}	// set variable / prevents recreate
		virtual void setRotationOffsetValue(vector3df v)	{ m_RotationOffset = v; }	// set variable / prevents recreate
		virtual void setScaleValue(vector3df v)				{ m_Scale = v;			}	// set variable / prevents recreate
		virtual void setScaleOffsetValue(vector3df v)		{ m_ScaleOffset = v;	}	// set variable / prevents recreate
		virtual void setBBOffsetValue(vector3df v)			{ m_BBOffset = v;		}	// set variable / prevents recreate

		virtual int			getId(); 				// simple id	
		virtual stringc		getName(); 				// simple name
		virtual int			getDead(); 				// set to true to auto destroy the object
		virtual vector3df	getPosition(); 			// position
		virtual vector3df	getPositionOffset();	// node to physx offset
		virtual vector3df	getRotation(); 			// rotation
		virtual vector3df	getRotationOffset(); 	// node to physx offset
		virtual vector3df	getScale(); 			// scale
		virtual vector3df	getScaleOffset(); 		// node to physx offset
		virtual vector3df	getBBOffset(); 			// node to physx offset 
		virtual float		getHealth(); 			// simple health - set to 0 to auto destroy object
		virtual long		getObjectType(); 		// flag for physx collision
		virtual long		getObjectMaterial();	// flag for physx interaction
		virtual bool		getIsDebugObject();		// set to true to prevent saving in level file
		virtual float		getMass(); 				// simple mass of the object
		virtual bool		getStatic();			// static objects dont get updated each frame

		// manipulate the primary scene node
		virtual void setUseLight(bool value);
		virtual void setUseFog(bool value);
		virtual void setUseEditorVisuals(bool value);
		virtual void setShowDebugInfo(bool value);

		// send and receive messages to and from the messagemanager
		virtual void sendMessage(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data);
		virtual bool receiveMessage(FSObjectMessage* m);

		FSPhysXObject*	getPhysXObject();					// physx object wrapper
		virtual void	setPhysXObject(FSPhysXObject* obj);
		virtual void	createPhysXObject();
		virtual void	destroyPhysXObject();

		// object serialization
	public:
		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0);

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0);

		// property access
	public:
		void registerProperty(stringc name, stringc type, void *callback, bool readwrite, stringc description, stringc min, stringc max);
		bool setVariable(stringc variable, stringc value);
		stringc getVariable(stringc variable);
		stringc getVariable(int index);

		// event reaction functions
	public:
		virtual void onEventFire(FSObjectMessage* m);
		virtual void onEventCold(FSObjectMessage* m);
		virtual void onEventAcid(FSObjectMessage* m);
		virtual void onEventPhysical(FSObjectMessage* m);

		virtual bool onMouseWheel(const SEvent & e) { return false; }

		public:
			FSObjectManager*	getChildren() {	return m_Children; 	};			// children of this object
			virtual void setParent(FSObject* obj);
			virtual void addChild(FSObject* obj);
			virtual void destroyChild(int id);
			virtual FSObject* getChild(int id);

	public:
		// add some variables that can be edited during runtime
		ADD_PROPERTY(Id, stringcToInt, intToStringc);
		ADD_PROPERTY(Dead, stringcToInt, intToStringc);
		ADD_PROPERTY(Name, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(IsDebugObject, stringcToBool, boolToStringc);
		ADD_PROPERTY(Position, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(Rotation, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(Scale, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(PositionOffset, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(RotationOffset, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(ScaleOffset, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(BBOffset, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(Health, stringcToFloat, floatToStringc);
		ADD_PROPERTY(Mass, stringcToFloat, floatToStringc);

		// unsafe getter functions!
	public:
		IrrlichtDevice*		getDevice();					// the irrlicht device
		IVideoDriver*		getDriver();					// the irrrlicht video driver
		IGUIEnvironment*	getGui();						// the irrlicht gui
		ISceneManager*		getSmgr();						// the irrlicht scenemanager
		stringc				getDirectory(stringc dirName);	// get a directory string

	public:
		// set the position and rotation of the primary scenenode
		void setPrimarySceneNodePositionAndRotation(vector3df pos, vector3df rot);

		// get the object in, left and up directions
		virtual vector3df getIn(ISceneNode* node);
		virtual vector3df getLeft(ISceneNode* node);
		virtual vector3df getUp(ISceneNode* node);

		// rotate the object to face towards a position
		virtual void lookAt(vector3df pos);

		// determine if we are facing an object
		virtual vector3df getFacing(vector3df targetposition, float window = 5.0f);

		// determine if we are facing a position
		virtual bool isFacing(vector3df targetposition, float window = 5.0f);


	};

} //end namespace