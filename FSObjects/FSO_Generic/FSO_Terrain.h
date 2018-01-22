#pragma once

#include "FSO_Generic.h"
#include "FSLevel.h"

namespace FS
{
	/*
	a simple cube object
	adds texturefilename to supply texture to the scenenode
	*/
	class FSO_Terrain : public FSObject
	{
	private:
		// object variables
		ITerrainSceneNode * m_Terrain;				// pointer to irrlicht terrain scenenode
		stringc				m_HeightmapFilename;	// the heightmap file to use
		stringc				m_TextureFilename;		// the texture file to use
		stringc				m_DetailmapFilename;	// the detailmap file to use
		vector3df			m_TextureScale;
		bool				m_Treeit;				// add random trees to the terrain
		array<int>			m_TreeID;				// id of all of the trees assocaited with this terrain
		dimension2du		m_HeightmapDimension;	// dimensions of the heightmap used to create the trrain
	public:
		// simple setters and getters 
		void setHeightmapFilename(stringc fn) { m_HeightmapFilename = fn; reCreate(); }
		void setTextureFilename(stringc fn) { m_TextureFilename = fn; reCreate(); }
		void setDetailmapFilename(stringc fn) { m_DetailmapFilename = fn; reCreate(); }
		void setTextureScale(vector3df v) { m_TextureScale = v; if (m_Terrain) m_Terrain->scaleTexture(m_TextureScale.X, m_TextureScale.Z); };
		void setHeightmapDimension(dimension2du size) { m_HeightmapDimension = size; }	// dimensions of the heightmap used to create the trrain

		stringc	getHeightmapFilename() { return m_HeightmapFilename; };	// the heightmap file to use
		stringc	getTextureFilename() { return m_TextureFilename; };		// the texture file to use
		stringc	getDetailmapFilename() { return m_DetailmapFilename;  };	// the detailmap file to use
		vector3df getTextureScale() { return m_TextureScale; };
		bool		getTreeit() { return 0; }
		dimension2du	getHeightmapDimension() { return m_HeightmapDimension; }	// dimensions of the heightmap used to create the trrain

	public:
		// object properties
		ADD_PROPERTY(HeightmapFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(DetailmapFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureScale, stringcToVector3df, vector3dfToStringc);
		ADD_PROPERTY(Treeit, stringcToInt, intToStringc);
		ADD_PROPERTY(HeightmapDimension, stringcToDimension2du, dimension2duToStringc);

	public:
		// all obejcts must override this function
		virtual ISceneNode* getPrimarySceneNode() { return m_Terrain; }

		// override this one to destroy all node
		virtual void destroyAllSceneNodes()
		{
			if (m_Terrain) m_Terrain->remove(); m_Terrain = 0;

			for (int x = 0; x < m_TreeID.size(); x++)
			{
				FSObject* obj = getLevel()->getObjectPointer(m_TreeID[x],true);
				if (obj) obj->setDead(true);
			}
			m_TreeID.clear();
		}

		// set all variables to a known value
		virtual void initialize()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::initialize()");

			// call the base class
			FSObject::initialize();

			// set these to a knwon value
			m_Terrain = 0;
			m_DetailmapFilename = FS_DEFAULT;
			m_HeightmapFilename = FS_DEFAULT;
			m_TextureFilename = FS_DEFAULT;
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Terrain::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(3, "FSO_TERRAIN", "A simple static terrain with physx attached", 1.00f, "Seven");

			REG_PROPERTY(FSO_Terrain, "HeightmapFilename", &FSO_Terrain::fsSetHeightmapFilename, &FSO_Terrain::fsGetHeightmapFilename, "FS_TEXTURE", "heightmap filename");
			REG_PROPERTY(FSO_Terrain, "TextureFilename", &FSO_Terrain::fsSetTextureFilename, &FSO_Terrain::fsGetTextureFilename, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain, "DetailmapFilename", &FSO_Terrain::fsSetDetailmapFilename, &FSO_Terrain::fsGetDetailmapFilename, "FS_TEXTURE", "detailmap texture filename");
			REG_PROPERTY(FSO_Terrain, "TextureScale", &FSO_Terrain::fsSetTextureScale, &FSO_Terrain::fsGetTextureScale, "FS_VEC3D", "texture scale x,z");
			REG_PROPERTY(FSO_Terrain, "Treeit", &FSO_Terrain::fsSetTreeit, &FSO_Terrain::fsGetTreeit, "FS_INT", "add random trees to the terrain");
			REG_PROPERTY(FSO_Terrain, "HeightmapDimensions", &FSO_Terrain::fsSetHeightmapDimension, &FSO_Terrain::fsGetHeightmapDimension, "FS_DIMENSION2DU", "size of heightmap image");

			m_Terrain = 0;
			m_HeightmapFilename = "_assets/terrain/terrain.jpg";
			m_TextureFilename = "terrain-texture.jpg";
			m_DetailmapFilename = "detailmap3.jpg";

			setScaleValue(vector3df(40, 3, 40));
			setTextureScale(vector3df(20, 20, 20));
			setObjectType(FSOT_TERRAIN);

			// everything went fine
			return true;
		}

		virtual bool reCreate()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Cube::reCreate()");

			// call the base class
			FSObject::reCreate();

			// recursively set the ids for this object
			setId(getId());

			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();

			FS_LOG(FSL_DEBUG, "  loading terrain : ");
			FS_LOG(FSL_DEBUG, "  Heightmap - %s", MEDIAPATH(m_HeightmapFilename));
			FS_LOG(FSL_DEBUG, "  Texture   - %s", MEDIAPATH(m_TextureFilename));
			FS_LOG(FSL_DEBUG, "  Detailmap - %s", MEDIAPATH(m_DetailmapFilename));

			// add terrain scene node
			m_Terrain = getSmgr()->addTerrainSceneNode(
				MEDIAPATH(m_HeightmapFilename),
				0,									// parent node
				getId(),							// node id
				getPosition(),						// position
				getRotation(),						// rotation
				getScale() + getScaleOffset(),		// scale
				video::SColor(255, 255, 255, 255),  // vertexColor
				5,									// maxLOD
				scene::ETPS_17,						// patchSize
				4									// smoothFactor
			);

			ITexture* t = getDriver()->getTexture(MEDIAPATH(m_HeightmapFilename));
			setHeightmapDimension(t->getSize());

			if (!m_Terrain) { FS_LOG(FSL_DEBUG, "Warning : unable to load terrain"); return; }

			m_Terrain->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(m_TextureFilename)));
			m_Terrain->setMaterialTexture(1, getDriver()->getTexture(MEDIAPATH(m_DetailmapFilename)));
			m_Terrain->setMaterialType(video::EMT_DETAIL_MAP);
			m_Terrain->scaleTexture(1.0f, 20.0f);
			m_Terrain->setMaterialFlag(video::EMF_LIGHTING, getLevel()->getUseLight());
			m_Terrain->setMaterialFlag(video::EMF_FOG_ENABLE, getLevel()->getUseFog());

			setId(getId());
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Terrain::serialize()");

			FSObject::serializeAttributes(out, options);
			out->addString("HeightmapFileName", m_HeightmapFilename.c_str());
			out->addString("DetailFileName", m_DetailmapFilename.c_str());
			out->addString("TextureFileName", m_TextureFilename.c_str());
			out->addVector3d("TextureScale", m_TextureScale);
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Terrain::deserialize()");

			FSObject::deserializeAttributes(in, options);
			m_HeightmapFilename = in->getAttributeAsString("HeightmapFileName");
			m_DetailmapFilename = in->getAttributeAsString("DetailFileName");
			m_TextureFilename = in->getAttributeAsString("TextureFileName");
			m_TextureScale = in->getAttributeAsVector3d("TextureScale");
		}

		virtual void setTreeit(int count)
		{
			if (count)
			{
				if (m_Terrain)
				{
					float buffer = 0;

					float w = m_Terrain->getBoundingBox().getExtent().X;
					float h = m_Terrain->getBoundingBox().getExtent().Z;
					for (int index = 0; index<count; index++)
					{
						float x = getRandomFloat(buffer, w - buffer);
						float z = getRandomFloat(buffer, h - buffer);
						FSObject* obj = getLevel()->getObjectFactory()->createObjectByType("FSO_TREE", true, 0, "Tree", true);
						if (obj)
						{
							obj->setVariable(L"Style", stringc(getRandomInteger(1, 4) - 1));
							obj->setScale(vector3df(2,2,2));

							vector3df pos = m_Terrain->getPosition();
							pos.X = x;
							pos.Z = z;
							float tempY = m_Terrain->getHeight(x, z);
							pos.Y = tempY + obj->getBBOffset().Y;
							obj->setPosition(pos);
							m_TreeID.push_back(obj->getId());
						}
					}
				}
			}
		}

		virtual void setUseLight(bool value)
		{
			if (m_Terrain) m_Terrain->setMaterialFlag(EMF_LIGHTING, value);
		}

		virtual void createPhysXObject()
		{
			FSObject::createPhysXObject();

			// make sure the node is valid
			if (!m_Terrain) { FS_LOG(FSL_DEBUG, "FSO_Terrain-- no node to create PhysX object from"); return; }

			// create a PhysX object to represent this node
			IPhysXObjectData data;
			data.type = POT_TERRAIN;
			data.node = m_Terrain;
			data.position = getPosition();
			data.rotation = getRotation();
			data.scale = getScale() * getScaleOffset();
			data.mass = INFINITY;
			data.gravity = false;
			data.bboffset = vector3df(0, 0, 0);
			data.userdata = getId();
			data.dynamic = false;
			data.objecttype = FSOT_TERRAIN;
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}

		// terrain is too big, do not show debug info
		virtual void setShowDebugInfo(bool value)
		{
		}
	};

} // end namespace