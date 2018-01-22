#include "FSUtils.h"
#include "FSPhysX.h"
#include "FSLogger.h"

namespace FS
{ 
	///////////////////////////////////////////////////////////////////////////////
	// from irrlicht forums somewhere
	inline PxQuat EulerAngleToQuat(const PxVec3 &rot)
	{
		PxQuat qx(degToRad(rot.x), PxVec3(1.0f, 0.0f, 0.0f));
		PxQuat qy(degToRad(rot.y), PxVec3(0.0f, 1.0f, 0.0f));
		PxQuat qz(degToRad(rot.z), PxVec3(0.0f, 0.0f, 1.0f));
		return qz * qy * qx;
	}

	// initialize the class. 
	// Set all variables to a known value
	void FSPhysXObject_RigidDynamic::initialize()
	{
		// call the base class
		FSPhysXObject::initialize();

		// set to a known value
		m_Actor = 0;
	}

	// dual creation allows for better error handling. 
	// All class variables are valid after this call
	// return false on failure
	bool FSPhysXObject_RigidDynamic::create(FSPhysXWorld* world)
	{
		// call the base class
		FSPhysXObject::create(world);

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made.
	// all class variables are made invalid
	// always return false from a cleanup function
	bool FSPhysXObject_RigidDynamic::cleanup()
	{
		// safely release the actor
		if (m_Actor) { m_Actor->release(); m_Actor = 0; }

		// call the base class
		return FSPhysXObject::cleanup();
	}

	vector3df FSPhysXObject_RigidDynamic::getPosition()
	{
		vector3df pos(0, 0, 0);
		if (getActor())
		{
			PxTransform tx = getActor()->getGlobalPose();
			pos.set(tx.p.x, tx.p.y, tx.p.z);
		}
		return pos;
	}

	vector3df FSPhysXObject_RigidDynamic::getRotation()
	{
		vector3df rot(0, 0, 0);
		if (getActor())
		{
			PxMat33 mat = PxMat33::PxMat33(getActor()->getGlobalPose().q);
			irr::core::matrix4 irrM;
			irr::f32 fM[16];
			fM[0] = mat.column0.x;
			fM[1] = mat.column0.y;
			fM[2] = mat.column0.z;
			fM[4] = mat.column1.x;
			fM[5] = mat.column1.y;
			fM[6] = mat.column1.z;
			fM[8] = mat.column2.x;
			fM[9] = mat.column2.y;
			fM[10] = mat.column2.z;
			fM[15] = 1.0;
			irrM.setM(fM);
			rot = irrM.getRotationDegrees();
		}
		return rot;
	}

	void FSPhysXObject_RigidDynamic::setPosition(vector3df pos)
	{
		if (getActor())
		{
			PxTransform tx = getActor()->getGlobalPose();
			tx.p.x = pos.X;
			tx.p.y = pos.Y;
			tx.p.z = pos.Z;
			getActor()->setGlobalPose(tx);
		}
	}

	void FSPhysXObject_RigidDynamic::setRotation(vector3df rot)
	{
		matrix4 irrM;
		irrM.setRotationDegrees(rot);
		PxTransform xform = getActor()->getGlobalPose();
		quaternion q(irrM);
		xform.q.w = q.W;
		xform.q.x = q.X;
		xform.q.y = q.Y;
		xform.q.z = q.Z;
		getActor()->setGlobalPose(xform);
	}

	void FSPhysXObject_RigidDynamic::getPositionAndRotation(vector3df &pos, vector3df &rot)
	{
		if (getActor())
		{
			PxTransform tx = getActor()->getGlobalPose();
			pos.set(tx.p.x, tx.p.y, tx.p.z);

			PxMat33 mat = PxMat33::PxMat33(tx.q);
			irr::core::matrix4 irrM;

			irr::f32 fM[16];
			fM[0] = mat.column0.x;
			fM[1] = mat.column0.y;
			fM[2] = mat.column0.z;
			fM[4] = mat.column1.x;
			fM[5] = mat.column1.y;
			fM[6] = mat.column1.z;
			fM[8] = mat.column2.x;
			fM[9] = mat.column2.y;
			fM[10] = mat.column2.z;

			irrM.setM(fM);
			rot = irrM.getRotationDegrees();
		}
	}

	// add a force to the object
	void FSPhysXObject_RigidDynamic::addForce(vector3df dir, float magnitude)
	{
		if (getActor())
		{
			vector3df v = dir * magnitude;
			PxVec3 f(v.X, v.Y, v.Z);
			getActor()->setLinearVelocity(f);
		}
	}

	void FSPhysXObject_RigidDynamic::setFreeze(bool value)
	{
		if (getActor())
		{
			if (!value) getActor()->wakeUp();
			else getActor()->putToSleep();
		}
	}
	void FSPhysXObject_RigidDynamic::setMass(float value)
	{
		if (getActor())	getActor()->setMass(value);
	}
	float FSPhysXObject_RigidDynamic::getMass()
	{
		if (getActor())	return getActor()->getMass();
		return 0;
	}
	void FSPhysXObject_RigidDynamic::setLinearDamping(float value)
	{
		if (getActor())	getActor()->setLinearDamping(value);
	}

	void FSPhysXObject_RigidDynamic::setAngularDamping(float value)
	{
		if (getActor())	getActor()->setAngularDamping(value);
	}

	void FSPhysXObject_RigidDynamic::rotate(vector3df rotOffset)
	{
		if (getActor())
		{
			vector3df rot = getRotation();
			rot.X = 0;
			rot.Z = 0;
			rot.Y += rotOffset.Y;
			setRotation(rot);
		}
	}

	// attach this to an object
	void FSPhysXObject_RigidDynamic::setUserData(int id)
	{
		if (getActor())
		{
			getActor()->userData = (void*)id;
			PxShape* shapes[10];
			PxU32 nShapes = getActor()->getShapes(shapes, 10);
			while (nShapes--)
			{
				shapes[nShapes]->userData = (void*)id;
			}
		}
	}

	void FSPhysXObject_RigidDynamic::setObjectType(long t)
	{
		if (getActor())
		{
			PxShape* shapes[10];
			PxU32 nShapes = getActor()->getShapes(shapes, 10);
			while (nShapes--)
			{
				PxFilterData data = shapes[nShapes]->getSimulationFilterData();
				data.word0 = t;

				//shapes[nShapes]->setSimulationFilterData(PxFilterData(t, 0, 0, 0));
				shapes[nShapes]->setSimulationFilterData(data);
				shapes[nShapes]->setQueryFilterData(data);
				shapes[nShapes]->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// initialize the class. 
	// Set all variables to a known value
	void FSPhysXObject_RigidStatic::initialize()
	{
		// call the base class
		FSPhysXObject::initialize();

		// set to a known value
		m_Actor = 0;
	}

	// dual creation allows for better error handling. 
	// All class variables are valid after this call
	// return false on failure
	bool FSPhysXObject_RigidStatic::create(FSPhysXWorld* world)
	{
		// call the base class
		FSPhysXObject::create(world);

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made.
	// all class variables are made invalid
	// always return false from a cleanup function
	bool FSPhysXObject_RigidStatic::cleanup()
	{
		// safely release the actor
		if (m_Actor) { m_Actor->release(); m_Actor = 0; }

		// call the base class
		return FSPhysXObject::cleanup();
	}

	vector3df FSPhysXObject_RigidStatic::getPosition()
	{
		vector3df pos(0, 0, 0);
		if (getActor())
		{
			PxTransform tx = getActor()->getGlobalPose();
			pos.set(tx.p.x, tx.p.y, tx.p.z);
		}
		return pos;
	}

	vector3df FSPhysXObject_RigidStatic::getRotation()
	{
		vector3df rot(0, 0, 0);
		if (getActor())
		{
			PxMat33 mat = PxMat33::PxMat33(getActor()->getGlobalPose().q);
			PxVec3 v = getActor()->getGlobalPose().p;

			irr::core::matrix4 irrM;
			irr::f32 fM[16];
			fM[0] = mat.column0.x;
			fM[1] = mat.column0.y;
			fM[2] = mat.column0.z;
			fM[4] = mat.column1.x;
			fM[5] = mat.column1.y;
			fM[6] = mat.column1.z;
			fM[8] = mat.column2.x;
			fM[9] = mat.column2.y;
			fM[10] = mat.column2.z;
			fM[15] = 0.0;
			irrM.setM(fM);
			irrM.setTranslation(vector3df(v.x, v.y, v.z));
			rot = irrM.getRotationDegrees();
		}
		return rot;
	}

	void FSPhysXObject_RigidStatic::setPosition(vector3df pos)
	{
		if (getActor())
		{
			PxTransform tx = getActor()->getGlobalPose();
			tx.p.x = pos.X;
			tx.p.y = pos.Y;
			tx.p.z = pos.Z;
			getActor()->setGlobalPose(tx);
		}
	}

	void FSPhysXObject_RigidStatic::setRotation(vector3df rot)
	{
		matrix4 irrM;
		irrM.setRotationDegrees(rot);
		PxTransform xform = getActor()->getGlobalPose();
		quaternion q(irrM);
		xform.q.w = q.W;
		xform.q.x = q.X;
		xform.q.y = q.Y;
		xform.q.z = q.Z;
		getActor()->setGlobalPose(xform);
	}

	void FSPhysXObject_RigidStatic::getPositionAndRotation(vector3df &pos, vector3df &rot)
	{
		if (getActor())
		{
			PxTransform tx = getActor()->getGlobalPose();
			pos.set(tx.p.x, tx.p.y, tx.p.z);

			PxMat33 mat = PxMat33::PxMat33(tx.q);
			irr::core::matrix4 irrM;

			irr::f32 fM[16];
			fM[0] = mat.column0.x;
			fM[1] = mat.column0.y;
			fM[2] = mat.column0.z;
			fM[4] = mat.column1.x;
			fM[5] = mat.column1.y;
			fM[6] = mat.column1.z;
			fM[8] = mat.column2.x;
			fM[9] = mat.column2.y;
			fM[10] = mat.column2.z;

			irrM.setM(fM);
			rot = irrM.getRotationDegrees();
		}
	}

	void FSPhysXObject_RigidStatic::rotate(vector3df rotOffset)
	{
		if (getActor())
		{
			vector3df rot = getRotation();
			rot.X = 0;
			rot.Z = 0;
			rot.Y += rotOffset.Y;
			setRotation(rot);
		}
	}

	// attach this to an object
	void FSPhysXObject_RigidStatic::setUserData(int id)
	{
		if (getActor())
		{
			getActor()->userData = (void*)id;
			PxShape* shapes[10];
			PxU32 nShapes = getActor()->getShapes(shapes, 10);
			while (nShapes--)
			{
				shapes[nShapes]->userData = (void*)id;
			}
		}
	}

	void FSPhysXObject_RigidStatic::setObjectType(long t)
	{
		if (getActor())
		{
			PxShape* shapes[10];
			PxU32 nShapes = getActor()->getShapes(shapes, 10);
			while (nShapes--)
			{
				PxFilterData data = shapes[nShapes]->getSimulationFilterData();
				data.word0 = t;
				data.word1 = 0xffffffff;
				shapes[nShapes]->setSimulationFilterData(data);
				shapes[nShapes]->setQueryFilterData(data);
				shapes[nShapes]->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// initialize the class. 
	// Set all variables to a known value
	void FSPhysXObject_StaticTree::initialize()
	{
		// call the base class
		FSPhysXObject_RigidStatic::initialize();

		// set to a known value
		for (int x = 0; x<MAX_ACTORS; x++) m_Actors[x] = 0;
	}

	// dual creation allows for better error handling. 
	// All class variables are valid after this call
	// return false on failure
	bool FSPhysXObject_StaticTree::create(FSPhysXWorld* world)
	{
		// call the base class
		FSPhysXObject_RigidStatic::create(world);

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made.
	// all class variables are made invalid
	// always return false from a cleanup function
	bool FSPhysXObject_StaticTree::cleanup()
	{
		// safely release the actors
		for (int x = 1; x < MAX_ACTORS; x++)
		{
			if (m_Actors[x]) { m_Actors[x]->release(); m_Actors[x] = 0; }
		}

		// call the base class
		return FSPhysXObject_RigidStatic::cleanup();
	}

	void FSPhysXObject_StaticTree::setPosition(vector3df pos)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				PxTransform tx = m_Actors[x]->getGlobalPose();
				tx.p.x = pos.X;
				tx.p.y = pos.Y;
				tx.p.z = pos.Z;
				m_Actors[x]->setGlobalPose(tx);
			}
		}
	}

	void FSPhysXObject_StaticTree::setRotation(vector3df rot)
	{
		for (int x = 0; x < MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				matrix4 irrM;
				irrM.setRotationDegrees(rot);
				PxTransform xform = m_Actors[x]->getGlobalPose();
				quaternion q(irrM);
				xform.q.w = q.W;
				xform.q.x = q.X;
				xform.q.y = q.Y;
				xform.q.z = q.Z;
				m_Actors[x]->setGlobalPose(xform);
			}
		}
	}

	void FSPhysXObject_StaticTree::addActor(PxRigidStatic* actor)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (!m_Actors[x])
			{
				m_Actors[x] = actor;
				if (x == 0)
				{
					setActor(m_Actors[0]);
				}
				return;
			}
		}
	}

	// attach this to an object
	void FSPhysXObject_StaticTree::setUserData(int id)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				m_Actors[x]->userData = (void*)id;
				PxShape* shapes[10];
				PxU32 nShapes = m_Actors[x]->getShapes(shapes, 10);
				while (nShapes--)
				{
					shapes[nShapes]->userData = (void*)id;
				}
			}
		}
	}

	void FSPhysXObject_StaticTree::setObjectType(long t)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				PxShape* shapes[10];
				PxU32 nShapes = m_Actors[x]->getShapes(shapes, 10);
				while (nShapes--)
				{
					PxFilterData data = shapes[nShapes]->getSimulationFilterData();
					data.word0 = t;
					shapes[nShapes]->setSimulationFilterData(data);
					shapes[nShapes]->setQueryFilterData(data);
					shapes[nShapes]->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// initialize the class. 
	// Set all variables to a known value
	void FSPhysXObject_DynamicTree::initialize()
	{
		// call the base class
		FSPhysXObject_RigidDynamic::initialize();

		// set to a known value
		for (int x = 0; x<MAX_ACTORS; x++) m_Actors[x] = 0;
	}

	// dual creation allows for better error handling. 
	// All class variables are valid after this call
	// return false on failure
	bool FSPhysXObject_DynamicTree::create(FSPhysXWorld* world)
	{
		// call the base class
		FSPhysXObject_RigidDynamic::create(world);

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made.
	// all class variables are made invalid
	// always return false from a cleanup function
	bool FSPhysXObject_DynamicTree::cleanup()
	{
		// safely release the actors
		for (int x = 1; x < MAX_ACTORS; x++)
		{
			if (m_Actors[x]) { m_Actors[x]->release(); m_Actors[x] = 0; }
		}

		// call the base class
		return FSPhysXObject_RigidDynamic::cleanup();
	}

	void FSPhysXObject_DynamicTree::setPosition(vector3df pos)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				PxTransform tx = m_Actors[x]->getGlobalPose();
				tx.p.x = pos.X;
				tx.p.y = pos.Y;
				tx.p.z = pos.Z;
				m_Actors[x]->setGlobalPose(tx);
			}
		}
	}

	void FSPhysXObject_DynamicTree::setRotation(vector3df rot)
	{
		for (int x = 0; x < MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				matrix4 irrM;
				irrM.setRotationDegrees(rot);
				PxTransform xform = m_Actors[x]->getGlobalPose();
				quaternion q(irrM);
				xform.q.w = q.W;
				xform.q.x = q.X;
				xform.q.y = q.Y;
				xform.q.z = q.Z;
				m_Actors[x]->setGlobalPose(xform);
			}
		}
	}

	void FSPhysXObject_DynamicTree::addActor(PxRigidDynamic* actor)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (!m_Actors[x])
			{
				m_Actors[x] = actor;
				if (x == 0)
				{
					setActor(m_Actors[0]);
				}
				return;
			}
		}
	}

	// attach this to an object
	void FSPhysXObject_DynamicTree::setUserData(int id)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				m_Actors[x]->userData = (void*)id;
				PxShape* shapes[10];
				PxU32 nShapes = m_Actors[x]->getShapes(shapes, 10);
				while (nShapes--)
				{
					shapes[nShapes]->userData = (void*)id;
				}
			}
		}
	}

	void FSPhysXObject_DynamicTree::setObjectType(long t)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (m_Actors[x])
			{
				PxShape* shapes[10];
				PxU32 nShapes = m_Actors[x]->getShapes(shapes, 10);
				while (nShapes--)
				{
					PxFilterData data = shapes[nShapes]->getSimulationFilterData();
					data.word0 = t;
					shapes[nShapes]->setSimulationFilterData(data);
					shapes[nShapes]->setQueryFilterData(data);
					shapes[nShapes]->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	ControllerHitReport gControllerHitReport;

	void FSPhysXObject_Character::initialize()
	{
		FSPhysXObject::initialize();
		m_Controller = 0;
		m_Jumping = false;
		m_JumpTime = 0;
		m_Crouching = false;
		m_TotalForce.set(0, 0, 0);
		m_MaxJumpTime = 2;
		m_JumpFinished = false;
	}

	bool FSPhysXObject_Character::create(FSPhysXWorld* world, IPhysXObjectData data)
	{
		if (!FSPhysXObject::create(world)) return false;

		#define SKINWIDTH	0.0001f
		
		PxF32	gInitialRadius = data.scale.X;
		PxF32	gInitialHeight = data.scale.Y;

		if (data.node)
		{
			gInitialRadius = data.node->getBoundingBox().getExtent().X / 3 *data.scale.X;
			gInitialHeight = data.node->getBoundingBox().getExtent().Y / 2 *data.scale.Y;
		}

		PxCapsuleControllerDesc desc;
		desc.position.x			= data.position.X;
		desc.position.y			= data.position.Y;
		desc.position.z			= data.position.Z;
		desc.radius				= gInitialRadius;
		desc.height				= gInitialHeight;
		desc.upDirection		= PxVec3(0,1,0);
		desc.slopeLimit			= cosf(degToRad(45.0f));
		desc.stepOffset			= 0.02f;
		desc.callback			= &gControllerHitReport;
		desc.userData			= (void*)data.userdata;
		desc.scaleCoeff			= 0.9f;
		desc.volumeGrowth		= 1.2f;
		desc.density			= 10;
		desc.material			= getWorld()->getPhysXManager()->m_DefaultMaterial;
		m_Controller			= world->getControllerManager()->createController(*getWorld()->getPhysXManager()->m_PhysXSDK, getWorld()->getScene(), desc);
		m_Controller->setUserData((void*)data.userdata);
		m_Controller->getActor()->userData = (void*)data.userdata;
		
		//m_Controller->setContactOffset(-1.0f);

		if (!m_Controller) { FS_LOG(FSL_WARNING, "FSPhysXObject_Character::create() Controller creaation failed"); return NULL; }

		// everything went fine
		return true;
	}

	bool FSPhysXObject_Character::cleanup()
	{
		// safely release the controller
		if (m_Controller) { m_Controller->release(); m_Controller = 0; }

		// always return false form a cleanup function
		return FSPhysXObject::cleanup();
	}

	vector3df FSPhysXObject_Character::getPosition()
	{
		vector3df pos(0, 0, 0);
		if (getController())
		{
			PxExtendedVec3 p = getController()->getPosition();
			pos.set((float)p.x, (float)p.y, (float)p.z);
		}
		return pos;
	}

	void FSPhysXObject_Character::setPosition(vector3df pos)
	{
		PxExtendedVec3 p(pos.X, pos.Y, pos.Z);
		if (getController()) getController()->setPosition(p);
	}

	void FSPhysXObject_Character::drawDebugInfo(SColor color)
	{
		if (!m_Controller) return;
		if (!m_Controller->getActor()) return;

		// get a pointer to the device
		IrrlichtDevice* device = getWorld()->getPhysXManager()->getDevice();
		core::matrix4 mat;
		video::SMaterial material;
		material.TextureLayer->Texture = 0;
		material.Lighting = false;
		device->getVideoDriver()->setMaterial(material);
		device->getVideoDriver()->setTransform(video::ETS_WORLD, core::matrix4());

		{
			PxBounds3 dest = getController()->getActor()->getWorldBounds();
			const core::aabbox3d<f32> box(dest.minimum.x, dest.minimum.y, dest.minimum.z, dest.maximum.x, dest.maximum.y, dest.maximum.z);
			device->getVideoDriver()->draw3DBox(box, color);
		}

	}

	void FSPhysXObject_Character::setFreeze(bool value)
	{
		if (!m_Controller) return;
		if (!m_Controller->getActor()) return;
		if (!value) m_Controller->getActor()->wakeUp();
		else m_Controller->getActor()->putToSleep();
	}
	void FSPhysXObject_Character::setMass(float value)
	{
		if (!m_Controller) return;
		if (!m_Controller->getActor()) return;
		m_Controller->getActor()->setMass(value);
	}
	float FSPhysXObject_Character::getMass()
	{
		if (!m_Controller) return 1;
		if (!m_Controller->getActor()) return 1;
		return m_Controller->getActor()->getMass();
	}
	void FSPhysXObject_Character::setLinearDamping(float value)
	{
		if (!m_Controller) return;
		if (!m_Controller->getActor()) return;
		m_Controller->getActor()->setLinearDamping(value);
	}

	void FSPhysXObject_Character::setAngularDamping(float value)
	{
		if (!m_Controller) return;
		if (!m_Controller->getActor()) return;
		m_Controller->getActor()->setAngularDamping(value);
	}

	void FSPhysXObject_Character::addForce(vector3df dir, float force)
	{
		if (!m_Controller) return;
		if (!m_Controller->getActor()) return;

		dir.normalize();
		PxVec3 disp = PxVec3(dir.X, dir.Y, dir.Z) * force;
		m_TotalForce += vector3df(disp.x, disp.y, disp.z);
	}


	PxU32 FSPhysXObject_Character::moveCharacter(const PxVec3& dispVector, PxF32 elapsedTime, PxU32 collisionGroups)
	{
		if (!m_Controller) return 0;
		if (!m_Controller->getActor()) return 0;

		PxF32 sharpness = 1.0f;
		PxVec3 d = dispVector*elapsedTime;

		const PxU32 collisionFlags = m_Controller->move(d, -100, elapsedTime, NULL); // PxControllerFilters());
		return collisionFlags;
	}

	void FSPhysXObject_Character::frame(const float &elapsedtime)
	{
		float et = elapsedtime;
		
		// add the gravity each frame
		PxVec3 disp(0,-132,0);

		vector3df tf = getTotalForce();
		m_TotalForce = vector3df(0, 0, 0);

		disp += PxVec3(tf.X, tf.Y, tf.Z);
		disp.y += getHeight(et);

		PxU32 collisionFlags = moveCharacter(disp, et, 0);
		if (m_Jumping)	if (collisionFlags & PxControllerFlag::eCOLLISION_DOWN) stopJumping();
	}

	bool FSPhysXObject_Character::filter(const PxController& a, const PxController& b)
	{
		return true;
	}


	void FSPhysXObject_Character::jump()
	{
		if (m_Jumping) return;
		m_JumpTime = 0.0f;
		m_Jumping = true;
		m_JumpFinished = false;
	}

	void FSPhysXObject_Character::stopJumping()
	{
		m_Jumping = false;
		m_JumpTime = 0.0f;
		m_JumpFinished = true;
	}

	PxF32 FSPhysXObject_Character::getHeight(PxF32 elapsedTime)
	{
		m_MaxJumpTime = 2;

		//if (m_Crouching) return 0;
		if (!m_Jumping)	return 0.0f;

		float G = 32.0f;
		float mV0 = 3.0f;

		m_JumpTime += elapsedTime;
		PxF32 h = G*m_JumpTime*m_JumpTime + mV0*m_JumpTime;

		if (m_JumpTime > m_MaxJumpTime) return 0;
	
//		return 65;
		return (32+h); //*elapsedTime;
	}

	void FSPhysXObject_Character::crouch(bool v)
	{
		if (m_Jumping) return;
		if (m_Crouching == v) return;

		if (m_Crouching)
		{
			// stand up
			getController()->resize(20);
		}
		else
		{
			// crouch down
			getController()->resize(40);
		}
		m_Crouching = v;
	}

	// attach this to an object
	void FSPhysXObject_Character::setUserData(int id)
	{
		for (int x = 0; x<MAX_ACTORS; x++)
		{
			if (m_Controller)
			{
				m_Controller->getActor()->userData = (void*)id;
				PxShape* shapes[10];
				PxU32 nShapes = m_Controller->getActor()->getShapes(shapes, 10);
				while (nShapes--)
				{
					shapes[nShapes]->userData = (void*)id;
				}
			}
		}
	}

	void FSPhysXObject_Character::setObjectType(long t)
	{
		if (getActor())
		{
			PxShape* shapes[10];
			PxU32 nShapes = m_Controller->getActor()->getShapes(shapes, 10);
			while (nShapes--)
			{
				PxFilterData data = shapes[nShapes]->getSimulationFilterData();
				data.word0 = t;
				data.word1 = 0xffffffff;
				shapes[nShapes]->setSimulationFilterData(data);
				shapes[nShapes]->setQueryFilterData(data);
				shapes[nShapes]->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

			}
		}
	}

} // end namespace
