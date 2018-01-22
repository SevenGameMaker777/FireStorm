#pragma once
// http://irrlicht.sourceforge.net/forum/viewtopic.php?f=9&t=46237

#include "irrlicht.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSLevel.h"
#include "FSObject.h"
#include "FSGUITreeView.h"
#include "FSGUI_Toolbar.h"

using namespace irr;
using namespace core;
using namespace gui;
using namespace video;
using namespace scene;

#define FSID_OBJECTTREEVIEW	900
#define	FSID_REFRESH			901

namespace FS
{
	class FSGUI_ObjectViewer : public FSGUI_Window
	{
	private:
		FSLevel* m_Level;
		FSGUITreeView* m_TreeView;
		IGUIButton* m_Refresh;
	public:
		FSGUI_ObjectViewer(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, s32 id, rect<s32> r, long flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, FSLevel* level, s32 minwidth = 10, s32 minheight = 10)
			: FSGUI_Window(desktop,gui, parent,id, r, flags, ls, text, minwidth, minheight), m_Level(level)
		{
			createGui();
		}

		virtual ~FSGUI_ObjectViewer() {}

		FSLevel* getLevel() { return m_Level; }

		void setLevel(FSLevel* level)
		{
			m_Level = level;
			if (getLevel())
			{
				m_TreeView->getRoot()->clearChildren();
				FSObject* obj = getLevel()->getObjectFactory()->getObjectManager()->getNextObject(true);
				while (obj)
				{
					fillTreeView(m_TreeView->getRoot(), obj);
					obj = getLevel()->getObjectFactory()->getObjectManager()->getNextObject(false);
				}
			}
		}

		void createGui()
		{
			m_Refresh = getGui()->addButton(rect<s32>(0, 0, 200, 20), this, FSID_REFRESH, L"Refresh", L"refresh the list");
			m_TreeView = new FSGUITreeView(getGui(), this, FSID_OBJECTTREEVIEW, rect<s32>(0, 40, 400, 410), true, true, true);
		}

		virtual bool onButtonClicked(const SEvent & e)
		{
			switch (e.GUIEvent.Caller->getID())
			{
			case FSID_REFRESH: setLevel(getLevel()); break;
			}
			return FSGUI_Window::onButtonClicked(e);
		}

		virtual bool onTreeviewNodeSelect(const SEvent & e)
		{
			IGUITreeViewNode* node = m_TreeView->getSelected();
			int id = (int)node->getData();

			SEvent msg;
			msg.EventType = EET_USER_EVENT;
			msg.UserEvent.UserData1 = EVT_OBJECT_SELECTED;
			msg.UserEvent.UserData2 = id;
			getLevel()->getDevice()->postEventFromUser(msg);
			getParent()->OnEvent(msg);

			return FSGUI_Window::onTreeviewNodeSelect(e); 
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
				m_Refresh->setRelativePosition(r2);
				m_Refresh->updateAbsolutePosition();

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

		void fillTreeView(gui::IGUITreeViewNode* rootTreeNode, FSObject* obj)
		{
			gui::IGUITreeViewNode* node;
			gui::IGUITreeViewNode* node2;
			gui::IGUITreeViewNode* node3;
			gui::IGUITreeViewNode* node4;

			if (obj)
			{
				node = rootTreeNode->addChildBack(stringw(obj->getName()).c_str(), 0, -1, -1, (void*)obj);
				node->setData((void*)obj->getId());

				FSPropertyManager* pm = obj->getPropertyManager();
				list<varData*>::Iterator it;
				list<varData*> list = pm->m_List;

				if (!list.empty())
				{
					it = list.begin();

					node2 = node->addChildBack(L"Variables", 0, -1, -1, (void*)obj);
					node2->setData((void*)obj->getId());

					for (; it != list.end(); ++it)
					{
						node3 = node2->addChildBack(stringw((*it)->vName).c_str(), 0, -1, -1, (void*)obj);
						node3->setData((void*)obj->getId());
						node4 = node3->addChildBack(getText(L"Type          ", stringw((*it)->vType)).c_str(), 0, -1, -1, (void*)obj);
						node4->setData((void*)obj->getId());
						node4->addChildBack(getText(L"description   ", stringw((*it)->vDescription)).c_str(), 0, -1, -1, (void*)obj);
						node4->setData((void*)obj->getId());
						node4->addChildBack(getText(L"Value         ", stringw(obj->getVariable((*it)->vName))).c_str(), 0, -1, -1, (void*)obj);
						node4->setData((void*)obj->getId());
					}
				}
			}
		}
	};

} // end namespace

