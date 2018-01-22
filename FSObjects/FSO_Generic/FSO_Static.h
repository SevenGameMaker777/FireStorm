#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"

namespace FS
{
	class FSO_Static : public FSObject
	{
	private:
		IMeshSceneNode* m_Node;
		FS_VARIABLE_GET(stringc, MeshFilename);
		FS_VARIABLE_GET(stringc, TextureFilename);
		FS_VARIABLE_GET(stringc, NormalmapTextureFilename);
		FS_VARIABLE_GET(float, PlanerTextureResolution);
		FS_VARIABLE_GET(float, NormalmapAmplitude);
		FS_VARIABLE_GET(bool, Shadow);
	public:
		void setMeshFilename(stringc fn) { m_MeshFilename = fn; reCreate(); }
		void setTextureFilename(stringc fn) { m_TextureFilename = fn; reCreate(); }
		void setNormalmapTextureFilename(stringc fn) { m_NormalmapTextureFilename = fn; reCreate(); }
		void setPlanerTextureResolution(float v) { m_PlanerTextureResolution = v; reCreate(); }
		void setNormalmapAmplitude(float v) { m_NormalmapAmplitude = v; reCreate(); }
		void setShadow(bool fn) 
		{ 
			m_Shadow = fn; 
			reCreate(); 
		}

		ADD_PROPERTY(MeshFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(NormalmapTextureFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(PlanerTextureResolution, stringcToFloat, floatToStringc);
		ADD_PROPERTY(NormalmapAmplitude, stringcToFloat, floatToStringc);
		ADD_PROPERTY(Shadow, stringcToBool, boolToStringc);

	public:
		virtual ISceneNode* getPrimarySceneNode() { return m_Node; }

		virtual void destroyAllSceneNodes()
		{
			// remove the node
			if (m_Node) m_Node->remove(); m_Node = 0;

			// call the base class
			FSObject::destroyAllSceneNodes();
		}

		FSO_Static() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Static::FSO_Static()"); }
		virtual ~FSO_Static() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Static::~FSO_Static()"); }

		// set all variables to a known value
		virtual void initialize()
		{
			FSObject::initialize();

			m_Node = 0;
			m_MeshFilename = FS_DEFAULT;
			m_TextureFilename = FS_DEFAULT;
			m_NormalmapTextureFilename = FS_DEFAULT;
			m_PlanerTextureResolution = 0.001f;
			m_NormalmapAmplitude = 8.0f;
			m_Shadow = true;

			// default mesh to display
			m_MeshFilename = "_assets/models/_demo/torch/torch.b3d";
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Static::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_STATIC", "A simple static node", 1.00f, "Seven");

			REG_PROPERTY(FSO_Static, "MeshFilename", &FSO_Static::fsSetMeshFilename, &FSO_Static::fsGetMeshFilename, "FS_PCHAR", "mesh filename");
			REG_PROPERTY(FSO_Static, "TextureFilename", &FSO_Static::fsSetTextureFilename, &FSO_Static::fsGetTextureFilename, "FS_PCHAR", "Texture filename");
			REG_PROPERTY(FSO_Static, "NormalmapTextureFilename", &FSO_Static::fsSetNormalmapTextureFilename, &FSO_Static::fsGetNormalmapTextureFilename, "FS_PCHAR", "Texture filename");
			REG_PROPERTY(FSO_Static, "PlanerTextureResolution", &FSO_Static::fsSetPlanerTextureResolution, &FSO_Static::fsGetPlanerTextureResolution, "FS_FLOAT", "Planer Texture Resolution");
			REG_PROPERTY(FSO_Static, "NormalmapAmplitude", &FSO_Static::fsSetNormalmapAmplitude, &FSO_Static::fsGetNormalmapAmplitude, "FS_FLOAT", "Normalmap Amplitude");
			REG_PROPERTY(FSO_Static, "Shadow", &FSO_Static::fsSetShadow, &FSO_Static::fsGetShadow, "FS_BOOL", "does node cast Shadow");
			
			
			setObjectType(FSOT_OBJECT);

			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();


			printf("Static Object info \n");
			printf("MeshFilename %s\n", m_MeshFilename.c_str());
			printf("TextureFilename %s\n", m_TextureFilename.c_str());
			printf("NormalmapTextureFilename %s\n", m_NormalmapTextureFilename.c_str());
			printf("PlanerTextureResolution %f\n", m_PlanerTextureResolution);
			printf("NormalmapAmplitude %f\n", m_NormalmapAmplitude);
			printf("\n\n");

			FS_LOG(FSL_ERROR, "m_Shadow = %s", stringc(m_Shadow).c_str());

			// load a mesh 
			IAnimatedMesh* mesh = getSmgr()->getMesh(MEDIAPATH(getMeshFilename()));

			if (mesh)
			{
				// if normal mapped then setup correctly
				if (getNormalmapTextureFilename() != FS_DEFAULT)
				{
					getSmgr()->getMeshManipulator()->makePlanarTextureMapping(mesh->getMesh(0), getPlanerTextureResolution());
					ITexture* normalMap = getDriver()->getTexture(MEDIAPATH(getNormalmapTextureFilename().c_str()));
					if (normalMap) getDriver()->makeNormalMapTexture(normalMap, getNormalmapAmplitude());
					IMesh* tangentMesh = getSmgr()->getMeshManipulator()->createMeshWithTangents(mesh->getMesh(0),true,false,false,true);
					m_Node = getSmgr()->addMeshSceneNode(tangentMesh);
					if (m_Node)
					{
						m_Node->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(getTextureFilename().c_str())));
						m_Node->setMaterialTexture(1, normalMap);

						// Stones don't glitter..
						m_Node->getMaterial(0).SpecularColor.set(0, 0, 0, 0);
						m_Node->getMaterial(0).Shininess = 0.f;

						// adjust height for parallax effect
						m_Node->getMaterial(0).MaterialTypeParam = 1.f / 64.f;

						// drop mesh because we created it with a create.. call.
						tangentMesh->drop();

						m_Node->setMaterialType(video::EMT_PARALLAX_MAP_SOLID);
					}
				}
				else
				{
					m_Node = getSmgr()->addMeshSceneNode(mesh);
					
					// texture the node
					if (getTextureFilename() != FS_DEFAULT) m_Node->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(getTextureFilename())));
					m_Node->getMesh()->setHardwareMappingHint(EHM_STATIC);
					m_Node->getMaterial(0).SpecularColor.set(0, 0, 0, 0);
				}

				if (m_Node)
				{
					if ((getLevel()->getUseShadows()) && getShadow()) m_Node->addShadowVolumeSceneNode();
					m_Node->setMaterialFlag(video::EMF_LIGHTING, getLevel()->getUseLight());
					m_Node->setMaterialFlag(video::EMF_FOG_ENABLE, getLevel()->getUseFog());
					m_Node->setScale(getScale()*getScaleOffset());
				}
			}

			setId(getId());
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Static::serialize()");

			FSObject::serializeAttributes(out, options);
			out->addString("MeshFilename", m_MeshFilename.c_str());
			out->addString("TextureFilename", m_TextureFilename.c_str());
			out->addString("NormalmapTextureFilename", m_NormalmapTextureFilename.c_str());
			out->addFloat("PlanerTextureResolution", m_PlanerTextureResolution);
			out->addFloat("NormalmapAmplitude", m_NormalmapAmplitude);
			out->addBool("Shadow", m_Shadow);
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Static::deserialize()");

			FSObject::deserializeAttributes(in, options);
			m_MeshFilename = in->getAttributeAsString("MeshFilename");
			m_TextureFilename = in->getAttributeAsString("TextureFilename");
			m_NormalmapTextureFilename = in->getAttributeAsString("NormalmapTextureFilename");
			m_PlanerTextureResolution = in->getAttributeAsFloat("PlanerTextureResolution");
			m_NormalmapAmplitude = in->getAttributeAsFloat("NormalmapAmplitude");
			m_Shadow = in->getAttributeAsBool("Shadow");
		}

		virtual void createPhysXObject()
		{
			FSObject::createPhysXObject();

			if (!m_Node) return;

			IPhysXObjectData data;
			data.userdata = getId();
			data.type = POT_TREE;
			data.node = m_Node;
			data.mesh = m_Node->getMesh();
			data.position = getPosition();
			data.rotation = getRotation();
			data.scale = getScale();
			data.mass = 0;
			data.dynamic = false;
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}
	};

} // end namespace
