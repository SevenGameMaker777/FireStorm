
#ifndef __C_GUI_COLOR_PICKER_HEADER__
#define __C_GUI_COLOR_PICKER_HEADER__

#include <IGUIElement.h>
#include <S3DVertex.h>

// Copyright (C) <2014> <Jehan-antoine vayssade>
// Ovan/Magun contact on irrlicht-fr.org or ovan@sleek-think.ovh
// Code is under the zlib license (same as Irrlicht)
// For conditions of distribution and use, see copyright notice in irrlicht.h


namespace irr
{
	namespace gui
	{
		class IGUIButton;
		class IGUIStaticText;
		class IGUIScrollBar;

		class IGUIColorPicker : public IGUIElement
		{
		public:
			IGUIColorPicker(IGUIEnvironment *environment, IGUIElement *parent, s32 id, video::SColor v, bool useAdvenced = true) ;
			~IGUIColorPicker() ;

			virtual void setEnabled(bool);
			virtual void setRelativePosition(const core::recti &r);

			virtual bool OnEvent(const SEvent&) ;
			virtual void updateAbsolutePosition();

			virtual void setAdvenced(bool) ;
			virtual bool getAdvenced() const ;

			virtual void setPickedColor(const video::SColor&, bool sendevent = true) ;
			virtual const video::SColor& getPickedColor() const ;

			virtual void setSelectedColor(const video::SColor&, bool sendevent = true) ;
			virtual const video::SColor& getSelectedColor() const ;

			virtual void setBackgroundColor(const video::SColor&) ;
			virtual const video::SColor& getBackgroundColor() const ;

			IGUIButton *getCloseButton() const ;
			IGUIButton *getExtendButton() const ;

			virtual void draw();
		protected:
			virtual void setScrollValueHSV(bool set, int h, int s, int v) ;
			virtual void setScrollValueRGB(bool set, int r, int g, int b) ;
			virtual void updateFromScroll(bool HsvOrRgb) ;

			virtual void recalculateAdvenced() ;
			virtual void recalculatePickedColor(bool sendevent = true) ;

			virtual void createAlphaTexture() ;
			virtual void createGradientTexture() ;
		protected:
			bool isGradient, isColor, isInside,
				isExtended, useAdvenced;

			int colorpos;
			core::vector2di  pickpos;

			video::SColor pickcolor, selectcolor, color;
			video::SColor background, white, black, alpha;
			core::recti   box, selectbox, pickbox, gradient;

			IGUIButton      *close, *extend;
			IGUIScrollBar   *scroll;
			IGUIScrollBar   *sb[6];
			IGUIStaticText  *tx[6][2];
			video::ITexture *img[2];
		};
	}
}

#endif
