#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"

namespace FS
{
	/**
	Class for the constants set callback of the triplanar mapping shader.
	The texture setup is as follows:
	slots 0 to 2 are diffuse textures
	slots 3 to 5 are the normal maps
	1 and 4 are horizontal maps
	*/

	//constants class for the triplanar mapping shader

	class triplanarMappingShader6 : public irr::video::IShaderConstantSetCallBack
	{
		//All these constants are picked up from the material
		irr::video::SColorf ambient;
		irr::video::SColorf diffuse;
		irr::video::SColorf specular;
		irr::f32 specularPower;
		irr::f32 tileSize; //This tileSize parameter is taken from the material parameter, which is a float.
		irr::core::vector3df lightDirection;

	public:
		irr::scene::ICameraSceneNode* camera;

		triplanarMappingShader6()
		{
			//An initial light direction, it can be switched at will later.
			setlightDirection(irr::core::vector3df(1, 1, 1));
			//If no camera... default values, can be changed later.
			camera = 0;
		}


		virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
		{
			//We set the constants here. nothing too odd to explain.

			irr::core::matrix4 mat;
			irr::core::vector3df viewPos = irr::core::vector3df(0, 0, 0);
			irr::video::IVideoDriver* drv = services->getVideoDriver();

			mat = drv->getTransform(irr::video::ETS_PROJECTION);
			mat *= drv->getTransform(irr::video::ETS_VIEW);
			mat *= drv->getTransform(irr::video::ETS_WORLD);

			if (userData == 1) //If user data is 1, the driver is directX, else, it is openGL and this variable can be omitted
			{
				services->setVertexShaderConstant("matWorldViewProjection", mat.pointer(), 16);
			}

			mat = drv->getTransform(irr::video::ETS_WORLD);

			services->setVertexShaderConstant("matWorld", mat.pointer(), 16);

			if (camera)
			{
				viewPos = camera->getPosition();
			}

			services->setVertexShaderConstant("viewPos", &viewPos.X, 3);
			services->setVertexShaderConstant("tileSize", &tileSize, 1);
			services->setPixelShaderConstant("fvAmbient", &ambient.r, 4);
			services->setPixelShaderConstant("fvDiffuse", &diffuse.r, 4);
			services->setPixelShaderConstant("fvSpecular", &specular.r, 4);
			services->setPixelShaderConstant("fSpecularPower", &specularPower, 1);
			services->setPixelShaderConstant("lightDirection", &lightDirection.X, 3);

			if (userData == -1)//OpenGL
			{
				irr::s32 textureSampler;
				textureSampler = 0;
				services->setPixelShaderConstant("baseX", &textureSampler, 1);
				textureSampler = 1;
				services->setPixelShaderConstant("baseY", &textureSampler, 1);
				textureSampler = 2;
				services->setPixelShaderConstant("baseZ", &textureSampler, 1);
				textureSampler = 3;
				services->setPixelShaderConstant("normalX", &textureSampler, 1);
				textureSampler = 4;
				services->setPixelShaderConstant("normalY", &textureSampler, 1);
				textureSampler = 5;
				services->setPixelShaderConstant("normalZ", &textureSampler, 1);

				if (camera)
				{
					irr::f32 depth = camera->getFarValue();
					services->setPixelShaderConstant("camDepth", &depth, 1);
				}
			}
		}

		virtual void OnSetMaterial(const irr::video::SMaterial &material)
		{
			//Nothing too complex to explain here, it simply grabs the colors and parameters for the onSetConstants, 
			//previously converting them to float values

			ambient = material.AmbientColor;
			diffuse = material.DiffuseColor;
			specular = material.SpecularColor;
			specularPower = material.Shininess;
			tileSize = material.MaterialTypeParam;
		}

		void setlightDirection(irr::core::vector3df& lightDir)
		{
			lightDirection = lightDir;
			lightDirection.normalize();
		}
	};

	class FSO_Terrain_TriPlaner : public FSObject
	{
	private:
//		IMeshSceneNode * m_Terrain;
		ITerrainSceneNode* m_Terrain;

		FS_VARIABLE_GET(stringc, HeightmapFilename);// the heightmap file to use
		FS_VARIABLE_GET(stringc, TextureFilename);// the texture file to use
		FS_VARIABLE_GET(stringc, TextureFilename1);// the texture file to use
		FS_VARIABLE_GET(stringc, TextureFilename2);// the texture file to use
		FS_VARIABLE_GET(stringc, TextureFilename3);// the texture file to use
		FS_VARIABLE_GET(stringc, TextureFilename4);// the texture file to use
		FS_VARIABLE_GET(stringc, TextureFilename5);// the texture file to use
		FS_VARIABLE_GET(stringc, TextureFilename6);// the texture file to use
		FS_VARIABLE_GET(stringc, DetailmapFilename);// the detailmap file to use

		FS_VARIABLE_GET(SColor, Ambient);
		FS_VARIABLE_GET(SColor, Diffuse);
		FS_VARIABLE_GET(SColor, Specular);
		FS_VARIABLE_GET(float, SpecularPower);
		FS_VARIABLE_GET(float, TileSize);

		FS_VARIABLE_GET(int, TreeIt);// add trees
		array<int>			m_TreeID;				// id of all of the trees assocaited with this terrain

		FS_VARIABLE_SETGET(s32, ShaderIndex);
		FS_VARIABLE_SETGET(triplanarMappingShader6*, TriplanarConstants);
		FS_VARIABLE_SETGET(dimension2du,HeightmapDimension); // dimensions of the heightmap used to create the trrain

	public:
		void setHeightmapFilename(stringc fn) { m_HeightmapFilename = fn; reCreate(); }
		void setTextureFilename(stringc fn) { m_TextureFilename = fn; reCreate(); }
		void setTextureFilename1(stringc fn) { m_TextureFilename1 = fn; reCreate(); }
		void setTextureFilename2(stringc fn) { m_TextureFilename2 = fn; reCreate(); }
		void setTextureFilename3(stringc fn) { m_TextureFilename3 = fn; reCreate(); }
		void setTextureFilename4(stringc fn) { m_TextureFilename4 = fn; reCreate(); }
		void setTextureFilename5(stringc fn) { m_TextureFilename5 = fn; reCreate(); }
		void setTextureFilename6(stringc fn) { m_TextureFilename6 = fn; reCreate(); }
		void setDetailmapFilename(stringc fn) { m_DetailmapFilename = fn; reCreate(); }

		void setTileSize(float v) { m_TileSize = v; m_Terrain->getMaterial(0).MaterialTypeParam = v; }
		void setAmbient(SColor v) { m_Ambient = v; m_Terrain->getMaterial(0).AmbientColor = m_Ambient; }
		void setDiffuse(SColor v) { m_Diffuse = v; m_Terrain->getMaterial(0).DiffuseColor = m_Diffuse; }
		void setSpecular(SColor v) { m_Specular = v; m_Terrain->getMaterial(0).SpecularColor = m_Specular; }
		void setSpecularPower(float v) { m_SpecularPower = v; m_Terrain->getMaterial(0).Shininess = m_SpecularPower; }

		ADD_PROPERTY(HeightmapFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename1, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename2, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename3, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename4, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename5, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename6, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(DetailmapFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TreeIt, stringcToInt, intToStringc);

		ADD_PROPERTY(TileSize, stringcToFloat, floatToStringc);
		ADD_PROPERTY(Ambient, stringcToSColor, SColorToStringc);
		ADD_PROPERTY(Diffuse, stringcToSColor, SColorToStringc);
		ADD_PROPERTY(Specular, stringcToSColor, SColorToStringc);
		ADD_PROPERTY(SpecularPower, stringcToFloat, floatToStringc);

		ADD_PROPERTY(HeightmapDimension, stringcToDimension2du, dimension2duToStringc);

		int USESHADER = 1;

	public:
		virtual ISceneNode* getPrimarySceneNode() { return m_Terrain; }

		virtual void destroyAllSceneNodes()
		{
			if (m_Terrain) m_Terrain->remove(); m_Terrain = 0;
			if (m_TriplanarConstants) m_TriplanarConstants->drop(); m_TriplanarConstants = 0;
		}

		FSO_Terrain_TriPlaner() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Terrain_TriPlaner::FSO_Terrain_TriPlaner()"); }
		virtual ~FSO_Terrain_TriPlaner() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Terrain_TriPlaner::~FSO_Terrain_TriPlaner()"); }

		// set all variables to a known value
		virtual void initialize()
		{
			FSObject::initialize();

			m_Terrain = 0;
			m_TriplanarConstants = 0;
			m_DetailmapFilename = FS_DEFAULT;
			m_HeightmapFilename = FS_DEFAULT;
			m_TextureFilename = FS_DEFAULT;
			m_TextureFilename1 = FS_DEFAULT;
			m_TextureFilename2 = FS_DEFAULT;
			m_TextureFilename3 = FS_DEFAULT;
			m_TextureFilename4 = FS_DEFAULT;
			m_TextureFilename5 = FS_DEFAULT;
			m_TextureFilename6 = FS_DEFAULT;
			setScaleValue(vector3df(40, 3, 40));

			m_TileSize = 200;
			m_Ambient = SColor(255, 100, 100, 100);
			m_Diffuse = SColor(255, 118, 132, 204);
			m_Specular = SColor(255, 15,15,15);
			m_SpecularPower = 0.15;

		}

		void preFrame(const float &elapsedtime, bool forceupdate)
		{
			FSObject::preFrame(elapsedtime, forceupdate);
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Terrain::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(3, "FSO_TERRAIN_TRIPLANER", "A simple static terrain with physx attached", 1.00f, "Seven");

			REG_PROPERTY(FSO_Terrain_TriPlaner, "HeightmapFilename", &FSO_Terrain_TriPlaner::fsSetHeightmapFilename, &FSO_Terrain_TriPlaner::fsGetHeightmapFilename, "FS_TEXTURE", "heightmap filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TextureFilename", &FSO_Terrain_TriPlaner::fsSetTextureFilename, &FSO_Terrain_TriPlaner::fsGetTextureFilename, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TextureFilename1", &FSO_Terrain_TriPlaner::fsSetTextureFilename1, &FSO_Terrain_TriPlaner::fsGetTextureFilename1, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TextureFilename2", &FSO_Terrain_TriPlaner::fsSetTextureFilename2, &FSO_Terrain_TriPlaner::fsGetTextureFilename2, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TextureFilename3", &FSO_Terrain_TriPlaner::fsSetTextureFilename3, &FSO_Terrain_TriPlaner::fsGetTextureFilename3, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TextureFilename4", &FSO_Terrain_TriPlaner::fsSetTextureFilename4, &FSO_Terrain_TriPlaner::fsGetTextureFilename4, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TextureFilename5", &FSO_Terrain_TriPlaner::fsSetTextureFilename5, &FSO_Terrain_TriPlaner::fsGetTextureFilename5, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TextureFilename6", &FSO_Terrain_TriPlaner::fsSetTextureFilename6, &FSO_Terrain_TriPlaner::fsGetTextureFilename6, "FS_TEXTURE", "Texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "DetailmapFilename", &FSO_Terrain_TriPlaner::fsSetDetailmapFilename, &FSO_Terrain_TriPlaner::fsGetDetailmapFilename, "FS_TEXTURE", "detailmap texture filename");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "TreeIt", &FSO_Terrain_TriPlaner::fsSetTreeIt, &FSO_Terrain_TriPlaner::fsGetTreeIt, "FS_INT", "add a number of trees to the terrain");

			REG_PROPERTY(FSO_Terrain_TriPlaner, "TileSize", &FSO_Terrain_TriPlaner::fsSetTileSize, &FSO_Terrain_TriPlaner::fsGetTileSize, "FS_FLOAT", "terrain texture Tilesize");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "Ambient", &FSO_Terrain_TriPlaner::fsSetAmbient, &FSO_Terrain_TriPlaner::fsGetAmbient, "FS_COLOR", "terrain texture Tilesize");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "Diffuse", &FSO_Terrain_TriPlaner::fsSetDiffuse, &FSO_Terrain_TriPlaner::fsGetDiffuse, "FS_COLOR", "terrain texture Tilesize");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "Specular", &FSO_Terrain_TriPlaner::fsSetSpecular, &FSO_Terrain_TriPlaner::fsGetSpecular, "FS_COLOR", "terrain exture Tilesize");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "SpecularPower", &FSO_Terrain_TriPlaner::fsSetSpecularPower, &FSO_Terrain_TriPlaner::fsGetSpecularPower, "FS_FLOAT", "terrain texture Tilesize");
			REG_PROPERTY(FSO_Terrain_TriPlaner, "HeightmapDimensions", &FSO_Terrain_TriPlaner::fsSetHeightmapDimension, &FSO_Terrain_TriPlaner::fsGetHeightmapDimension, "FS_DIMENSION2DU", "size of heightmap image");

			m_Terrain = 0;
			m_HeightmapFilename = "_assets/terrain/terrain-plains.jpg";
			m_DetailmapFilename = "detailmap3.jpg";
			m_TextureFilename1 = "triplanerterrain/data/rock.png";
			m_TextureFilename2 = "triplanerterrain/data/grass.png";
			m_TextureFilename3 = "triplanerterrain/data/dirt.png";
			m_TextureFilename4 = "triplanerterrain/data/rockn.png";
			m_TextureFilename5 = "triplanerterrain/data/grassn.png";
			m_TextureFilename6 = "triplanerterrain/data/rockn.png";

			setObjectType(FSOT_TERRAIN);

			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();

			FS_LOG(FSL_DEBUG, "  loading terrain : ");
			FS_LOG(FSL_DEBUG, "  Heightmap - %s", MEDIAPATH(getHeightmapFilename()));
			FS_LOG(FSL_DEBUG, "  Texture   - %s", MEDIAPATH(getTextureFilename1()));
			FS_LOG(FSL_DEBUG, "  Detailmap - %s", MEDIAPATH(getDetailmapFilename()));

			//Terrain creation.
//			ITerrainSceneNode* terrain = getSmgr()->addTerrainSceneNode(MEDIAPATH(getHeightmapFilename()),
//				getSmgr()->getRootSceneNode(), -1, getPosition(), getRotation(), getScale(),
//				irr::video::SColor(255, 255, 255, 255), 5, irr::scene::ETPS_17, 2);

			//Terrain creation.
			m_Terrain = getSmgr()->addTerrainSceneNode(MEDIAPATH(getHeightmapFilename()),
				getSmgr()->getRootSceneNode(), -1, getPosition(), getRotation(), getScale(),
				irr::video::SColor(255, 255, 255, 255), 5, irr::scene::ETPS_17, 2);

			ITexture* t = getDriver()->getTexture(MEDIAPATH(m_HeightmapFilename));
			setHeightmapDimension(t->getSize());

//			convertToMesh(terrain);

//			terrain->remove();


			if (USESHADER)
			{
				//shader compilation
				m_TriplanarConstants = new triplanarMappingShader6();
	
				//The constants need a camera...
				m_TriplanarConstants->camera = getLevel()->getCamera();

				//The actual shader compilation
				irr::core::stringc filenameV, filenameP;
				filenameV = MEDIAPATH("triplanerterrain/data/triplanarMappingV");
				filenameP = MEDIAPATH("triplanerterrain/data/triplanarMapping6TP");

				filenameV += irr::core::stringc(".hlsl");
				filenameP += irr::core::stringc(".hlsl");

				m_ShaderIndex = getDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
					filenameV, "main", irr::video::EVST_VS_3_0,filenameP, "main", irr::video::EPST_PS_3_0, m_TriplanarConstants, EMT_SOLID, 1);

				//We set the shader, the textures and the colors...
				m_Terrain->getMaterial(0).MaterialType = (irr::video::E_MATERIAL_TYPE) m_ShaderIndex;
				m_Terrain->getMaterial(0).AmbientColor = m_Ambient;
				m_Terrain->getMaterial(0).DiffuseColor = m_Diffuse;
				m_Terrain->getMaterial(0).SpecularColor = m_Specular;
				m_Terrain->getMaterial(0).Shininess = 0.15f;
				m_Terrain->getMaterial(0).setTexture(0, getDriver()->getTexture(MEDIAPATH(m_TextureFilename1)));
				m_Terrain->getMaterial(0).setTexture(1, getDriver()->getTexture(MEDIAPATH(m_TextureFilename2)));
				m_Terrain->getMaterial(0).setTexture(2, getDriver()->getTexture(MEDIAPATH(m_TextureFilename3)));
				m_Terrain->getMaterial(0).setTexture(3, getDriver()->getTexture(MEDIAPATH(m_TextureFilename4)));
				m_Terrain->getMaterial(0).setTexture(4, getDriver()->getTexture(MEDIAPATH(m_TextureFilename5)));
				m_Terrain->getMaterial(0).setTexture(5, getDriver()->getTexture(MEDIAPATH(m_TextureFilename6)));
				m_Terrain->getMaterial(0).MaterialTypeParam = m_TileSize;//tile size!
				m_TriplanarConstants->setlightDirection(irr::core::vector3df(-0.1f, 1.0f, 0.1f));
			}
			else
			{
				m_Terrain->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(m_TextureFilename)));
				m_Terrain->setMaterialTexture(1, getDriver()->getTexture(MEDIAPATH(m_DetailmapFilename)));
				m_Terrain->setMaterialType(video::EMT_DETAIL_MAP);
				m_Terrain->setMaterialFlag(video::EMF_LIGHTING, getLevel()->getUseLight());
				m_Terrain->setMaterialFlag(video::EMF_FOG_ENABLE, getLevel()->getUseFog());
			}
			setId(getId());
		}

/*
		virtual void createPhysXObject()
		{
			FSObject::createPhysXObject();

			// make sure the node is valid
			if (!m_Terrain) { FS_LOG(FSL_DEBUG, "FSO_Terrain_TriPlaner-- no node to create PhysX object from"); return; }

			// create a PhysX object to represent this node
			IPhysXObjectData data;
			data.type = POT_TREE;
			data.node = m_Terrain;
			data.mesh = m_Terrain->getMesh();
			data.position = getPosition();
			data.rotation = getRotation();
			data.scale = getScale() * getScaleOffset();
			data.mass = INFINITY;
			data.gravity = false;
			data.bboffset = vector3df(0, 0, 0);
			data.userdata = getId();
			data.dynamic = false;
			data.objecttype = FSOT_STATIC;
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}
*/

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


		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Terrain::serialize()");

			FSObject::serializeAttributes(out, options);
			out->addString("HeightmapFileName", getHeightmapFilename().c_str());
			out->addString("DetailFileName", m_DetailmapFilename.c_str());
			out->addString("TextureFileName", m_TextureFilename.c_str());
			out->addString("TextureFileName1", m_TextureFilename1.c_str());
			out->addString("TextureFileName2", m_TextureFilename2.c_str());
			out->addString("TextureFileName3", m_TextureFilename3.c_str());
			out->addString("TextureFileName4", m_TextureFilename4.c_str());
			out->addString("TextureFileName5", m_TextureFilename5.c_str());
			out->addString("TextureFileName6", m_TextureFilename6.c_str());
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
			m_TextureFilename1 = in->getAttributeAsString("TextureFileName1");
			m_TextureFilename2 = in->getAttributeAsString("TextureFileName2");
			m_TextureFilename3 = in->getAttributeAsString("TextureFileName3");
			m_TextureFilename4 = in->getAttributeAsString("TextureFileName4");
			m_TextureFilename5 = in->getAttributeAsString("TextureFileName5");
			m_TextureFilename6 = in->getAttributeAsString("TextureFileName6");
		}

		virtual void setTreeIt(int count)
		{
			if (count)
			{
				//Terrain creation.
				ITerrainSceneNode* m_Terrain = getSmgr()->addTerrainSceneNode(MEDIAPATH(getHeightmapFilename()),
					getSmgr()->getRootSceneNode(), -1, getPosition(), getRotation(), getScale(),
					irr::video::SColor(255, 255, 255, 255), 5, irr::scene::ETPS_17, 2);

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
							obj->setScale(vector3df(2, 2, 2));

							vector3df pos = m_Terrain->getPosition();
							pos.X = x;
							pos.Z = z;
							float tempY = m_Terrain->getHeight(x, z);
							pos.Y = tempY + obj->getBBOffset().Y;
							obj->setPosition(pos);
						}
					}
				}
				m_Terrain->remove();
			}
		}

/*
		void convertToMesh(ITerrainSceneNode* terrain)
		{
			// make sure that we have a valid scenenode
			if (!terrain) { FS_LOG(FSL_WARNING, "WARNING createMesh() terrain node is not valid"); return; }

			// temporary variable
			CDynamicMeshBuffer buffer(EVT_2TCOORDS, EIT_16BIT);

			// get pointer to dynamic buffer for LOD 0
			terrain->getMeshBufferForLOD(buffer, 0);

			// vertex and index count
			int vertexcount = buffer.getVertexCount();
			int indexcount = buffer.getIndexCount();

			// get a pointer to the vertices
			S3DVertex2TCoords* vertices = (S3DVertex2TCoords*)buffer.getVertices();

			// get a pointer to the idices
			u16* indices = buffer.getIndices();

			// create a temporary mesh
			scene::SMeshBuffer *meshbuffer = new scene::SMeshBuffer();

			// grab the vertices and indices
			for (int y = 0; y<vertexcount; y++)	meshbuffer->Vertices.push_back(vertices[y]);
			for (int i = 0; i<indexcount; i++)	meshbuffer->Indices.push_back(indices[i]);

			//recalculate the bounding box;
			meshbuffer->recalculateBoundingBox();

			//use VBO
			meshbuffer->setHardwareMappingHint(scene::EHM_STATIC);

			// set it dirty
			meshbuffer->setDirty();

			// create a temporary mesh
			SMesh* mesh = new SMesh;

			// add the meshbuffer to it
			mesh->addMeshBuffer(meshbuffer);

			//recalculate the bounding box;
			mesh->recalculateBoundingBox();

			// create the actual node
			m_Terrain = getSmgr()->addMeshSceneNode(mesh, terrain->getParent(), terrain->getID(), terrain->getPosition(), terrain->getRotation(), getScale());

			// cleanup our memory mess
			mesh->drop();
			meshbuffer->drop();
		}
*/
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
							obj->setScale(vector3df(2, 2, 2));

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

		// terrain is too big, do not show debug info
		virtual void setShowDebugInfo(bool value)
		{
		}

};

} // end namespace#pragma once
