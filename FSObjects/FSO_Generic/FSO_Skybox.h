#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"

namespace FS
{
	class FSO_Skybox : public FSObject
	{
	private:
		FS_VARIABLE_GET(ISceneNode*, Skybox);
		FS_VARIABLE_GET(stringc, UpFilename);
		FS_VARIABLE_GET(stringc, DownFilename);
		FS_VARIABLE_GET(stringc, LeftFilename);
		FS_VARIABLE_GET(stringc, RightFilename);
		FS_VARIABLE_GET(stringc, FrontFilename);
		FS_VARIABLE_GET(stringc, BackFilename);

		FS_VARIABLE_GET(stringc, Theme);
	public:

		void setTheme(stringc fn)
		{
			m_Theme = fn;
			loadTheme(fn);
		}

		void setUpFilename(stringc fn) { m_UpFilename = fn; reCreate(); }
		void setDownFilename(stringc fn) { m_DownFilename = fn; reCreate(); }
		void setLeftFilename(stringc fn) { m_LeftFilename = fn; reCreate(); }
		void setRightFilename(stringc fn) { m_RightFilename = fn; reCreate(); }
		void setFrontFilename(stringc fn) { m_FrontFilename = fn; reCreate(); }
		void setBackFilename(stringc fn) { m_BackFilename = fn; reCreate(); }

		ADD_PROPERTY(Theme, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(UpFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(DownFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(LeftFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(RightFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(FrontFilename, stringcToStringc, stringcToStringc);
		ADD_PROPERTY(BackFilename, stringcToStringc, stringcToStringc);

	public:
		virtual ISceneNode* getPrimarySceneNode() { return m_Skybox; }

		virtual void destroyAllSceneNodes()
		{
			// remove the node
			if (m_Skybox) m_Skybox->remove(); m_Skybox = 0;
		}

		FSO_Skybox() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Skybox::FSO_Skybox()"); }
		virtual ~FSO_Skybox() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Skybox::~FSO_Skybox()"); }

		// set all variables to a known value
		virtual void initialize()
		{
			FSObject::initialize();
			m_Theme = FS_DEFAULT;
			m_UpFilename = "irrlicht2_up.jpg";
			m_DownFilename = "irrlicht2_dn.jpg";
			m_LeftFilename = "irrlicht2_lf.jpg";
			m_RightFilename = "irrlicht2_rt.jpg";
			m_FrontFilename = "irrlicht2_ft.jpg";
			m_BackFilename = "irrlicht2_bk.jpg";
			m_Skybox = 0;
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Skybox::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(2, "FSO_SKYBOX", "A simple skybox", 1.00f, "Seven");

			REG_PROPERTY(FSO_Skybox, "Theme", &FSO_Skybox::fsSetTheme, &FSO_Skybox::fsGetTheme, "FS_FILENAME", "filename");

			REG_PROPERTY(FSO_Skybox, "UpFilename", &FSO_Skybox::fsSetUpFilename, &FSO_Skybox::fsGetUpFilename, "FS_TEXTURE", "filename");
			REG_PROPERTY(FSO_Skybox, "DownFilename", &FSO_Skybox::fsSetDownFilename, &FSO_Skybox::fsGetDownFilename, "FS_TEXTURE", "filename");
			REG_PROPERTY(FSO_Skybox, "LeftFilename", &FSO_Skybox::fsSetLeftFilename, &FSO_Skybox::fsGetLeftFilename, "FS_TEXTURE", "filename");
			REG_PROPERTY(FSO_Skybox, "RightFilename", &FSO_Skybox::fsSetRightFilename, &FSO_Skybox::fsGetRightFilename, "FS_TEXTURE", "filename");
			REG_PROPERTY(FSO_Skybox, "FrontFilename", &FSO_Skybox::fsSetFrontFilename, &FSO_Skybox::fsGetFrontFilename, "FS_TEXTURE", "filename");
			REG_PROPERTY(FSO_Skybox, "BackFilename", &FSO_Skybox::fsSetBackFilename, &FSO_Skybox::fsGetBackFilename, "FS_TEXTURE", "filename");

			setObjectType(FSOT_SKYBOX);

			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();

			m_Skybox = getSmgr()->addSkyBoxSceneNode(
				getDriver()->getTexture(MEDIAPATH(getUpFilename())),
				getDriver()->getTexture(MEDIAPATH(getDownFilename())),
				getDriver()->getTexture(MEDIAPATH(getLeftFilename())),
				getDriver()->getTexture(MEDIAPATH(getRightFilename())),
				getDriver()->getTexture(MEDIAPATH(getFrontFilename())),
				getDriver()->getTexture(MEDIAPATH(getBackFilename())));

			setId(getId());
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Skybox::serialize()");

			FSObject::serializeAttributes(out, options);
			out->addString("Theme", m_Theme.c_str());
			out->addString("UpFilename", m_UpFilename.c_str());
			out->addString("DownFilename", m_DownFilename.c_str());
			out->addString("LeftFilename", m_LeftFilename.c_str());
			out->addString("RightFilename", m_RightFilename.c_str());
			out->addString("FrontFilename", m_FrontFilename.c_str());
			out->addString("BackFilename", m_BackFilename.c_str());
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Skybox::deserialize()");

			FSObject::deserializeAttributes(in, options);
			m_Theme = in->getAttributeAsString("Theme");
			m_UpFilename = in->getAttributeAsString("UpFilename");
			m_DownFilename = in->getAttributeAsString("DownFilename");
			m_LeftFilename = in->getAttributeAsString("LeftFilename");
			m_RightFilename = in->getAttributeAsString("RightFilename");
			m_FrontFilename = in->getAttributeAsString("FrontFilename");
			m_BackFilename = in->getAttributeAsString("BackFilename");

			if (m_Theme != FS_DEFAULT) setTheme(getTheme());
		}

		virtual void setUseLight(bool value) {}

		virtual void setUseFog(bool value) {}

		void saveTheme(stringc filename)
		{
			// create a temporary device
			IrrlichtDevice* device = createDevice(EDT_NULL);

			if (device)
			{
				stringc fn = getDirectory("SkyboxThemeDirectory") + filename + ".sbtheme";

				// create the xml writer
				IXMLWriter* writer = device->getFileSystem()->createXMLWriter(fn);
				if (writer)
				{
					// write the xml header
					writer->writeXMLHeader();

					// let the object serialize itself into our attributes structure
					IAttributes* attr = getLevel()->getDevice()->getFileSystem()->createEmptyAttributes(getLevel()->getDriver());
					if (attr)
					{
						SAttributeReadWriteOptions options;
						attr->addString("Theme", m_Theme.c_str());
						attr->addString("UpFilename", m_UpFilename.c_str());
						attr->addString("DownFilename", m_DownFilename.c_str());
						attr->addString("LeftFilename", m_LeftFilename.c_str());
						attr->addString("RightFilename", m_RightFilename.c_str());
						attr->addString("FrontFilename", m_FrontFilename.c_str());
						attr->addString("BackFilename", m_BackFilename.c_str());

						// if there are attributes
						if (attr->getAttributeCount() != 0)
						{
							// write the attributes to the xml file
							attr->write(writer);

							// make the file pretty
							writer->writeLineBreak();
						}

						// drop the pointer
						attr->drop();
					}

					// drop the pointer
					writer->drop();
				}

				// drop the temporary device
				device->drop();
			}
		}

		bool loadTheme(stringc filename)
		{
			// create a temporary device
			IrrlichtDevice* device = createDevice(EDT_NULL);

			if (device)
			{
				stringc fn = filename; // getDirectory("SkyboxThemeDirectory") + filename + ".sbtheme";
				//stringc fn = getDirectory("SkyboxThemeDirectory") + filename + ".sbtheme";
				FS_LOG(FSL_DEBUG, "loading skybox theme %s", fn.c_str());

				// attempt to open the file
				IXMLReader* reader = device->getFileSystem()->createXMLReader(fn);
				if (reader)
				{
					// read file
					while (reader->read())
					{
						// based on the node type
						switch (reader->getNodeType())
						{
						case io::EXN_ELEMENT:
						{
							// get the node name
							stringw name = reader->getNodeName();

							// load the attributes from the file
							IAttributes* attr = getLevel()->getDevice()->getFileSystem()->createEmptyAttributes(getLevel()->getDriver());
							attr->read(reader, false);
							m_UpFilename = attr->getAttributeAsString("UpFilename");
							m_DownFilename = attr->getAttributeAsString("DownFilename");
							m_LeftFilename = attr->getAttributeAsString("LeftFilename");
							m_RightFilename = attr->getAttributeAsString("RightFilename");
							m_FrontFilename = attr->getAttributeAsString("FrontFilename");
							m_BackFilename = attr->getAttributeAsString("BackFilename");

							// drop the pointer
							attr->drop();
						}
						}
					} // reader read
				}

				// drop the temporary device
				device->drop();
			} // device	

			reCreate();
			return true;
		}

	};

} // end namespace#pragma once
