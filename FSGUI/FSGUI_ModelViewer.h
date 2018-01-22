// include this file only once
#pragma once

// include the needed header files
#include "FSGUI_Toolbar.h"
#include "FSGUI_Systembar.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"

namespace FS
{
	/*
	a ribbonbar window
	*/
	class FSGUI_ModelViewer : public FSGUI_Window
	{
	public:

		ICameraSceneNode* m_Camera;
		ISceneManager* m_Smgr;
		IAnimatedMeshSceneNode* m_Node;
		stringc m_FileName;

		// simple constructor that auto sets the FSGUI_LAYOUT::FSGUI_BORDER flag and layout
		FSGUI_ModelViewer(FSGUI_Desktop* desktop, IGUIEnvironment* env, FSGUI_Window* FSparent, int id, rect<s32> r, long flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minWidth, int minHeight)
			: FSGUI_Window(desktop, env, FSparent, id, r, flags, ls, text, minWidth, minHeight)
		{
			m_Camera = 0;
			m_Smgr = 0;
			m_Node = 0;
			m_FileName = "DEFAULT";

			m_Smgr = getDesktop()->getDevice()->getSceneManager()->createNewSceneManager();
			m_Camera = m_Smgr->addCameraSceneNode();
			//m_Camera->setFarValue(20000.f);
			m_Camera->setPosition(core::vector3df(0, 100, -100));
			m_Camera->setTarget(core::vector3df(0, 0, 0));
		}

		virtual ~FSGUI_ModelViewer()
		{
			if (m_Camera) m_Camera->remove(); m_Camera = 0;
			if (m_Node) m_Node->remove(); m_Node = 0;
			if (m_Smgr) m_Smgr->drop(); m_Smgr = 0;
		}


		virtual bool OnKeyInputPressedEvent(const SEvent& e)
		{
			if (m_Camera)
			{
				float amount = 5;
				vector3df pos = m_Camera->getPosition();
				switch (e.KeyInput.Key)
				{
				case KEY_KEY_W: pos.Y += amount; break;
				case KEY_KEY_S: pos.Y -= amount; break;
				case KEY_KEY_A: pos.X -= amount; break;
				case KEY_KEY_D: pos.X += amount; break;
				}
				m_Camera->setPosition(pos);
			}
			return FSGUI_Window::OnKeyInputPressedEvent(e);
		}


		virtual bool onMouseWheel(const SEvent& e)
		{
			if (m_Camera)
			{
				vector3df pos = m_Camera->getPosition();
				float amount = 1;
				if (e.MouseInput.Shift) amount = 10;

				if (e.MouseInput.Wheel > 0) pos.Z += amount;
				if (e.MouseInput.Wheel < 0) pos.Z -= amount;

				m_Camera->setPosition(pos);

				return true;
			}
			return FSGUI_Window::onMouseWheel(e);
		}

		virtual void draw()
		{
			if (!isFlagged(WS_VISIBLE)) return;

			FSGUI_Window::draw();

			if (m_Node)
			{
				rect<s32> r = getClientRect();
				r.UpperLeftCorner += 20;
				r.LowerRightCorner -= 20;
				rect<s32> oldViewport = getDesktop()->getDevice()->getVideoDriver()->getViewPort();
				m_Camera->setAspectRatio((f32)getClientRect().getWidth() / (f32)getClientRect().getHeight());
				m_Smgr->setActiveCamera(m_Camera);
				getDesktop()->getDevice()->getVideoDriver()->setViewPort(r);

				m_Smgr->drawAll();

				getDesktop()->getDevice()->getVideoDriver()->setViewPort(oldViewport);
			}
		}

		virtual void setFilename(stringc filename)
		{
			if (m_Node) m_Node->remove(); m_Node = 0;

			IAnimatedMesh* mesh = m_Smgr->getMesh(filename);
			if (!mesh) return;
			m_Node = m_Smgr->addAnimatedMeshSceneNode(mesh);
			if (m_Node)
			{
				m_FileName = filename;
				m_Node->setMaterialFlag(video::EMF_LIGHTING, false);
				m_Node->setMaterialFlag(video::EMF_FOG_ENABLE, false);
			}
		}

		void moveNodeTo(ISceneNode* node, rect<s32> r, float depth)
		{
			if (!node) return;

			// get the position of the screen element
			vector2d<s32> upperLeftCorner = r.UpperLeftCorner;
			vector2d<s32> lowerRightCorner = r.LowerRightCorner;

			// create two rays that go from the camera, into world space, 
			// one at the topleft corner and one at the bottom right corner of the gui element
			line3d<f32> upperLeftRay = m_Smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(upperLeftCorner);
			line3d<f32> lowerRightRay = m_Smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(lowerRightCorner);

			// create a plane that exists on the Z at 10 units
			plane3df const planeXZ(vector3df(0, 0, 10), vector3df(0.0f, 0.0f, 1.0f));
			vector3df intersectWithPlane;

			// find where the upperleft ray hits the plane
			if (planeXZ.getIntersectionWithLine(upperLeftRay.start, upperLeftRay.getVector(), intersectWithPlane))
				upperLeftRay.end = intersectWithPlane;
			// find where the lowerright ray hits the plane
			if (planeXZ.getIntersectionWithLine(lowerRightRay.start, lowerRightRay.getVector(), intersectWithPlane))
				lowerRightRay.end = intersectWithPlane;


			// scale the node between the two rays
			vector3df scale(lowerRightRay.end.X - upperLeftRay.end.X, lowerRightRay.end.Y - upperLeftRay.end.Y, 10);

			// the node position will be where the upperleft ray hits the plane
			// and at 10 units deep
			vector3df pos(upperLeftRay.end.X + scale.X / 2, upperLeftRay.end.Y + scale.Y / 2, 10 - depth);

			// set the node position and scale
			node->setPosition(pos);
			node->setScale(scale);
		}

		virtual FSGUI_DRAGDROPINFO* getDragDropInfo()
		{
			if (m_Node)
			{
				FSGUI_DRAGDROPINFO_FILENAME* ddi = new FSGUI_DRAGDROPINFO_FILENAME(getDesktop(), DDIT2_STATIC, getID(), getID(), FPT_STATICMODEL, SHORTMEDIAPATH(m_FileName));
				return ddi;
			}
			return NULL;
		}

		virtual bool acceptDragDropInfo(FSGUI_DRAGDROPINFO* ddi) 
		{
			if (ddi->getFrom() != getID())
			switch (ddi->getType())
			{
				case DDIT_FILENAME :
				{
					FSGUI_DRAGDROPINFO_FILENAME* tddi = dynamic_cast<FSGUI_DRAGDROPINFO_FILENAME*>(ddi);
					if (tddi)
					{
						switch (tddi->getFiletype())
						{
							case FPT_TEXTURE	: 
							{
								if (m_Node) m_Node->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(tddi->getFilename())));
							} break;
						
							case FPT_STATICMODEL:
							case FPT_ANIMATEDMODEL:
							{
								setFilename(MEDIAPATH(tddi->getFilename()));
							} break;
						}
					}
				} break;
			}
			return FSGUI_Window::acceptDragDropInfo(ddi);
		}
	};

} // end namespace