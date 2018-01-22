#pragma once

#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_PropertyGrid.h"

namespace FS
{

	class FSGUI_PropertyGridObject : public FSGUI_PropertyGrid
	{
	private:
		FS_VARIABLE_GET(int, ObjectId);
		FS_VARIABLE_GET(FSLevel*, Level);
	public:
		FSGUI_PropertyGridObject(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, int minwidth, int minheight, wchar_t* text) :
			FSGUI_PropertyGrid(desktop, gui, parent, id, r, flags, ls, text, minwidth, minheight)
		{
			m_ObjectId = 0;
			m_Level = 0;
		}

		virtual ~FSGUI_PropertyGridObject()
		{
		}

		virtual void setObjectId(FSLevel* level, int id)
		{
			if (level)
			{
				getPropertyGrid()->clear();

				m_Level = level;
				int Grid_id = 0;
				FSObject* obj = level->getObjectPointer(id, true);
				if (obj)
				{
					m_ObjectId = id;

					FSPropertyManager* pm = obj->getPropertyManager();
					list<varData*>::Iterator it;
					list<varData*> list = pm->m_List;

					if (!list.empty())
					{
						for (it = list.begin(); it != list.end();)
						{
							Grid_id++;
							stringc name = (*it)->vName;
							stringc value = obj->getVariable((*it)->vName);
							if ((*it)->vType == "FS_FLOAT") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_Float(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, stringcToFloat(value)));
							if ((*it)->vType == "FS_INT") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_Int(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, stringcToInt(value)));
							if ((*it)->vType == "FS_VEC3D") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_Vec3d(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, stringcToVector3df(value)));
							if ((*it)->vType == "FS_BOOL") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_Bool(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, stringcToBool(value)));
							if ((*it)->vType == "FS_PCHAR") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_String(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, value));
							if ((*it)->vType == "FS_TEXTURE") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_String(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, value));
							if ((*it)->vType == "FS_MODEL") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_String(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, value));
							if ((*it)->vType == "FS_COLOR") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_SColor(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, stringcToSColor(value)));
							if ((*it)->vType == "FS_COLORF") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_SColorf(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, stringcToSColorf(value)));
							if ((*it)->vType == "FS_FILENAME") getPropertyGrid()->m_Properties.push_back(new CPropertyGrid_Filename(getDesktop()->getDevice(), getDesktop()->getGui(), getPropertyGrid(), Grid_id, name, stringcToStringc(value)));
							it++;
						}
					}
				}
			}
			calculateLayout();
		}

		virtual bool onUserEvent(const SEvent & e)
		{
			switch (e.UserEvent.UserData1)
			{
			case EET_PROPERTYGRID_PROPERTYCHANGED:
			{
				CPropertyGrid_Base* p = getPropertyGrid()->getPropertyById(e.UserEvent.UserData2);
				if (p)
				{
					FSObject* obj = getLevel()->getObjectPointer(getObjectId(), true);
					if (obj)
					{
						obj->setVariable(p->getName()->getText(), p->getValue());
					}
				}
			} break;
			}
			return false;
		}
	};

} // end namespace