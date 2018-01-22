#pragma once

#include "FSUtils.h"

namespace FS
{
	class FSTerrainEditorNode : public scene::ISceneNode
	{
		core::aabbox3d<f32> Box;
//		video::S3DVertex Vertices[9];
		video::SMaterial Material;

		array<S3DVertex> m_Vertices;
		array<u16> m_Indices;
		dimension2du m_Size;

		void createVertices()
		{
			// clear the vertices
			m_Vertices.clear();

			// clear the indices
			m_Indices.clear();

			SColor color(255, 125, 125, 125);


			// 0   1   2
			// 3   4   5
			// 6   7   8
//			u16 indices[] = { 0,1,3,  3,1,4,  1,2,4,  4,2,5,  3,4,6,  6,4,7,  4,5,7,  7,5,8 };

			m_Size.Width = 3;
			m_Size.Height = 3;

			vector3df up(0, 1, 0);
			for (int z = 0; z < m_Size.Height; z++)
			for (int x = 0; x < m_Size.Width; x++)
			{
				m_Vertices.push_back(S3DVertex(x, 0, z, up.X, up.Y, up.Z, color, 0, 0));
			}

			for (int x = 0; x < m_Size.Width; x++)
			for (int z = 0; z < m_Size.Height; z++)
			{
				int i1 = (x*m_Size.Width) + (z*m_Size.Height);
				int i2 = (i1+1);
				int i3 = (i1+m_Size.Width);
				printf("%d %d %d\n",i1,i2,i3);
				m_Indices.push_back(i1);
				m_Indices.push_back(i2);
				m_Indices.push_back(i3);
			}

			Box.reset(m_Vertices[0].Pos);
			for (s32 i = 1; i < m_Size.Width*m_Size.Height; ++i) Box.addInternalPoint(m_Vertices[i].Pos);

		}

	public:

		FSTerrainEditorNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
			: scene::ISceneNode(parent, mgr, id)
		{
			Material.Wireframe = false;
			Material.Lighting = false;
			Material.BackfaceCulling = false;

			createVertices();

			// 0   1   2
			// 3   4   5
			// 6   7   8
/*
			vector3df always_up_vector(0, 1, 0);
			Vertices[0] = video::S3DVertex(-10,	0, -10,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 0, 255, 255),	0,		0);
			Vertices[1] = video::S3DVertex(0,	0, -10,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 255, 0, 255),	0.5,	0);
			Vertices[2] = video::S3DVertex(10,	0, -10,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 255, 255, 0),	1,		0);
			Vertices[3] = video::S3DVertex(-10,	0, 0,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 0, 255, 0),	0,		0.5);
			Vertices[4] = video::S3DVertex(0,	0, 0,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 0, 255, 0),	0.5,	0.5);
			Vertices[5] = video::S3DVertex(10,	0, 0,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 0, 255, 0),	1,		0.5);
			Vertices[6] = video::S3DVertex(-10,	0, 10,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 0, 255, 0),	0,		1);
			Vertices[7] = video::S3DVertex(0,	0, 10,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 0, 255, 0),	0.5,	1);
			Vertices[8] = video::S3DVertex(10,	0, 10,	always_up_vector.X,always_up_vector.Y,always_up_vector.Z, video::SColor(255, 0, 255, 0),	1,		1);
			Box.reset(Vertices[0].Pos);
			for (s32 i = 1; i < 8; ++i)	Box.addInternalPoint(Vertices[i].Pos);
*/
		}

		virtual void OnRegisterSceneNode()
		{
			if (IsVisible) SceneManager->registerNodeForRendering(this);

			ISceneNode::OnRegisterSceneNode();
		}

		virtual void render()
		{	
			// 0   1   2
			// 3   4   5
			// 6   7   8
			// u16 indices[] = { 0,1,3,  3,1,4,  1,2,4,  4,2,5,  3,4,6,  6,4,7,  4,5,7,  7,5,8 };
			video::IVideoDriver* driver = SceneManager->getVideoDriver();

			driver->setMaterial(Material);
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			driver->drawVertexPrimitiveList(&m_Vertices[0], m_Vertices.size(), &m_Indices[0], 8, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
		}

		virtual const core::aabbox3d<f32>& getBoundingBox() const
		{
			return Box;
		}

		virtual u32 getMaterialCount() const
		{
			return 1;
		}

		virtual video::SMaterial& getMaterial(u32 i)
		{
			return Material;
		}
		void setVertexPosition(int index, vector3df pos)
		{
			//			Vertices[index].Pos.Y = pos.Y;
			m_Vertices[index].Pos = pos;
		}
	};

} // end namespace