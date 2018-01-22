// include this file only once
#pragma once

// include the needed header files
#include "FSGUI_Ribbonbar.h"
#include "FSGUI_Systembar.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_FilePicker.h"
#include "FSGUI_ModelViewer.h"
#include "FSGUI_RibbonbarImage.h"

#define FSID_MODELVIEWER 100
#define FSID_IMAGERIBBON 101

namespace FS
{
	/*
	a window
	*/
	class FSGUI_AssetViewer : public FSGUI_Window
	{
	public:

		FSGUI_FilePicker* m_FilePicker;
		FSGUI_ModelViewer* m_Viewer;
		FSGUI_RibbonbarImage*	m_RibbonImage;

		// simple constructor that auto sets the FSGUI_LAYOUT::FSGUI_BORDER flag and layout
		FSGUI_AssetViewer(FSGUI_Desktop* desktop, IGUIEnvironment* env, FSGUI_Window* FSparent, int id, rect<s32> r, long flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minWidth, int minHeight)
			: FSGUI_Window(desktop,env,FSparent, id, r, flags, ls, text,minWidth, minHeight)
		{
			createGUI();
		}

		void createGUI()
		{
			m_RibbonImage = new FSGUI_RibbonbarImage(getDesktop(), getGui(), this, FSID_IMAGERIBBON, rect<s32>(0, 0, 80, 80), WS_VISIBLE | WS_THICKFRAME | WS_BORDER | WS_CAPTION, LS_LEFTFILL, true, 4);
			m_Viewer = new FSGUI_ModelViewer(getDesktop(),  Environment, this, FSID_MODELVIEWER,	rect<s32>(0, 0, 100, 220), WS_VISIBLE | WS_THICKFRAME , LS_TOPFILL, L"Model Viewer", 10, 10);
			m_FilePicker = new FSGUI_FilePicker(getDesktop(), Environment, this, FSID_FILEPICKER, rect<s32>(0, 0, 100, 400), WS_VISIBLE | WS_THICKFRAME, LS_REST,L"FilePicker", 10, 10, FPT_NONE);
			m_RibbonImage->setDirectory(getDirectory("MediaDirectory"));
		}

		virtual bool onDirectorySelected(const SEvent &e) 
		{ 
			m_RibbonImage->setDirectory(e.GUIEvent.Caller->getText());
			return false; 
		}

		virtual bool onFileSelected(const SEvent& e)
		{
			switch (m_FilePicker->getFilePickerType())
			{
			case FPT_NONE: return true;
			case FPT_STATICMODEL:
			case FPT_ANIMATEDMODEL:
			{
				if (m_Viewer)
				{
					m_Viewer->setFilename(m_FilePicker->getFileName());
				}
				return true;
			} break;
			}
			return FSGUI_Window::onFileSelected(e);
		}
	};

} // end namespace