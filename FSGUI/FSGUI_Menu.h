#pragma once

#include "FSUtils.h"
#include "CGUIContextMenu.h"

namespace FS
{
	//! GUI menu interface.
	class FSGUI_Menu : public CGUIContextMenu
	{
	private:
		IrrlichtDevice* m_Device;
		bool mouseClicked;
	public:

		//! constructor
		FSGUI_Menu(IrrlichtDevice* device, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! draws the element and its children
		virtual void draw();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		//! Updates the absolute position.
		virtual void updateAbsolutePosition();

	protected:

		virtual void recalculateSize();

		//! returns the item highlight-area
		virtual core::rect<s32> getHRect(const SItem& i, const core::rect<s32>& absolute) const;

		//! Gets drawing rect of Item
		virtual core::rect<s32> getRect(const SItem& i, const core::rect<s32>& absolute) const;
	};

} // end namespace FS


