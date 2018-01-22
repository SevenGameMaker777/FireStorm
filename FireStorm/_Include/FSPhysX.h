#pragma once

// include the needed header files
#include "FSUtils.h"
#include "PxSceneDesc.h"
#include "FSMessageManager.h"

// define this for pvd use
// #define FS_USE_PVD

namespace FS
{
	// forward class declaration
	typedef class FSPhysXObject FSPhysXObject;
	typedef class FSLevel FSLevel;

	struct FSInContact
	{
		FSMT type;
		int obj1;
		int obj2;
	};
	struct FSFilterData
	{
		long type1;
		long type2;
	};

	enum IPhysXObjectType
	{
		POT_AUTO_DETECT,
		POT_PRIMITIVE_BOX,
		POT_PRIMITIVE_SPHERE,
		POT_PRIMITIVE_CONE,
		POT_PRIMITIVE_CAPSULE,
		POT_PRIMITIVE_CYLINDER,
		POT_PRIMITIVE_CHAMFERCYLINDER,
		POT_PRIMITIVE_PLANE,
		POT_CONVEXHULL,
		POT_TREE,
		POT_TERRAIN,
		POT_CHARACTER,
		POT_CHARACTER2,
		POT_TRIGGER,
		POT_MULTISHAPE
	};

	struct IPhysXObjectData
	{
		IPhysXObjectData() : type(POT_AUTO_DETECT), objecttype(0), node(0), mesh(0),
			position(vector3df(0, 0, 0)), rotation(vector3df(0, 0, 0)), scale(vector3df(1, 1, 1)),
			radius(1), mass(0), gravity(false), linearDamping(0), angularDamping(0.5),
			bboffset(vector3df(0, 0, 0)), plane(1), normal(vector3df(0, 1, 0)), istrigger(0), frozen(0),
			userdata(0), version(1), dynamic(true) {}
		IPhysXObjectType type;
		long objecttype;
		ISceneNode* node;
		IMesh* mesh;
		vector3df position;
		vector3df rotation;
		vector3df scale;
		float radius;
		float mass;
		bool gravity;
		float linearDamping;
		float angularDamping;
		vector3df bboffset;
		float plane;
		vector3df normal;
		bool istrigger;
		bool frozen;
		int userdata;
		int version;
		bool dynamic;
	};

	class FSPhysXManager
	{
	public:
		FSPhysXManager()	{ /* do nothing */ }
		~FSPhysXManager()	{ /* do nothing */ }

		// initialize the class. 
		// Set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling. 
		// All class variables are valid after this call
		// return false on failure
		virtual bool create(IrrlichtDevice* device, IVideoDriver* driver);

		// cleanup whatever memory mess we made.
		// all class variables are made invalid
		// always return false from a cleanup function
		virtual bool cleanup();

		// default physix material
		PxMaterial* m_DefaultMaterial;

		IrrlichtDevice* m_Device;
		IrrlichtDevice* getDevice() { return m_Device; }
		IVideoDriver* m_Driver;
		IVideoDriver*  getDriver() { return m_Driver; }

		PxPhysics* m_PhysXSDK;
		PxPhysics* getPhysXSDK() { return m_PhysXSDK; }

		PxFoundation* m_Foundation;
		PxFoundation* getFoundation() { return m_Foundation; }
		PxDefaultErrorCallback m_DefaultErrorCallback;
		PxDefaultAllocator m_DefaultAllocatorCallback;
		PxCooking* m_Cooking;
		PxCooking* getCooking() { return m_Cooking; }
	private:
		bool createPhysX();
		void destroyPhysX();
		void connectPVD();
	};

	class FSPhysXWorld : public PxSimulationEventCallback, public PxQueryFilterCallback
	{
	public:
		vector3df m_Start;
		vector3df m_End;
		void drawLine();

		FSPhysXWorld()	{ /* do nothing */ }
		~FSPhysXWorld()	{ /* do nothing */ }

		// initialize the class. 
		// Set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling. 
		// All class variables are valid after this call
		// return false on failure
		virtual bool create(FSPhysXManager* manager, FSLevel* level);

		// cleanup whatever memory mess we made.
		// all class variables are made invalid
		// always return false from a cleanup function
		virtual bool cleanup();

		// let the class do whatever it does each frame
		virtual void preFrame(const float &elapsedtime);
		virtual void frame(const float &elapsedtime);
		virtual void postFrame(const float &elapsedtime);

		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count);
		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
		virtual void onWake(PxActor** actors, PxU32 count) {}
		virtual void onSleep(PxActor** actors, PxU32 count) {};
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);

		virtual PxQueryHitType::Enum preFilter(
			const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
		{
			return PxQueryHitType::eBLOCK;
		}
		virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
		{
			return PxQueryHitType::eBLOCK;
		}


		// render the physx world in 3d lines
		void renderDebugInfo();

		// unsafe methods for getting irrlicht pointers
		IrrlichtDevice*		getDevice();
		IVideoDriver*		getDriver();

		// class variables with setters and getters
		FSPhysXManager* m_PhysXManager;
		FSPhysXManager* getPhysXManager() { return m_PhysXManager; }
		PxScene* m_Scene;
		PxScene* getScene() { return m_Scene; }
		PxControllerManager* m_ControllerManager;
		PxControllerManager* getControllerManager() { return m_ControllerManager; }
		
		array<FSInContact> m_ContactList;
		bool inContactList(int id, int id2)
		{
			for (u32 i = 0; i < m_ContactList.size(); i++)
			{
				if ((m_ContactList[i].obj1 == id) && (m_ContactList[i].obj2 == id2)) return true;
				if ((m_ContactList[i].obj2 == id) && (m_ContactList[i].obj1 == id2)) return true;
			}
			return false;
		}
		void removeFromContactList(int id, int id2)
		{
			if (m_ContactList.empty()) return;
			u32 i = 0;
			while (i<=m_ContactList.size())
			{
				if ((m_ContactList[i].obj1 == id) && (m_ContactList[i].obj2 == id2)) m_ContactList.erase(i);
				if ((m_ContactList[i].obj2 == id) && (m_ContactList[i].obj1 == id2)) m_ContactList.erase(i);
				i++;
			}
		}
		void addToContactList(int id, int id2, FSMT type)
		{
			FSInContact c;
			c.obj1 = id;
			c.obj2 = id2;
			c.type = type;
			m_ContactList.push_back(c);
		}
		void removeAllContactInstances(int id)
		{
			for (u32 i = 0; i < m_ContactList.size(); i++)
			{
				if (m_ContactList[i].obj1 == id) m_ContactList.erase(i);
				else if (m_ContactList[i].obj2 == id) m_ContactList.erase(i);
			}
		}

		list<FSFilterData*> m_CollisionInfo;
		void removeAllCollisionTypes();
		void detectCollisionsBetween(long o1, long o2);
		bool canCollide(long obj1, long obj2);


		bool m_RenderDebugInfo;
		void setRenderDebugInfo(bool value) { m_RenderDebugInfo = value; }
		bool getRenderDebugInfo() { return m_RenderDebugInfo; }
		bool m_ResolvePhysX;
		bool getResolvePhysX() { return m_ResolvePhysX; }
		void setResolvePhysX(bool value) { m_ResolvePhysX = value; }

		FSLevel* m_Level;
		FSLevel* getLevel() { return m_Level; }

		FSPhysXObject* createPhysXObject(const IPhysXObjectData &data);

		FSPhysXObject* createCapsuleObject(const IPhysXObjectData &data);
		FSPhysXObject* createCubeObject(const IPhysXObjectData &data);
		FSPhysXObject* createPlaneObject(const IPhysXObjectData &data);
		FSPhysXObject* createTerrainObject(const IPhysXObjectData &data);
		FSPhysXObject* createCharacterObject(const IPhysXObjectData &data);

		FSPhysXObject* createTreeObject(const IPhysXObjectData &data);
		FSPhysXObject* createStaticTreeObject(const IPhysXObjectData &data);
		FSPhysXObject* createDynamicTreeObject(const IPhysXObjectData &data);
		PxRigidStatic* createStaticTreeChunk(IPhysXObjectData data, int index);
		PxRigidDynamic* createDynamicTreeChunk(IPhysXObjectData data, int index);

		int pickClosestObject(int x, int y, ISceneCollisionManager* collMan, ICameraSceneNode* cam, long ot);
	};

	class FSPhysXObject
	{
	public:
		FSPhysXObject()	{ /* do nothing */ }
		~FSPhysXObject()	{ /* do nothing */ }

		// initialize the class. 
		// Set all variables to a known value
		virtual void initialize() { m_World = 0; };

		// dual creation allows for better error handling. 
		// All class variables are valid after this call
		// return false on failure
		virtual bool create(FSPhysXWorld* world) { m_World = world;  return true; };

		// cleanup whatever memory mess we made.
		// all class variables are made invalid
		// always return false from a cleanup function
		virtual bool cleanup() { m_World = 0;  return false; };

		// let the class do whatever it does each frame
		virtual void preFrame(const float &elapsedtime)		{};
		virtual void frame(const float &elapsedtime)		{};
		virtual void postFrame(const float &elapsedtime)	{};

		virtual vector3df getPosition()										{ return vector3df(0, 0, 0); }
		virtual vector3df getRotation()										{ return vector3df(0, 0, 0); }
		virtual void setPosition(vector3df pos)								{														}
		virtual void setRotation(vector3df rot)								{														}
		virtual void getPositionAndRotation(vector3df &pos, vector3df &rot) { pos = vector3df(0, 0, 0); rot = vector3df(0, 0, 0); }

		virtual void setFreeze(bool value)					{};
		virtual void setMass(float value)					{};
		virtual float getMass()								{ return 0; };
		virtual void setLinearDamping(float value)			{};
		virtual void setAngularDamping(float value)			{};
		virtual void rotate(vector3df rotOffset)			{ setRotation(getRotation() + rotOffset); }

		// add a force to the object
		virtual void addForce(vector3df dir, float magnitude) {}

		virtual vector3df getIn()		{ return vector3df(0, 0, 0); }
		virtual vector3df getLeft()		{ return vector3df(0, 0, 0); }
		virtual vector3df getUp()		{ return vector3df(0, 0, 0); }

		// attach this to an object
		virtual void setUserData(int id) {}
		virtual void setObjectType(long t) {}

		FSPhysXWorld* m_World;
		FSPhysXWorld* getWorld() { return m_World; }

	};

	class FSPhysXObject_RigidDynamic : public FSPhysXObject
	{
	public:
		FSPhysXObject_RigidDynamic()	{ /* do nothing */ }
		~FSPhysXObject_RigidDynamic()	{ /* do nothing */ }

		// initialize the class. 
		// Set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling. 
		// All class variables are valid after this call
		// return false on failure
		virtual bool create(FSPhysXWorld* world);

		// cleanup whatever memory mess we made.
		// all class variables are made invalid
		// always return false from a cleanup function
		virtual bool cleanup();

		virtual vector3df getPosition();
		virtual vector3df getRotation();
		virtual void setPosition(vector3df pos);
		virtual void setRotation(vector3df rot);
		virtual void getPositionAndRotation(vector3df &pos, vector3df &rot);

		virtual void setFreeze(bool value);
		virtual void setMass(float value);
		virtual float getMass();
		virtual void setLinearDamping(float value);
		virtual void setAngularDamping(float value);
		virtual void rotate(vector3df rotOffset);

		// add a force to the object
		virtual void addForce(vector3df dir, float magnitude);

		// attach this to an object
		virtual void setUserData(int id);
		virtual void setObjectType(long t);

		PxRigidDynamic* m_Actor;
		void setActor(PxRigidDynamic* a) { m_Actor = a; };
		PxRigidDynamic* getActor() { return m_Actor; }
	};

	class FSPhysXObject_RigidStatic : public FSPhysXObject
	{
	public:
		FSPhysXObject_RigidStatic()		{ /* do nothing */ }
		~FSPhysXObject_RigidStatic()	{ /* do nothing */ }

		// initialize the class. 
		// Set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling. 
		// All class variables are valid after this call
		// return false on failure
		virtual bool create(FSPhysXWorld* world);

		// cleanup whatever memory mess we made.
		// all class variables are made invalid
		// always return false from a cleanup function
		virtual bool cleanup();

		virtual vector3df getPosition();
		virtual vector3df getRotation();
		virtual void setPosition(vector3df pos);
		virtual void setRotation(vector3df rot);
		virtual void getPositionAndRotation(vector3df &pos, vector3df &rot);

		virtual void rotate(vector3df rotOffset);

		// attach this to an object
		virtual void setUserData(int id);
		virtual void setObjectType(long t);

		PxRigidStatic* m_Actor;
		PxRigidStatic* getActor() { return m_Actor; }
		void setActor(PxRigidStatic* a) { m_Actor = a; }
	};

#define MAX_ACTORS 100
	class FSPhysXObject_StaticTree : public FSPhysXObject_RigidStatic
	{
	public:
		FSPhysXObject_StaticTree() { /* do nothing */ }
		~FSPhysXObject_StaticTree() { /* do nothing */ }

		// initialize the class. 
		// Set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling. 
		// All class variables are valid after this call
		// return false on failure
		virtual bool create(FSPhysXWorld* world);

		// cleanup whatever memory mess we made.
		// all class variables are made invalid
		// always return false from a cleanup function
		virtual bool cleanup();

		virtual void setPosition(vector3df pos);
		virtual void setRotation(vector3df rot);

		virtual void addActor(PxRigidStatic* actor);
		PxRigidStatic* m_Actors[MAX_ACTORS];

		// attach this to an object
		virtual void setUserData(int id);
		virtual void setObjectType(long t);
	};

	class FSPhysXObject_DynamicTree : public FSPhysXObject_RigidDynamic
	{
	public:
		FSPhysXObject_DynamicTree() { /* do nothing */ }
		~FSPhysXObject_DynamicTree() { /* do nothing */ }

		// initialize the class. 
		// Set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling. 
		// All class variables are valid after this call
		// return false on failure
		virtual bool create(FSPhysXWorld* world);

		// cleanup whatever memory mess we made.
		// all class variables are made invalid
		// always return false from a cleanup function
		virtual bool cleanup();

		virtual void setPosition(vector3df pos);
		virtual void setRotation(vector3df rot);

		virtual void addActor(PxRigidDynamic* actor);
		PxRigidDynamic* m_Actors[MAX_ACTORS];

		// attach this to an object
		virtual void setUserData(int id);
		virtual void setObjectType(long t);
	};

	class ControllerHitReport : public PxUserControllerHitReport
	{
	public:
		virtual void onControllerHit(const PxControllersHit& hit)
		{
			
		}

		virtual void onObstacleHit(const PxControllerObstacleHit& hit)
		{
		}

		virtual void onShapeHit(const PxControllerShapeHit& hit)
		{
		}

	};

	extern ControllerHitReport gControllerHitReport;

	class FSPhysXObject_Character : public FSPhysXObject, public PxControllerFilterCallback
	{
	public:
		FSPhysXObject_Character(void)			{}
		virtual ~FSPhysXObject_Character(void)	{}

		virtual void initialize();
		virtual bool create(FSPhysXWorld* world, IPhysXObjectData data);
		virtual bool cleanup();

		virtual void frame(const float &elapsedtime);


		virtual bool filter(const PxController& a, const PxController& b);

		virtual void setPosition(vector3df pos);
		virtual vector3df getPosition();

		virtual void drawDebugInfo(SColor color);
		virtual void setFreeze(bool value);
		virtual void setMass(float value);
		virtual float getMass();
		virtual void setLinearDamping(float value);
		virtual void setAngularDamping(float value);

		vector3df m_TotalForce;
		void setTotalForce(vector3df f) { m_TotalForce = f; }
		vector3df getTotalForce() { return m_TotalForce; }
		virtual void addForce(vector3df dir, float force);

		virtual PxActor*	getActor()	{ if (m_Controller) return m_Controller->getActor(); else return NULL; }
		PxU32 moveCharacter(const PxVec3& dispVector, PxF32 elapsedTime, PxU32 collisionGroups);

		PxController* getController() { return m_Controller; }
		PxController* m_Controller;

		virtual void jump();
		bool getJumping() { return m_Jumping; }
		bool m_JumpFinished;
		bool getJumpFinished() { return m_JumpFinished; }

		PxF32 getHeight(PxF32 elapsedTime);

		virtual void crouch(bool v);
		bool getCrouching() { return m_Crouching; }

		// attach this to an object
		virtual void setUserData(int id);
		virtual void setObjectType(long t);

	private:
		bool	m_Jumping;
		bool	m_Crouching;
		PxF32	m_JumpTime;
		PxF32	m_MaxJumpTime;
		void stopJumping();
//		void inJump();
	};


	void setGlobalPhysXWorld(FSPhysXWorld* w);

} // end namespace






