// include this file only once
#pragma once

// include the needed header files
#include "FSUtils.h"
#include "FSLogger.h"

// defines
#define MAX_ANIMATIONS 100

// use our namespace so we dont clutter the global namespace
namespace FS
{
	// simple structure to hold the animation ranges for a model
	struct FSActionTableEntry
	{
		stringc m_AnimationName;
		int m_AnimationStart;
		int m_AnimationEnd;
	};

	// wrapper class for managing animations
	class FSActionTable
	{
	public:

		FSActionTableEntry*   m_Animations[MAX_ANIMATIONS];

		FSActionTable::FSActionTable() {  }
		virtual FSActionTable::~FSActionTable() {  }

		// set all variables to a known value
		virtual void initialize()
		{
			for (int x = 0; x < MAX_ANIMATIONS; x++) m_Animations[x] = 0;
		}

		// dual creation allows for better error handling
		virtual bool create()
		{
			return true;
		}

		// cleanup whatever memory mess we made
		virtual bool cleanup()
		{
			for (int x = 0; x < MAX_ANIMATIONS; x++) { if (m_Animations[x]) delete(m_Animations[x]); m_Animations[x] = 0; }
			return false;
		}

		virtual void updateEntry(stringc name, int start, int end)
		{
			for (int x = 0; x < MAX_ANIMATIONS; x++)
			{
				if ((m_Animations[x]) && (m_Animations[x]->m_AnimationName == name))
				{
					m_Animations[x]->m_AnimationStart = start;
					m_Animations[x]->m_AnimationEnd = end;
				}
			}
		}

		virtual int getMaxIndex()
		{
			int value = 0;
			for (int x = 0; x < MAX_ANIMATIONS; x++)
			{
				if (m_Animations[x])
				{
					if (value < m_Animations[x]->m_AnimationEnd)
						value = m_Animations[x]->m_AnimationEnd;
				}
			}
			return value;
		}

		virtual int addEntry(stringc name, int start, int end)
		{
			for (int x = 0; x < MAX_ANIMATIONS; x++)
			{
				if (!m_Animations[x])
				{
					m_Animations[x] = new FSActionTableEntry();
					m_Animations[x]->m_AnimationName = name;
					m_Animations[x]->m_AnimationStart = start;
					m_Animations[x]->m_AnimationEnd = end;
					return x;
				}
			}
			return 0;
		}

		virtual void removeEntry(stringc name)
		{
			for (int x = 0; x < MAX_ANIMATIONS; x++)
			{
				if (!m_Animations[x])
				{
					if (m_Animations[x]->m_AnimationName == name)
					{
						delete(m_Animations[x]);
					}
				}
			}
		}

		virtual FSActionTableEntry* getEntry(stringc name)
		{
			for (int x = 0; x < MAX_ANIMATIONS; x++)
			{
				if (m_Animations[x])
				{
					if (m_Animations[x]->m_AnimationName == name)
						return m_Animations[x];
				}
			}
			return NULL;
		}

		virtual FSActionTableEntry* getEntry(int index)
		{
			return m_Animations[index];
		}

		int getCount()
		{
			int count = 0;
			for (int x = 0; x < MAX_ANIMATIONS; x++) if (m_Animations[x]) count++;
			return count;
		}

		virtual void saveToDisk(stringc filename)
		{
			FILE* fp;
			errno_t error = fopen_s(&fp, filename.c_str(), "w");
			if (error == 0)
			{
				fprintf(fp, "ANIMATION_COUNT %d\n", getCount());
				for (int x = 0; x < MAX_ANIMATIONS; x++)
				{
					if (m_Animations[x])
						fprintf(fp, "%s %d %d\n", m_Animations[x]->m_AnimationName.c_str(), m_Animations[x]->m_AnimationStart, m_Animations[x]->m_AnimationEnd);
				}
				fclose(fp);
			}
			else FS_LOG(FSL_WARNING, "unable to save action table to disk........");
		}

		virtual void loadFromDisk(stringc filename)
		{
			FS_LOG(FSL_DEBUG, "%s %s", " Loading Action Table", filename);
			FILE* fp;
			errno_t error = fopen_s(&fp, filename.c_str(), "r");
			if (error == 0)
			{
				int count = 0;
				char text[255];
				int start, end;
				fscanf_s(fp, "ANIMATION_COUNT %d\n", &count);
				for (int x = 0; x < count; x++)
				{
					m_Animations[x] = new FSActionTableEntry();
					fscanf_s(fp, "%s %d %d\n", &text, 255, &start, &end);
					//printf("name %s   start %d   end %d\n",text,start,end);
					m_Animations[x]->m_AnimationName = stringc(text);
					m_Animations[x]->m_AnimationStart = start;
					m_Animations[x]->m_AnimationEnd = end;
				}
				fclose(fp);
			}
			else FS_LOG(FSL_DEBUG, "%s %s", "unable to load action table from disk........", filename);
		}

	};

} // end namespace