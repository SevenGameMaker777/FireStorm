// include the needed header files
#include "FSLevel.h"
#include "FSEngine.h"
#include "FSApplication.h"
#include "FSObjectFactory.h"
#include "FSObject.h"
#include "FSPhysX.h"

// unclutter the global namespace
namespace FS
{
	// unsafe object pointers
	IrrlichtDevice*		FSLevel::getDevice() { return getEngine()->getDevice(); }
	IGUIEnvironment*	FSLevel::getGui() { return getEngine()->getGui(); }
	IVideoDriver*		FSLevel::getDriver() { return getEngine()->getDriver(); }
	stringc				FSLevel::getDirectory(stringc dirName) { return getApp()->getDirectory(dirName); }

	// set all variables to a known value
	void FSLevel::initialize()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSLevel::initialize()");

		// initialize all variables
		m_Id = 0;
		m_Application = 0;
		m_Engine = 0;
		m_Smgr = 0;
		m_Camera = 0;
		m_ObjectFactory = 0;
		m_CollMan = 0;
		m_UseLight = false;
		m_UseFog = false;
		m_UseEditorVisuals = false;
		m_SelectedObject = 0;
		m_PreSelectedObject = 0;
		m_ObjectSelectType = FSOT_ALL;
		m_EditorMode = MODE_TRANSLATE;
		m_AutoRender = true;
		m_AmbientLight.set(0, 0, 0);
		m_ShowCameraPosition = false;
		m_PhysXWorld = 0;
		m_UseShadows = true;

	}

	// allows for better error handling
	bool FSLevel::create(FSApplication* app, int id)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSLevel::create()");

		// remember this object
		m_Id = id;
		m_Application = app;
		m_Engine = app->getEngine();

		// create the PhysXWorld
		m_PhysXWorld = new FSPhysXWorld();
		m_PhysXWorld->initialize();
		m_PhysXWorld->create(getApp()->getPhysXManager(), this);

		// create the object factory
		m_ObjectFactory = new FSObjectFactory();
		m_ObjectFactory->initialize();
		m_ObjectFactory->create(this);

		// get our own scenemanager
		m_Smgr = getEngine()->getSmgr()->createNewSceneManager();
		m_Smgr = getEngine()->getSmgr();

		// set the level fog
		setFogParams(m_FogParams);

		// get a pointer to this smgr's collision manager
		m_CollMan = m_Smgr->getSceneCollisionManager();

		m_Camera = getSmgr()->addCameraSceneNodeFPS();
		m_Camera->setIsDebugObject(true);

		// everything went fine
		return true;
	}

	// cleanup whatever memory mess we made
	bool FSLevel::cleanup()
	{
		// log this event
		FS_LOG(FSL_WARNING, "FSLevel::cleanup()");

		// safely delete the factory
		if (m_ObjectFactory) { m_ObjectFactory->cleanup(); delete(m_ObjectFactory); m_ObjectFactory = 0; }

		// drop the scenemanager
		if (getSmgr()) getSmgr()->drop();
		m_Smgr = 0;

		// delete the PhysXworld
		if (m_PhysXWorld) { m_PhysXWorld->cleanup(); delete(m_PhysXWorld); } m_PhysXWorld = 0;

		// forget this object
		m_Application = 0;
		m_Engine = 0;

		// always return false from a cleanup function
		return false;
	}

	// clear all items from the level
	void FSLevel::clear()
	{
		FS_LOG(FSL_ERROR, "Level Cleared");
		if (getObjectFactory()) getObjectFactory()->clear();
	}

	// called each frame
	void FSLevel::preFrame(const float & elapsedtime, bool forceupdate)
	{
		m_ElapsedTime = elapsedtime;
		if (getObjectFactory()) getObjectFactory()->preFrame(elapsedtime, forceupdate);
		if (getPhysXWorld()) getPhysXWorld()->preFrame(elapsedtime);
	}

	// called each frame
	void FSLevel::frame(const float & elapsedtime, bool forceupdate)
	{
		if (getObjectFactory()) getObjectFactory()->frame(elapsedtime, forceupdate);
		if (getPhysXWorld()) getPhysXWorld()->frame(elapsedtime);
		if (getAutoRender()) render();
	}

	// called each frame
	void FSLevel::postFrame(const float & elapsedtime, bool forceupdate)
	{
		if (getObjectFactory()) getObjectFactory()->postFrame(elapsedtime, forceupdate);
		if (getPhysXWorld()) getPhysXWorld()->postFrame(elapsedtime);

		// display the current camera position in the main window
		if (getShowCameraPosition())
		{
			stringc text("Camera Position ");
			text += vector3dfToStringc(getCamera()->getPosition());
			getEngine()->setWindowTitle(stringw(text));
		}
	}

	// let the level render the scene
	void FSLevel::render()
	{
		if (getSmgr()) getSmgr()->drawAll();
		if (getPhysXWorld()) getPhysXWorld()->renderDebugInfo();
	}

	// load the scene
	void FSLevel::load()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSLevel::load()");

		// make sure camera and whatnot get updated screen size
		SEvent e;
		e.EventType = EET_USER_EVENT;
		e.UserEvent.UserData1 = EVT_SCREENRESIZED;
		getDevice()->postEventFromUser(e);

		rect <s32> arr = getEngine()->getScreenRect();
		float ar = (float)arr.getWidth() / (float)arr.getHeight();
		if (getCamera()) getCamera()->setAspectRatio(ar);

		if (getSmgr()) getDevice()->setInputReceivingSceneManager(getSmgr());
		if (getPhysXWorld()) setGlobalPhysXWorld(getPhysXWorld());
	}

	// unlocad the scene
	void FSLevel::unload()
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSLevel::unload()");
		getDevice()->setInputReceivingSceneManager(getEngine()->getSmgr());
		setGlobalPhysXWorld(0);
	}

	bool FSLevel::onScreenResized(const SEvent & e)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSLevel::onScreenResized()");

		rect <s32> arr = getEngine()->getScreenRect();
		float ar = (float)arr.getWidth() / (float)arr.getHeight();
		if (getCamera()) getCamera()->setAspectRatio(ar);

		return FSEventHandler::onScreenResized(e);
	}

	bool FSLevel::saveToDisk(stringc filename)
	{
		FS_LOG(FSL_DEBUG, "%s %s", "saving level to file ", filename);

		IXMLWriter* writer = getDevice()->getFileSystem()->createXMLWriter(filename);
		if (!writer)
		{
			FS_LOG(FSL_WARNING, "Warning! unable to create save file");
			return false;
		}

		writer->writeXMLHeader();

		vector3df pos(0, 0, 0);
		vector3df tar(0, 0, 0);
		if (getSmgr()->getActiveCamera())
		{
			pos = getSmgr()->getActiveCamera()->getPosition();
			tar = getSmgr()->getActiveCamera()->getTarget();
		}

		// write the camera position and target
		writer->writeLineBreak();
		writer->writeElement(L"CAMERA", false,
			L"POSITION", stringw(vector3dfToStringc(pos)).c_str(),
			L"TARGET", stringw(vector3dfToStringc(tar)).c_str()
		);
		writer->writeLineBreak();

		writer->writeLineBreak();
		writer->writeElement(L"LIGHT", false,
			L"USE", stringw(boolToStringc(getUseLight())).c_str(),
			L"AMBIENTLIGHT", stringw(SColorfToStringc(m_AmbientLight)).c_str());
		writer->writeLineBreak();


		writer->writeLineBreak();
		writer->writeElement(L"FOG", false,
			L"USE", stringw(boolToStringc(getUseFog())).c_str(),
			L"COLOR", stringw(SColorToStringc(m_FogParams.m_Color)).c_str(),
			L"START", stringw(floatToStringc(m_FogParams.m_Start)).c_str(),
			L"END", stringw(floatToStringc(m_FogParams.m_End)).c_str());
		writer->writeLineBreak();

		// write the camera position and target
		writer->writeLineBreak();
		writer->writeElement(L"USESHADOWS", false,
			L"USESHADOWS", stringw(boolToStringc(m_UseShadows)).c_str(),
			L"", L"");
		writer->writeLineBreak();

		writer->writeLineBreak();


		FSObject* obj = getObjectFactory()->getObjectManager()->getNextObject(true);
		while (obj)
		{
			if (!obj->getIsDebugObject())
			{
				stringw name("FSOBJECT");
				writer->writeElement(name.c_str(), false, L"TYPE", stringw(obj->getInfo()->getName()).c_str());
				writer->writeLineBreak();

				IAttributes* attr = getDevice()->getFileSystem()->createEmptyAttributes(getDriver());
				SAttributeReadWriteOptions options;
				obj->serializeAttributes(attr, &options);

				if (attr->getAttributeCount() != 0)
				{
					attr->write(writer);
					writer->writeLineBreak();
				}

				attr->drop();

				writer->writeClosingTag(name.c_str());
				writer->writeLineBreak();
				writer->writeLineBreak();
			}
			obj = getObjectFactory()->getObjectManager()->getNextObject(false);
		}

		writer->drop();

		return true;
	}

	bool FSLevel::loadFromDisk(stringc filename)
	{
		FS_LOG(FSL_DEBUG, "%s %s", "loading level from file ", filename);

		IXMLReader* reader = getDevice()->getFileSystem()->createXMLReader(filename);
		if (!reader)
		{
			FS_LOG(FSL_WARNING, "Warning! unable to open save file");
			return false;
		}

		// read file
		while (reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				stringw name = reader->getNodeName();

				// if this is an object definition
				if (stringw("CAMERA") == name)
				{
					stringw pos = reader->getAttributeValueSafe(L"POSITION");
					stringw tar = reader->getAttributeValueSafe(L"TARGET");
					if (getSmgr()->getActiveCamera())
					{
						getSmgr()->getActiveCamera()->setPosition(stringcToVector3df(stringc(pos)));
						getSmgr()->getActiveCamera()->setTarget(stringcToVector3df(stringc(tar)));
					}
					else
					{
						FS_LOG(FSLOGTYPE::FSL_WARNING, "no camera in game save file");
						setCamera(getSmgr()->addCameraSceneNodeFPS());
						if (getSmgr()->getActiveCamera())
						{
							getSmgr()->getActiveCamera()->setPosition(stringcToVector3df(stringc(pos)));
							getSmgr()->getActiveCamera()->setTarget(stringcToVector3df(stringc(tar)));
						}
					}
				}

				// if this is an object definition
				if (stringw("LIGHT") == name)
				{
					stringw use = reader->getAttributeValueSafe(L"USE");
					stringw color = reader->getAttributeValueSafe(L"AMBIENTLIGHT");
					setUseLight(stringcToBool(use));
					setAmbientLight(stringcToSColorf(color));
				}

				// if this is an object definition
				if (stringw("FOG") == name)
				{
					stringw use = reader->getAttributeValueSafe(L"USE");
					stringw color = reader->getAttributeValueSafe(L"COLOR");
					stringw start = reader->getAttributeValueSafe(L"START");
					stringw end = reader->getAttributeValueSafe(L"END");
					m_FogParams.m_Color = stringcToSColor(color);
					m_FogParams.m_Start = stringcToFloat(start);
					m_FogParams.m_End = stringcToFloat(end);
					setFogParams(m_FogParams);
					setUseFog(stringcToBool(use));
				}

				// if this is an object definition
				if (stringw("USESHADOWS") == name)
				{
					stringw use = reader->getAttributeValueSafe(L"USESHADOWS");
					m_UseShadows = stringcToBool(use);
				}

				if (stringw("FSOBJECT") == name)
				{
					stringw type = reader->getAttributeValueSafe(L"TYPE");

					FSObject* obj = getObjectFactory()->createObjectByType(stringc(type), false, 0, stringc(type), true);
					if (obj)
					{
						IAttributes* attr = getDevice()->getFileSystem()->createEmptyAttributes(getDriver());
						attr->read(reader, false);
						obj->deserializeAttributes(attr);
						obj->reCreate();
						attr->drop();
					}
					else FS_LOG(FSL_WARNING, "Did not create object\n");
				}
			}
		}
		reader->drop();

		return true;
	}

	FSObject* FSLevel::selectObjectPhysX(ICameraSceneNode* cam, int screenX, int screenY, long ot)
	{
		if (getPhysXWorld())
		{
			int id = getPhysXWorld()->pickClosestObject(screenX, screenY, getCollisionManager(), cam, ot);
			return id > 0 ? getObjectFactory()->getObjectPointer(id, true) : NULL;
		}
		return NULL;
	}

	// pick a node using the mouse cursor location 
	ISceneNode* FSLevel::selectNode(int screenX, int screenY, long objectflags)
	{
		// log this event
		//FS_LOG(FSL_DEBUG, "%s %d %d","FSLevel::SelectNode()",screenX,screenY);

		// attempt to select a node. use the passed in mask
		ISceneNode* node = getCollisionManager()->getSceneNodeFromScreenCoordinatesBB(position2d<s32>(screenX, screenY), objectflags, true, 0);
		return node;
	}

	// pick an obejct (based on primaryscenenode) using the mouse cursor location 
	FSObject* FSLevel::selectObjectBySceneNode(int screenX, int screenY, long ot)
	{
		ISceneNode* node = getSceneNodeFromScreenCoordinatesBB(position2d<s32>(screenX, screenY), 0, true, 0, ot);
		FSObject* obj = getObjectFromNode(node);
		return obj;
	}

	FSObject* FSLevel::getObjectFromNode(ISceneNode* node)
	{
		// log this event
		FS_LOG(FSL_DEBUG, "FSLevel::getObjectFromNode()");

		if (!node)
		{
			FS_LOG(FSL_DEBUG, " no node selected");
			return NULL;
		}
		
//		int id = stringcToInt(node->getName());

		// seven - need to figure this out
		int id = node->getID();
		FS_LOG(FSL_DEBUG, "%s %d", " found object with ID", id);

		if (id == 0) return NULL;
		else return getObjectFactory()->getObjectPointer(id, true);
	}

	//! Returns the scene node, which is currently visible at the given
	//! screen coordinates, viewed from the currently active camera.
	ISceneNode* FSLevel::getSceneNodeFromScreenCoordinatesBB(
		const core::position2d<s32>& pos, s32 idBitMask, bool noDebugObjects, scene::ISceneNode* root, long ot)
	{
		const core::line3d<f32> ln = m_CollMan->getRayFromScreenCoordinates(pos, 0);
		if (ln.start == ln.end)	return 0;
		return getSceneNodeFromRayBB(ln, idBitMask, noDebugObjects, root, ot);
	}

	//! Returns the nearest scene node which collides with a 3d ray and
	//! which id matches a bitmask.
	ISceneNode* FSLevel::getSceneNodeFromRayBB(const line3d<f32>& ray, s32 idBitMask, bool noDebugObjects, scene::ISceneNode* root, long ot)
	{
		ISceneNode* best = 0;
		f32 dist = FLT_MAX;

		core::line3d<f32> truncatableRay(ray);

		getPickedNodeBB((root == 0) ? getSmgr()->getRootSceneNode() : root, truncatableRay, idBitMask, noDebugObjects, dist, best, ot);

		return best;
	}

	//! recursive method for going through all scene nodes
	void FSLevel::getPickedNodeBB(ISceneNode* root,
		core::line3df& ray, s32 bits, bool noDebugObjects,
		f32& outbestdistance, ISceneNode*& outbestnode, long ot)
	{
		const ISceneNodeList& children = root->getChildren();
		const core::vector3df rayVector = ray.getVector().normalize();

		ISceneNodeList::ConstIterator it = children.begin();
		for (; it != children.end(); ++it)
		{
			ISceneNode* current = *it;
			if ((noDebugObjects ? !current->isDebugObject() : true))
			{
				FSObject* obj = getObjectFromNode(current);
				if (!obj) return;
				if ((current->isVisible()) && (obj->isObjectType(ot)))
				{
					// Assume that single-point bounding-boxes are not meant for collision
					const core::aabbox3df & objectBox = current->getBoundingBox();
					if (objectBox.isEmpty())
						continue;

					// get world to object space transform
					core::matrix4 worldToObject;
					if (!current->getAbsoluteTransformation().getInverse(worldToObject))
						continue;

					// transform vector from world space to object space
					core::line3df objectRay(ray);
					worldToObject.transformVect(objectRay.start);
					worldToObject.transformVect(objectRay.end);

					// Do the initial intersection test in object space, since the
					// object space box test is more accurate.
					if (objectBox.isPointInside(objectRay.start))
					{
						// use fast bbox intersection to find distance to hitpoint
						// algorithm from Kay et al., code from gamedev.net
						const core::vector3df dir = (objectRay.end - objectRay.start).normalize();
						const core::vector3df minDist = (objectBox.MinEdge - objectRay.start) / dir;
						const core::vector3df maxDist = (objectBox.MaxEdge - objectRay.start) / dir;
						const core::vector3df realMin(core::min_(minDist.X, maxDist.X), core::min_(minDist.Y, maxDist.Y), core::min_(minDist.Z, maxDist.Z));
						const core::vector3df realMax(core::max_(minDist.X, maxDist.X), core::max_(minDist.Y, maxDist.Y), core::max_(minDist.Z, maxDist.Z));

						const f32 minmax = core::min_(realMax.X, realMax.Y, realMax.Z);
						// nearest distance to intersection
						const f32 maxmin = core::max_(realMin.X, realMin.Y, realMin.Z);

						const f32 toIntersectionSq = (maxmin>0 ? maxmin*maxmin : minmax*minmax);
						if (toIntersectionSq < outbestdistance)
						{
							outbestdistance = toIntersectionSq;
							outbestnode = current;

							// And we can truncate the ray to stop us hitting further nodes.
							ray.end = ray.start + (rayVector * sqrtf(toIntersectionSq));
						}
					}
					else
						if (objectBox.intersectsWithLine(objectRay))
						{
							// Now transform into world space, since we need to use world space
							// scales and distances.
							core::aabbox3df worldBox(objectBox);
							current->getAbsoluteTransformation().transformBox(worldBox);

							core::vector3df edges[8];
							worldBox.getEdges(edges);

							/* We need to check against each of 6 faces, composed of these corners:
							/3--------/7
							/  |      / |
							/   |     /  |
							1---------5  |
							|   2- - -| -6
							|  /      |  /
							|/        | /
							0---------4/

							Note that we define them as opposite pairs of faces.
							*/
							static const s32 faceEdges[6][3] =
							{
								{ 0, 1, 5 }, // Front
								{ 6, 7, 3 }, // Back
								{ 2, 3, 1 }, // Left
								{ 4, 5, 7 }, // Right
								{ 1, 3, 7 }, // Top
								{ 2, 0, 4 }  // Bottom
							};

							core::vector3df intersection;
							core::plane3df facePlane;
							f32 bestDistToBoxBorder = FLT_MAX;
							f32 bestToIntersectionSq = FLT_MAX;

							for (s32 face = 0; face < 6; ++face)
							{
								facePlane.setPlane(edges[faceEdges[face][0]],
									edges[faceEdges[face][1]],
									edges[faceEdges[face][2]]);

								// Only consider lines that might be entering through this face, since we
								// already know that the start point is outside the box.
								if (facePlane.classifyPointRelation(ray.start) != core::ISREL3D_FRONT)
									continue;

								// Don't bother using a limited ray, since we already know that it should be long
								// enough to intersect with the box.
								if (facePlane.getIntersectionWithLine(ray.start, rayVector, intersection))
								{
									const f32 toIntersectionSq = ray.start.getDistanceFromSQ(intersection);
									if (toIntersectionSq < outbestdistance)
									{
										// We have to check that the intersection with this plane is actually
										// on the box, so need to go back to object space again.
										worldToObject.transformVect(intersection);

										// find the closest point on the box borders. Have to do this as exact checks will fail due to floating point problems.
										f32 distToBorder = core::max_(core::min_(core::abs_(objectBox.MinEdge.X - intersection.X), core::abs_(objectBox.MaxEdge.X - intersection.X)),
											core::min_(core::abs_(objectBox.MinEdge.Y - intersection.Y), core::abs_(objectBox.MaxEdge.Y - intersection.Y)),
											core::min_(core::abs_(objectBox.MinEdge.Z - intersection.Z), core::abs_(objectBox.MaxEdge.Z - intersection.Z)));
										if (distToBorder < bestDistToBoxBorder)
										{
											bestDistToBoxBorder = distToBorder;
											bestToIntersectionSq = toIntersectionSq;
										}
									}
								}

								// If the ray could be entering through the first face of a pair, then it can't
								// also be entering through the opposite face, and so we can skip that face.
								if (!(face & 0x01))
									++face;
							}

							if (bestDistToBoxBorder < FLT_MAX)
							{
								outbestdistance = bestToIntersectionSq;
								outbestnode = current;

								// If we got a hit, we can now truncate the ray to stop us hitting further nodes.
								ray.end = ray.start + (rayVector * sqrtf(outbestdistance));
							}
						}
				}

				// Only check the children if this node is visible.
				getPickedNodeBB(current, ray, bits, noDebugObjects, outbestdistance, outbestnode, ot);
			}
		}
	}

	void FSLevel::setUseLight(bool value)
	{
		m_UseLight = value;
		if (getObjectFactory()) getObjectFactory()->setUseLight(value);
	}

	void FSLevel::setUseFog(bool value)
	{
		m_UseFog = value;
		if (getObjectFactory()) getObjectFactory()->setUseFog(value);
	}

	void FSLevel::setUseEditorVisuals(bool value)
	{
		m_UseEditorVisuals = value;
		if (getObjectFactory()) getObjectFactory()->setUseEditorVisuals(value);
	}

	// default shadow rendering
	void FSLevel::setUseShadows(bool v)
	{
		m_UseShadows = v;
		getObjectFactory()->getObjectManager()->setUseShadows(v);
	}

	void FSLevel::setAmbientLight(SColorf color)
	{
		m_AmbientLight = color;
		printf("setting AMBIENT lighting to %f %f %f %f\n", m_AmbientLight.r, m_AmbientLight.b, m_AmbientLight.g, m_AmbientLight.a);
		getSmgr()->setAmbientLight(m_AmbientLight);
	}

	void FSLevel::setFogParams(FS_FogParams params)
	{
		m_FogParams.m_Color = params.m_Color;
		m_FogParams.m_Start = params.m_Start;
		m_FogParams.m_End = params.m_End;
		getDriver()->setFog(params.m_Color, EFT_FOG_LINEAR, params.m_Start, params.m_End, 1, true, true);
	}

	void FSLevel::setId(int v)
	{
		m_Id = v;
	}

	void FSLevel::setCamera(ICameraSceneNode* v)
	{
		m_Camera = v;
	}

	float FSLevel::getDistanceBetween(FSObject* obj1, FSObject* obj2)
	{
		if (!obj1) return 99999999.99f;
		if (!obj2) return 99999999.99f;

		return abs(vector3df(obj1->getPosition() - obj2->getPosition()).getLength());
	}

	void FSLevel::setSelectedObject(int id)
	{
		FSObject* obj = getSelectedObjectPointer();
		if (obj) obj->setShowDebugInfo(false);
		m_SelectedObject = id;
		obj = getSelectedObjectPointer();
		if (obj) obj->setShowDebugInfo(true);

		// notify the application
		SEvent e;
		e.EventType = EET_USER_EVENT;
		e.UserEvent.UserData1 = EVT_OBJECT_SELECTED;
		e.UserEvent.UserData2 = id;
		getDevice()->postEventFromUser(e);
	}

	void FSLevel::setPreSelectedObject(int id)
	{
		if (id != getSelectedObject())
		{
			FSObject* obj = getObjectFactory()->getObjectPointer(m_PreSelectedObject, true);
			if (obj) obj->setShowDebugInfo(false);
			m_PreSelectedObject = id;
			obj = getObjectFactory()->getObjectPointer(m_PreSelectedObject, true);
			if (obj) obj->setShowDebugInfo(true);
		}
	}

	int FSLevel::getSelectedObject()
	{
		return m_SelectedObject;
	}

	FSObject* FSLevel::getSelectedObjectPointer()
	{
		if (getObjectFactory()) return getObjectFactory()->getObjectPointer(m_SelectedObject, true);
		return NULL;
	}


} // end namespace