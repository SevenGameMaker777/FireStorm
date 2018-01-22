#include "FSUtils.h"
#include "FSPhysX.h"
#include "FSLogger.h"
#include "FSLevel.h"
#include "FSObjectFactory.h"
#include "FSMessageManager.h"
#include "FSObject.h"
namespace FS
{
	// need to fix this. how to send world* to simulationfilter????
	// for now, this is set at FSLevel::load() and FSLevel::unload()
	FSPhysXWorld* gPhysXWorld = 0;
	void setGlobalPhysXWorld(FSPhysXWorld* w) { gPhysXWorld = w; }

	// filter the collisions. only process the collisions we care about
	PxFilterFlags SimulationFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}

		// generate contacts for all that were not filtered above
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		// if the world is valid (how did we get here if it isnt?)
		if (gPhysXWorld)
		{
			if (
				(pairFlags & PxPairFlag::eSOLVE_CONTACT) ||
				(pairFlags & PxPairFlag::eMODIFY_CONTACTS) ||
				(pairFlags & PxPairFlag::eNOTIFY_TOUCH_LOST) ||

				(pairFlags & PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND) ||
				(pairFlags & PxPairFlag::eNOTIFY_THRESHOLD_FORCE_PERSISTS) ||
				(pairFlags & PxPairFlag::eNOTIFY_THRESHOLD_FORCE_LOST) ||

				(pairFlags & PxPairFlag::eNOTIFY_CONTACT_POINTS) ||
				(pairFlags & PxPairFlag::eDETECT_DISCRETE_CONTACT) ||

				(pairFlags & PxPairFlag::ePRE_SOLVER_VELOCITY) ||
				(pairFlags & PxPairFlag::ePOST_SOLVER_VELOCITY) ||

				(pairFlags & PxPairFlag::eCONTACT_EVENT_POSE)
				)
			{
				// could handle all of these independently if we wanted
				// let the collision occur
				return PxFilterFlag::eDEFAULT;
			}
			else
			{
				// the collision flags
				//(pairFlags & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				//(pairFlags & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
				//(pairFlags & PxPairFlag::eNOTIFY_TOUCH_CCD)
				//(pairFlags & PxPairFlag::eNOTIFY_CCD_CONTACT)

				// trigger the contact callback if the OBJECT_TYPES of the two objects are supposed to report contact with each other
				if (gPhysXWorld->canCollide(filterData0.word0, filterData1.word0)) pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			}
		}

		// let the collision occur
		return PxFilterFlag::eDEFAULT;
	}

	// simple function. off of irrlicht forums somewhere
	PxQuat buildPxQuat(vector3df pos, vector3df rot)
	{
		matrix4 irrM;
		irrM.setTranslation(pos);
		irrM.setRotationDegrees(rot);
		quaternion q(irrM);
		return PxQuat(q.X, q.Y, q.Z, q.W);
	}

	/*
	simple wrapper for physx scene
	*/

	// unsafe methods for getting irrlicht pointers
	IrrlichtDevice*		FSPhysXWorld::getDevice()		{ return getPhysXManager()->getDevice(); }
	IVideoDriver*		FSPhysXWorld::getDriver()		{ return getPhysXManager()->getDriver(); }

	// initialize the class. 
	// Set all variables to a known value
	void FSPhysXWorld::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSPhysXWorld::initialize()");

		// set all variables to a known value
		m_PhysXManager = 0;
		m_Level = 0;
		m_Scene = 0;
		m_ControllerManager = 0;
		setResolvePhysX(true);

		// default to not showing the debug renderings
		setRenderDebugInfo(false);

		// clear the collision tests structures
		m_CollisionInfo.clear();
	}

	// dual creation allows for better error handling. 
	// All class variables are valid after this call
	// return false on failure
	bool FSPhysXWorld::create(FSPhysXManager* physx3, FSLevel* level)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSPhysXWorld::create()");

		// remember this
		m_PhysXManager = physx3;
		m_Level = level;

		//Create the scene
		PxSceneDesc sceneDesc(getPhysXManager()->getPhysXSDK()->getTolerancesScale());	//Descriptor class for scenes 
		sceneDesc.sanityBounds.minimum.x = -PX_MAX_BOUNDS_EXTENTS;
		sceneDesc.sanityBounds.minimum.y = -PX_MAX_BOUNDS_EXTENTS;
		sceneDesc.sanityBounds.minimum.z = -PX_MAX_BOUNDS_EXTENTS;
		sceneDesc.sanityBounds.maximum.x = PX_MAX_BOUNDS_EXTENTS;
		sceneDesc.sanityBounds.maximum.y = PX_MAX_BOUNDS_EXTENTS;
		sceneDesc.sanityBounds.maximum.z = PX_MAX_BOUNDS_EXTENTS;

		sceneDesc.gravity = PxVec3(0.0f, -132.0f, 0.0f);				//Setting gravity
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);	//Creates default CPU dispatcher for the scene
		sceneDesc.filterShader = SimulationFilterShader;	//Creates default collision filter shader for the scene

		sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS | PxSceneFlag::eENABLE_KINEMATIC_PAIRS;

		//Creates a scene 
		m_Scene = getPhysXManager()->getPhysXSDK()->createScene(sceneDesc);

		// send the callbacks to us
		m_Scene->setSimulationEventCallback(this);

		// Set the debug visualization parameters
		getScene()->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1);
		getScene()->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1);
		getScene()->setVisualizationParameter(PxVisualizationParameter::eBODY_ANG_VELOCITY, 1);
		getScene()->setVisualizationParameter(PxVisualizationParameter::eBODY_AXES, 1);
		getScene()->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1);

		// create the character controller manager
		m_ControllerManager = PxCreateControllerManager(*m_Scene);

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made.
	// all class variables are made invalid
	// always return false from a cleanup function
	bool FSPhysXWorld::cleanup()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSPhysXWorld::cleanup()");

		// cleanup the collision types
		removeAllCollisionTypes();

		// safely release the controller manager
		if (m_ControllerManager) { m_ControllerManager->release(); m_ControllerManager = 0; }

		// safely release the scene
		if (m_Scene) { m_Scene->release(); m_Scene = 0; }

		// forget this
		m_PhysXManager = 0;
		m_Level = 0;

		// always return false from a cleanup function
		return false;
	}

	// let the class do whatever it does each frame
	void FSPhysXWorld::preFrame(const float &elapsedtime)
	{
		// if we are resolving the simulation
		if (getResolvePhysX())
		{
			//Advances the simulation by elapsedtime
			if (getScene()) getScene()->simulate(elapsedtime);

			//Block until the simulation run is completed
			if (getScene()) getScene()->fetchResults(true);

		}
	}

	// let the class do whatever it does each frame
	void FSPhysXWorld::frame(const float &elapsedtime)
	{
		for (u32 i = 0; i < m_ContactList.size(); i++)
		{
			// hmmmm, do we want trigger messages or collision messages??
			getLevel()->getObjectFactory()->getMessageManager()->addMessage(m_ContactList[i].type, MSG_NULL, m_ContactList[i].obj2, m_ContactList[i].obj1, 0, 0, "");
			getLevel()->getObjectFactory()->getMessageManager()->addMessage(m_ContactList[i].type, MSG_NULL, m_ContactList[i].obj1, m_ContactList[i].obj2, 0, 0, "");
			if (m_ContactList[i].type == MESSAGE_TRIGGER_ENTER)  m_ContactList[i].type = MESSAGE_TRIGGER_CONTACT;
		}
	}

	// let the class do whatever it does each frame
	void FSPhysXWorld::postFrame(const float &elapsedtime)
	{
	}

	void FSPhysXWorld::removeAllCollisionTypes()
	{
		// runt hrought he list and remove them all
		if (!m_CollisionInfo.empty())
		{
			list<FSFilterData*>::Iterator it;
			for (it = m_CollisionInfo.begin(); it != m_CollisionInfo.end();)
			{
				delete((*it));
				it = m_CollisionInfo.erase(it);
			}
		}
		m_CollisionInfo.clear();
	}

	// maintains the list of collisions we will react to
	void FSPhysXWorld::detectCollisionsBetween(long o1, long o2)
	{
		FSFilterData* filterData = new FSFilterData();
		filterData->type1 = o1;
		filterData->type2 = o2;
		m_CollisionInfo.push_back(filterData);
	}

	// check to see if we care about this collision based on the two collision types
	bool FSPhysXWorld::canCollide(long obj1, long obj2)
	{
		if (!m_CollisionInfo.empty())
		{
			list<FSFilterData*>::ConstIterator it;
			for (it = m_CollisionInfo.begin(); it != m_CollisionInfo.end();)
			{
				if (((*it)->type1 == obj1) && ((*it)->type2 == obj2)) return true;
				if (((*it)->type1 == obj2) && ((*it)->type2 == obj1)) return true;
				it++;
			}
		}

		// we did not find the collision pair so we must not care about this one
		return false;
	}

	// called by PhysX when a trigger object detects a collision
	void FSPhysXWorld::onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		for (PxU32 i = 0; i < count; i++)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxTriggerPairFlag::eDELETED_SHAPE_TRIGGER | PxTriggerPairFlag::eDELETED_SHAPE_OTHER))
			{
				FS_LOG(FSL_DEBUG, "deleted shape trigger - no contact message sent");
				continue;
			}

			// get the id's of the two colliding objects
			int target = (int)pairs[i].otherShape->getActor()->userData;
			int me = (int)pairs[i].triggerShape->getActor()->userData;

			// if the level is valid
			if (getLevel())
			{
				if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					if (!inContactList(me, target))
					{
						//printf("adding to contact list %d %d\n", target, me);
						addToContactList(me, target, MESSAGE_TRIGGER_ENTER);
					}
				}
				if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					// currently there is no collision lost message so send trigger_exit isntead
					// maybe we need collision lost instead?
					getLevel()->getObjectFactory()->getMessageManager()->addMessage(MESSAGE_TRIGGER_EXIT, MSG_NULL, target, me, 0, 0, "");
					//printf("removing to contact list %d %d\n", target, me);
					removeFromContactList(target, me);
					removeFromContactList(me,target);
				}
			}

		}
	}

	// called by PhysX when two objects collide (if it passed out checkcollisions function)
	void FSPhysXWorld::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];

			// if the event type is touch_found
			if (
				(cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) || 
				(cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) ||
				(cp.events & PxPairFlag::eNOTIFY_TOUCH_CCD) ||
				(cp.events & PxPairFlag::eDETECT_CCD_CONTACT)
				)
			{
				if (getLevel())
				{
					// send the collision message to the level to broadcast through the objects
					getLevel()->getObjectFactory()->getMessageManager()->addMessage(MESSAGE_COLLISION, MSG_NULL, (int)pairHeader.actors[0]->userData, (int)pairHeader.actors[1]->userData, 0, 0, "");
					getLevel()->getObjectFactory()->getMessageManager()->addMessage(MESSAGE_COLLISION, MSG_NULL, (int)pairHeader.actors[1]->userData, (int)pairHeader.actors[0]->userData, 0, 0, "");
				}
			}
		}
	}

	int FSPhysXWorld::pickClosestObject(int x, int y, ISceneCollisionManager* collMan, ICameraSceneNode* cam, long ot)
	{
		// if the scene is notvalid then return NULL
		if (!getScene()) return NULL;

		position2d<s32> pos(x, y);
		const core::line3d<f32> ln = collMan->getRayFromScreenCoordinates(pos, cam);

		PxVec3 start(ln.start.X, ln.start.Y, ln.start.Z);
		vector3df d(ln.getVector());
		d.normalize();
		PxVec3 dir(d.X, d.Y, d.Z);
		PxSceneQueryHit hit;
		const PxSceneQueryFilterData filterData = PxSceneQueryFilterData();
			
		const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
		PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
		PxRaycastBuffer buf(hitBuffer, bufferSize); // [out] Blocking and touching hits will be stored here
		

		m_Start = vector3df(ln.start.X, ln.start.Y, ln.start.Z);
		m_End = vector3df(ln.end.X, ln.end.Y, ln.end.Z);

		float distance = 10000000;
		int best = -1;
		getScene()->raycast(start, dir,10000.0f,buf, PxHitFlags(PxHitFlag::eDEFAULT), PxQueryFilterData(),this);
		if (buf.nbTouches > 0)
		{
			for (PxU32 x = 0; x < buf.getNbTouches(); x++)
			{
				FSObject* obj = getLevel()->getObjectPointer((int)buf.touches[x].actor->userData, true);
				if ((obj) &&  ( (ot == FSOT_ALL) || (obj->getObjectType() == ot)) )
				{
					if (buf.getTouch(x).distance < distance)
					{
						distance = buf.getTouch(x).distance;
						best = x;
					}
				}
			}
		}
		if (best == -1) return 0;
		return (int)buf.getTouch(best).actor->userData;
	}

	void FSPhysXWorld::drawLine()
	{
		IrrlichtDevice* device = getDevice();
		IVideoDriver* driver = getDriver();

/*
		video::SMaterial material;
		material.Wireframe = false;
		material.Lighting = false;
		getDriver()->setMaterial(material);
		getDriver()->setTransform(video::ETS_WORLD, core::matrix4());
		SColor colour(255, 255, 0, 255);
		driver->draw3DLine(m_Start, m_End, SColor(255, 255, 255, 255));
*/
	}

	void FSPhysXWorld::renderDebugInfo()
	{
		if (!getRenderDebugInfo()) return;

		IrrlichtDevice* device = getDevice();
		IVideoDriver* driver = getDriver();

		video::SMaterial material;
		material.Wireframe = false;
		material.Lighting = false;
		getDriver()->setMaterial(material);
		getDriver()->setTransform(video::ETS_WORLD, core::matrix4());
		SColor colour(255, 255, 0, 255);

		const PxRenderBuffer& data = getScene()->getRenderBuffer();
//		if (data)
		{
			driver->setMaterial(material);
			driver->setTransform(video::ETS_WORLD, core::matrix4());

			u32 NbPoints = data.getNbPoints();
			if (NbPoints) {
				// printf("want to draw points\n");
			}

			u32 numLines = data.getNbLines();
			if (numLines) {
				video::S3DVertex* pVertList = new video::S3DVertex[numLines * 2];
				u16* pIndexList = new u16[numLines * 2];
				u32 vertIndex = 0;
				u32 indexIndex = 0;
				const PxDebugLine* lines = data.getLines();
				while (numLines--) {
					pIndexList[indexIndex++] = vertIndex;
					pVertList[vertIndex].Pos.X = lines->pos0.x;
					pVertList[vertIndex].Pos.Y = lines->pos0.y;
					pVertList[vertIndex].Pos.Z = lines->pos0.z;
					pVertList[vertIndex].Color = colour;
					vertIndex++;

					pIndexList[indexIndex++] = vertIndex;
					pVertList[vertIndex].Pos.X = lines->pos1.x;
					pVertList[vertIndex].Pos.Y = lines->pos1.y;
					pVertList[vertIndex].Pos.Z = lines->pos1.z;
					pVertList[vertIndex].Color = colour;
					vertIndex++;


					if (indexIndex >= 65530) { // 66536 index limit so before we reach it fire off a render and reset to zero
						driver->drawVertexPrimitiveList(pVertList, vertIndex, pIndexList, indexIndex / 2, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);
						indexIndex = 0;
						vertIndex = 0;
					}

					lines++;
				}

				driver->drawVertexPrimitiveList(pVertList, vertIndex, pIndexList, indexIndex / 2, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);

				delete[] pVertList;
				delete[] pIndexList;
			}

			u32 numTris = data.getNbTriangles();
			if (numTris) {
				video::S3DVertex* pVertList = new video::S3DVertex[numTris * 3];
				u16* pIndexList = new u16[numTris * 3];
				u32 vertIndex = 0;
				u32 indexIndex = 0;
				const PxDebugTriangle* triangles = data.getTriangles();
				while (numTris--) {
					pIndexList[indexIndex++] = vertIndex;
					pVertList[vertIndex].Pos.X = triangles->pos0.x;
					pVertList[vertIndex].Pos.Y = triangles->pos0.y;
					pVertList[vertIndex].Pos.Z = triangles->pos0.z;
					pVertList[vertIndex].Color = colour;
					vertIndex++;

					pIndexList[indexIndex++] = vertIndex;
					pVertList[vertIndex].Pos.X = triangles->pos1.x;
					pVertList[vertIndex].Pos.Y = triangles->pos1.y;
					pVertList[vertIndex].Pos.Z = triangles->pos1.z;
					pVertList[vertIndex].Color = colour;
					vertIndex++;

					pIndexList[indexIndex++] = vertIndex;
					pVertList[vertIndex].Pos.X = triangles->pos2.x;
					pVertList[vertIndex].Pos.Y = triangles->pos2.y;
					pVertList[vertIndex].Pos.Z = triangles->pos2.z;
					pVertList[vertIndex].Color = colour;
					vertIndex++;

					triangles++;
				}
				numTris = data.getNbTriangles();

				driver->drawIndexedTriangleList(pVertList, numTris * 3, pIndexList, numTris);
				delete[] pVertList;
				delete[] pIndexList;
			}
		}
	}

	FSPhysXObject* FSPhysXWorld::createPhysXObject(const IPhysXObjectData &data)
	{
		switch (data.type)
		{
			case POT_PRIMITIVE_CAPSULE	: return createCapsuleObject(data);		break;
			case POT_PRIMITIVE_BOX		: return createCubeObject(data);		break;
			case POT_PRIMITIVE_PLANE	: return createCubeObject(data);		break;
			case POT_TRIGGER			: return createCubeObject(data);		break;
			case POT_TREE				:
			case POT_MULTISHAPE			: return createTreeObject(data);		break;
			case POT_TERRAIN			: return createTerrainObject(data);		break;
			case POT_CHARACTER			: return createCharacterObject(data);	break;
		}

		// undefined object type
		return NULL;
	}

	FSPhysXObject* FSPhysXWorld::createCapsuleObject(const IPhysXObjectData &data)
	{
		vector3df scale = data.scale + data.bboffset;
		scale.X /= 2;

		vector3df pos = data.position;
//		if (data.bboffset.Y != 0) pos.Y += data.bboffset.Y / 2;

		if (data.dynamic)
		{
			FSPhysXObject_RigidDynamic* obj = new FSPhysXObject_RigidDynamic();
			obj->initialize();
			obj->create(this);

			PxRigidDynamic* actor = getPhysXManager()->getPhysXSDK()->createRigidDynamic(PxTransform(pos.X, pos.Y, pos.Z));
			if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in addCapsuleActor() call"); return NULL; }
			actor->setMass(data.mass);
			actor->setAngularDamping(1.0f);
			actor->setLinearDamping(1.0f);
			actor->setLinearVelocity(PxVec3(0, 0, 0));

			PxShape* shape = actor->createShape(PxCapsuleGeometry(scale.X, scale.Y), *getPhysXManager()->m_DefaultMaterial);
			if (!shape) { FS_LOG(FSL_DEBUG, "WARNING! shape is not valid in addCapsuleActor() call"); return NULL; }
			
			PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			shape->setLocalPose(relativePose);

			if (data.istrigger)
			{
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			}

			PxRigidBodyExt::updateMassAndInertia(*actor, PxReal(data.mass));

			getScene()->addActor(*actor);
			obj->setActor(actor);

			actor->userData = (void*)data.userdata;
			shape->userData = (void*)data.userdata;

			obj->setRotation(data.rotation);
			return obj;
		}
		else
		{
			FSPhysXObject_RigidStatic* obj = new FSPhysXObject_RigidStatic();
			obj->initialize();
			obj->create(this);

			PxRigidStatic* actor = getPhysXManager()->getPhysXSDK()->createRigidStatic(PxTransform(PxVec3(data.position.X, data.position.Y, data.position.Z), buildPxQuat(data.position, data.rotation)));
			if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in addCapsuleActor() call"); return NULL; }

			PxShape* shape = actor->createShape(PxCapsuleGeometry(scale.X, scale.Y), *getPhysXManager()->m_DefaultMaterial);
			if (!shape) { FS_LOG(FSL_WARNING, "WARNING! shape is not valid in addCapsuleActor() call"); return NULL;	}

			PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			shape->setLocalPose(relativePose);

			if (data.istrigger)
			{
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			}

			getScene()->addActor(*actor);
			obj->setActor(actor);

			actor->userData = (void*)data.userdata;
			shape->userData = (void*)data.userdata;

			return obj;
		}
		return NULL;
	}

	FSPhysXObject* FSPhysXWorld::createCubeObject(const IPhysXObjectData &data)
	{
		vector3df scale = data.scale/2 + data.bboffset;
		vector3df pos = data.position;
		
		if (data.dynamic)
		{
			FSPhysXObject_RigidDynamic* obj = new FSPhysXObject_RigidDynamic();
			obj->initialize();
			obj->create(this);

			PxRigidDynamic* actor = getPhysXManager()->getPhysXSDK()->createRigidDynamic(PxTransform(pos.X, pos.Y, pos.Z));
			if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in addCubeActor() call"); return NULL; }
			actor->setAngularDamping(1.0f);
			actor->setLinearDamping(1.0f);
			actor->setLinearVelocity(PxVec3(0, 0, 0));
			actor->setMass(data.mass);

			PxShape* shape = actor->createShape(PxBoxGeometry(scale.X, scale.Y, scale.Z), *getPhysXManager()->m_DefaultMaterial);
			if (!shape) { FS_LOG(FSL_WARNING, "WARNING! shape is not valid in addCubeActor() call"); return NULL; }
			if (data.istrigger)
			{
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			}
			PxRigidBodyExt::updateMassAndInertia(*actor, PxReal(data.mass));
			
			getScene()->addActor(*actor);
			obj->setActor(actor);

			actor->userData = (void*)data.userdata;
			shape->userData = (void*)data.userdata;

			obj->setRotation(data.rotation);
			return obj;
		}
		else
		{
			FSPhysXObject_RigidStatic* obj = new FSPhysXObject_RigidStatic();
			obj->initialize();
			obj->create(this);

			PxRigidStatic* actor = getPhysXManager()->getPhysXSDK()->createRigidStatic(PxTransform(PxVec3(data.position.X,data.position.Y,data.position.Z),buildPxQuat(data.position,data.rotation)));
			if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in addCubeActor() call"); return NULL; }
			
			PxShape* shape = actor->createShape(PxBoxGeometry(scale.X, scale.Y, scale.Z), *getPhysXManager()->m_DefaultMaterial);
			if (!shape) { FS_LOG(FSL_WARNING, "WARNING! shape is not valid in addCubeActor() call"); return NULL; }
			if (data.istrigger)
			{
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			}

			getScene()->addActor(*actor);
			obj->setActor(actor);

			actor->userData = (void*)data.userdata;
			shape->userData = (void*)data.userdata;

			obj->setRotation(data.rotation);
			return obj;
		}
		return NULL;
	}

	FSPhysXObject* FSPhysXWorld::createPlaneObject(const IPhysXObjectData &data)
	{
		vector3df scale = data.scale;
		scale /= 2;

		FSPhysXObject_RigidStatic* obj = new FSPhysXObject_RigidStatic();
		obj->initialize();
		obj->create(this);

		PxRigidStatic* actor = getPhysXManager()->getPhysXSDK()->createRigidStatic(PxTransform(PxVec3(data.position.X, data.position.Y, data.position.Z)));
		if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in addCubeActor() call"); return NULL; }

		PxShape* shape = actor->createShape(PxBoxGeometry(scale.X, scale.Y, scale.Z), *getPhysXManager()->m_DefaultMaterial);
		if (!shape) { FS_LOG(FSL_WARNING, "WARNING! shape is not valid in addCubeActor() call"); return NULL; }

		getScene()->addActor(*actor);
		obj->setActor(actor);

		actor->userData = (void*)data.userdata;
		shape->userData = (void*)data.userdata;

		return obj;
	}

	FSPhysXObject* FSPhysXWorld::createTreeObject(const IPhysXObjectData &data)
	{
		if (data.dynamic) return createDynamicTreeObject(data);
		else return createStaticTreeObject(data);
	}

	FSPhysXObject* FSPhysXWorld::createStaticTreeObject(const IPhysXObjectData &data)
	{
		FSPhysXObject_StaticTree* obj = new FSPhysXObject_StaticTree();
		obj->initialize();
		obj->create(this);

		if (data.mesh)
		{
			for (u32 x = 0; x < data.mesh->getMeshBufferCount(); x++)
				obj->addActor(createStaticTreeChunk(data, x));
		}

		obj->setRotation(data.rotation);
		return obj;
	}

	FSPhysXObject* FSPhysXWorld::createDynamicTreeObject(const IPhysXObjectData &data)
	{
		FSPhysXObject_DynamicTree* obj = new FSPhysXObject_DynamicTree();
		obj->initialize();
		obj->create(this);

		if (data.mesh)
		{
			for (u32 x = 0; x < data.mesh->getMeshBufferCount(); x++)
				obj->addActor(createDynamicTreeChunk(data, x));
		}

		obj->setRotation(data.rotation);
		return obj;
	}

	FSPhysXObject* FSPhysXWorld::createCharacterObject(const IPhysXObjectData &data)
	{
		FSPhysXObject_Character* obj = new FSPhysXObject_Character();
		obj->initialize();
		obj->create(this,data);
		return obj;
	}

	PxRigidStatic* FSPhysXWorld::createStaticTreeChunk(IPhysXObjectData data, int index)
	{
		PxTriangleMesh* triMesh = NULL;
		vector3df scale = data.scale;

		IMeshBuffer* meshBuffer = data.mesh->getMeshBuffer(index);
		PxVec3* verts = new PxVec3[meshBuffer->getVertexCount()];
		u32* indices = new u32[meshBuffer->getIndexCount()];

		switch (meshBuffer->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			video::S3DVertex* vertices = (video::S3DVertex*)meshBuffer->getVertices();
			for (u32 i = 0; i < meshBuffer->getVertexCount(); ++i)
				verts[i] = PxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_2TCOORDS:
		{
			video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)meshBuffer->getVertices();
			for (u32 i = 0; i < meshBuffer->getVertexCount(); ++i)
				verts[i] = PxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_TANGENTS:
		{
			video::S3DVertexTangents* vertices = (video::S3DVertexTangents*)meshBuffer->getVertices();
			for (u32 i = 0; i < meshBuffer->getVertexCount(); ++i)
				verts[i] = PxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		}

		for (u32 i = 0; i < meshBuffer->getIndexCount(); ++i)
			indices[i] = meshBuffer->getIndices()[i];

		//Cooking mesh
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = meshBuffer->getVertexCount();
		meshDesc.triangles.count = meshBuffer->getIndexCount() / 3;
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.triangles.stride = sizeof(PxU32) * 3;
		meshDesc.points.data = verts;
		meshDesc.triangles.data = indices;
		meshDesc.flags = PxMeshFlags(0);

		PxDefaultMemoryOutputStream stream;
		bool ok = getPhysXManager()->getCooking()->cookTriangleMesh(meshDesc, stream);
		PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());
		PxTriangleMesh* triangleMesh = getPhysXManager()->getPhysXSDK()->createTriangleMesh(rb);

		if (!triangleMesh) { FS_LOG(FSL_WARNING, "WARNING traingle mesh is not valid"); return false; }

		PxRigidStatic* actor = getPhysXManager()->getPhysXSDK()->createRigidStatic(PxTransform(PxVec3(data.position.X, data.position.Y, data.position.Z)));
		if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in addtreechunk() call"); return NULL; }

		PxShape* shape = actor->createShape(PxTriangleMeshGeometry(triangleMesh), *getPhysXManager()->m_DefaultMaterial);
		if (!shape) { FS_LOG(FSL_WARNING, "WARNING! shape is not valid in addtreechunk() call"); return NULL; }

		actor->userData = (void*)data.userdata;
		shape->userData = (void*)data.userdata;

		getScene()->addActor(*actor);

		return actor;
	}

	PxRigidDynamic* FSPhysXWorld::createDynamicTreeChunk(IPhysXObjectData data, int index)
	{
		vector3df scale = data.scale;

		IMeshBuffer* meshBuffer = data.mesh->getMeshBuffer(index);
		PxVec3* verts = new PxVec3[meshBuffer->getVertexCount()];
		u32* indices = new u32[meshBuffer->getIndexCount()];

		switch (meshBuffer->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			video::S3DVertex* vertices = (video::S3DVertex*)meshBuffer->getVertices();
			for (u32 i = 0; i < meshBuffer->getVertexCount(); ++i)
				verts[i] = PxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_2TCOORDS:
		{
			video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)meshBuffer->getVertices();
			for (u32 i = 0; i < meshBuffer->getVertexCount(); ++i)
				verts[i] = PxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_TANGENTS:
		{
			video::S3DVertexTangents* vertices = (video::S3DVertexTangents*)meshBuffer->getVertices();
			for (u32 i = 0; i < meshBuffer->getVertexCount(); ++i)
				verts[i] = PxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		}

		for (u32 i = 0; i < meshBuffer->getIndexCount(); ++i)
			indices[i] = meshBuffer->getIndices()[i];

		int indexcount = meshBuffer->getIndexCount() / 3;
		int vertexcount = meshBuffer->getVertexCount();


		//Cooking mesh
		PxConvexMeshDesc meshDesc;
		meshDesc.points.count = vertexcount;
		meshDesc.triangles.count = indexcount;
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.triangles.stride = sizeof(PxU32) * 3;
		meshDesc.points.data = verts;
		meshDesc.triangles.data = indices;
		meshDesc.flags = PxConvexFlags(0);

		PxDefaultMemoryOutputStream stream;
		bool ok = getPhysXManager()->getCooking()->cookConvexMesh(meshDesc, stream);
		PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());
		PxConvexMesh* convexMesh = getPhysXManager()->getPhysXSDK()->createConvexMesh(rb);

		if (!convexMesh) { FS_LOG(FSL_WARNING, "WARNING convex mesh is not valid"); return false; }

		PxRigidDynamic* actor = getPhysXManager()->getPhysXSDK()->createRigidDynamic(PxTransform(PxVec3(data.position.X, data.position.Y, data.position.Z)));
		if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in addtreechunk() call"); return NULL; }

		actor->setAngularDamping(1.0f);
		actor->setLinearDamping(1.0f);
		actor->setLinearVelocity(PxVec3(0, 0, 0));
		actor->setMass(data.mass);

		PxShape* shape = actor->createShape(PxConvexMeshGeometry(convexMesh), *getPhysXManager()->m_DefaultMaterial);
		if (!shape) { FS_LOG(FSL_WARNING, "WARNING! shape is not valid in addtreechunk() call"); return NULL; }

		actor->userData = (void*)data.userdata;
		shape->userData = (void*)data.userdata;

		getScene()->addActor(*actor);
		return actor;
	}


	FSPhysXObject* FSPhysXWorld::createTerrainObject(const IPhysXObjectData &data)
	{
		// pointer to the terrain scenenode
		ITerrainSceneNode* terrain = dynamic_cast<ITerrainSceneNode*>(data.node);

		// make sure that we have a valid scenenode
		if (!terrain) { FS_LOG(FSL_WARNING, "WARNING PhysX createTerrainObject() terrain node is not valid"); return NULL; }

		// temporary variable
		CDynamicMeshBuffer buffer(EVT_2TCOORDS, EIT_16BIT);

		// get pointer to dynamic buffer for LOD 0
		terrain->getMeshBufferForLOD(buffer, 0);

		// remember the scale of the terrain
		vector3df scale = terrain->getScale();

		// allocate a temporary array of vertices
		PxVec3* verts = new PxVec3[buffer.getVertexCount()];

		// get a pointer to the vertices
		S3DVertex2TCoords* vertices = (S3DVertex2TCoords*)buffer.getVertices();

		// scale each vertex properly
		for (u32 i = 0; i < buffer.getVertexCount(); ++i)
			verts[i] = PxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);

		// Cooking recipe
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = buffer.getVertexCount();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = verts;
		meshDesc.triangles.count = buffer.getIndexCount() / 3;
		meshDesc.triangles.stride = sizeof(u16) * 3;
		meshDesc.triangles.data = buffer.getIndexBuffer().getData();
		meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;

		// temproary variable
		PxDefaultMemoryOutputStream stream;

		// check the cooking result
		if (!getPhysXManager()->getCooking()->cookTriangleMesh(meshDesc, stream))
		{
			// log this event
			FS_LOG(FSL_WARNING, "WARNING PhysX createTerrainObject() cooktrianglemesh failed");

			// cleanup our memory mess
			delete[] verts;

			// bail......
			return NULL;
		}

		// create the triangle mesh
		PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());
		PxTriangleMesh* triangleMesh = getPhysXManager()->getPhysXSDK()->createTriangleMesh(rb);

		// cleanup our memory mess
		delete[] verts;

		// check if the mesh is valid, return NULL if not
		if (!triangleMesh) { FS_LOG(FSL_WARNING, "WARNING traingle mesh is not valid"); return NULL; }

		// create the actor
		PxRigidStatic* actor = getPhysXManager()->getPhysXSDK()->createRigidStatic(PxTransform(PxVec3(data.position.X, data.position.Y, data.position.Z)));
		if (!actor) { FS_LOG(FSL_WARNING, "WARNING! actor is not valid in createTerrainObject() call"); return NULL; }

		// create the shape
		PxShape* shape = actor->createShape(PxTriangleMeshGeometry(triangleMesh), *getPhysXManager()->m_DefaultMaterial);
		if (!shape) { FS_LOG(FSL_WARNING, "WARNING! shape is not valid in createTerrainObject() call"); return NULL; }

		PxFilterData d = shape->getSimulationFilterData();
		d.word0 = FSOT_TERRAIN;
		d.word1 = 0xffffffff;
		shape->setQueryFilterData(d);

		// add the actor to the physx scene
		getScene()->addActor(*actor);

		actor->userData = (void*)data.userdata;
		shape->userData = (void*)data.userdata;

		// safely release the mesh
		if (triangleMesh) { triangleMesh->release(); triangleMesh = 0; }

		// create our physx object holder
		FSPhysXObject_RigidStatic* obj = new FSPhysXObject_RigidStatic();
			obj->initialize();		// initialize the object
			obj->create(this);		// create the object
			obj->setActor(actor);	// remember the actor

		obj->setRotation(data.rotation);

		// everything went fine, return our physx object
		return obj;
	}

} // end namespace