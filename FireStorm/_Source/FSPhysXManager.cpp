
#include "FSUtils.h"
#include "FSPhysX.h"
#include "FSLogger.h"

namespace FS
{

	// initialize the class. 
	// Set all variables to a known value
	void FSPhysXManager::initialize()
	{
		FS_LOG(FSL_DEBUG, "FSPhysXManager::initialize()");

		m_Device = 0;
		m_Driver = 0;
		m_PhysXSDK = 0;
		m_Foundation = 0;
	}

	// dual creation allows for better error handling. 
	// All class variables are valid after this call
	// return false on failure
	bool FSPhysXManager::create(IrrlichtDevice* device, IVideoDriver* driver)
	{
		FS_LOG(FSL_DEBUG, "FSPhysXManager::create()");

		// remember these
		m_Device = device;
		m_Driver = driver;
		
		// initialize the physx sdk
		if (!createPhysX()) { FS_LOG(FSL_DEBUG, "ERROR unable to create PhysX sdk"); return false;	}

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made.
	// all class variables are made invalid
	// always return false from a cleanup function
	bool FSPhysXManager::cleanup()
	{
		FS_LOG(FSL_DEBUG, "FSPhysXManager::cleanup()");

		// shutdown the physx sdk
		destroyPhysX();

		// forget these
		m_Device = 0;
		m_Driver = 0;

		// always return false from a cleanup function
		return false;
	}

	// initialize the physx sdk objects
	bool FSPhysXManager::createPhysX()
	{
		FS_LOG(FSL_DEBUG, "FSPhysXManager::createPhysX()");

		// Create the physX foundation
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback);

		// create the PhysX SDK
		m_PhysXSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale());

		// verify that we succeeded
		if (!m_PhysXSDK) { FS_LOG(FSL_ERROR, "ERROR physx sdk is not initialized properly"); return false; }

		// create the default material
		getPhysXSDK()->createMaterial(0.5f, 0.5f, 0.5f);
		getPhysXSDK()->getMaterials(&m_DefaultMaterial, 1);
		m_DefaultMaterial->setRestitution(0);
		m_DefaultMaterial->setStaticFriction(0.5f);
		m_DefaultMaterial->setDynamicFriction(0.5f);

		PxTolerancesScale scale;
		PxCookingParams params(scale);
		params.meshWeldTolerance = 0.001f;
		params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES | PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
		m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, params);
		if (!m_Cooking) { FS_LOG(FSL_ERROR, "PxCreateCooking failed!"); return false; }

		// connect to the visualization
		connectPVD();

		// everything went fine
		return true;
	}

	// shutdown PhysX
	void FSPhysXManager::destroyPhysX()
	{
		FS_LOG(FSL_DEBUG, "FSPhysXManager::destroyPhysX()");
		if (m_Cooking)		{ m_Cooking->release(); m_Cooking = 0; }
		if (m_PhysXSDK)	{ m_PhysXSDK->release(); m_PhysXSDK = 0; }
		if (m_Foundation)	{ m_Foundation->release(); m_Foundation = 0; }
	}

	void FSPhysXManager::connectPVD()					//Function for the visualization of PhysX simulation (Optional and 'Debug' mode only) 
	{
		FS_LOG(FSL_DEBUG, "FSPhysXManager::connectPVD()");

#if(0)
		// check if PvdConnection manager is available on this platform
		if (getPhysXSDK()->getPvdConnectionManager() == NULL)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "no pvd connection found.....");

			// bail 
			return;
		}

		// setup connection parameters
		const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
		int             port = 5425;         // TCP port to connect to, where PVD is listening
		unsigned int    timeout = 100;          // timeout in milliseconds to wait for PVD to respond,
		// consoles and remote PFS need a higher timeout.
		PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();

		// and now try to connect
		debugger::comm::PvdConnection* theConnection = PxVisualDebuggerExt::createConnection(getPhysXSDK()->getPvdConnectionManager(),
			pvd_host_ip, port, timeout, connectionFlags);
#endif
	}

} // end namespace
