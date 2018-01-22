#pragma once
#include "FSEngine.h"
#include "CGUI_ColorPicker.h"
#include "FSGUI_FilePicker.h"

namespace FS
{
	class CPropertyGrid_Base : public IGUIElement
	{
	private:
		FS_VARIABLE_GET(IrrlichtDevice*, Device);
		FS_VARIABLE_SETGET(int, Depth);
		FS_VARIABLE_GET(IGUIButton*, MinimizeButton);
		FS_VARIABLE_GET(IGUIStaticText*, Name);
		FS_VARIABLE_GET(IGUIEditBox*, ValueEdit);
		FS_VARIABLE_GET(stringc, Value);
		FS_VARIABLE_GET(bool, Minimized);
		FS_VARIABLE_GET(rect<s32>, Rect);
		FS_VARIABLE_SETGET(SColor, Color);
	public:
		CPropertyGrid_Base(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, rect<s32> r, stringw name );
		virtual ~CPropertyGrid_Base();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual void draw();
		virtual bool OnEvent(const SEvent &e);
		virtual void setMinimized(bool v);
	};

	class CPropertyGrid_String : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(IGUIStaticText*, Text);
		FS_VARIABLE_GET(IGUIEditBox*, Box);
	public:
		CPropertyGrid_String(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, stringc v);
		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};

	class CPropertyGrid_Int : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(IGUIStaticText*, Text);
		FS_VARIABLE_GET(IGUISpinBox*, Spin);
	public:
		CPropertyGrid_Int(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, int v);

		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};

	class CPropertyGrid_Float : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(IGUIStaticText*, Text);
		FS_VARIABLE_GET(IGUISpinBox*, Spin);
	public:
		CPropertyGrid_Float(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, float v);

		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};


	class CPropertyGrid_Bool : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(IGUIStaticText*, Text);
		FS_VARIABLE_GET(IGUICheckBox*, Box);
	public:
		CPropertyGrid_Bool(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, bool v);
		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};

	class CPropertyGrid_Vec3d : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(IGUIStaticText*, TextX);
		FS_VARIABLE_GET(IGUIStaticText*, TextY);
		FS_VARIABLE_GET(IGUIStaticText*, TextZ);
		FS_VARIABLE_GET(IGUISpinBox*, SpinX);
		FS_VARIABLE_GET(IGUISpinBox*, SpinY);
		FS_VARIABLE_GET(IGUISpinBox*, SpinZ);
	public:
		CPropertyGrid_Vec3d(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, vector3df v);
		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};

	class CPropertyGrid_SColor : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(IGUIColorPicker*, ColorControl);
	public:
		CPropertyGrid_SColor(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, SColor v);
		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};

	class CPropertyGrid_SColorf : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(IGUIColorPicker*, ColorControl);
	public:
		CPropertyGrid_SColorf(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, SColorf v);
		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};

	class CPropertyGrid_Filename : public CPropertyGrid_Base
	{
	private:
		FS_VARIABLE_GET(FSGUI_FileOpenDialog*, FileControl);
	public:
		CPropertyGrid_Filename(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, stringc v);
		virtual stringc getValue();
		virtual void setValue(stringc v);
		virtual void calculateLayout();
		virtual bool OnEvent(const SEvent &e);
	};

	class CPropertyGrid : public IGUIElement
	{
	public:
		list<CPropertyGrid_Base*> m_Properties;
		FS_VARIABLE_SETGET(bool, AlternateColor);
	public:
		CPropertyGrid(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, rect<s32> r);
		virtual ~CPropertyGrid();
		void clear();
		int getTotalHeight();
		void minimizeAll();
		void maximizeAll();
		CPropertyGrid_Base* getPropertyById(int id);
		virtual void calculateLayout(int xoffset, int yoffset);
		virtual void draw();
		virtual bool OnEvent(const SEvent &e);
	};

} // end namespace