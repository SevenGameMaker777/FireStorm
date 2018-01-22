// include the needed header files
#include "FSUtils.h"


// unclutter the global namespace
namespace FS
{
	stringc getEditorSelectionString(long e)
	{
		switch (e)
		{
		case FSOT_NONE: return stringc("FSOT_NONE"); break;
		case FSOT_ALL: return stringc("FSOT_ALL"); break;
		case FSOT_OBJECT: return stringc("FSOT_OBJECT"); break;
		case FSOT_TERRAIN: return stringc("FSOT_TERRAIN"); break;
		case FSOT_STATIC: return stringc("FSOT_STATIC"); break;
		case FSOT_SKYBOX: return stringc("FSOT_SKYBOX"); break;
		case FSOT_LIGHT: return stringc("FSOT_LIGHT"); break;
		case FSOT_TRIGGER: return stringc("FSOT_TRIGGER"); break;
		case FSOT_WATER: return stringc("FSOT_WATER"); break;
		case FSOT_CHARACTER: return stringc("FSOT_CHARACTER"); break;
		}
		return "UNKOWN";
	}

	// Note: smgr = scene manager pointer is needed for manipulating and creating the elements
	// Note: guienv = guienvironment pointer is needed for the font. You can also define your own font instead.
	// Create and animate a text over the object head
	void createTextAnim(IGUIEnvironment* gui, ISceneManager* smgr, ISceneNode* node, stringw text, SColor color, u32 duration, dimension2d<f32> size)
	{
		const wchar_t * ttext = L" ";

		if (!text.empty()) ttext = text.c_str();
		vector3df start = node->getAbsolutePosition();
		f32 height = node->getBoundingBox().getExtent().Y*node->getScale().Y;
		start.Y += height - 30;
		vector3df end = start;
		end.Y += height + 50;
		IBillboardTextSceneNode * nodetext = smgr->addBillboardTextSceneNode(gui->getBuiltInFont(), ttext, smgr->getRootSceneNode(), size, start, -1, color, color);

		scene::ISceneNodeAnimator * anim = smgr->createDeleteAnimator(duration);
		scene::ISceneNodeAnimator * anim2 = smgr->createFlyStraightAnimator(start, end, duration);
		if (nodetext && anim)
		{
			nodetext->addAnimator(anim);
			nodetext->addAnimator(anim2);
		}
	}

	// a couple helper functions
	int getRandomInteger(int Low, int High)
	{
		return (rand() % (High - Low + 1)) + Low;

		float Range;
		Range = (float)(High - Low);
		return (int)(((int)(((rand() % 1000) + 1))) / 1000.0f * Range + Low);
	}

	float getRandomFloat(float Low, float High)
	{
		float Range;
		Range = (float)(High - Low);
		return (float)(((float)(((rand() % 1000) + 1))) / 1000.0f * Range + Low);
	}

	bool isFileType(FilePickerType type, stringc s)
	{
		stringc extension;
		extension = getFileNameExtension(extension, s);

		switch (type)
		{
		case FPT_STATICMODEL:
		case FPT_ANIMATEDMODEL:
		{
			if (extension == ".x") return true;
			if (extension == ".obj") return true;
			if (extension == ".ms3d") return true;
			if (extension == ".b3d") return true;
			if (extension == ".3ds") return true;
		} break;
		case FPT_TEXTURE:
		{
			if (extension == ".jpg" || extension == ".pcx" ||
				extension == ".png" || extension == ".ppm" ||
				extension == ".pgm" || extension == ".pbm" ||
				extension == ".psd" || extension == ".tga" ||
				extension == ".bmp" || extension == ".wal" ||
				extension == ".rgb" || extension == ".rgba"
				) return true;
		} break;
		case FPT_SKYBOXTHEME:
		{
			if (extension == ".sbtheme") return true;
		} break;
		}

		return false;
	}

	vector3df getIn(ISceneNode* node)
	{
		if (node)
		{
			matrix4 mat = node->getRelativeTransformation();
			vector3df in(mat[8], mat[9], mat[10]);
			in.normalize();
			return in;
		}
		else return vector3df(0, 0, 0);
	}

	stringc stringcToStringc(stringc v) { return v; }
	int stringcToInt(stringc v)
	{
		return atoi(v.c_str());
	}

	bool stringcToBool(stringc v)
	{
		int i = atoi(v.c_str());
		if (i == 0) return 0;
		return 1;
	}

	float stringcToFloat(stringc v)
	{
		return (float)atof(v.c_str());
	}

	float stringwToFloat(stringw v)
	{
		return stringcToFloat(stringc(v));
	}

	vector3df stringcToVector3df(stringc v)
	{
		vector3df r(0, 0, 0);
		sscanf_s(v.c_str(), "%f %f %f", &r.X, &r.Y, &r.Z);
		return r;
	}

	SColorf stringcToSColorf(stringc v)
	{
		SColorf r(0, 0, 0, 0);
		sscanf_s(v.c_str(), "%f %f %f %f", &r.r, &r.g, &r.b, &r.a);
		return r;
	}

	SColor stringcToSColor(stringc v)
	{
		int r, g, b, a;
		sscanf_s(v.c_str(), "%d %d %d %d", &a, &r, &g, &b);
		SColor r1(a, r, g, b);
		return r1;
	}

	dimension2df stringcToDimension2df(stringc v)
	{
		dimension2df r(0, 0);
		sscanf_s(v.c_str(), "%f %f", &r.Width, &r.Height);
		return r;
	}

	dimension2du stringcToDimension2du(stringc v)
	{
		dimension2du r(0, 0);
		sscanf_s(v.c_str(), "%d %d", &r.Width, &r.Height);
		return r;
	}

	aabbox3df stringcToaabbox3df(stringc v)
	{
		aabbox3df r(0, 0, 0, 0, 0, 0);
		sscanf_s(v.c_str(), "%f %f %f %f %f %f", &r.MinEdge.X, &r.MinEdge.Y, &r.MinEdge.Z, &r.MaxEdge.X, &r.MaxEdge.Y, &r.MaxEdge.Z);
		return r;
	}

	stringc aabbox3dfToStringc(aabbox3df v)
	{
		stringc r(v.MinEdge.X);
		r += " ";
		r += v.MinEdge.Y;
		r += " ";
		r += v.MinEdge.Z;
		r += " ";
		r += v.MaxEdge.X;
		r += " ";
		r += v.MaxEdge.Y;
		r += " ";
		r += v.MaxEdge.Z;
		return r;
	}

	stringc dimension2dfToStringc(dimension2df v)
	{
		stringc r(v.Width);
		r += " ";
		r += v.Height;
		return r;
	}

	stringc dimension2duToStringc(dimension2du v)
	{
		stringc r(v.Width);
		r += " ";
		r += v.Height;
		return r;
	}

	stringc intToStringc(int v)
	{
		return stringc((int)v);
	}

	stringc boolToStringc(bool v)
	{
		return stringc((bool)v);
	}

	stringc floatToStringc(float v)
	{
		return stringc((float)v);
	}

	stringw floatToStringw(float v)
	{
		return stringw(v);
	}

	stringc vector3dfToStringc(vector3df v)
	{
		stringc r((float)v.X);
		r += " ";
		r += v.Y;
		r += " ";
		r += v.Z;
		return r;
	}

	stringc SColorfToStringc(SColorf v)
	{
		stringc r(v.r);
		r += " ";
		r += v.g;
		r += " ";
		r += v.b;
		r += " ";
		r += v.a;
		return r;
	}

	stringc SColorToStringc(SColor v)
	{
		stringc r(v.getAlpha());
		r += " ";
		r += v.getRed();
		r += " ";
		r += v.getGreen();
		r += " ";
		r += v.getBlue();
		return r;
	}
} // end namespace

