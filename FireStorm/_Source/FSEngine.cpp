// include the needed header files
#include "FSEngine.h"
#include "FSLevel.h"
#include "FSLevelManager.h"
#include "FSLogger.h"
#include "FSApplication.h"

// unclutter the global namespace
namespace FS
{
	// unsafe access to variables (no validation checks)
	IrrlichtDevice*		FSEngine::getDevice() { return m_Device; }
	IVideoDriver*		FSEngine::getDriver() { return m_Driver; }
	ISceneManager*		FSEngine::getSmgr() { return m_Smgr; }
	IGUIEnvironment*	FSEngine::getGui() { return m_Gui; }
	FSApplication*		FSEngine::getApp() { return m_App; }
	stringc				FSEngine::getDirectory(stringc name) { return getApp()->getDirectory(name); }
	SColor				FSEngine::getBackgroundColor() { return m_BackgroundColor; }
	bool				FSEngine::getRenderSmgr() { return m_RenderSmgr; }	// toggle rendering the main engine smgr
	ISoundEngine*		FSEngine::getSoundEngine() { return m_SoundEngine; }

	void FSEngine::setBackgroundColor(SColor c) { m_BackgroundColor = c; }
	void FSEngine::setRenderSmgr(bool v) { m_RenderSmgr = v; } 

	void FSEngine::initialize()
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSEngine::initialize()");

		// set all variables to a known value
		m_App = 0;
		m_Device = 0;
		m_Driver = 0;
		m_Gui = 0;
		m_Smgr = 0;
		m_hWnd = 0;
		m_BackgroundColor = SColor(0, 0, 0, 0);
		m_RenderSmgr = true;
		m_SoundEngine = 0;

		// setup the default params object
		m_Filename = "engineparams.xml";
		m_Params.DriverType = EDT_DIRECT3D9;
		m_Params.Fullscreen = false;
		m_Params.WindowSize = dimension2d<u32>(800, 600);
		m_WindowInfo = rect<s32>(0, 0, 800, 600);
		m_WindowPos = rect<s32>(0, 0, 800, 600);
		m_WindowTitle = FS_DEFAULT;

		// setup the eventhandler id and name
		FSEventHandler::setEventHandlerId(ID_ENGINE);
		FSEventHandler::setEventHandlerName("ENGINE");
	}

	// allows for better error handling
	bool FSEngine::create(FSApplication* app, stringc filename, HWND hwnd)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSEngine::create()");

		// remember this object
		m_App = app;


		if (filename != FS_DEFAULT) m_Filename = filename;

		// load the params from file
		if (!loadFromDisk(m_Filename)) { FS_LOG(FSL_WARNING, "FSEngine::create() failed to load params from disk"); }

		// set the hwnd of the engine objects
		m_Params.WindowId = hwnd;
		m_hWnd = hwnd;

		// create the irrlicht objects
		if (!postCreate()) { FS_LOG(FSL_ERROR, "FSEngine::postCreation failed"); return false; }

		setSkinTransparency(255, getGui()->getSkin());

		// everything went fine
		return true;
	}

	bool FSEngine::create(FSApplication* app, E_DEVICE_TYPE DeviceType, E_DRIVER_TYPE DriverType, dimension2d<u32> WindowSize,
		position2di WindowPosition, u8 Bits, u8 ZBufferBits, bool Fullscreen, bool Stencilbuffer,
		bool Vsync, u8 AntiAlias, bool HandleSRGB, bool WithAlphaChannel, bool Doublebuffer,
		bool IgnoreInput, bool Stereobuffer, bool HighPrecisionFPU, IEventReceiver* EventReceiver, void* WindowId,
		ELOG_LEVEL LoggingLevel, u32 DisplayAdapter, bool DriverMultithreaded, bool UsePerformanceTimer, rect<s32> windowinfo)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSEngine::create()");

		// remember these
		m_App = app;

		// remember these values
		m_Params.DeviceType = DeviceType;
		m_Params.DriverType = DriverType;
		m_Params.WindowSize = WindowSize;
		m_Params.Bits = Bits;
		m_Params.ZBufferBits = ZBufferBits;
		m_Params.Fullscreen = Fullscreen;
		m_Params.Stencilbuffer = Stencilbuffer;
		m_Params.Vsync = Vsync;
		m_Params.AntiAlias = AntiAlias;
		m_Params.HandleSRGB = HandleSRGB;
		m_Params.WithAlphaChannel = WithAlphaChannel;
		m_Params.Doublebuffer = Doublebuffer;
		m_Params.IgnoreInput = IgnoreInput;
		m_Params.Stereobuffer = Stereobuffer;
		m_Params.HighPrecisionFPU = HighPrecisionFPU;
		m_Params.EventReceiver = EventReceiver;
		m_Params.WindowId = WindowId;
		m_Params.LoggingLevel = LoggingLevel;
		m_Params.DisplayAdapter = DisplayAdapter;
		m_Params.DriverMultithreaded = DriverMultithreaded;
		m_Params.UsePerformanceTimer = UsePerformanceTimer;
		//m_WindowInfo = windowinfo;

		// create the irrlicht objects
		if (!postCreate()) { FS_LOG(FSL_ERROR, "FSEngine::postCreation failed"); return false; }

		// everything went fine
		return true;
	}

	// allows for better error handling
	bool FSEngine::postCreate()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSEngine::postCreate()");

		// create the irrlicht device
		m_Device = createDeviceEx(m_Params);
		if (!m_Device) { FS_LOG(FSL_ERROR, "Unable to create Irrlicht device"); return false; }

		// get pointers to all of the irrlicht objects
		m_Driver = getDevice()->getVideoDriver();
		m_Smgr = getDevice()->getSceneManager();
		m_Gui = getDevice()->getGUIEnvironment();

		// make the window resizable
		getDevice()->setResizable(!m_Params.Fullscreen);

		// set up the driver parameters
		getDriver()->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);

		// route all events to the application
		getDevice()->setEventReceiver(getApp());

		// set a nicer font
		IGUISkin* skin = getGui()->getSkin();
		IGUIFont* font = getGui()->getFont(MEDIAPATH("fonthaettenschweiler.bmp"));
//		IGUIFont* font = getGui()->getFont(MEDIAPATH("myfont0.bmp"));
		if (font) skin->setFont(font);

		if (!m_hWnd)
		{
			// get the window handle
			switch (getDriver()->getDriverType())
			{
			case EDT_DIRECT3D9: m_hWnd = (HWND)getDriver()->getExposedVideoData().D3D9.HWnd; break;
			case EDT_OPENGL: m_hWnd = (HWND)getDriver()->getExposedVideoData().OpenGLWin32.HWnd; break;
			}

			if (!m_Params.Fullscreen)
			{
				// set the window position and size
				SetWindowPos(m_hWnd, 0, m_WindowPos.UpperLeftCorner.X, m_WindowPos.UpperLeftCorner.Y,
					m_WindowPos.LowerRightCorner.X - m_WindowPos.UpperLeftCorner.X,
					m_WindowPos.LowerRightCorner.Y - m_WindowPos.UpperLeftCorner.Y,
					WM_SHOWWINDOW);
			}
		}

		m_SoundEngine = createIrrKlangDevice();

		// error check on sound engine
		if (!m_SoundEngine) FS_LOG(FSL_WARNING, "Could not create sound engine");

		// everything went fine
		return true;
	}

	bool FSEngine::cleanup()
	{
		// log this function call
		FS_LOG(FSL_DEBUG, "FSEngine::cleanup()");

		if (m_SoundEngine) m_SoundEngine->drop(); m_SoundEngine = 0;

		// forget this
		m_App = 0;

		// always return false from a cleanup function
		return false;
	}

	void FSEngine::preFrame(const float & elapsedtime)
	{
		// check if someone moved / resized the main window
		if (!m_Params.Fullscreen)
		{
			RECT r;
			GetClientRect(m_hWnd, &r);
			if ((r.left != m_WindowInfo.UpperLeftCorner.X) ||
				(r.top != m_WindowInfo.UpperLeftCorner.Y) ||
				(r.right != m_WindowInfo.LowerRightCorner.X) ||
				(r.bottom != m_WindowInfo.LowerRightCorner.Y))
			{
				// notify the application
				SEvent e;
				e.EventType = EET_USER_EVENT;
				e.UserEvent.UserData1 = EVT_SCREENRESIZED;
				OnEvent(e);
				getDevice()->postEventFromUser(e);
			}
		}

		// begin the render
		getDriver()->beginScene(true, true, getBackgroundColor());
	}

	void FSEngine::frame(const float & elapsedtime)
	{
		// render the scene and gui
		if (getRenderSmgr()) getSmgr()->drawAll();
		getGui()->drawAll();
	}

	void FSEngine::postFrame(const float & elapsedtime)
	{
		// flip the buffers
		getDriver()->endScene();

		// show the framerate in the window titlebar
		if (!m_Params.Fullscreen)
		{
			if (m_WindowTitle != FS_DEFAULT) getDevice()->setWindowCaption(stringw(m_WindowTitle).c_str());
			else getDevice()->setWindowCaption(stringw(getDriver()->getFPS()).c_str());
		}
	}

	// get a normalized screen rectangle
	rect<s32> FSEngine::getScreenRect()
	{
		return m_WindowInfo;
	}

	// someone resized the main window
	bool FSEngine::onScreenResized(const SEvent & e)
	{
		RECT r;
		GetWindowRect(m_hWnd, &r);
		m_WindowPos = rect<s32>(r.left, r.top, r.right, r.bottom);

		GetClientRect(m_hWnd, &r);

		// remember the currect position / size 
		m_WindowInfo = rect<s32>(r.left, r.top, r.right, r.bottom);

		m_WindowInfo.UpperLeftCorner.set(0, 0);
		m_WindowInfo.LowerRightCorner.set(getDriver()->getScreenSize().Width, getDriver()->getScreenSize().Height);

		// save to disk
		saveToDisk(m_Filename);

		// call the base class
		return FSEventHandler::onScreenResized(e);
	}

	bool FSEngine::saveToDisk(stringc filename)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%s\n %s", "FSEngine::saveParamsToDisk()", getApp()->getDirectory("GameDataDirectory") + filename.c_str());

		// remember this
		m_Filename = filename;

		// create a null device, setup the attributes and then save to disk
		IrrlichtDevice* device = createDevice(EDT_NULL);
		IXMLWriter* writer = device->getFileSystem()->createXMLWriter(getApp()->getDirectory("GameDataDirectory") + filename);
		if (!writer) { FS_LOG(FSL_DEBUG, "Warning! unable to create engine param file %s", filename.c_str()); return false; }

		// write the xml header to the file
		writer->writeXMLHeader();

		writer->writeElement(L"ENGINE_PARAMS", true);
		writer->writeLineBreak();

		// create an attributes structure
		IAttributes* attr = device->getFileSystem()->createEmptyAttributes();

		if (attr)
		{
			// add the variables to the attributes structure
			switch (m_Params.DriverType)
			{
			case EDT_DIRECT3D9: attr->addString("DRIVERTYPE", "EDT_DIRECT3D9"); break;
			case EDT_OPENGL: attr->addString("DRIVERTYPE", "EDT_OPENGL"); break;
			}
			attr->addInt("ANTIALIAS", m_Params.AntiAlias);
			attr->addInt("BITS", m_Params.Bits);
			attr->addInt("DISPLAYADAPTER", m_Params.DisplayAdapter);
			attr->addBool("DOUBLEBUFFER", m_Params.Doublebuffer);
			attr->addBool("DRIVERMULTITHREADED", m_Params.DriverMultithreaded);
			attr->addBool("FULLSCREEN", m_Params.Fullscreen);
			attr->addBool("HANDLESRGB", m_Params.HandleSRGB);
			attr->addBool("HIGHPRECISIONFPU", m_Params.HighPrecisionFPU);
			attr->addBool("IGNOREINPUT", m_Params.IgnoreInput);
			attr->addBool("STENCILBUFFER", m_Params.Stencilbuffer);
			attr->addBool("STEREOBUFFER", m_Params.Stereobuffer);
			attr->addBool("PORFORMANCETIMER", m_Params.UsePerformanceTimer);
			attr->addBool("VSYNC", m_Params.Vsync);
			attr->addDimension2d("WINDOWSIZE", m_Params.WindowSize);
			attr->addRect("WINDOWPOS", m_WindowPos);

			attr->addRect("WINDOWINFO", m_WindowInfo);
			attr->addBool("WITHALPHACHANNEL", m_Params.WithAlphaChannel);
			attr->addInt("ZBUFFERBITS", m_Params.ZBufferBits);

			// if thee are attributes, write them to the file
			if (attr->getAttributeCount() != 0)	attr->write(writer);

			// close the file writing
			writer->writeLineBreak();
			writer->writeClosingTag(L"ENGINE_PARAMS");

			// drop all of the pointers
			attr->drop();
			writer->drop();
			device->drop();

			// everything went fine
			return true;
		}

		// something went wrong
		return false;
	}

	bool FSEngine::loadFromDisk(stringc filename)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "%s\n %s", "FSEngine::loadFromDisk()", getApp()->getDirectory("GameDataDirectory") + filename.c_str());

		if (filename == FS_DEFAULT) return true;

		// create a null device, create the attributes and then load from disk
		IrrlichtDevice* device = createDevice(EDT_NULL);
		IXMLReader* reader = device->getFileSystem()->createXMLReader(getApp()->getDirectory("GameDataDirectory") + filename);
		if (!reader) { FS_LOG(FSL_DEBUG, "Warning! unable to load engine param file %s", filename.c_str()); return false; }

		// create an attributes structure
		IAttributes* attr = device->getFileSystem()->createEmptyAttributes();

		while (reader->read())
			switch (reader->getNodeType())
			{
			case EXN_ELEMENT:

				attr->read(reader, false);

				// add the variables to the attributes structure
				stringc tempString = attr->getAttributeAsString("DRIVERTYPE");
				if (tempString == "EDT_DIRECT3D9") m_Params.DriverType = EDT_DIRECT3D9;
				if (tempString == "EDT_OPENGL") m_Params.DriverType = EDT_OPENGL;

				m_Params.AntiAlias = attr->getAttributeAsInt("ANTIALIAS");
				m_Params.Bits = attr->getAttributeAsInt("BITS");
				m_Params.DisplayAdapter = attr->getAttributeAsBool("DISPLAYADAPTER");
				m_Params.Doublebuffer = attr->getAttributeAsBool("DOUBLEBUFFER");
				m_Params.DriverMultithreaded = attr->getAttributeAsBool("DRIVERMULTITHREADED");
				m_Params.Fullscreen = attr->getAttributeAsBool("FULLSCREEN");
				m_Params.HandleSRGB = attr->getAttributeAsBool("HANDLESRGB");
				m_Params.HighPrecisionFPU = attr->getAttributeAsBool("HIGHPRECISIONFPU");
				m_Params.IgnoreInput = attr->getAttributeAsBool("IGNOREINPUT");
				m_Params.Stencilbuffer = attr->getAttributeAsBool("STENCILBUFFER");
				m_Params.Stereobuffer = attr->getAttributeAsBool("STEREOBUFFER");
				m_Params.UsePerformanceTimer = attr->getAttributeAsBool("PORFORMANCETIMER");
				m_Params.Vsync = attr->getAttributeAsBool("VSYNC");
				m_Params.WindowSize = attr->getAttributeAsDimension2d("WINDOWSIZE");
				m_Params.WithAlphaChannel = attr->getAttributeAsBool("WITHALPHACHANNEL");
				m_Params.ZBufferBits = attr->getAttributeAsInt("ZBUFFERBITS");
				m_WindowPos = attr->getAttributeAsRect("WINDOWPOS");
				m_WindowInfo = attr->getAttributeAsRect("WINDOWINFO");
				break;
			}


		// drop all of the pointers
		attr->drop();
		reader->drop();
		device->drop();

		// everything went fine
		return true;
	}

	void FSEngine::setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin)
	{
		if (!skin) skin = getGui()->getSkin();

		for (s32 i = 0; i<irr::gui::EGDC_COUNT; ++i)
		{
			video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
			col.setAlpha(alpha);
			skin->setColor((EGUI_DEFAULT_COLOR)i, col);
		}
	}

	void FSEngine::playSound(stringc filename, bool loop)
	{
		if (m_SoundEngine) m_SoundEngine->play2D(filename.c_str(),loop);
	}

} // end namespace

