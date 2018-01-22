#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"

namespace FS
{
	class FSO_AnimatedMesh : public FSObject
	{
	private:
		IAnimatedMeshSceneNode* m_Node;
		FS_VARIABLE_GET(stringc, MeshFilename);
		FS_VARIABLE_GET(stringc, TextureFilename);
		FS_VARIABLE_GET(int, AnimationSpeed);
		FS_VARIABLE_GET(bool, Shadow);
	public:
		void setMeshFilename(stringc fn) { m_MeshFilename = fn; reCreate(); }
		void setTextureFilename(stringc fn) { m_TextureFilename = fn; reCreate(); }
		void setAnimationSpeed(int fn) { m_AnimationSpeed = fn; reCreate(); }
		void setShadow(bool fn) { m_Shadow = fn; reCreate(); }

		ADD_PROPERTY(MeshFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(TextureFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(AnimationSpeed, stringcToInt, intToStringc);
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

		FSO_AnimatedMesh() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_AnimatedMesh::FSO_AnimatedMesh()"); }
		virtual ~FSO_AnimatedMesh() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_AnimatedMesh::~FSO_AnimatedMesh()"); }

		// set all variables to a known value
		virtual void initialize()
		{
			FSObject::initialize();

			m_Node = 0;
			m_MeshFilename = FS_DEFAULT;
			m_TextureFilename = FS_DEFAULT;
			m_Shadow = true;
			m_AnimationSpeed = 0;

			// default mesh to display
			m_MeshFilename = "sydney.md2";
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_AnimatedMesh::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_ANIMATEDMESH", "A simple static node", 1.00f, "Seven");

			REG_PROPERTY(FSO_AnimatedMesh, "MeshFilename", &FSO_AnimatedMesh::fsSetMeshFilename, &FSO_AnimatedMesh::fsGetMeshFilename, "FS_PCHAR", "mesh filename");
			REG_PROPERTY(FSO_AnimatedMesh, "TextureFilename", &FSO_AnimatedMesh::fsSetTextureFilename, &FSO_AnimatedMesh::fsGetTextureFilename, "FS_PCHAR", "Texture filename");
			REG_PROPERTY(FSO_AnimatedMesh, "AnimationSpeed", &FSO_AnimatedMesh::fsSetAnimationSpeed, &FSO_AnimatedMesh::fsGetAnimationSpeed, "FS_INT", "Animation Speed");
			REG_PROPERTY(FSO_AnimatedMesh, "Shadow", &FSO_AnimatedMesh::fsSetShadow, &FSO_AnimatedMesh::fsGetShadow, "FS_BOOL", "does node cast Shadow");

			setObjectType(FSOT_OBJECT);

			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();

			// load a mesh 
			IAnimatedMesh* mesh = getSmgr()->getMesh(MEDIAPATH(getMeshFilename()));

			if (mesh)
			{
				// create a node
				m_Node = getSmgr()->addAnimatedMeshSceneNode(mesh, 0, getId(), getPosition(), getRotation(), getScale());

				if (m_Node)
				{
					// texture the node
					if (getTextureFilename() != FS_DEFAULT)	m_Node->setMaterialTexture(0, getDriver()->getTexture(MEDIAPATH(getTextureFilename())));

					// set the node material parameters
					m_Node->setMaterialFlag(video::EMF_LIGHTING, getLevel()->getUseLight());
					m_Node->setMaterialFlag(video::EMF_FOG_ENABLE, getLevel()->getUseFog());

					if (getShadow()) m_Node->addShadowVolumeSceneNode();
					if (getAnimationSpeed() != 0) m_Node->setAnimationSpeed(getAnimationSpeed());
					m_Node->getMesh()->setHardwareMappingHint(EHM_STATIC);
				}
			}

			setId(getId());
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_AnimatedMesh::serialize()");

			FSObject::serializeAttributes(out, options);
			out->addString("MeshFilename", m_MeshFilename.c_str());
			out->addString("TextureFilename", m_TextureFilename.c_str());
			out->addBool("Shadow", m_Shadow);
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_AnimatedMesh::deserialize()");

			FSObject::deserializeAttributes(in, options);
			m_MeshFilename = in->getAttributeAsString("MeshFilename");
			m_TextureFilename = in->getAttributeAsString("TextureFilename");
			m_Shadow = in->getAttributeAsBool("Shadow");
		}

		virtual void setScale(vector3df scale)
		{
			FSObject::setScale(scale);
			if (m_Node) m_Node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
		}
	};

} // end namespace
