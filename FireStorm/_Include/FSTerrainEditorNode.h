#pragma once

#include "FSUtils.h"

namespace FS
{
	class FSTerrainEditorNode : public scene::ISceneNode
	{
	public:
		core::aabbox3d<f32> Box;
		video::SMaterial Material;
		array<S3DVertex> m_Vertices;
		array<u16> m_Indices;
		dimension2du m_Size;
		dimension2du m_TempSize;

		dimension2du getTempSize() { return m_TempSize; }
		dimension2du getSize() { return m_Size; }
		void setSize(dimension2du size) { m_Size = size; createVertices(); }

		void setup(ITerrainSceneNode* terrain, int index, bool up, bool active, float x, float z, int hmwidth, int hmheight)
		{
			setScale(vector3df(terrain->getScale().X, 1, terrain->getScale().X));
			RaiseTerrainVertex(terrain, index, up, hmwidth, hmheight,active);
		}


		void createVertices()
		{
			m_TempSize.Width = m_Size.Width + 1;
			m_TempSize.Height = m_Size.Height + 1;

			// clear the vertices
			m_Vertices.clear();

			// clear the indices
			m_Indices.clear();

			// default color
			SColor color(255, 25, 125, 125);

			// 0   1   2
			// 3   4   5
			// 6   7   8
			int index = 0;
			for (int z = 0; z < m_TempSize.Height; z++)
			for (int x = 0; x < m_TempSize.Width; x++)
			{
				m_Vertices.push_back(S3DVertex(x, 0, z, 0, 1, 0, SColor(255, 255, 255, 255), float((float)x/ (float)m_Size.Width), float((float)z/ (float)m_Size.Height)));
				index++;
			}

			// 0   1   2
			// 3   4   5
			// 6   7   8
			int index1;
			int index2;
			int index3;
			int index4;
			for (int j = 0; j < m_TempSize.Height; j++)
			for (int i = 0; i < m_TempSize.Width; i++)
			{
				if ( (i != m_TempSize.Width-1) && (j != m_TempSize.Height-1))
				{
				// Get the indexes to the four points of the quad.
				index1 = (m_TempSize.Width * j) + i;          // Upper left.
				index2 = (m_TempSize.Width * j) + (i + 1);      // Upper right.
				index3 = (m_TempSize.Width * (j + 1)) + i;      // Bottom left.
				index4 = (m_TempSize.Width * (j + 1)) + (i + 1);  // Bottom right.

				m_Indices.push_back(index1);
				m_Indices.push_back(index2);
				m_Indices.push_back(index3);
				m_Indices.push_back(index2);
				m_Indices.push_back(index4);
				m_Indices.push_back(index3);
				}
			}


			Box.reset(m_Vertices[0].Pos);
			for (s32 i = 1; i < m_TempSize.Width*m_TempSize.Height; ++i) Box.addInternalPoint(m_Vertices[i].Pos);
		}

	public:
		FSTerrainEditorNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
			: scene::ISceneNode(parent, mgr, id)
		{
			Material.Wireframe = false;
			Material.Lighting = false;
			Material.BackfaceCulling = false;
			Material.MaterialType = EMT_TRANSPARENT_ADD_COLOR;
			Material.ZWriteEnable = false;
			m_Size.set(1, 1);
			createVertices();
		}

		virtual void OnRegisterSceneNode()
		{
			if (IsVisible) SceneManager->registerNodeForRendering(this);

			ISceneNode::OnRegisterSceneNode();
		}

		virtual void render()
		{	
			video::IVideoDriver* driver = SceneManager->getVideoDriver();
			driver->setMaterial(Material);
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			driver->drawVertexPrimitiveList(&m_Vertices[0], m_Vertices.size(), &m_Indices[0], m_Indices.size() / 3, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
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
			m_Vertices[index].Pos = pos;
		}
	
		/*==============================================================================
		Raise or lower terrain (selected vertice)
		==============================================================================*/
		void RaiseTerrainVertex(ITerrainSceneNode* terrain, s32 index, bool up, int hmwidth, int hmheight, bool active)
		{
			// dont go off of the terrain

			scene::IMesh* pMesh = terrain->getMesh();

			s32 heightmapWidth = hmwidth; // heightmap->getDimension().Width;
			s32 heightmapHeight = hmheight; // heightmap->getDimension().Height;

			s32 b;
			for (b = 0; b<pMesh->getMeshBufferCount(); ++b)
			{
				IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(b);
				if (pMeshBuffer->getVertexType() != video::EVT_2TCOORDS) continue;
				S3DVertex2TCoords* pVertices = (S3DVertex2TCoords*)pMeshBuffer->getVertices();

				s32 brushWidth = m_Size.Width;
				s32 brushHeight = m_Size.Height;

				for (int y = 0; y < brushHeight; y++)
				{
					for (int x = 0; x < brushWidth; x++)
					{
						f32 hy = pVertices[index - (brushWidth / 2) - ((brushWidth / 2)*heightmapWidth) + (x + (y*heightmapWidth))].Pos.Y;
						
						// using the brush image, raise or lower the vertex 

						
						
						f32 bp = 0.1;
						bp = (up) ? bp : -bp;
						if (up && active)
						{
							if (bp > 0 && hy + bp <= 255)
								pVertices[index - (brushWidth / 2) - ((brushWidth / 2)*heightmapWidth) + (x + (y*heightmapWidth))].Pos.Y = hy + bp;
						}
						if (!up && active)
						{
							if (hy + bp >= 0 && hy + bp <= 255)
								pVertices[index - (brushWidth / 2) - ((brushWidth / 2)*heightmapWidth) + (x + (y*heightmapWidth))].Pos.Y = hy + bp;
						}
					}
				}

				SColor UPCOLOR(255, 0, 0, 255);
				SColor DOWNCOLOR(255, 0, 255, 0);

				for (int z = 0; z < m_TempSize.Height; z++)
				for (int x = 0; x < m_TempSize.Width; x++)
					{
						vector3df p = pVertices[index - (brushWidth / 2) - ((brushWidth / 2)*heightmapWidth) + (x + (z*heightmapWidth))].Pos + (terrain->getPosition() / terrain->getScale());
						p.Y = (p.Y * terrain->getScale().Y) + 2;
						setVertexPosition(x + (z*m_TempSize.Width), p);
						if (up) m_Vertices[x + (z*m_TempSize.Width)].Color = UPCOLOR;
						else m_Vertices[x + (z*m_TempSize.Width)].Color = DOWNCOLOR;
					}
			}

			// force terrain render buffer to reload
			terrain->setPosition(terrain->getPosition());

			Box.reset(m_Vertices[0].Pos);
			for (s32 i = 1; i < m_TempSize.Width*m_TempSize.Height; ++i) Box.addInternalPoint(m_Vertices[i].Pos);
		}


	};

} // end namespace