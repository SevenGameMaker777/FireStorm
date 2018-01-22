// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FSLogger.h"
#include "FSEventHandler.h"

// unclutter the global namespace
namespace FS
{
	// forward declarations
	typedef class FSApplication FSApplication;

	/*
	*/
	class FSEngine : public FSEventHandler
	{
	private:
		// class variables
		FSApplication* m_App;
		IrrlichtDevice* m_Device;
		IGUIEnvironment* m_Gui;
		IVideoDriver* m_Driver;
		ISceneManager* m_Smgr;
		SIrrlichtCreationParameters m_Params;		// irrlicht engine creation paramaeters
		rect<s32>					m_WindowInfo;	// window information
		rect<s32>					m_WindowPos;	// window position
		stringc						m_Filename;		// engine creation parameters filename
		HWND						m_hWnd;			// main rendering window handle
		stringc						m_WindowTitle;	// the Gates window text
		SColor						m_BackgroundColor;	// color to clear the screen to
		bool						m_RenderSmgr;	// toggle rendering the main engine smgr

		ISoundEngine* m_SoundEngine;

	public:
		// setter functions
		void setWindowTitle(stringc wt) { m_WindowTitle = wt; }
		void setBackgroundColor(SColor c);
		void setRenderSmgr(bool v);	// toggle rendering the main engine smgr

	public:
		// getter functions
		FSApplication*		getApp();
		IrrlichtDevice*		getDevice();
		IVideoDriver*		getDriver();
		ISceneManager*		getSmgr();
		IGUIEnvironment*	getGui();
		stringc				getDirectory(stringc dirName);	// get a relative directory path name
		SColor				getBackgroundColor();
		bool				getRenderSmgr();	// toggle rendering the main engine smgr
		ISoundEngine*		getSoundEngine();

		void playSound(stringc filename, bool loop = false);

	public:
		// class constructor / destructor (use initialize(), create() and cleanup() )
		FSEngine() { FS_LOG(FSL_DEBUG, "FSEngine::FSEngine()"); }
		~FSEngine() { FS_LOG(FSL_DEBUG, "FSEngine::~FSEngine()"); }

		// set all variables to a known value
		virtual void initialize();

		// dual creation allows for better error handling
		virtual bool create(FSApplication* app, stringc filename = FS_DEFAULT, HWND hwnd = 0);
		virtual bool create(FSApplication* app, E_DEVICE_TYPE DeviceType, E_DRIVER_TYPE DriverType, dimension2d<u32> WindowSize,
			position2di WindowPosition, u8 Bits, u8 ZBufferBits, bool Fullscreen, bool Stencilbuffer,
			bool Vsync, u8 AntiAlias, bool HandleSRGB, bool WithAlphaChannel, bool Doublebuffer,
			bool IgnoreInput, bool Stereobuffer, bool HighPrecisionFPU, IEventReceiver* EventReceiver, void* WindowId,
			ELOG_LEVEL LoggingLevel, u32 DisplayAdapter, bool DriverMultithreaded, bool UsePerformanceTimer, rect<s32> windowinfo);

		// cleanup whatever memory mess we made
		virtual bool cleanup();

		// class frame functions
		virtual void preFrame(const float & elapsedtime);
		virtual void frame(const float & elapsedtime);
		virtual void postFrame(const float & elapsedtime);

	public:
		rect<s32> getScreenRect();						// get a normalized screen rectangle
		virtual bool onScreenResized(const SEvent & e);	// someone resized the main window
		virtual bool saveToDisk(stringc filename);		// save the engine creation parameters to disk	
		virtual bool loadFromDisk(stringc filename);	// load the engine creation parameters from disk

		void setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin);

	private:
		virtual bool postCreate();
	};

} // end namespace
