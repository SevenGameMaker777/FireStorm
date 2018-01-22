#pragma once
// http://irrlicht.sourceforge.net/forum/viewtopic.php?f=9&t=46237

#include "irrlicht.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSLevel.h"
#include "FSObject.h"
#include "FSGUITreeView.h"
#include "FSPropertyManager.h"

using namespace irr;
using namespace core;
using namespace gui;
using namespace video;
using namespace scene;

#define FSID_OBJECTMAKERTREEVIEW	400
#define FSID_CREATEOBJECT		401

namespace FS
{
	class FSGUI_ObjectMaker : public FSGUI_Window
	{
	private:
		FSLevel* m_Level;
		FSGUITreeView* m_TreeView;
		IGUIButton* m_CreateObject;
	public:
		FSGUI_ObjectMaker(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, s32 id, rect<s32> r, long flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, FSLevel* level, s32 minwidth = 10, s32 minheight = 10)
			: FSGUI_Window(desktop, gui, parent, id, r, flags, ls, text, minwidth, minheight), m_Level(level)
		{
			createGui();
		}

		virtual ~FSGUI_ObjectMaker() {}

		FSLevel* getLevel() { return m_Level; }

		void setLevel(FSLevel* level)
		{
			m_Level = level;
			fillTreeView();
		}

		void createGui()
		{
			m_TreeView = new FSGUITreeView(getGui(), this, FSID_OBJECTMAKERTREEVIEW, rect<s32>(0, 0, 400, 410), true, true, true);
			m_CreateObject = getGui()->addButton(rect<s32>(0, 0, 10, 40), this, FSID_CREATEOBJECT, L"Create", L"Create selected object type");
		}

		virtual void calculateLayout()
		{
			FSGUI_Window::calculateLayout();

			if (m_TreeView)
			{
				//updateAbsolutePosition();

				rect<s32> r = getClientRect();
				r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
				r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
				r.UpperLeftCorner += 5;
				r.LowerRightCorner -= 5;

				rect<s32> r2 = r;
				r2.LowerRightCorner.Y = r.UpperLeftCorner.Y + 30;
				m_CreateObject->setRelativePosition(r2);
				m_CreateObject->updateAbsolutePosition();

				r.UpperLeftCorner.Y += 35;

				m_TreeView->setRelativePosition(r);
				m_TreeView->updateAbsolutePosition();


				rect<s32> scrollbar = m_TreeView->getRelativePosition();
				scrollbar.UpperLeftCorner.X = scrollbar.LowerRightCorner.X - 32;
				scrollbar.LowerRightCorner.X = scrollbar.UpperLeftCorner.X + 20;
				scrollbar.UpperLeftCorner.Y = 0;
				scrollbar.LowerRightCorner.Y -= 64;
				m_TreeView->getVertScrollbar()->setRelativePosition(scrollbar);

				scrollbar = m_TreeView->getRelativePosition();
				scrollbar.UpperLeftCorner.Y = scrollbar.LowerRightCorner.Y - 64;
				scrollbar.LowerRightCorner.Y = scrollbar.UpperLeftCorner.Y + 20;
				scrollbar.UpperLeftCorner.X = 0;
				scrollbar.LowerRightCorner.X -= 10;
				m_TreeView->getHorizScrollbar()->setRelativePosition(scrollbar);
			}

		}

		stringw getText(stringc s, stringc s2)
		{
			return stringw(s + s2);
		}

		void fillTreeView()
		{
			if (!getLevel()) return;

			gui::IGUITreeViewNode* node = 0;
			gui::IGUITreeViewNode* node2 = 0;
			gui::IGUITreeViewNode* node3 = 0;

			stringc dllName = "";

			m_TreeView->getRoot()->clearChildren();
			node = m_TreeView->getRoot()->addChildBack(stringw("FSObject Definitions").c_str(), 0, -1, -1);
			node->setExpanded(true);

			FSClassManager* cm = getLevel()->getObjectFactory()->getClassManager();
			FSObjectData* data = cm->getNextObject(true);
			while (data)
			{
				if (data->DLLName != dllName)
				{
					dllName = data->DLLName;
					node2 = node->addChildBack(stringw(data->DLLName).c_str(), 0, -1, -1);
					node2->setExpanded(true);
				}
				node3 = node2->addChildBack(stringw(data->ObjectName).c_str(), 0, -1, -1,data);
				node3->setExpanded(true);
				data = cm->getNextObject(false);
			}

		}

		virtual stringc getSelectedObjectType()
		{
			gui::IGUITreeViewNode* node = m_TreeView->getSelected();
			if ((node) && (node->getData()))
			{
				FSObjectData* data = (FSObjectData*)node->getData();
				return data->ObjectName;
			}
			return FS_DEFAULT;
		}
	};

} // end namespace

