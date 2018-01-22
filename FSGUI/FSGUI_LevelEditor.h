#pragma once

#include "FSGUI_Window.h"
#include "FSLevel.h"
#include "FSApplication.h"
#include "FSEngine.h"
#include "FSSceneNodeAnimatorFPS.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_LevelViewer.h"
#include "FSGUI_PropertyGridObject.h"
#include "FSGUI_ObjectMaker.h"
#include "FSGUI_ObjectViewer.h"
#include "FSGUI_RibbonbarImage.h"
#include "FSGUI_AssetViewer.h"
#include "FSGUI_PrefabMaker.h"
#include "FSGUI_Combobox.h"
#include "Defines.h"
#include "FSTerrainEditorNode.h"

namespace FS
{
	class FSGUI_LevelEditor : public FSGUI_LevelViewer
	{
	private:
		FSGUI_PropertyGridObject* pg;
		FSGUI_ObjectMaker* om;
		FSGUI_PrefabMaker* pm;
		FSGUI_RibbonbarImage* ri;
		FSGUI_ObjectViewer* ov;
		FSGUI_AssetViewer* mv;

		position2di				m_InitialCursorPosition;	// where mouse cursor started
		position2di				m_InitialObjectPosition;	// where ray from camera to object intersected screen
		vector3df				m_ModeXYZ;
		float					m_ScaleSpeed;
		float					m_RotateSpeed;
		int						m_SelectMode;


		bool					m_EditingTerrain;
		bool					m_EditingTerrainRaise;
		bool					m_EditingTerrainLower;
		ITriangleSelector* selector = 0;
		float strength = 0.1f;
		IImage* heightmap = 0;
		int	m_BrushSize = 4;
		FSTerrainEditorNode* m_TerrainEditorNode;


	public:
		vector3df getEditorModeXYZ() { return m_ModeXYZ; }
		float getScaleSpeed() { return m_ScaleSpeed; }
		void setScaleSpeed(float speed) { m_ScaleSpeed = speed; }
		float getRotateSpeed() { return m_RotateSpeed; }
		void setRotateSpeed(float speed) { m_RotateSpeed = speed; }
		void setSlectMode(int v) { m_SelectMode = v; }
		int getSelectMode() { return m_SelectMode; }

	public:
		FSGUI_LevelEditor(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r,
			DWORD flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minwidth, int minheight, FSLevel* level) :
			FSGUI_LevelViewer(desktop, gui, parent, id, r, flags, ls, text, minwidth, minheight, 0)
		{
			FS_LOG(FSL_WARNING, "this is atest warning for the ");
			FS_LOG(FSL_ERROR, "this is a test error");

			setScaleSpeed(0.01f);
			setRotateSpeed(0.1f);
			m_ModeXYZ.set(1, 1, 1);
			m_InitialCursorPosition.set(0, 0);
			m_InitialObjectPosition.set(0, 0);
			m_SelectMode = SELECT_MODE_PHYSX;
			m_EditingTerrain = false;
			m_EditingTerrainRaise = false;
			m_EditingTerrainLower = false;
			selector = 0;
			m_TerrainEditorNode = 0;

			// create the application level toolbar - in this case an editor type application
			FSGUI_Toolbar* tb = addToolbar(FSID_MAINTOOLBAR, rect<s32>(0, 0, 36, 36), LS_TOPFILL, false, 4);

			IGUIButton* b;
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_SAVEPREFAB, L"", L"Save prefab"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\prefab.jpg")));

			IGUIEditBox* peb = (IGUIEditBox*)tb->addElement(getGui()->addEditBox(L"", rect<s32>(0, 0, 124, 24), true, tb, FSID_PREFABMAKERFN));

			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_USELIGHT, L"", L"Use Light"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\light.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_USEFOG, L"", L"Use Fog"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\fog.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_SHOWEDITOR, L"", L"Editor Visuals"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\visuals.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_RENDERDEBUG, L"", L"PhysX Debug"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\physxdebug.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_SHADOWS, L"", L"Shadows"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\shadow.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_SELECTMODE, L"", L"Select Mode"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\px.jpg")));

			// add our own combobox. we had to modify the irrlicht combobox slightly so
			// that it functions properly in our framework
			IGUIComboBox* cb = (IGUIComboBox*)tb->addElement(new FSGUI_Combobox(getDesktop()->getDevice(), getGui(), tb, FSID_OBJECTTYPE, rect<s32>(0, 0, 120, 24)));

			// add some items to the combobox for the user to select
			cb->addItem(L"NONE", FSOT_NONE);
			cb->addItem(L"ALL", FSOT_ALL);
			cb->addItem(L"OBJECT", FSOT_OBJECT);
			cb->addItem(L"TERRAIN", FSOT_TERRAIN);
			cb->addItem(L"STATIC", FSOT_STATIC);
			cb->addItem(L"SKYBOX", FSOT_SKYBOX);
			cb->addItem(L"LIGHT", FSOT_LIGHT);
			cb->addItem(L"TRIGGER", FSOT_TRIGGER);
			cb->addItem(L"WATER", FSOT_WATER);
			cb->addItem(L"CHARACTER", FSOT_CHARACTER);

			// menus and comboboxes have to be rendered last to work properly
			// becasue they have dropdown gui elements. since we render windows in 
			// the order of creation, later windows can draw over these items
			// it's a hack, but it works 
			getDesktop()->addToRenderLastList(cb);

			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_X, L"X", L"X"));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_Y, L"Y", L"Y"));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_Z, L"Z", L"Z"));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_NONE, L"", L"No Mode"));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_TRANSLATE, L"", L"Translate"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\translate.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_ROTATE, L"", L"Rotate"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\rotate.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_SCALE, L"", L"Scale"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\scale.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_BBOX, L"", L"Bounding Box"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\bbox.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_POSOFFSET, L"", L"Position Offset"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\posoffset.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_ROTOFFSET, L"", L"Rotation Offset"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\rotoffset.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_SCALEOFFSET, L"", L"Scale Offset"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\scaleoffset.jpg")));
			b->setIsPushButton(true);

			b = (IGUIButton*)tb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), tb, FSID_DELETEOBJECT, L"", L"Delete Object"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\deleteobject.jpg")));


			// create the application level toolbar - in this case an editor type application
			FSGUI_Toolbar* ttb = tb->addToolbar(FSID_TERRAIN_EDITOR_TOOLBAR, rect<s32>(0, 0, 236, 36), LS_RIGHTFILL, false, 4);
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_EDIT, L"", L"Edit Terrain"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\terrain_edit.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_RAISE, L"", L"raise terrain"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\raise.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_LOWER, L"", L"lower terrain"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\lower.jpg")));
			b->setIsPushButton(true);
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_PHYSXREBUILD, L"", L"rebuild physX"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\physxrebuild.jpg")));
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_SMALL, L"", L"small area edit"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\terrainedit_small.jpg")));
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_MEDIUM, L"", L"medium area edit"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\terrainedit_medium.jpg")));
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_LARGE, L"", L"large area edit"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\terrainedit_large.jpg")));
			b = (IGUIButton*)ttb->addElement(getGui()->addButton(rect<s32>(0, 0, 24, 24), ttb, FSID_TERRAIN_EXTRALARGE, L"", L"extra large area edit"));
			b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\terrainedit_extralarge.jpg")));

			// add a prefabmaker
			pm = new FSGUI_PrefabMaker(getDesktop(), getGui(), this, FSID_PREFABMAKER, rect<s32>(0, 0, 200, 300), WS_THICKFRAME | WS_TITLEBAR | WS_SYSTEMBAR, LS_LEFTFILL, L"Prefab Creator", getLevel(), 20, 10);

			// add an objectmaker
			om = new FSGUI_ObjectMaker(getDesktop(), getGui(), this, FSID_OBJECTMAKER, rect<s32>(0, 0, 200, 300), WS_VISIBLE | WS_THICKFRAME | WS_TITLEBAR | WS_SYSTEMBAR, LS_LEFTFILL, L"Object Creator", getLevel(), 20, 10);

			// add an objectviewer
			ov = new FSGUI_ObjectViewer(getDesktop(), getGui(), this, FSID_OBJECTVIEWER, rect<s32>(0, 0, 200, 300), WS_VISIBLE | WS_THICKFRAME | WS_TITLEBAR | WS_SYSTEMBAR, LS_LEFTFILL, L"Object Viewer", 0, 10, 20);

			// add an assetviewer
			mv = new FSGUI_AssetViewer(getDesktop(), getGui(), this, FSID_ASSETVIEWER, rect<s32>(0, 0, 200, 300), WS_VISIBLE | WS_THICKFRAME | WS_TITLEBAR | WS_SYSTEMBAR, LS_RIGHTFILL, L"Asset Viewer", 10, 20);

			// add an object propertygrid view to allow editing of object variables
			pg = new FSGUI_PropertyGridObject(getDesktop(), getGui(), this, FSID_PROPERTYGRID, rect<s32>(0, 0, 200, 300), WS_VISIBLE | WS_THICKFRAME | WS_TITLEBAR | WS_HSCROLL | WS_VSCROLL | WS_SYSTEMBAR, LS_RIGHTFILL, 10, 10, L"A Property Grid");

			setLevel(level);
			if (getLevel()) getLevel()->setEditorMode(MODE_NONE);

			// update all of the gui controls
			updateGui();
		}

		virtual ~FSGUI_LevelEditor()
		{
		}

		virtual bool setLevel(FSLevel* level)
		{
			FSGUI_LevelViewer::setLevel(level);

			if (selector) selector->drop(); selector = 0;

			pm->setLevel(level);
			pg->setObjectId(getLevel(), 0);
			om->setLevel(level);
			ov->setLevel(level);

			// update all of the gui controls
			updateGui();

			return true;
		}

		virtual void deleteSelectedObject()
		{
			if (getLevel())
			{
				if (getLevel()->getSelectedObject())
				{
					FSObject* obj = getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true);
					if (obj) obj->setDead(true);

					// notify the application
					SEvent e;
					e.EventType = EET_USER_EVENT;
					e.UserEvent.UserData1 = EVT_OBJECT_SELECTED;
					e.UserEvent.UserData2 = 0;
					getLevel()->getDevice()->postEventFromUser(e);
					onObjectSelected(e);
				}

				FSGUI_Toolbar* tb = (FSGUI_Toolbar*)getElementFromId(FSID_TERRAIN_EDITOR_TOOLBAR);
				if (tb) tb->setVisible(false);
			}
		}

		virtual bool onObjectSelected(const SEvent & e)
		{
			if (getLevel())
			{
				getLevel()->setSelectedObject(e.UserEvent.UserData2);
				if (pg)
				{
					pg->setObjectId(getLevel(), e.UserEvent.UserData2);
					pg->minimizeAll();
				}
				FSGUI_Toolbar* tb = (FSGUI_Toolbar*)getElementFromId(FSID_TERRAIN_EDITOR_TOOLBAR,true);
				if (tb)
				{
					if (getLevel())
					{
						FSObject* obj = getLevel()->getObjectPointer(e.UserEvent.UserData2, true);
						if ((obj) && (obj->getObjectType() == FSOT_TERRAIN))
						{
							tb->setVisible(true);
							selector = getLevel()->getSmgr()->createTerrainTriangleSelector((ITerrainSceneNode*)obj->getPrimarySceneNode(), 0);
							heightmap = getDesktop()->getDriver()->createImageFromFile(MEDIAPATH(obj->getVariable("HeightmapFilename").c_str()));
							m_TerrainEditorNode = new FSTerrainEditorNode(getLevel()->getSmgr()->getRootSceneNode(), getLevel()->getSmgr(), 777);
							m_TerrainEditorNode->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\toolbar\\TerrainEditImage.jpg")));
						}
						else
						{
							tb->setVisible(false);
							if (m_TerrainEditorNode) m_TerrainEditorNode->drop(); m_TerrainEditorNode = 0;
							if (selector) selector->drop(); selector = 0;
							if (heightmap) heightmap->drop(); heightmap = 0;
						}
					}
				}
			}
			return FSGUI_LevelViewer::onObjectSelected(e);
		}

		virtual void updateGui()
		{
			if (!getLevel()) return;

			IGUICheckBox* cb;
			IGUIButton* b;

			FSGUI_Toolbar* tb = (FSGUI_Toolbar*)getFSGUIChild(FSID_MAINTOOLBAR);
			if (tb)
			{
				b = (IGUIButton*)tb->getElementFromId(FSID_USELIGHT);
				if (b) b->setPressed(getLevel()->getUseLight() > 0 ? true : false);
				b = (IGUIButton*)tb->getElementFromId(FSID_USEFOG);
				if (b) b->setPressed(getLevel()->getUseFog() > 0 ? true : false);
				b = (IGUIButton*)tb->getElementFromId(FSID_SHOWEDITOR);
				if (b) b->setPressed(getLevel()->getUseEditorVisuals() > 0 ? true : false);
				b = (IGUIButton*)tb->getElementFromId(FSID_RENDERDEBUG);
				if (b) b->setPressed(getLevel()->getPhysXWorld()->getRenderDebugInfo() > 0 ? true : false);
				b = (IGUIButton*)tb->getElementFromId(FSID_SHADOWS);
				if (b) b->setPressed(getLevel()->getUseShadows() > 0 ? true : false);

				b = (IGUIButton*)tb->getElementFromId(FSID_NONE);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_NONE);
				b = (IGUIButton*)tb->getElementFromId(FSID_TRANSLATE);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_TRANSLATE);
				b = (IGUIButton*)tb->getElementFromId(FSID_ROTATE);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_ROTATE);
				b = (IGUIButton*)tb->getElementFromId(FSID_SCALE);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_SCALE);
				b = (IGUIButton*)tb->getElementFromId(FSID_BBOX);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_BBOX);
				b = (IGUIButton*)tb->getElementFromId(FSID_POSOFFSET);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_POSITIONOFFSET);
				b = (IGUIButton*)tb->getElementFromId(FSID_ROTOFFSET);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_ROTATIONOFFSET);
				b = (IGUIButton*)tb->getElementFromId(FSID_SCALEOFFSET);
				if (b) b->setPressed(getLevel()->getEditorMode() == MODE_SCALEOFFSET);

				b = (IGUIButton*)tb->getElementFromId(FSID_X);
				if (b) b->setPressed(m_ModeXYZ.X > 0 ? true : false);
				b = (IGUIButton*)tb->getElementFromId(FSID_Y);
				if (b) b->setPressed(m_ModeXYZ.Y > 0 ? true : false);
				b = (IGUIButton*)tb->getElementFromId(FSID_Z);
				if (b) b->setPressed(m_ModeXYZ.Z > 0 ? true : false);
			}
		}

		virtual bool onEvent(const SEvent & e)
		{
			if (getDesktop()->getHoldMouse()) return false;
			return FSEventHandler::OnEvent(e);
		}

		virtual bool onRMouseButtonDown(const SEvent & e)
		{
			if (getLevel())	getLevel()->getCamera()->setInputReceiverEnabled(!getLevel()->getCamera()->isInputReceiverEnabled()); 
			return FSGUI_Window::onRMouseButtonDown(e);
		}

		virtual bool onLMouseButtonUp(const SEvent & e)
		{

			return FSGUI_LevelViewer::onLMouseButtonUp(e);
		}

		virtual bool onLMouseButtonDown(const SEvent & e)
		{
			if (getDesktop()->getHoldMouse()) return false;
			if (m_EditingTerrain) return false;


			//////////////////////////////////////////////
			// setup the viewport for camera picking
			//////////////////////////////////////////////
			rect<s32> r = getClientRect();
			int x = e.MouseInput.X - r.UpperLeftCorner.X;
			int y = e.MouseInput.Y - r.UpperLeftCorner.Y;
			rect<s32> oldViewport = getDesktop()->getDevice()->getVideoDriver()->getViewPort();
			getDesktop()->getDevice()->getVideoDriver()->setViewPort(r);

			FSObject* obj = 0;

			if (getLevel())
			{
				if (m_SelectMode == SELECT_MODE_NODE)
					obj = getLevel()->selectObjectBySceneNode(x, y, getLevel()->getObjectSelectType());
				else
					obj = getLevel()->selectObjectPhysX(getLevel()->getCamera(), x, y, getLevel()->getObjectSelectType());

				getDesktop()->getDevice()->getVideoDriver()->setViewPort(oldViewport);
				
				//////////////////////////////////////////////
				// end setup the viewport for camera picking
				//////////////////////////////////////////////

				if (obj && (obj->getId() != getLevel()->getSelectedObject()))
				{
					// notify the application
					SEvent e;
					e.EventType = EET_USER_EVENT;
					e.UserEvent.UserData1 = EVT_OBJECT_SELECTED;
					e.UserEvent.UserData2 = obj->getId();
					getLevel()->getDevice()->postEventFromUser(e);
					onObjectSelected(e);

					m_InitialCursorPosition.set(x, y);
					m_InitialObjectPosition = getLevel()->getCollisionManager()->getScreenCoordinatesFrom3DPosition(obj->getPosition(), getLevel()->getCamera());
				}
			}
			return FSGUI_LevelViewer::onLMouseButtonDown(e);
		}

		virtual bool onMMouseButtonDown(const SEvent & e)
		{
			return FSGUI_LevelViewer::onMMouseButtonDown(e);
		}

		virtual bool onMouseWheel(const SEvent & e) 
		{ 
			if (m_EditingTerrain)
			{
				editTerrain(e, e.MouseInput.Wheel > 0 ? true : false, true);
			}

			return FSGUI_LevelViewer::onMouseWheel(e);
		}

		void editTerrain(const SEvent & e, bool up, bool active)
		{
			ITerrainSceneNode* terrain = 0;
			FSObject* obj = getLevel()->getSelectedObjectPointer();
			if ((obj) && (obj->getObjectType() == FSOT_TERRAIN)) 
				terrain = (ITerrainSceneNode*)obj->getPrimarySceneNode();

			//////////////////////////////////////////////
			// setup the viewport for camera picking
			//////////////////////////////////////////////
			rect<s32> r = getClientRect();
			int x = e.MouseInput.X - r.UpperLeftCorner.X;
			int y = e.MouseInput.Y - r.UpperLeftCorner.Y;
			rect<s32> oldViewport = getDesktop()->getDevice()->getVideoDriver()->getViewPort();
			getDesktop()->getDevice()->getVideoDriver()->setViewPort(r);

			position2di clickPosition(x, y);
			line3d<float> ray = getLevel()->getSmgr()->getSceneCollisionManager()->getRayFromScreenCoordinates(clickPosition, 0);

			getDesktop()->getDevice()->getVideoDriver()->setViewPort(oldViewport);

			vector3df pos;
			triangle3df Tri;

			ISceneNode* out;
			if (getLevel()->getSmgr()->getSceneCollisionManager()->getCollisionPoint(ray, selector, pos, Tri, out))
			{
				if (terrain)
				{
					s32 scale = terrain->getScale().X;
					dimension2du s = stringcToDimension2du(obj->getVariable("HeightmapDimensions"));
					s32 x = (s32)(pos.X / scale);
					s32 z = (s32)(pos.Z / scale);

					int bwh = m_TerrainEditorNode->getSize().Width / 2 + 1;
					int bhh = m_TerrainEditorNode->getSize().Height / 2 + 1;
					if (x - bwh <= 0) x = bwh-1;
					if (z - bhh <= 0) z = bhh-1;
					if (x + bwh > s.Width) x = s.Width - bwh;
					if (z + bhh > s.Height) z = s.Height - bhh;
					
					s32 index = x * s.Width + z;
					m_TerrainEditorNode->setup(terrain, index, up, active, x*scale,z*scale,s.Width,s.Height);
				}
			}
		}

		virtual bool onMouseMoved(const SEvent & e)
		{
			if (getDesktop()->getHoldMouse()) return false;

			if (getLevel())
			{
				if (m_EditingTerrain)
				{
					editTerrain(e,m_EditingTerrainRaise, e.MouseInput.isLeftPressed());
				}
				else
				if (getLevel()->getSelectedObject())
				{
					if (e.MouseInput.isLeftPressed())
					{
						switch (getLevel()->getEditorMode())
						{
						case MODE_TRANSLATE: if (onTranslateObject(getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true), e)) return true; break;
						case MODE_ROTATE: if (OnRotateObject(getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true), e)) return true; break;
						case MODE_SCALE: if (OnScaleObject(getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true), e)) return true; break;
						case MODE_BBOX: if (OnBBoxObject(getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true), e)) return true; break;
						case MODE_POSITIONOFFSET: if (OnPositionOffsetObject(getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true), e)) return true; break;
						}
					}
				}
			}
			return FSGUI_LevelViewer::onMouseMoved(e);
		}

		virtual bool onFileSelected(const SEvent & e) 
		{
			switch (e.GUIEvent.Caller->getID())
			{
			case FSID_SAVEPREFAB :
			{
				if (getLevel())
				{
					IGUIFileOpenDialog* fp = (IGUIFileOpenDialog*)e.GUIEvent.Caller;
					stringc filename(fp->getFileName());
					FSObject* obj = getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true);
					if (obj) getLevel()->getObjectFactory()->savePrefab(obj, SHORTPREFABPATH(filename));
					fp->remove();
					pm->fillTreeView();
				}
			} break;
			}
			return false; 
		}

		virtual bool onButtonClicked(const SEvent & e)
		{
			if (!getLevel()) return FSGUI_LevelViewer::onButtonClicked(e);

			IGUIButton* b = (IGUIButton*)(e.GUIEvent.Caller);
			IGUIFileOpenDialog* fp;
			IGUIEditBox* eb;
			FSObject* obj = 0;

			stringc path;
			switch (e.GUIEvent.Caller->getID())
			{
			case FSID_SAVEPREFAB:
				eb = (IGUIEditBox*)getGui()->getRootGUIElement()->getElementFromId(FSID_PREFABMAKERFN,true);
				if (eb)
				{
					stringc filename(eb->getText());
					filename += ".prefab";
					FSObject* obj = getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true);
					if (obj) getLevel()->getObjectFactory()->savePrefab(obj, filename);
					pm->fillTreeView();
					return true;
				}
				else fp = getGui()->addFileOpenDialog(L"Prefabs", true, this, FSID_SAVEPREFAB, false, "");
				// we used this event
				return true;
				break;
			case FSID_USELIGHT:
				// toggle using light in the level
				getLevel()->setUseLight(!getLevel()->getUseLight());
				// we used this event
				return true;
				break;
			case FSID_USEFOG:
				// toggle using fog in the level
				getLevel()->setUseFog(!getLevel()->getUseFog());
				// we used this event
				return true;
				break;
			case FSID_SHOWEDITOR:
				// toggle rendering the editor visuals
				getLevel()->setUseEditorVisuals(!getLevel()->getUseEditorVisuals());
				// we used this event
				return true;
				break;
			case FSID_RENDERDEBUG:
				// toggle rendering the physx representations
				getLevel()->getPhysXWorld()->setRenderDebugInfo(!getLevel()->getPhysXWorld()->getRenderDebugInfo());
				// we used this event
				return true;
				break;
			case FSID_SHADOWS:
				// toggle rendering the physx representations
				getLevel()->setUseShadows(!getLevel()->getUseShadows());
				// we used this event
				return true;
				break;
			case FSID_SELECTMODE:
				// toggle rendering the physx representations
				if (m_SelectMode == SELECT_MODE_PHYSX) m_SelectMode = SELECT_MODE_NODE;
				else m_SelectMode = SELECT_MODE_PHYSX;
				b = (IGUIButton*)getGui()->getRootGUIElement()->getElementFromId(FSID_SELECTMODE,true);
				if (b)
				{
					if (m_SelectMode == SELECT_MODE_PHYSX) b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\px.jpg")));
					if (m_SelectMode == SELECT_MODE_NODE) b->setImage(getDriver()->getTexture(MEDIAPATH("_Assets\\GUI\\Toolbar\\node.jpg")));
				}
				// we used this event
				return true;
				break;

			case FSID_DELETEOBJECT:
			{
				deleteSelectedObject();
			} break;
			case FSID_X:
			{
				m_ModeXYZ.X = b->isPressed() ? 1 : 0;
			} break;
			case FSID_Y:
			{
				m_ModeXYZ.Y = b->isPressed() ? 1 : 0;
			} break;
			case FSID_Z:
			{
				m_ModeXYZ.Z = b->isPressed() ? 1 : 0;
			} break;
			case FSID_NONE:
			{
				getLevel()->setEditorMode(MODE_NONE);
			} break;
			case FSID_TRANSLATE:
			{
				getLevel()->setEditorMode(MODE_TRANSLATE);
			} break;
			case FSID_SCALE:
			{
				getLevel()->setEditorMode(MODE_SCALE);
			} break;
			case FSID_ROTATE:
			{
				getLevel()->setEditorMode(MODE_ROTATE);
			} break;
			case FSID_BBOX:
			{
				getLevel()->setEditorMode(MODE_BBOX);
			} break;
			case FSID_POSOFFSET:
			{
				getLevel()->setEditorMode(MODE_POSITIONOFFSET);
			} break;
			case FSID_ROTOFFSET:
			{
				getLevel()->setEditorMode(MODE_ROTATIONOFFSET);
			} break;
			case FSID_SCALEOFFSET:
			{
				getLevel()->setEditorMode(MODE_SCALEOFFSET);
			} break;
			case FSID_TERRAIN_EDIT:
			{
				m_EditingTerrain = !m_EditingTerrain;
			} break;
			case FSID_TERRAIN_RAISE:
			{
				m_EditingTerrainRaise = !m_EditingTerrainRaise;
				m_EditingTerrainLower = !m_EditingTerrainRaise;
				b = (IGUIButton*)getGui()->getRootGUIElement()->getElementFromId(FSID_TERRAIN_LOWER, true);
				if (b) b->setPressed(false);
			} break;
			case FSID_TERRAIN_LOWER:
			{
				m_EditingTerrainLower = !m_EditingTerrainLower;
				m_EditingTerrainRaise = !m_EditingTerrainLower;
				b = (IGUIButton*)getGui()->getRootGUIElement()->getElementFromId(FSID_TERRAIN_RAISE, true);
				if (b) b->setPressed(false);
			} break;
			case FSID_TERRAIN_PHYSXREBUILD:
			{
				obj = getLevel()->getObjectPointer(getLevel()->getSelectedObject(), true);
				if ((obj) && (obj->getObjectType() == FSOT_TERRAIN))
					obj->createPhysXObject();
			} break;
			case FSID_TERRAIN_SMALL:
			{
				m_BrushSize = 2;
				m_TerrainEditorNode->setSize(dimension2du(m_BrushSize, m_BrushSize));
			} break;
			case FSID_TERRAIN_MEDIUM:
			{
				m_BrushSize = 4;
				m_TerrainEditorNode->setSize(dimension2du(m_BrushSize, m_BrushSize));
			} break;
			case FSID_TERRAIN_LARGE:
			{
				m_BrushSize = 6;
				m_TerrainEditorNode->setSize(dimension2du(m_BrushSize, m_BrushSize));
			} break;
			case FSID_TERRAIN_EXTRALARGE:
			{
				m_BrushSize = 12;
				m_TerrainEditorNode->setSize(dimension2du(m_BrushSize, m_BrushSize));
			} break;

			case FSID_CREATEOBJECT:
			{
				FSGUI_ObjectMaker* om = (FSGUI_ObjectMaker*)getFSGUIChild(FSID_OBJECTMAKER);
				if (om)
				{
					stringc type = om->getSelectedObjectType();
					if (type != FS_DEFAULT)
					{
						FSObject* obj = getLevel()->getObjectFactory()->createObjectByType(type, true, 0, type, true);
						if (obj) obj->setPosition(getLevel()->getCamera()->getPosition() + (getLevel()->getCamera()->getTarget() - getLevel()->getCamera()->getPosition()).normalize() * 100);
					}
					return true;
				}
			} break;
			case FSID_CREATEPREFAB:
			{
				FSGUI_PrefabMaker* om = (FSGUI_PrefabMaker*)getFSGUIChild(FSID_PREFABMAKER);
				if (om)
				{
					stringc type = om->getSelectedObjectType();
					if (type != FS_DEFAULT)
					{
						FSObject* obj = getLevel()->getObjectFactory()->loadPrefab(type);
						obj->setPosition(getLevel()->getCamera()->getPosition() + (getLevel()->getCamera()->getTarget() - getLevel()->getCamera()->getPosition()).normalize() * 100);
					}
					return true;
				}
			} break;

			}

			// update all of the gui controls
			updateGui();

			// call the base class
			return FSGUI_LevelViewer::onButtonClicked(e);
		}

		virtual bool OnKeyInputPressedEvent(const SEvent & e)
		{
			if (!getLevel()) return FSGUI_LevelViewer::OnKeyInputPressedEvent(e);

			switch (e.KeyInput.Key)
			{
				// toggle using light in the level
			case KEY_KEY_L: getLevel()->setUseLight(!getLevel()->getUseLight()); break;

				// toggle using fog in the level
			case KEY_KEY_F: getLevel()->setUseFog(!getLevel()->getUseFog()); break;

				// toggle rendering the physx resresentations
//			case KEY_KEY_P: getLevel()->getPhysXWorld()->setRenderDebugInfo(!getLevel()->getPhysXWorld()->getRenderDebugInfo()); break;

				// toggle rendering the physx resresentations
			case KEY_DELETE: deleteSelectedObject(); break;

			case KEY_KEY_C:
			{
				if ((e.KeyInput.Control) && (getLevel()->getSelectedObject()))
				{
//					FSObject* obj = getLevel()->getObjectPointer(getLevel()->getLastSelectedObject(), true);
//					if (obj) getLevel()->getObjectFactory()->savePrefab(obj, "tempCopy.prefab");
				}
			} break;

			case KEY_KEY_V:
			{
				if (e.KeyInput.Control)
				{
					FSObject* obj = getLevel()->getObjectFactory()->loadPrefab("tempCopy.prefab");
					if (obj)
						obj->setPosition(getLevel()->getCamera()->getPosition() + (getLevel()->getCamera()->getTarget() - getLevel()->getCamera()->getPosition()).normalize() * 100);
				}
			} break;
			}

			// update all of the gui controls
			updateGui();

			// call the base class
			return FSGUI_LevelViewer::OnKeyInputPressedEvent(e);
		}

		virtual bool onComboboxChanged(const SEvent & e)
		{
			if (!getLevel()) return FSGUI_LevelViewer::onComboboxChanged(e);

			switch (e.GUIEvent.Caller->getID())
			{
			case FSID_OBJECTTYPE:
			{
				// get a pointer to the combobox
				IGUIComboBox* cb = (IGUIComboBox*)e.GUIEvent.Caller;

				// set the object selection type
				getLevel()->setObjectSelectType(cb->getItemData(cb->getSelected()));
			} break;
			}

			// update all of the gui controls
			updateGui();

			// call the base class
			return FSGUI_LevelViewer::onComboboxChanged(e);
		}

		virtual void drawClientArea()
		{
			FSGUI_LevelViewer::drawClientArea();
		}

		virtual bool onTranslateObject(FSObject* obj, const SEvent& e)
		{
			if (!obj) return false;

			vector3df p = obj->getPosition();

			if (e.MouseInput.Shift)
			{
				plane3df const planeXY(obj->getPosition(), vector3df(1.f, 0.f, 0.f));
				position2di currentCursorPosition(getLevel()->getDevice()->getCursorControl()->getPosition());

				rect<s32> r = getClientRect();
				currentCursorPosition.X -= r.UpperLeftCorner.X;
				currentCursorPosition.Y -= r.UpperLeftCorner.Y;

				position2di effectiveObjectPosition = m_InitialObjectPosition + currentCursorPosition - m_InitialCursorPosition;
				line3df ray(getLevel()->getCollisionManager()->getRayFromScreenCoordinates(effectiveObjectPosition, getLevel()->getCamera()));
				vector3df intersectWithPlane;
				vector3df oldPos = obj->getPosition();

				if (planeXY.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane))
				{
					intersectWithPlane.X = oldPos.X;
					intersectWithPlane.Z = oldPos.Z;
					if (!getEditorModeXYZ().X) intersectWithPlane.X = p.X;
					if (!getEditorModeXYZ().Y) intersectWithPlane.Y = p.Y;
					if (!getEditorModeXYZ().Z) intersectWithPlane.Z = p.Z;
					obj->setPosition(intersectWithPlane);
					return true;
				}
			}
			else
			{
				plane3df const planeXZ(obj->getPosition(), vector3df(0.f, 1.f, 0.f));

				position2di currentCursorPosition(getLevel()->getDevice()->getCursorControl()->getPosition());
				rect<s32> r = getClientRect();
				currentCursorPosition.X -= r.UpperLeftCorner.X;
				currentCursorPosition.Y -= r.UpperLeftCorner.Y;

				position2di effectiveObjectPosition = m_InitialObjectPosition + currentCursorPosition - m_InitialCursorPosition;
				line3df ray(getLevel()->getCollisionManager()->getRayFromScreenCoordinates(effectiveObjectPosition, getLevel()->getCamera()));
				vector3df intersectWithPlane;
				if (planeXZ.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane))
				{
					if (!getEditorModeXYZ().X) intersectWithPlane.X = p.X;
					if (!getEditorModeXYZ().Y) intersectWithPlane.Y = p.Y;
					if (!getEditorModeXYZ().Z) intersectWithPlane.Z = p.Z;

					obj->setPosition(intersectWithPlane);
					return true;
				}
			}
			return false;
		}

		virtual bool OnRotateObject(FSObject* obj, const SEvent& e)
		{
			if (!obj) return false;

			vector3df rot = obj->getRotation();

			position2di currentCursorPosition(getLevel()->getDevice()->getCursorControl()->getPosition());
			rect<s32> r = getClientRect();
			currentCursorPosition.X -= r.UpperLeftCorner.X;
			currentCursorPosition.Y -= r.UpperLeftCorner.Y;

			if (e.MouseInput.Shift)
			{
				if (m_InitialObjectPosition.X < currentCursorPosition.X) rot.X -= getRotateSpeed();
				if (m_InitialObjectPosition.X > currentCursorPosition.X) rot.X += getRotateSpeed();
			}
			else
			{
				if (m_InitialObjectPosition.X < currentCursorPosition.X) rot.Y -= getRotateSpeed();
				if (m_InitialObjectPosition.X > currentCursorPosition.X) rot.Y += getRotateSpeed();
				if (m_InitialObjectPosition.Y < currentCursorPosition.Y) rot.Z -= 1;
				if (m_InitialObjectPosition.Y > currentCursorPosition.Y) rot.Z += 1;
			}

			m_InitialObjectPosition = currentCursorPosition;

			vector3df ro2 = obj->getRotation();
			if (!getEditorModeXYZ().X) rot.X = ro2.X;
			if (!getEditorModeXYZ().Y) rot.Y = ro2.Y;
			if (!getEditorModeXYZ().Z) rot.Z = ro2.Z;
			obj->setRotation(rot);
			return true;
		}

		virtual bool OnScaleObject(FSObject* obj, const SEvent& e)
		{
			if (!obj) return false;
			vector3df scale = obj->getScale();
			position2di currentCursorPosition(getLevel()->getDevice()->getCursorControl()->getPosition());
			rect<s32> r = getClientRect();
			currentCursorPosition.X -= r.UpperLeftCorner.X;
			currentCursorPosition.Y -= r.UpperLeftCorner.Y;

			if (m_InitialObjectPosition.X >= currentCursorPosition.X)
			{
				scale.X -= getScaleSpeed();
				scale.Y -= getScaleSpeed();
				scale.Z -= getScaleSpeed();
			}
			if (m_InitialObjectPosition.X < currentCursorPosition.X)
			{
				scale.X += getScaleSpeed();
				scale.Y += getScaleSpeed();
				scale.Z += getScaleSpeed();
			}
			m_InitialObjectPosition = currentCursorPosition;

			if (e.MouseInput.Shift)
			{
				vector3df pos = obj->getPosition();
				pos.Y += getScaleSpeed();
				obj->setPosition(pos);
			}

			vector3df s = obj->getScale();
			if (!getEditorModeXYZ().X) scale.X = s.X;
			if (!getEditorModeXYZ().Y) scale.Y = s.Y;
			if (!getEditorModeXYZ().Z) scale.Z = s.Z;
			obj->setScale(scale);

			return true;
		}

		virtual bool OnBBoxObject(FSObject* obj, const SEvent& e)
		{
			if (!obj) return false;
			position2di currentCursorPosition(getLevel()->getDevice()->getCursorControl()->getPosition());
			rect<s32> r = getClientRect();
			currentCursorPosition.X -= r.UpperLeftCorner.X;
			currentCursorPosition.Y -= r.UpperLeftCorner.Y;

			vector3df bb = obj->getBBOffset();
			if (m_InitialCursorPosition.X > currentCursorPosition.X)
			{
				if (getEditorModeXYZ().X) bb.X += getScaleSpeed();
				if (getEditorModeXYZ().Y) bb.Y += getScaleSpeed();
				if (getEditorModeXYZ().Z) bb.Z += getScaleSpeed();
				obj->setBBOffset(bb);
			}
			if (m_InitialCursorPosition.X < currentCursorPosition.X)
			{
				if (getEditorModeXYZ().X)	bb.X -= getScaleSpeed();
				if (getEditorModeXYZ().Y)	bb.Y -= getScaleSpeed();
				if (getEditorModeXYZ().Z)	bb.Z -= getScaleSpeed();
				obj->setBBOffset(bb);
			}
			m_InitialCursorPosition = currentCursorPosition;

			return true;
		}

		virtual bool OnPositionOffsetObject(FSObject* obj, const SEvent& e)
		{
			if (!obj) return false;

			position2di currentCursorPosition(getLevel()->getDevice()->getCursorControl()->getPosition());
			rect<s32> r = getClientRect();
			currentCursorPosition.X -= r.UpperLeftCorner.X;
			currentCursorPosition.Y -= r.UpperLeftCorner.Y;

			vector3df bb = obj->getPositionOffset();
			if (m_InitialCursorPosition.X > currentCursorPosition.X)
			{
				if (getEditorModeXYZ().X)	bb.X += getScaleSpeed();
				if (getEditorModeXYZ().Y)	bb.Y += getScaleSpeed();
				if (getEditorModeXYZ().Z)	bb.Z += getScaleSpeed();
				obj->setPositionOffset(bb);
			}
			if (m_InitialCursorPosition.X < currentCursorPosition.X)
			{
				if (getEditorModeXYZ().X)	bb.X -= getScaleSpeed();
				if (getEditorModeXYZ().Y)	bb.Y -= getScaleSpeed();
				if (getEditorModeXYZ().Z)	bb.Z -= getScaleSpeed();
				obj->setPositionOffset(bb);
			}
			m_InitialCursorPosition = currentCursorPosition;

			return true;
		}

		virtual bool acceptDragDropInfo(FSGUI_DRAGDROPINFO* ddi)
		{
			if (ddi->getFrom() != getID())
				switch (ddi->getType())
				{
				case DDIT_FILENAME:
				{
					FSGUI_DRAGDROPINFO_FILENAME* tddi = dynamic_cast<FSGUI_DRAGDROPINFO_FILENAME*>(ddi);
					if (tddi)
					{
						switch (tddi->getFiletype())
						{
						case FPT_TEXTURE:
						{
						} break;

						case FPT_ANIMATEDMODEL:
						{
							FSObject* obj = getLevel()->getObjectFactory()->createObjectByType("FSO_CHARACTER", true, 0, "FSO_CHARACTER", true);
							if (obj)
							{
								obj->setPosition(getLevel()->getCamera()->getPosition() + (getLevel()->getCamera()->getTarget() - getLevel()->getCamera()->getPosition()).normalize() * 100);
								obj->setVariable("MeshFilename", tddi->getFilename());
							}
						} break;
						case FPT_STATICMODEL:
						{
							FSObject* obj = getLevel()->getObjectFactory()->createObjectByType("FSO_STATIC", true, 0, "FSO_STATIC", true);
							if (obj)
							{
								obj->setPosition(getLevel()->getCamera()->getPosition() + (getLevel()->getCamera()->getTarget() - getLevel()->getCamera()->getPosition()).normalize() * 100);
								obj->setVariable("MeshFilename", tddi->getFilename());
							}
						} break;
						}
					}
				} break;
				}
			return FSGUI_Window::acceptDragDropInfo(ddi);
		}



	};
} // end namespace
