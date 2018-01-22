// include theneeded header files
#include "FSObject.h"
#include "FSApplication.h"
#include "FSLogger.h"
#include "FSLevel.h"
#include "FSPhysX.h"

// unclutter the global namespace
namespace FS
{
	// unsafe getter functions!
	IrrlichtDevice*		FSObject::getDevice() { return getLevel()->getDevice(); }
	IVideoDriver*		FSObject::getDriver() { return getLevel()->getDriver(); }
	IGUIEnvironment*	FSObject::getGui() { return getLevel()->getGui(); }
	ISceneManager*		FSObject::getSmgr() { return getLevel()->getSmgr(); }
	stringc				FSObject::getDirectory(stringc dirName) { return getLevel()->getDirectory(dirName); }
	FSLevel*			FSObject::getLevel() { return m_Level; }
	FSObjectInfo*		FSObject::getInfo() { return m_Info; }
	FSPropertyManager*	FSObject::getPropertyManager() { return m_PropertyManager; }

	void FSObject::setParent(FSObject* obj)
	{
		m_Parent = obj;
	}
	void FSObject::addChild(FSObject* obj)
	{
		m_Children->addObject(obj);
		obj->setParent(this);
		obj->setScale(obj->getScale());
	}
	void FSObject::destroyChild(int id)
	{
		m_Children->destroyObject(id);
	}
	FSObject* FSObject::getChild(int id)
	{
		return m_Children->getObjectPointer(id, true);
	}

	FSObject::FSObject()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObject::FSObject()");
	}

	FSObject::~FSObject()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObject::~FSObject()");
	}

	void FSObject::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObject::initialize()");

		m_Parent = 0;
		m_PropertyManager = 0;
		m_Children = 0;
		m_Level = 0;
		m_Info = 0;
		m_IsDebugObject = 0;
		m_ObjectType = 0;
		m_Id = 0;
		m_Dead = 0;
		m_Health = 100000;
		m_Name = "FSObject";
		m_Position = vector3df(0, 0, 0);
		m_Rotation = vector3df(0, 0, 0);
		m_Scale = vector3df(1, 1, 1);
		m_PositionOffset = vector3df(0, 0, 0);
		m_RotationOffset = vector3df(0, 0, 0);
		m_ScaleOffset = vector3df(1, 1, 1);
		m_BBOffset = vector3df(0, 0, 0);
		m_Mass = 10;
		m_Static = false;
		m_PhysXObject = 0;
	}

	bool FSObject::create(FSLevel* level, int id, stringc name)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObject::create()");

		// create the info structure
		m_Info = new FSObjectInfo(1, "FSOBJECT", "The base FSObject class", 1.00f, "Seven");

		// remember this
		m_Level = level;
		m_Id = id;
		m_Name = name;

		// create the property manager
		m_PropertyManager = new FSPropertyManager();
		m_PropertyManager->initialize();
		m_PropertyManager->create();

		// create the children manager
		m_Children = new FSObjectManager();
		m_Children->initialize();
		m_Children->create(getLevel());

		// Register base class properties
		REG_PROPERTY(FSObject, "Id", NULL, &FSObject::fsGetId, "FS_INT", "unique object identifier");
		REG_PROPERTY(FSObject, "Name", &FSObject::fsSetName, &FSObject::fsGetName, "FS_PCHAR", "this is the object's name, i am using a long text to test the gui in the editor");
		REG_PROPERTY(FSObject, "Dead", &FSObject::fsSetDead, &FSObject::fsGetDead, "FS_INT", "set to -1 to auto cleanup theobject");
		REG_PROPERTY(FSObject, "IsDebugObject", &FSObject::fsSetIsDebugObject, &FSObject::fsGetIsDebugObject, "FS_BOOL", "set to true to prevent saving in level file");
		REG_PROPERTY(FSObject, "Position", &FSObject::fsSetPosition, &FSObject::fsGetPosition, "FS_VEC3D", "object position");
		REG_PROPERTY(FSObject, "Rotation", &FSObject::fsSetRotation, &FSObject::fsGetRotation, "FS_VEC3D", "object rotation");
		REG_PROPERTY(FSObject, "Scale", &FSObject::fsSetScale, &FSObject::fsGetScale, "FS_VEC3D", "object scale");
		REG_PROPERTY(FSObject, "PositionOffset", &FSObject::fsSetPositionOffset, &FSObject::fsGetPositionOffset, "FS_VEC3D", "Bounding box position offset");
		REG_PROPERTY(FSObject, "RotationOffset", &FSObject::fsSetRotationOffset, &FSObject::fsGetRotationOffset, "FS_VEC3D", "Bounding box rotation offset");
		REG_PROPERTY(FSObject, "ScaleOffset", &FSObject::fsSetScaleOffset, &FSObject::fsGetScaleOffset, "FS_VEC3D", "Scale offset of the Object");
		REG_PROPERTY(FSObject, "BBOffset", &FSObject::fsSetBBOffset, &FSObject::fsGetBBOffset, "FS_VEC3D", "Bounding box offset");
		REG_PROPERTY(FSObject, "Health", &FSObject::fsSetHealth, &FSObject::fsGetHealth, "FS_FLOAT", "health");
		REG_PROPERTY(FSObject, "Mass", &FSObject::fsSetMass, &FSObject::fsGetMass, "FS_FLOAT", "mass");

		// everything went fine
		return true;
	}

	bool FSObject::reCreate()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%d %s FSObject::reCreate()", getId(), getName().c_str());

		// destroy all of the children
		if (m_Children) m_Children->destroyAllObjects();

		// create the new scenenodes
		createSceneNodes();

		// create the PhysX objects
		createPhysXObject();

		// set the object's id
		setId(getId());

		// create the PhysX collision types
		setObjectType(getObjectType());

		// destroy all of the children
		if (m_Children) m_Children->reCreateAllObjects();

		// everything went fine
		return true;
	}

	void FSObject::createSceneNodes()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%d %s FSObject::createscenenodes()", getId(), getName().c_str());

		// destroy the old scenenodes first
		destroyAllSceneNodes();

		// destroy all of the children
		if (m_Children) m_Children->createSceneNodes();
	}

	void FSObject::destroyAllSceneNodes()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%d %s FSObject::destroyAllScenenodes()", getId(), getName().c_str());

		// destroy all of the children
		if (m_Children) m_Children->destroyAllSceneNodes();
	}

	bool FSObject::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObject::cleanup()");

		// delete the children
		if (m_Children) { m_Children->cleanup(); delete(m_Children); m_Children = NULL; }

		// destroy the physx object
		destroyPhysXObject();

		// destroy all of the scenenodes attached to this object
		destroyAllSceneNodes();

		// delete the property manager
		if (m_PropertyManager) { delete(m_PropertyManager); m_PropertyManager = NULL; }

		// safely delete the info structure
		if (m_Info) delete(m_Info); m_Info = 0;

		// forget this
		m_Id = 0;
		m_Level = 0;

		// always return false from a cleanup function
		return false;
	}

	void FSObject::preFrame(const float &elapsedtime, bool forceupdate)
	{
#if(0)
		if (getLevel()->getCamera()->getPosition().getDistanceFrom(getPosition()) > getLevel()->getCamera()->getFarValue())
		{
			getPrimarySceneNode()->setVisible(false);
			return;
		}
		else getPrimarySceneNode()->setVisible(true);
#endif

		if (getHealth() <= 0) setDead(true);

		// static objects do not get updated each frame
		if ((getStatic()) && (!forceupdate)) return;

		if (getPhysXObject()) getPhysXObject()->preFrame(elapsedtime);

		if (getHealth() <= 0) setDead(true);

		if ((getPhysXObject()) && (getPrimarySceneNode()))
		{
			m_Rotation = getPhysXObject()->getRotation();
			m_Position = getPhysXObject()->getPosition();
			vector3df up = getUp(getPrimarySceneNode()) * getPositionOffset().Y;
			vector3df left = getLeft(getPrimarySceneNode()) * getPositionOffset().X;
			vector3df in = getIn(getPrimarySceneNode()) * getPositionOffset().Z;
			vector3df posoffset = (up + in + left);

			setPrimarySceneNodePositionAndRotation(getPosition() + posoffset, getRotation() + getRotationOffset());
		}
		else
			setPrimarySceneNodePositionAndRotation(getPosition(), getRotation());

		setPrimarySceneNodePositionAndRotation(getPosition(), getRotation());

		if (m_Children) m_Children->preFrame(elapsedtime, forceupdate);
	}

	void FSObject::frame(const float &elapsedtime, bool forceupdate)
	{
		// static objects do not get updated each frame
		if ((getStatic()) && (!forceupdate)) return;
		if (getPhysXObject()) getPhysXObject()->frame(elapsedtime);

		if (m_Children) m_Children->frame(elapsedtime, forceupdate);
	}

	void FSObject::postFrame(const float &elapsedtime, bool forceupdate)
	{
		// static objects do not get updated each frame
		if ((getStatic()) && (!forceupdate)) return;
		if (getPhysXObject()) getPhysXObject()->postFrame(elapsedtime);

		if (m_Children) m_Children->postFrame(elapsedtime, forceupdate);
	}

	void FSObject::setPrimarySceneNodePositionAndRotation(vector3df pos, vector3df rot)
	{
		if (getPrimarySceneNode()) getPrimarySceneNode()->setRotation(rot + getRotationOffset());
		if (getPrimarySceneNode()) getPrimarySceneNode()->setPosition(pos + getPositionOffset());
	}

	void FSObject::setUseLight(bool value)
	{
		if (getPrimarySceneNode()) getPrimarySceneNode()->setMaterialFlag(video::EMF_LIGHTING, value);
		if (m_Children) m_Children->setUseLight(value);
	}

	void FSObject::setUseFog(bool value)
	{
		if (getPrimarySceneNode()) getPrimarySceneNode()->setMaterialFlag(video::EMF_FOG_ENABLE, value);
		if (m_Children) m_Children->setUseFog(value);
	}

	void FSObject::setUseEditorVisuals(bool value)
	{
	}

	void FSObject::setShowDebugInfo(bool value)
	{
		if (getPrimarySceneNode())
		{
			if (value) getPrimarySceneNode()->setDebugDataVisible(EDS_FULL);
			else getPrimarySceneNode()->setDebugDataVisible(EDS_OFF);
		}
	}

	void FSObject::setIsDebugObject(bool value)
	{
		m_IsDebugObject = value;
	}

	void FSObject::setStatic(bool v)
	{
		//m_Static = v;
	}

	bool FSObject::getStatic()
	{
		return m_Static;
	}

	void FSObject::setIdRecursive(ISceneNode* node)
	{
		if (node) node->setID(m_Id);
	}
	void FSObject::setId(int id)
	{
		m_Id = id;
		setIdRecursive(getPrimarySceneNode());
		if (m_Children) m_Children->setId(id);
	}
	void FSObject::setName(stringc v) { m_Name = v; }
	void FSObject::setObjectType(long t) { m_ObjectType = t; }
	void FSObject::setDead(int v) { m_Dead = v; }
	void FSObject::setPosition(vector3df v) { m_Position = v;	if (getPhysXObject()) getPhysXObject()->setPosition(v);	}
	void FSObject::setPositionOffset(vector3df v) { m_PositionOffset = v; }
	void FSObject::setRotation(vector3df v)
	{
		if (v.Y < 0) v.Y = 360 + v.Y;
		if (v.Y > 360) v.Y = v.Y - 360;
		m_Rotation = v;	if (getPhysXObject()) getPhysXObject()->setRotation(v);
	}


	void FSObject::setRotationOffset(vector3df v) { m_RotationOffset = v; }
	void FSObject::setScale(vector3df v) { m_Scale = v; reCreate(); }
	void FSObject::setScaleOffset(vector3df v) { m_ScaleOffset = v;	reCreate(); }
	void FSObject::setBBOffset(vector3df v) { m_BBOffset = v;	reCreate(); }
	void FSObject::setHealth(float v) { m_Health = v; }
	void FSObject::setMass(float v) { m_Mass = v; if (getPhysXObject()) getPhysXObject()->setMass(v); }

	int			FSObject::getId() { return m_Id; }	// simple id	
	stringc		FSObject::getName() { return m_Name; }	// simple name
	int			FSObject::getDead() { return m_Dead; }	// set to true to auto destroy the object

	vector3df	FSObject::getPosition() { if (m_Parent) return m_Position + m_Parent->getPosition(); return m_Position; } 	// position
	vector3df	FSObject::getRotation() { if (m_Parent) return m_Rotation + m_Parent->getRotation(); return m_Rotation; }	// rotation
	vector3df	FSObject::getScale() { if (m_Parent) return m_Scale* getScaleOffset() * m_Parent->getScale(); return m_Scale* getScaleOffset(); }	// scale

	vector3df	FSObject::getPositionOffset() { return m_PositionOffset; }	// node to physx offset
	vector3df	FSObject::getRotationOffset() { return m_RotationOffset; } 	// node to physx offset
	vector3df	FSObject::getScaleOffset() { return m_ScaleOffset; }	// node to physx offset
	vector3df	FSObject::getBBOffset() { return m_BBOffset; }	// node to physx offset 
	float		FSObject::getHealth() { return m_Health; } 	// simple health - set to 0 to auto destroy object
	long		FSObject::getObjectType() { return m_ObjectType; } 	// flag for physx collision
	long		FSObject::getObjectMaterial() { return m_ObjectMaterial; }	// flag for physx interaction
	bool		FSObject::getIsDebugObject() { return m_IsDebugObject; }	// set to true to prevent saving in level file
	float		FSObject::getMass() { return m_Mass; } 	// simple Mass

	FSPhysXObject* FSObject::getPhysXObject() { return m_PhysXObject; }					// physx object wrapper

	void FSObject::setPhysXObject(FSPhysXObject* obj)
	{
		destroyPhysXObject();
		m_PhysXObject = obj;
	}

	void FSObject::createPhysXObject()
	{
		destroyPhysXObject();

		// the children
		if (m_Children) m_Children->createPhysXObject();
	}

	void FSObject::destroyPhysXObject()
	{
		// the children
		if (m_Children) m_Children->destroyPhysXObject();

		if (getPhysXObject())
		{
			m_PhysXObject->cleanup();
			delete(m_PhysXObject);
			m_PhysXObject = 0;
		}
	}
	// send a message through the system
	void FSObject::sendMessage(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data)
	{
		getLevel()->sendMessage(type, type2, source, dest, rtime, dtime, data);
	}

	// receive a message from the system
	bool FSObject::receiveMessage(FSObjectMessage* m)
	{
		switch (m->getType())
		{
		case DAMAGE_FIRE: onEventFire(m); break;
		case DAMAGE_COLD: onEventCold(m); break;
		case DAMAGE_ACID: onEventAcid(m); break;
		case DAMAGE_PHYSICAL: onEventPhysical(m); break;
		}

		if (m->getSource() != getId()) return false;

		FSObject* obj = getLevel()->getObjectPointer(m->getDest(), true);
		switch (m->getType())
		{
		case MESSAGE_TRIGGER_ENTER: printf("enter trigger from %s\n", obj->getName().c_str()); break;
		case MESSAGE_TRIGGER_CONTACT: printf("inside trigger from %s\n", obj->getName().c_str()); break;
		case MESSAGE_TRIGGER_EXIT: printf("exit trigger from %s\n", obj->getName().c_str()); break;
		case MESSAGE_COLLISION: printf("collision with %s\n", obj->getName().c_str()); break;
		default:printf("UNKNONW %d\n", m->getType()); break;
		}

		return true;
	}

	//! Writes attributes of the object.
	void FSObject::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObject::serialize()");

		out->addInt("Id", m_Id);
		out->addString("Name", m_Name.c_str());
		out->addVector3d("Position", m_Position);
		out->addVector3d("Rotation", m_Rotation);
		out->addVector3d("Scale", m_Scale);
		out->addVector3d("PositionOffset", m_PositionOffset);
		out->addVector3d("RotationOffset", m_RotationOffset);
		out->addVector3d("ScaleOffset", m_ScaleOffset);
		out->addVector3d("BBOffset", m_BBOffset);
		out->addFloat("Health", m_Health);
		out->addInt("Dead", m_Dead);
		out->addFloat("Mass", m_Mass);
	}

	//! Reads attributes of the object.
	void FSObject::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSObject::deserialize()");

		m_Id = in->getAttributeAsInt("Id");
		m_Name = in->getAttributeAsString("Name");
		m_Position = in->getAttributeAsVector3d("Position");
		m_Rotation = in->getAttributeAsVector3d("Rotation");
		m_Scale = in->getAttributeAsVector3d("Scale");
		m_PositionOffset = in->getAttributeAsVector3d("PositionOffset");
		m_RotationOffset = in->getAttributeAsVector3d("RotationOffset");
		m_ScaleOffset = in->getAttributeAsVector3d("ScaleOffset");
		m_BBOffset = in->getAttributeAsVector3d("BBOffset");
		m_Health = in->getAttributeAsFloat("Health");
		m_Dead = in->getAttributeAsInt("Dead");
		m_Mass = in->getAttributeAsFloat("Mass");
	}

	void FSObject::logInfo()
	{
		FS_LOG(FSL_DEBUG, "Object Params    : ");
		FS_LOG(FSL_DEBUG, "Name             : %s", m_Name);
		FS_LOG(FSL_DEBUG, "Position         : %v", m_Position);
		FS_LOG(FSL_DEBUG, "Roation          : %v", m_Rotation);
		FS_LOG(FSL_DEBUG, "Scale            : %v", m_Scale);
		FS_LOG(FSL_DEBUG, "PositionOffset   : %v", m_PositionOffset);
		FS_LOG(FSL_DEBUG, "RotationOffset   : %v", m_RotationOffset);
		FS_LOG(FSL_DEBUG, "ScaleOffset      : %v", m_ScaleOffset);
		FS_LOG(FSL_DEBUG, "BBOffset      : %v", m_BBOffset);
		FS_LOG(FSL_DEBUG, "Object Params end: ");
	}

	void FSObject::registerProperty(stringc name, stringc type, void *callback, bool readwrite, stringc description, stringc min, stringc max)
	{
		varData* data = new varData();
		data->vName = name;
		data->vType = type;
		data->vControl = type;
		data->vAccess = readwrite;
		data->vCallback = callback;
		data->vDescription = description;
		data->vMin = min;
		data->vMax = max;
		getPropertyManager()->add(data);
	}

	stringc FSObject::getVariable(stringc variable)
	{
		return m_PropertyManager->getVariable(variable);
	}

	stringc FSObject::getVariable(int index)
	{
		return m_PropertyManager->getVariable(index);
	}

	bool FSObject::setVariable(stringc variable, stringc value)
	{
		return m_PropertyManager->setVariable(variable, value);
	}

	vector3df FSObject::getIn(ISceneNode* node)
	{
		if (node)
		{
			matrix4 mat = node->getRelativeTransformation();
			vector3df in(mat[8], mat[9], mat[10]);
			in.normalize();
			return in;
		}
		else return vector3df(0, 0, 0);
	}

	vector3df FSObject::getLeft(ISceneNode* node)
	{
		if (node)
		{
			matrix4 mat = node->getRelativeTransformation();
			vector3df left(mat[0], mat[1], mat[2]);
			left.normalize();
			return left;
		}
		else return vector3df(0, 0, 0);
	}

	vector3df FSObject::getUp(ISceneNode* node)
	{
		if (node)
		{
			matrix4 mat = node->getRelativeTransformation();
			vector3df up(mat[4], mat[5], mat[6]);
			up.normalize();
			return up;
		}
		else return vector3df(0, 0, 0);
	}

	void FSObject::lookAt(vector3df pos)
	{
		// rotate to face target pos
		vector3df nodePos = getPosition();
		vector3df targetPos = pos;
		vector3df diff = targetPos - nodePos;
		setRotation(diff.getHorizontalAngle());
	}



	vector3df FSObject::getFacing(vector3df targetposition, float window)

	{
		// rotate to face target pos
		vector3df nodePos = getPosition();
		vector3df targetPos = targetposition;
		vector3df diff = targetPos - nodePos;
		vector3df lookat = diff.getHorizontalAngle();
		return lookat;
	}



	bool FSObject::isFacing(vector3df targetposition, float window)
	{
		vector3df lookat = getFacing(targetposition);
		if (
			((getRotation().Y > lookat.Y - window) && (getRotation().Y < lookat.Y + window))
			||
			((lookat.Y > getRotation().Y - window) && (lookat.Y < getRotation().Y + window))
			)
			return true;
		else
			return false;
	}

	void FSObject::onEventFire(FSObjectMessage* m)
	{
		switch (getObjectMaterial())
		{
		case FSOM_CHARACTER: m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_WOOD: m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_CLOTH:m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_PLASTIC:m_Health -= stringcToFloat(m->getData()); break;
		}
	}

	void FSObject::onEventCold(FSObjectMessage* m)
	{
		switch (getObjectMaterial())
		{
		case FSOM_WOOD: m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_CLOTH:m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_METAL: m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_PLASTIC:m_Health -= stringcToFloat(m->getData()); break;
		}
	}

	void FSObject::onEventAcid(FSObjectMessage* m)
	{
		switch (getObjectMaterial())
		{
		case FSOM_WOOD: m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_CLOTH:m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_METAL: m_Health -= stringcToFloat(m->getData()); break;
		case FSOM_PLASTIC:m_Health -= stringcToFloat(m->getData()); break;
		}
	}

	void FSObject::onEventPhysical(FSObjectMessage* m)
	{
		switch (getObjectMaterial())
		{
		case FSOM_WOOD: m_Health -= stringcToInt(m->getData()); break;
		case FSOM_PLASTIC:m_Health -= stringcToInt(m->getData()); break;
		}
	}

} // end namespace
