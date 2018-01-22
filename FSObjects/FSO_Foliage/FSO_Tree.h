// include this file only once
#pragma once

// include the needed header files
#include "FSOFoliage.h"
#include "FSApplication.h"
#include "FSObject.h"
#include "FSLevel.h"
#include "FSObjectFactory.h"
#include "FSMessageManager.h"
#include "CTreeSceneNode.h"
#include "CTreeGenerator.h"
#include "FSPhysX.h"
#include "FSEngine.h"

namespace FS
{

	struct STreeDesignFiles
	{
		stringc DesignFile;
		stringc TreeTextureFile;
		stringc LeafTextureFile;
		stringc BillTextureFile;
	};

	struct STreeDesign
	{
		CTreeGenerator* Generator;
		video::ITexture* TreeTexture;
		video::ITexture* LeafTexture;
		video::ITexture* BillTexture;
	};

	const s32 NUM_TREE_DESIGNS = 4;

	STreeDesignFiles treeDesignFiles[NUM_TREE_DESIGNS];
	/*
	=
	{
	{ MEDIAPATH("trees/trees/Oak.xml"),		MEDIAPATH("trees/textures/OakBark.png"),	MEDIAPATH("trees/textures/OakLeaf.png"),	MEDIAPATH("trees/textures/OakBillboard.png")	},
	{ MEDIAPATH("trees/trees/Aspen.xml"),	MEDIAPATH("trees/textures/AspenBark.png"),	MEDIAPATH("trees/textures/AspenLeaf.png"),	MEDIAPATH("trees/textures/AspenBillboard.png")	},
	{ MEDIAPATH("trees/trees/Pine.xml"),	MEDIAPATH("trees/textures/PineBark.png"),	MEDIAPATH("trees/textures/PineLeaf.png"),	MEDIAPATH("trees/textures/PineBillboard.png")	},
	{ MEDIAPATH("trees/trees/Willow.xml"),	MEDIAPATH("trees/textures/WillowBark.png"), MEDIAPATH("trees/textures/WillowLeaf.png"),	MEDIAPATH("trees/textures/WillowBillboard.png") }
	};
	*/
	STreeDesign treeDesigns[NUM_TREE_DESIGNS];
	video::E_MATERIAL_TYPE leafMaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;

	class FSTreeManager
	{
	public:
		FS_VARIABLE_GET(FSLevel*, Level);
		video::ITexture* billTexture;
		video::ITexture* leafTexture;
		s32 seed;
		bool isValid;
		bool Valid() { return isValid; };

		FSTreeManager::FSTreeManager() { };
		~FSTreeManager() { Cleanup(); };

		void Initialize()
		{
			m_Level = 0;
			seed = rand();
			isValid = false;
		}

		bool Create(FSLevel* level)
		{
			m_Level = level;
			stringc dir = getLevel()->getDirectory("MediaDirectory");
			dir += "_assets/trees/";

			treeDesignFiles[0].DesignFile = dir + "trees/Oak.xml";
			treeDesignFiles[0].TreeTextureFile = dir + "/textures/OakBark.png";
			treeDesignFiles[0].LeafTextureFile = dir + "textures/OakLeaf.png";
			treeDesignFiles[0].BillTextureFile = dir + "textures/OakBillboard.png";

			treeDesignFiles[1].DesignFile = dir + "/trees/Aspen.xml";
			treeDesignFiles[1].TreeTextureFile = dir + "textures/AspenBark.png";
			treeDesignFiles[1].LeafTextureFile = dir + "textures/AspenLeaf.png";
			treeDesignFiles[1].BillTextureFile = dir + "textures/AspenBillboard.png";

			treeDesignFiles[2].DesignFile = dir + "trees/Pine.xml";
			treeDesignFiles[2].TreeTextureFile = dir + "textures/PineBark.png";
			treeDesignFiles[2].LeafTextureFile = dir + "textures/PineLeaf.png";
			treeDesignFiles[2].BillTextureFile = dir + "textures/PineBillboard.png";

			treeDesignFiles[3].DesignFile = dir + "trees/Willow.xml";
			treeDesignFiles[3].TreeTextureFile = dir + "textures/WillowBark.png";
			treeDesignFiles[3].LeafTextureFile = dir + "textures/WillowLeaf.png";
			treeDesignFiles[3].BillTextureFile = dir + "textures/WillowBillboard.png";

			if (level == NULL) return printf("tree manager - level not valid\n");
			for (s32 i = 0; i<NUM_TREE_DESIGNS; i++)
			{
				treeDesigns[i].Generator = new CTreeGenerator(level->getSmgr());
				io::IXMLReader* xml = level->getDevice()->getFileSystem()->createXMLReader(treeDesignFiles[i].DesignFile);
				treeDesigns[i].Generator->loadFromXML(xml);
				xml->drop();
				treeDesigns[i].TreeTexture = level->getDriver()->getTexture(treeDesignFiles[i].TreeTextureFile);
				treeDesigns[i].LeafTexture = level->getDriver()->getTexture(treeDesignFiles[i].LeafTextureFile);
				treeDesigns[i].BillTexture = level->getDriver()->getTexture(treeDesignFiles[i].BillTextureFile);
			}

			//		leafMaterialType = (video::E_MATERIAL_TYPE) level->GetApp()->GetEngine()->m_Driver->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
			//			"media/trees/shaders/leaves.vert","main",EVST_VS_2_0,"media/trees/shaders/leaves.frag","main",
			//			EPST_PS_2_0,0,EMT_TRANSPARENT_ALPHA_CHANNEL,0);

			leafMaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
			isValid = true;

			return true;
		}

		bool Cleanup()
		{
			m_Level = 0;

			for (s32 i = 0; i<NUM_TREE_DESIGNS; i++)
			{
				//treeDesigns[i].Generator->drop();
			}
			return false;
		}

		CTreeSceneNode* GenerateNewTree(FSLevel* l, int style)
		{
			CTreeSceneNode* tree;
			tree = new CTreeSceneNode(l->getSmgr()->getRootSceneNode(), l->getSmgr());

			tree->setup(treeDesigns[style].Generator, seed, treeDesigns[style].BillTexture);

			tree->getLeafNode()->getMaterial(0).TextureLayer[0].AnisotropicFilter = true;
			tree->getLeafNode()->getMaterial(0).TextureLayer[0].BilinearFilter = false;

			tree->getLeafNode()->setMaterialTexture(0, treeDesigns[style].LeafTexture);
			tree->getLeafNode()->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

			// add
			tree->getLeafNode()->setMaterialFlag(video::EMF_LIGHTING, l->getUseLight());
			tree->getLeafNode()->setMaterialFlag(video::EMF_FOG_ENABLE, l->getUseFog());
			// add end

			tree->setMaterialTexture(0, treeDesigns[style].TreeTexture);

			tree->getMaterial(0).setFlag(video::EMF_LIGHTING, l->getUseLight());
			tree->getMaterial(0).setFlag(video::EMF_FOG_ENABLE, l->getUseFog());
			//if ( l->getUseLight() )
			//tree->getLeafNode()->applyVertexShadows( lightDir, 1.0f, 0.25f );

			tree->getLeafNode()->setMaterialType(leafMaterialType);

//			tree->setDistances(100, 200);

			return tree;
		}
	};

	FSTreeManager TreeManager;

	// This class is exported from the dll
	class FSO_Tree : public FSObject
	{
	public:
		FS_VARIABLE_GET(CTreeSceneNode*, Tree);
		int m_TreeStyle;
		ADD_PROPERTY(TreeStyle, stringcToInt, intToStringc);

		virtual int getTreeStyle() { return m_TreeStyle; }
		virtual void setTreeStyle(int s) { if (s<0) s = 0; if (s>3) s = 3; m_TreeStyle = s; reCreate(); };

		virtual ISceneNode* getPrimarySceneNode() { return m_Tree; }
		virtual void destroyAllSceneNodes()
		{
			// call the base class
			FSObject::destroyAllSceneNodes();

			// remove the cube node
			if (m_Tree) m_Tree->remove(); m_Tree = 0;
		}

		FSO_Tree(void) { FS_LOG(FSL_DEBUG, "FSO_Tree::FSO_Tree()"); }
		virtual ~FSO_Tree(void) { FS_LOG(FSL_DEBUG, "FSO_Tree::~FSO_Tree()"); }

		virtual bool cleanup()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Tree::Cleanup()");

			// let the base class cleanup
			return FSObject::cleanup();
		}

		virtual void initialize()
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Tree::Initialize()");

			// initialize the base class
			FSObject::initialize();

			m_Tree = 0;
			m_TreeStyle = 0;
			setBBOffsetValue(vector3df(20, 100, 20));
			setStatic(true);
		}

		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Tree::Create()");

			// create the base class
			FSObject::create(level, id, name);

			// create the information structure for this object
			getInfo()->create(1, "FSO_TREE", L"a tree with PhysX attached", 1.00, L"Seven");

			REG_PROPERTY(FSO_Tree, "TreeStyle", &FSO_Tree::fsSetTreeStyle, &FSO_Tree::fsGetTreeStyle, "FS_INT", "0 through 4");

			if (!TreeManager.Valid())
			{
				TreeManager.Initialize();
				TreeManager.Create(getLevel());
			}

			//			setStatic(true);
			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			// call the base class
			FSObject::createSceneNodes();

			// remove the tree node
			if (m_Tree) m_Tree->remove(); m_Tree = 0;

			// create a new tree node
			m_Tree = TreeManager.GenerateNewTree(getLevel(), getTreeStyle());
			setPosition(getPosition());
			setRotation(getRotation());
			m_Tree->setScale(getScale());
			setBBOffsetValue(vector3df(20, 100, 20)*getScale());
			setPositionOffset(vector3df(0, -getBBOffset().Y, 0));


			// set the object type
			setId(getId());
			m_Tree->setName("tree");
			m_Tree->getLeafNode()->setName("leaf");

			//use VBO
			m_Tree->setAutomaticCulling(EAC_FRUSTUM_BOX);

			SColor color;
			E_FOG_TYPE fogType;
			f32 start;
			f32 end;
			f32 density;
			bool pixelFog;
			bool rangeFog;
			getDriver()->getFog(color, fogType, start, end, density, pixelFog, rangeFog);
			m_Tree->setDistances(end, end*30);
		}

		virtual void createPhysXObject()
		{
			FS_LOG(FSL_DEBUG, "FSO_Tree::CreatePhysXObject()");

			// call the base class
			FSObject::createPhysXObject();

			if (!m_Tree) return;

			IPhysXObjectData data;
			data.userdata = getId();
			data.type = POT_PRIMITIVE_BOX;
			data.position = getPosition();
			data.rotation = getRotation();
			data.scale = getScale();
			data.bboffset = getBBOffset();
//			data.mass = getMass();
			data.node = m_Tree;
			data.mesh = 0;
			data.dynamic = false;
			setPhysXObject(getLevel()->getPhysXWorld()->createPhysXObject(data));
		}


		virtual void setUseLight(bool value)
		{
			FSObject::setUseLight(value);
			if (getTree())
			{
				m_Tree->setMaterialFlag(video::EMF_LIGHTING, value);
				m_Tree->getLeafNode()->setMaterialFlag(video::EMF_LIGHTING, value);
			}
		}

		virtual void setUseFog(bool value)
		{
			FSObject::setUseFog(value);
			if (getTree())
			{
				m_Tree->setMaterialFlag(video::EMF_FOG_ENABLE, value);
				m_Tree->getLeafNode()->setMaterialFlag(video::EMF_FOG_ENABLE, value);
			}
		}

		void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			FSObject::serializeAttributes(out, options);

			out->addInt("TreeStyle", getTreeStyle());
		}

		void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			FSObject::deserializeAttributes(in, options);

			m_TreeStyle = in->getAttributeAsInt("TreeStyle");
		}
	};
} // end namespace
