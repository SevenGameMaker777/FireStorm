#include "CPropertyGrid.h"
#include "CGUIFileOpenDialog.h"
#include "CGUI_ColorPicker.h"

namespace FS
{
	#define HEIGHT 20
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  CPropertyGrid_Base
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CPropertyGrid_Base::CPropertyGrid_Base(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, rect<s32> r, stringw name):
		IGUIElement(EGUIET_ELEMENT, gui, parent, id, r)
	{
		m_Device = device;
		m_Rect = r;
		m_Value = "";
		m_Name = gui->addStaticText(name.c_str(), rect<s32>(0, 0, HEIGHT, HEIGHT), true, false, this, -1, true);
		m_ValueEdit = gui->addEditBox(L"", rect<s32>(0, 0, HEIGHT, HEIGHT), true, this, -1);
		m_Minimized = false;
		m_MinimizeButton = gui->addButton(rect<s32>(2, 4, 14, 24), this, EET_PROPERTYGRID_MINIMIZE, L"");
		m_MinimizeButton->setImage(Environment->getVideoDriver()->getTexture("C:\\Programming\\CobbleStones\\_Bin\\media\\_Assets\\GUI\\Toolbar\\expand.jpg"));
	}

	CPropertyGrid_Base::~CPropertyGrid_Base()
	{
	}

	void CPropertyGrid_Base::setMinimized(bool v)
	{
		m_Minimized = v;
		if (m_Minimized)
		{
			rect<s32> r = getAbsolutePosition();
			r.LowerRightCorner -= r.UpperLeftCorner;
			r.UpperLeftCorner -= r.UpperLeftCorner;
			r.LowerRightCorner.Y = r.UpperLeftCorner.Y + HEIGHT + 8;
			setRelativePosition(r);
			updateAbsolutePosition();
		}
		else
		{
			setRelativePosition(m_Rect);
			updateAbsolutePosition();
		}
		if (getMinimized()) m_MinimizeButton->setImage(Environment->getVideoDriver()->getTexture("C:\\Programming\\CobbleStones\\_Bin\\media\\_Assets\\GUI\\Toolbar\\minimize.jpg"));
		else m_MinimizeButton->setImage(Environment->getVideoDriver()->getTexture("C:\\Programming\\CobbleStones\\_Bin\\media\\_Assets\\GUI\\Toolbar\\expand.jpg"));

		SEvent e;
		e.EventType = EEVENT_TYPE::EET_USER_EVENT;
		e.UserEvent.UserData1 = EET_PROPERTYGRID_MINIMIZE;
		getParent()->OnEvent(e);
	}

	void CPropertyGrid_Base::setValue(stringc v)
	{
		m_ValueEdit->setText(stringw(v).c_str());

		SEvent e1;
		e1.EventType = EEVENT_TYPE::EET_USER_EVENT;
		e1.UserEvent.UserData1 = EET_PROPERTYGRID_PROPERTYCHANGED;
		e1.UserEvent.UserData2 = getID();
		getParent()->OnEvent(e1);
	}

	void CPropertyGrid_Base::draw()
	{
		rect<s32> r = getAbsolutePosition();
		rect<s32> cr = getAbsoluteClippingRect();
		Environment->getVideoDriver()->draw2DRectangle(getColor(), r, &cr);
		IGUIElement::draw();
	}

	void CPropertyGrid_Base::calculateLayout()
	{
		rect<s32> r = getAbsolutePosition();
		r.LowerRightCorner -= r.UpperLeftCorner;
		r.UpperLeftCorner -= r.UpperLeftCorner;

		r.UpperLeftCorner.Y += 4;
		r.UpperLeftCorner.X += 16;
		rect<s32> r2 = r;

		r2.LowerRightCorner.X = 120;
		r2.LowerRightCorner.Y = r.UpperLeftCorner.Y + HEIGHT;
		m_Name->setRelativePosition(r2);
		m_Name->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 2;
		r2.LowerRightCorner.X = r.LowerRightCorner.X;
		m_ValueEdit->setRelativePosition(r2);
		m_ValueEdit->updateAbsolutePosition();
	}

	bool CPropertyGrid_Base::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_BUTTON_CLICKED:
			{
				switch (e.GUIEvent.Caller->getID())
				{
				case EET_PROPERTYGRID_MINIMIZE: setMinimized(!getMinimized()); break;
				} break;
			} break;
			}
		} break;
		}
		return IGUIElement::OnEvent(e);
	}

	CPropertyGrid_String::CPropertyGrid_String(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, stringc v) :
		CPropertyGrid_Base(device,gui, parent, id, rect<s32>(0, 0, 120, 52), name)
	{
		m_Text = gui->addStaticText(L"String", rect<s32>(0, 0, 120, HEIGHT), true, false, this, 1, true);
		m_Text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		m_Box = gui->addEditBox(L"", rect<s32>(0, 0, 120, HEIGHT), true, this, 1);
		setValue(v);
	}

	stringc CPropertyGrid_String::getValue()
	{
		return m_Box->getText();
	}

	void CPropertyGrid_String::setValue(stringc v)
	{
		m_Box->setText(stringw(v).c_str());
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_String::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();
		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
		rect<s32> r2 = r;

		r2.UpperLeftCorner.Y = r2.LowerRightCorner.Y + 4;
		r2.LowerRightCorner.Y = r2.UpperLeftCorner.Y + HEIGHT;
		r2.UpperLeftCorner.X += 12;
		r2.LowerRightCorner.X = r2.UpperLeftCorner.X + 40;
		m_Text->setRelativePosition(r2);
		m_Text->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 4;
		r2.LowerRightCorner.X = rm.LowerRightCorner.X - 40;
		m_Box->setRelativePosition(r2);
		m_Box->updateAbsolutePosition();
	}

	bool CPropertyGrid_String::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			case EGUI_EVENT_TYPE::EGET_EDITBOX_ENTER:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
				if (e.GUIEvent.Caller == m_Box) setValue(m_Box->getText());
			} break;
			} break;
		} break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}

	CPropertyGrid_Bool::CPropertyGrid_Bool(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, bool v) :
		CPropertyGrid_Base(device,gui, parent, id, rect<s32>(0, 0, 120, 52),name)
	{
		m_Text = gui->addStaticText(L"Bool", rect<s32>(0, 0, 120, HEIGHT), true, false, this, 1, true);
		m_Text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		m_Box = gui->addCheckBox(false,rect<s32>(0, 0, 120, HEIGHT), this, 1,L"");
		setValue(v == 0 ? "0" : "1");
	}

	stringc CPropertyGrid_Bool::getValue()
	{
		return m_Box->isChecked() ? stringc("1") : stringc("0");
	}

	void CPropertyGrid_Bool::setValue(stringc v)
	{
		m_Box->setChecked(v == "0" ? false : true);
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_Bool::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();
		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
		rect<s32> r2 = r;

		r2.UpperLeftCorner.Y = r2.LowerRightCorner.Y + 4;
		r2.LowerRightCorner.Y = r2.UpperLeftCorner.Y + HEIGHT;
		r2.UpperLeftCorner.X += 12;
		r2.LowerRightCorner.X = r2.UpperLeftCorner.X + 40;
		m_Text->setRelativePosition(r2);
		m_Text->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 4;
		r2.LowerRightCorner.X = rm.LowerRightCorner.X - 40;
		m_Box->setRelativePosition(r2);
		m_Box->updateAbsolutePosition();
	}

	bool CPropertyGrid_Bool::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_CHECKBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == m_Box) setValue(stringc(m_Box->isChecked()));
			}break;
			}break;
		}break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}

	CPropertyGrid_Int::CPropertyGrid_Int(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, int v) :
		CPropertyGrid_Base(device, gui, parent, id, rect<s32>(0, 0, 120, 52), name)
	{
		m_Text = gui->addStaticText(L"Integer", rect<s32>(0, 0, 120, HEIGHT), true, false, this, 1, true);
		m_Text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		m_Spin = gui->addSpinBox(L"", rect<s32>(0, 0, 120, HEIGHT), true, this, 1);
		setValue(intToStringc(v));
	}

	stringc CPropertyGrid_Int::getValue()
	{
		return stringc(m_Spin->getValue());
	}

	void CPropertyGrid_Int::setValue(stringc v)
	{
		m_Spin->setValue((float)stringcToInt(v));
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_Int::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();

		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
		rect<s32> r2 = r;

		r2.UpperLeftCorner.Y = r2.LowerRightCorner.Y + 4;
		r2.LowerRightCorner.Y = r2.UpperLeftCorner.Y + HEIGHT;
		r2.UpperLeftCorner.X += 12;
		r2.LowerRightCorner.X = r2.UpperLeftCorner.X + 40;
		m_Text->setRelativePosition(r2);
		m_Text->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 4;
		r2.LowerRightCorner.X = rm.LowerRightCorner.X - 40;
		m_Spin->setRelativePosition(r2);
		m_Spin->updateAbsolutePosition();
	}

	bool CPropertyGrid_Int::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_SPINBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == m_Spin) setValue(stringc(m_Spin->getValue()));
			}break;
			}break;
		}break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}

	CPropertyGrid_Float::CPropertyGrid_Float(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, float v) :
		CPropertyGrid_Base(device, gui, parent, id, rect<s32>(0, 0, 120, 52), name)
	{
		m_Text = gui->addStaticText(L"Float", rect<s32>(0, 0, 120, HEIGHT), true, false, this, 1, true);
		m_Text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		m_Spin = gui->addSpinBox(L"", rect<s32>(0, 0, 120, HEIGHT), true, this, 1);
		setValue(floatToStringc(v));
	}

	stringc CPropertyGrid_Float::getValue()
	{
		return stringc(m_Spin->getValue());
	}

	void CPropertyGrid_Float::setValue(stringc v)
	{
		m_Spin->setValue((float)stringcToFloat(v));
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_Float::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();

		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
		rect<s32> r2 = r;

		r2.UpperLeftCorner.Y = r2.LowerRightCorner.Y + 4;
		r2.LowerRightCorner.Y = r2.UpperLeftCorner.Y + HEIGHT;
		r2.UpperLeftCorner.X += 12;
		r2.LowerRightCorner.X = r2.UpperLeftCorner.X + 40;
		m_Text->setRelativePosition(r2);
		m_Text->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 4;
		r2.LowerRightCorner.X = rm.LowerRightCorner.X - 40;
		m_Spin->setRelativePosition(r2);
		m_Spin->updateAbsolutePosition();
	}

	bool CPropertyGrid_Float::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_SPINBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == m_Spin) setValue(stringc(m_Spin->getValue()));
			}break;
			}break;
		}break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}

	CPropertyGrid_Vec3d::CPropertyGrid_Vec3d(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, vector3df v) :
		CPropertyGrid_Base(device, gui, parent, id, rect<s32>(0, 0, 100, 92), name)
	{
		m_TextX = gui->addStaticText(L"X", rect<s32>(0, 0, 120, HEIGHT), true, false, this, 1, true);
		m_TextX->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		m_SpinX = gui->addSpinBox(L"", rect<s32>(0, 0, 120, HEIGHT), true, this, 1);
		m_TextY = gui->addStaticText(L"Y", rect<s32>(0, 0, 120, HEIGHT), true, false, this, 1, true);
		m_TextY->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		m_SpinY = gui->addSpinBox(L"", rect<s32>(0, 0, 120, HEIGHT), true, this, 1);
		m_TextZ = gui->addStaticText(L"Z", rect<s32>(0, 0, 120, HEIGHT), true, false, this, 1, true);
		m_TextZ->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		m_SpinZ = gui->addSpinBox(L"", rect<s32>(0, 0, 120, HEIGHT), true, this, 1);
		setValue(vector3dfToStringc(v));
	}

	stringc CPropertyGrid_Vec3d::getValue()
	{
		stringw w(m_SpinX->getValue());
		w += L" ";
		w += m_SpinY->getValue();
		w += L" ";
		w += m_SpinZ->getValue();
		return stringc(w);
	}

	void CPropertyGrid_Vec3d::setValue(stringc v)
	{
		vector3df vec = stringcToVector3df(v);
		m_SpinX->setValue(vec.X);
		m_SpinY->setValue(vec.Y);
		m_SpinZ->setValue(vec.Z);
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_Vec3d::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();

		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
		rect<s32> r2 = r;

		r2.UpperLeftCorner.Y = r2.LowerRightCorner.Y + 4;
		r2.LowerRightCorner.Y = r2.UpperLeftCorner.Y + HEIGHT;
		r2.UpperLeftCorner.X += 12;
		r2.LowerRightCorner.X = r2.UpperLeftCorner.X + 20;
		m_TextX->setRelativePosition(r2);
		m_TextX->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 4;
		r2.LowerRightCorner.X = rm.LowerRightCorner.X - 40;
		m_SpinX->setRelativePosition(r2);
		m_SpinX->updateAbsolutePosition();

		r2 = r;
		r2.UpperLeftCorner.Y += m_SpinX->getAbsolutePosition().getHeight();
		r2.LowerRightCorner.Y += m_SpinX->getAbsolutePosition().getHeight();
		r2.UpperLeftCorner.Y = r2.LowerRightCorner.Y + 4;
		r2.LowerRightCorner.Y = r2.UpperLeftCorner.Y + HEIGHT;
		r2.UpperLeftCorner.X += 12;
		r2.LowerRightCorner.X = r2.UpperLeftCorner.X + 20;
		m_TextY->setRelativePosition(r2);
		m_TextY->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 4;
		r2.LowerRightCorner.X = rm.LowerRightCorner.X - 40;
		m_SpinY->setRelativePosition(r2);
		m_SpinY->updateAbsolutePosition();

		r2 = r;
		r2.UpperLeftCorner.Y += m_SpinX->getAbsolutePosition().getHeight() + m_SpinY->getAbsolutePosition().getHeight();
		r2.LowerRightCorner.Y += m_SpinX->getAbsolutePosition().getHeight() + m_SpinY->getAbsolutePosition().getHeight();
		r2.UpperLeftCorner.Y = r2.LowerRightCorner.Y + 4;
		r2.LowerRightCorner.Y = r2.UpperLeftCorner.Y + HEIGHT;
		r2.UpperLeftCorner.X += 12;
		r2.LowerRightCorner.X = r2.UpperLeftCorner.X + 20;
		m_TextZ->setRelativePosition(r2);
		m_TextZ->updateAbsolutePosition();

		r2.UpperLeftCorner.X = r2.LowerRightCorner.X + 4;
		r2.LowerRightCorner.X = rm.LowerRightCorner.X - 40;
		m_SpinZ->setRelativePosition(r2);
		m_SpinZ->updateAbsolutePosition();
	}

	bool CPropertyGrid_Vec3d::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_SPINBOX_CHANGED:
			{
				setValue(getValue());
			}break;

			// WRONG!
			case EGUI_EVENT_TYPE::EGET_TABLE_CHANGED:
			{
				IGUIColorPicker* cp = (IGUIColorPicker*)e.GUIEvent.Caller;
				SColor s = cp->getSelectedColor();
				setValue(SColorToStringc(s));
			} break;
			}break;
		}break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}

	CPropertyGrid_SColor::CPropertyGrid_SColor(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, SColor v) :
		CPropertyGrid_Base(device, gui, parent, id, rect<s32>(0, 0, 100, 192), name)
	{
		m_ColorControl = new IGUIColorPicker(gui, this, 1, v, true);
		setValue(SColorToStringc(v));
	}

	stringc CPropertyGrid_SColor::getValue()
	{
		return SColorToStringc(m_ColorControl->getPickedColor());
	}

	void CPropertyGrid_SColor::setValue(stringc v)
	{
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_SColor::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();
		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r2 = r;
		r2.UpperLeftCorner.X += 4;
		r2.UpperLeftCorner.Y += 24;
		m_ColorControl->setRelativePosition(r2);
		m_ColorControl->updateAbsolutePosition();
	}

	bool CPropertyGrid_SColor::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_SPINBOX_CHANGED:
			{
				setValue(getValue());
			}break;
			case EGET_TABLE_CHANGED:
			{
				// why not?
				setValue(getValue());
				//				return true;
			} break;
			}break;
		}break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}


	CPropertyGrid_SColorf::CPropertyGrid_SColorf(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, SColorf v) :
		CPropertyGrid_Base(device, gui, parent, id, rect<s32>(0, 0, 100, 192), name)
	{
		m_ColorControl = new IGUIColorPicker(gui, this, 1, SColor(), true);
//		SColorf cf(v.getRed() * 255, v.getGreen() / 255, v.getBlue() / 255, v.getAlpha() / 255);
		setValue(SColorfToStringc(v));
	}

	stringc CPropertyGrid_SColorf::getValue()
	{
		SColor c = m_ColorControl->getPickedColor();
		SColorf cf(c);
		printf(" color %d %d %d %d\n", c.getAlpha(), c.getRed(), c.getGreen(), c.getGreen());
		printf(" colorf %f %f %f %f\n", cf.getAlpha(), cf.getRed(), cf.getGreen(), cf.getGreen());
		return SColorfToStringc(cf);
	}

	void CPropertyGrid_SColorf::setValue(stringc v)
	{
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_SColorf::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();
		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r2 = r;
		r2.UpperLeftCorner.X += 4;
		r2.UpperLeftCorner.Y += 24;
		m_ColorControl->setRelativePosition(r2);
		m_ColorControl->updateAbsolutePosition();
	}

	bool CPropertyGrid_SColorf::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_SPINBOX_CHANGED:
			{
				setValue(getValue());
			}break;
			case EGET_TABLE_CHANGED:
			{
				printf("HERE\n");
				// why not?
				setValue(getValue());
				//				return true;
			} break;
			}break;
		}break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}




	CPropertyGrid_Filename::CPropertyGrid_Filename(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, stringw name, stringc v) :
		CPropertyGrid_Base(device, gui, parent, id, rect<s32>(0, 0, 400, 260), name)
	{
		m_FileControl = new FSGUI_FileOpenDialog(L"title",gui, this, 1, true,"", FPT_SKYBOXTHEME);
		setValue(v);
	}

	stringc CPropertyGrid_Filename::getValue()
	{
		return stringc(m_FileControl->getFileName());
	}

	void CPropertyGrid_Filename::setValue(stringc v)
	{
		CPropertyGrid_Base::setValue(v);
	}

	void CPropertyGrid_Filename::calculateLayout()
	{
		CPropertyGrid_Base::calculateLayout();
		rect<s32> rm = getAbsolutePosition();
		rm.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		rm.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r = getName()->getAbsolutePosition();
		r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
		r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;

		rect<s32> r2 = r;
		r2.UpperLeftCorner.X += 4;
		r2.UpperLeftCorner.Y += 24;

		r2.LowerRightCorner.Y = 240;
		r2.LowerRightCorner.X = rm.getWidth() - r2.UpperLeftCorner.X;

		m_FileControl->setRelativePosition(r2);
		m_FileControl->updateAbsolutePosition();
	}

	bool CPropertyGrid_Filename::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
		case EEVENT_TYPE::EET_GUI_EVENT:
		{
			switch (e.GUIEvent.EventType)
			{
			case EGUI_EVENT_TYPE::EGET_FILE_SELECTED:
			{
				setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_EDITBOX_CHANGED:
			{
				if (e.GUIEvent.Caller == getValueEdit()) setValue(getValueEdit()->getText());
			}break;
			case EGUI_EVENT_TYPE::EGET_TABLE_CHANGED:
			{
				setValue(getValue());
			}break;
			}break;
		}break;
		}
		return CPropertyGrid_Base::OnEvent(e);
	}




	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  CPropertyGrid
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CPropertyGrid::CPropertyGrid(IrrlichtDevice* device, IGUIEnvironment* gui, IGUIElement* parent, int id, rect<s32> r):
		IGUIElement(EGUIET_WINDOW, gui, parent, id, r)
	{
		//		m_Properties.push_back(new CPropertyGrid_Filename(gui, this, 1, L"filename",""));

/*
		m_Properties.push_back(new CPropertyGrid_Int(device, gui, this, 1, L"test int", 100));
		m_Properties.push_back(new CPropertyGrid_Vec3d(device, gui, this, 2, L"test vec3d", vector3df(19, 20, 21)));
		m_Properties.push_back(new CPropertyGrid_String(device, gui, this, 3, L"test string", "this is a string"));
		m_Properties.push_back(new CPropertyGrid_Bool(device, gui, this, 4, L"test bool", true));

*/
	}


	CPropertyGrid::~CPropertyGrid()
	{
	}

	void CPropertyGrid::clear()
	{
		if (!m_Properties.empty())
		{
			list<CPropertyGrid_Base*>::Iterator it;
			for (it = m_Properties.begin(); it != m_Properties.end();)
			{
				(*it)->remove();
				it = m_Properties.erase(it);
			}
		}
	}

	int CPropertyGrid::getTotalHeight()
	{
		int result = getAbsolutePosition().getHeight();
		int temp = 0;
		list<CPropertyGrid_Base*>::Iterator it;
		for (it = m_Properties.begin(); it != m_Properties.end(); it++)
		{
			temp += (*it)->getAbsolutePosition().getHeight();
			if (temp > result) result += temp - result;
		}
		return result;
	}

	void CPropertyGrid::calculateLayout(int xoffset, int yoffset)
	{
		list<CPropertyGrid_Base*>::Iterator it;
		rect<s32> r = getAbsolutePosition();
		r.LowerRightCorner -= r.UpperLeftCorner;
		r.UpperLeftCorner -= r.UpperLeftCorner;

		r.UpperLeftCorner.X += xoffset;
		r.UpperLeftCorner.Y += yoffset;
		r.LowerRightCorner.X += xoffset;
		r.LowerRightCorner.Y += yoffset;

		for (it = m_Properties.begin(); it != m_Properties.end(); it++)
		{
			r.LowerRightCorner.Y = r.UpperLeftCorner.Y+(*it)->getAbsolutePosition().getHeight();
			(*it)->setRelativePosition(r);
			(*it)->updateAbsolutePosition();
			(*it)->calculateLayout();
			r.UpperLeftCorner.Y = r.LowerRightCorner.Y;
		}
	}

	void CPropertyGrid::minimizeAll()
	{
		list<CPropertyGrid_Base*>::Iterator it;
		for (it = m_Properties.begin(); it != m_Properties.end(); it++)
		{
			(*it)->setMinimized(true);
		}
	}

	void CPropertyGrid::maximizeAll()
	{
		list<CPropertyGrid_Base*>::Iterator it;
		for (it = m_Properties.begin(); it != m_Properties.end(); it++)
		{
			(*it)->setMinimized(false);
		}
	}

	CPropertyGrid_Base* CPropertyGrid::getPropertyById(int id)
	{
		list<CPropertyGrid_Base*>::Iterator it;
		for (it = m_Properties.begin(); it != m_Properties.end(); it++)
		{
			if ((*it)->getID() == id) return (*it);
		}
		return NULL;
	}

	void CPropertyGrid::draw()
	{
		setAlternateColor(false);
		rect<s32> r = getAbsolutePosition();
		list<CPropertyGrid_Base*>::Iterator it;
		for (it = m_Properties.begin(); it != m_Properties.end(); it++)
		{
			SColor c(255, 165, 165, 165);
			if (getAlternateColor()) c = SColor(255, 175, 175, 175);
			(*it)->setColor(c);
			(*it)->draw();
			setAlternateColor(!getAlternateColor());
		}
	}

	bool CPropertyGrid::OnEvent(const SEvent &e)
	{
		switch (e.EventType)
		{
			case EEVENT_TYPE::EET_USER_EVENT:
			{
				switch (e.UserEvent.UserData1)
				{
					case EET_PROPERTYGRID_MINIMIZE: calculateLayout(0,0); break;
				} break;
			} break;
		}
		return IGUIElement::OnEvent(e);
	}

} // end namespace

