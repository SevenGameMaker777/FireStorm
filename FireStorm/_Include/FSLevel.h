// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FSLogger.h"
#include "FSEventHandler.h"
#include "FSObjectfactory.h"
#include "FSPhysX.h"

// unclutter the global namespace
namespace FS
{
	// forward declarations
	typedef class FSApplication FSApplication;
	typedef class FSEngine FSEngine;

	/*
	responsible for the creation, management and destruction of all game objects
	*/
	class FSLevel : public FSEventHandler
	{

	private: // private access
			 // class variable declarations
		int						m_Id;					// simple id variable
		FSApplication*			m_Application;			// pointer to the application
		FSEngine*				m_Engine;				// poitner to the engine
		ISceneManager*			m_Smgr;					// pointer to our own scenemanager
		ICameraSceneNode*		m_Camera;				// pointer to camera
		FSObjectFactory*		m_ObjectFactory;		// pointer to obejct factory
		bool					m_UseFog;				// set to true to use fog in scene
		bool					m_UseLight;				// set to true to use light in scene
		bool					m_UseEditorVisuals;		// set to true to show object editor visuals
		SColorf					m_AmbientLight;			// scene ambient lighting
		FS_FogParams			m_FogParams;			// scene fog parameters
		float					m_ElapsedTime;			// elapsedtime since last frame
		ISceneCollisionManager* m_CollMan;				// pointer to irrlicht collision manager
		bool					m_AutoRender;			// toggle rendering call
		bool					m_ShowCameraPosition;	// display the current camera position in the main window
		FSPhysXWorld*			m_PhysXWorld;			// physx world 
		bool					m_UseShadows;				// default shadow rendering

	public:
		int						getId() { return m_Id; }	// simple id variable
		FSApplication*			getApp() { return m_Application; }	// pointer to the application
		FSEngine*				getEngine() { return m_Engine; }	// pointer to the engine
		ISceneManager*			getSmgr() { return m_Smgr; }	// pointer to our own scenemanager
		ICameraSceneNode*		getCamera() { return m_Camera; }	// pointer to camera
		FSObjectFactory*		getObjectFactory() { return m_ObjectFactory; }	// pointer to obejct factory
		bool					getUseFog() { return m_UseFog; }	// set to true to use fog in scene
		bool					getUseLight() { return m_UseLight; }	// set to true to use light in scene
		bool					getUseEditorVisuals() { return m_UseEditorVisuals; }	// set to true to show object editor visuals
		SColorf					getAmbientLight() { return m_AmbientLight; }	// scene ambient lighting
		FS_FogParams			getFogParams() { return m_FogParams; }	// scene fog parameters
		float					getElapsedTime() { return m_ElapsedTime; }	// elapsedtime since last frame
		ISceneCollisionManager* getCollisionManager() { return m_CollMan; }	// pointer to irrlicht collision manager
		bool					getAutoRender() { return m_AutoRender; }	//  toggle auto rendering during frame calls
		bool					getShowCameraPosition() { return m_ShowCameraPosition; }	//  display the current camera position in the main window
		FSPhysXWorld*			getPhysXWorld() { return m_PhysXWorld; }	// physx world 
		bool					getUseShadows() { return m_UseShadows; }			// default shadow rendering

	public:
		// get object pointer
		FSObject* getObjectPointer(int id, bool deep) { return getObjectFactory()->getObjectPointer(id, deep); }

		virtual void setUseShadows(bool v);			// default shadow rendering

		// set the scene lighting / fog
		virtual void setAmbientLight(SColorf color);
		virtual void setFogParams(FS_FogParams params);

		// set the level id
		virtual void setId(int v);

		// set the active camera
		virtual void setCamera(ICameraSceneNode* v);

		// get the distance between two obejcts
		float		 getDistanceBetween(FSObject* obj1, FSObject* obj2);

		void setShowCameraPosition(bool v) { m_ShowCameraPosition = v; }

	private:
		//! Returns the scene node, which is currently visible at the given
		//! screen coordinates, viewed from the currently active camera.
		ISceneNode* getSceneNodeFromScreenCoordinatesBB(const core::position2d<s32>& pos, s32 idBitMask, bool noDebugObjects, scene::ISceneNode* root, long ot);

		//! Returns the nearest scene node which collides with a 3d ray and
		//! which id matches a bitmask.
		ISceneNode* getSceneNodeFromRayBB(const core::line3d<f32>& ray, s32 idBitMask, bool noDebugObjects, scene::ISceneNode* root, long ot);

		//! recursive method for going through all scene nodes
		void getPickedNodeBB(ISceneNode* root, core::line3df& ray, s32 bits, bool noDebugObjects, f32& outbestdistance, ISceneNode*& outbestnode, long ot);

	public:	// public access
			// class constructor / destructor
			// use initialize / create / cleanup for better error handling
		FSLevel() : FSEventHandler() { /* do nothing constructor */ FS_LOG(FSL_DEBUG, "FSLevel::FSLevel()"); }
		virtual ~FSLevel() { /* do nothing destructor */  FS_LOG(FSL_DEBUG, "FSLevel::~FSLevel()"); }

		// set all variables to a known value
		virtual void initialize();

		// allows for better error handling
		virtual bool create(FSApplication* app, int id);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

		// clear all items from the level
		virtual void clear();

		// called each frame
		virtual void preFrame(const float & elapsedtime, bool forceupdate);
		virtual void frame(const float & elapsedtime, bool forceupdate);
		virtual void postFrame(const float & elapsedtime, bool forceupdate);

		// let the level render the scene
		virtual void render();

		// load / unload the level from memory
		virtual void load();
		virtual void unload();

		// save / load disk functions
		virtual bool saveToDisk(stringc filename);
		virtual bool loadFromDisk(stringc filename);

		// send a message through the system
		virtual void sendMessage(FSMT type, FSMT type2, int source, int dest, float rtime, float dtime, stringc data) {}

		// receive a message from the system
		virtual bool receiveMessage(FSObjectMessage* m) { return false; }

		// event handlers
		virtual bool onScreenResized(const SEvent & e);

		// runt hrought he list of objects and set these variables
		virtual void setUseLight(bool value);
		virtual void setUseFog(bool value);
		virtual void setUseEditorVisuals(bool value);

		// toggle auto render during frame calls (example, FSGUI_LevelViewer)
		void setAutoRender(bool v) { m_AutoRender = v; }
	public:
		// unsafe object pointers
		IrrlichtDevice*		getDevice();
		IGUIEnvironment*	getGui();
		IVideoDriver*		getDriver();
		stringc				getDirectory(stringc dirName);


		// editor variables / functions
	private:
		int		m_SelectedObject;
		int		m_PreSelectedObject;
		long	m_ObjectSelectType;
		long	m_EditorMode;
	public:
		// select an object from screen coordinates
		FSObject*	selectObjectPhysX(ICameraSceneNode* cam, int screenX, int screenY, long ot);
		FSObject*	selectObjectBySceneNode(int screenX, int screenY, long ot);
		ISceneNode* selectNode(int screenX, int screenY, long objectflags);
		FSObject*	getObjectFromNode(ISceneNode* node);
		void		setSelectedObject(int id);
		void		setPreSelectedObject(int id);
		int			getSelectedObject();
		FSObject*	getSelectedObjectPointer();
		void		setObjectSelectType(long t) { m_ObjectSelectType = t; }
		long		getObjectSelectType() { return m_ObjectSelectType; }
		long		getEditorMode() { return m_EditorMode; }
		void		setEditorMode(long mode) { m_EditorMode = mode; }
	};

} // end namespace