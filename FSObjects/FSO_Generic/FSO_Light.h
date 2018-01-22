#pragma once

#include "FSUtils.h"
#include "FSObject.h"
#include "FSLogger.h"
#include "FSLevel.h"

namespace FS
{
	class FSO_Light : public FSObject
	{
	private:
		// declare the persistent variables
		FS_VARIABLE_GET(ILightSceneNode*, LightNode);
		FS_VARIABLE_GET(ISceneNode*, BillBoard);
		FS_VARIABLE_GET(SColorf, Ambient);
		FS_VARIABLE_GET(SColorf, Diffuse);
		FS_VARIABLE_GET(SColorf, Specular);
		FS_VARIABLE_GET(vector3df, Attenuation);
		FS_VARIABLE_GET(float, Radius);
		FS_VARIABLE_GET(float, Outercone);
		FS_VARIABLE_GET(float, Innercone);
		FS_VARIABLE_GET(bool, CastShadows);
		FS_VARIABLE_GET(int, LightType);
	public:
		virtual ISceneNode* getPrimarySceneNode() { return m_LightNode; }
//		virtual ISceneNode* getPrimarySceneNode() { return m_BillBoard; }
		virtual void destroyAllSceneNodes()
		{
			if (m_LightNode) m_LightNode->remove(); m_LightNode = 0;
			m_BillBoard = 0;
		}

		ADD_PROPERTY(Ambient, stringcToSColorf, SColorfToStringc);
		virtual void setAmbient(SColorf d) { m_Ambient = d; setLightData(); }
		ADD_PROPERTY(Diffuse, stringcToSColorf, SColorfToStringc);
		virtual void setDiffuse(SColorf d) { m_Diffuse = d; setLightData(); }
		ADD_PROPERTY(Specular, stringcToSColorf, SColorfToStringc);
		virtual void setSpecular(SColorf d) { m_Specular = d; m_Specular.a = 0; setLightData(); }
		ADD_PROPERTY(Attenuation, stringcToVector3df, vector3dfToStringc);
		virtual void setAttenuation(vector3df d) { m_Attenuation = d; setLightData(); }
		ADD_PROPERTY(Radius, stringcToFloat, floatToStringc);
		virtual void setRadius(float d) { m_Radius = d; setLightData(); }
		ADD_PROPERTY(Outercone, stringcToFloat, floatToStringc);
		virtual void setOutercone(float d) { m_Outercone = d; setLightData(); }
		ADD_PROPERTY(Innercone, stringcToFloat, floatToStringc);
		virtual void setInnercone(float d) { m_Innercone = d; setLightData(); }
		ADD_PROPERTY(CastShadows, stringcToBool, boolToStringc);
		virtual void setCastShadows(bool d) { m_CastShadows = d; setLightData(); }
		ADD_PROPERTY(LightType, stringcToInt, intToStringc);
		virtual void setLightType(int d) { m_LightType = d; setLightData(); }

		FSO_Light() { /* do nothing constructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Light::FSO_Light()"); }
		virtual ~FSO_Light() { /* do nothing destructor */	FS_LOG(FSL_DEBUG, "%s", "FSO_Light::~FSO_Light()"); }

		// set all variables to a known value
		virtual void initialize()
		{
			FSObject::initialize();

			// set everythign to a known value
			m_LightNode = 0;
			m_BillBoard = 0;

			setScaleOffsetValue(vector3df(1, 1, 1));
			m_LightType = ELT_POINT;
			m_Ambient = SColorf(0.0f,0.0f,0.0f,0.0f);
			m_Diffuse = SColorf(1.0f,1.0f,1.0f,1.0f);
			m_Specular = SColorf(0.0f, 0.0f, 0.0f, 0.0f);
			m_Attenuation = vector3df(0.0f, 0.0020f, 0.0f);
			m_Radius = 1;
			m_Outercone = 45;
			m_Innercone = 0;
			m_CastShadows = true;
		}

		// dual creation allows for better error handling
		virtual bool create(FSLevel* level, int id, stringc name)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Light::create()");

			// call the base class
			FSObject::create(level, id, name);

			// update the info structure
			getInfo()->create(3, "FSO_LIGHT", "A simple Light", 1.00f, "Seven");

			REG_PROPERTY(FSO_Light, "Ambient", &FSO_Light::fsSetAmbient, &FSO_Light::fsGetAmbient, "FS_COLORF", "Ambient Light Color");
			REG_PROPERTY(FSO_Light, "Diffuse", &FSO_Light::fsSetDiffuse, &FSO_Light::fsGetDiffuse, "FS_COLORF", "Diffuse Light Color");
			REG_PROPERTY(FSO_Light, "Specular", &FSO_Light::fsSetSpecular, &FSO_Light::fsGetSpecular, "FS_COLORF", "Specular Light Color");
			REG_PROPERTY(FSO_Light, "Attenuation", &FSO_Light::fsSetAttenuation, &FSO_Light::fsGetAttenuation, "FS_VEC3D", "Attenuation");
			REG_PROPERTY(FSO_Light, "Radius", &FSO_Light::fsSetRadius, &FSO_Light::fsGetRadius, "FS_FLOAT", "Radius");
			REG_PROPERTY(FSO_Light, "Outercone", &FSO_Light::fsSetOutercone, &FSO_Light::fsGetOutercone, "FS_FLOAT", "Outercone");
			REG_PROPERTY(FSO_Light, "Innercone", &FSO_Light::fsSetInnercone, &FSO_Light::fsGetInnercone, "FS_FLOAT", "Innercone");
			REG_PROPERTY(FSO_Light, "CastShadows", &FSO_Light::fsSetCastShadows, &FSO_Light::fsGetCastShadows, "FS_BOOL", "CastShadows");
			REG_PROPERTY(FSO_Light, "LightType", &FSO_Light::fsSetLightType, &FSO_Light::fsGetLightType, "FS_INT", "Light Type");

			setObjectType(FSOT_LIGHT);

			// everything went fine
			return true;
		}

		virtual void createSceneNodes()
		{
			FSObject::createSceneNodes();

			m_LightNode = getLevel()->getSmgr()->addLightSceneNode(0,getPosition(),getDiffuse(),getRadius(),getId());
			setLightData();

			m_BillBoard = getLevel()->getSmgr()->addBillboardSceneNode(m_LightNode, core::dimension2d<f32>(50, 50));
			m_BillBoard->setMaterialFlag(video::EMF_LIGHTING, false);
			m_BillBoard->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
			m_BillBoard->setMaterialTexture(0, getLevel()->getDriver()->getTexture(MEDIAPATH("particlewhite.bmp")));
			m_BillBoard->setID(getId());

			setUseEditorVisuals(getLevel()->getUseEditorVisuals());

			setId(getId());
		}

		//! Writes attributes of the object.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Light::serialize()");

			FSObject::serializeAttributes(out, options);
			out->addColorf("Ambient", getAmbient());
			out->addColorf("Diffuse", getDiffuse());
			out->addColorf("Specular", getSpecular());
			out->addVector3d("Attenuation", getAttenuation());
			out->addFloat("Radius", getRadius());
			out->addFloat("OuterCone", getOutercone());
			out->addFloat("InnerCOne", getInnercone());
			out->addFloat("CastShadows", getCastShadows());
			out->addInt("LightType", getLightType());
		}

		//! Reads attributes of the object.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			// log this event
			FS_LOG(FSL_DEBUG, "FSO_Light::deserialize()");

			FSObject::deserializeAttributes(in, options);
			m_Ambient = in->getAttributeAsColorf("Ambient");
			m_Diffuse = in->getAttributeAsColorf("Diffuse");
			m_Specular = in->getAttributeAsColorf("Specular");
			m_Attenuation = in->getAttributeAsVector3d("Attenuation");
			m_Radius = in->getAttributeAsFloat("Radius");
			m_Outercone = in->getAttributeAsFloat("OuterCone");
			m_Innercone = in->getAttributeAsFloat("InnerCone");
			m_CastShadows = in->getAttributeAsBool("CastShadows");
			int type = in->getAttributeAsInt("LightType");
			switch (type)
			{
			case ELT_POINT:	m_LightType = ELT_POINT; break;
			case ELT_SPOT:	m_LightType = ELT_SPOT; break;
			case ELT_DIRECTIONAL:	m_LightType = ELT_DIRECTIONAL; break;
			default:m_LightType = ELT_POINT;
			}

		}

		virtual void setLightData()
		{
			if (!m_LightNode) return;

			SLight LightData = m_LightNode->getLightData();
			LightData.AmbientColor = m_Ambient;
			LightData.DiffuseColor = m_Diffuse;
			LightData.SpecularColor = m_Specular;
			LightData.Attenuation = m_Attenuation;
			LightData.Radius = m_Radius * getScale().X;
			LightData.OuterCone = m_Outercone;
			LightData.InnerCone = m_Innercone;
			LightData.CastShadows = m_CastShadows;
			switch (m_LightType)
			{
			case ELT_POINT:	LightData.Type = ELT_POINT; break;
			case ELT_SPOT:	LightData.Type = ELT_SPOT; break;
			case ELT_DIRECTIONAL:	LightData.Type = ELT_DIRECTIONAL; break;
			default:LightData.Type = ELT_POINT;
			}
			LightData.Falloff = 30;
			m_LightNode->setLightData(LightData);
//			m_LightNode->setRadius(getRadius()*getScale().X);

#if(10)
			printf("setting light data\n");
			printf("  setting ambient %f %f %f %f\n", LightData.AmbientColor.r, LightData.AmbientColor.g, LightData.AmbientColor.b, LightData.AmbientColor.a);
			printf("  setting diffuse %f %f %f %f\n", LightData.DiffuseColor.r, LightData.DiffuseColor.g, LightData.DiffuseColor.b, LightData.DiffuseColor.a);
			printf("  setting specular %f %f %f %f\n", LightData.SpecularColor.r, LightData.SpecularColor.g, LightData.SpecularColor.b, LightData.SpecularColor.a);
			printf("  setting attenuation %f %f %f \n", LightData.Attenuation.X, LightData.Attenuation.Y, LightData.Attenuation.Z);
			printf("  setting radius %f\n", LightData.Radius);
			printf("  setting outercone %f\n", LightData.OuterCone);
			printf("  setting inner cone %f\n", LightData.InnerCone);
			printf("  setting shadows %d\n", LightData.CastShadows);
#endif
		}

		virtual void setUseEditorVisuals(bool value)
		{
			if (m_BillBoard) m_BillBoard->setVisible(value);
		}

		virtual void setPosition(vector3df pos)
		{
			FSObject::setPosition(pos);
			if (m_LightNode) m_LightNode->setPosition(pos);
		}

	};

} // end namespace#pragma once
