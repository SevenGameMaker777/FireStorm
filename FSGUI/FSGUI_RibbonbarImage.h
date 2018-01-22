// include this file only once
#pragma once

// include the needed header file
#include "FSUtils.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "FSGUI_Ribbonbar.h"

namespace FS
{

	class FSGUI_RibbonbarImage : public FSGUI_Ribbonbar
	{
	private:
	public:
	public:
		FSGUI_RibbonbarImage(FSGUI_Desktop* desktop, IGUIEnvironment* gui, FSGUI_Window* parent, int id, rect<s32> r, long flags, FSGUI_LAYOUTSTYLE ls, bool vertical, int spacer) :
			FSGUI_Ribbonbar(desktop, gui, parent, id, r, flags|WS_VSCROLL, ls, vertical, spacer)
		{
		}

		void setDirectory(stringc dirname)
		{
			destroyControls();
//			setSelectedItem(-1);

			const irr::io::path OriginalWorkingDirectory = getDesktop()->getDevice()->getFileSystem()->getWorkingDirectory();
			getDesktop()->getDevice()->getFileSystem()->changeWorkingDirectoryTo(dirname);
			irr::io::IFileList* Files = getDesktop()->getDevice()->getFileSystem()->createFileList();
			getDesktop()->getDevice()->getFileSystem()->changeWorkingDirectoryTo(OriginalWorkingDirectory);

			for (unsigned int i = 0; i < Files->getFileCount(); i++)
			{
				if (!Files->isDirectory(i))
				{
					const irr::io::path filename = Files->getFullFileName(i);
					ITexture* texture = Environment->getVideoDriver()->getTexture(filename.c_str());
					if (texture)
					{
						IGUIImage* image = (IGUIImage*)addElement(Environment->addImage(rect<s32>(0, 0, 130, 30), this, -1, L"", false));
						image->setImage(texture);
						image->setScaleImage(true);
					}
				}
			}
			Files->drop();
			calculateLayout();
		}

		virtual FSGUI_DRAGDROPINFO* getDragDropInfo()
		{
			if (getSelectedItem() < 0) return 0;
			IGUIElement* e = m_RibbonbarChildren[getSelectedItem()];
			if (e)
			{
				IGUIImage* i = dynamic_cast<IGUIImage*>(e);
				if (i)
				{
					stringc fn(SHORTMEDIAPATH(stringc(i->getImage()->getName())));
					FSGUI_DRAGDROPINFO_FILENAME* ddi = new FSGUI_DRAGDROPINFO_FILENAME(getDesktop(), DDIT2_TEXTURE, getID(), i->getID(), FPT_TEXTURE, fn);
					return ddi;
				}
			}
			return NULL;
		}
	};


} // end namespace#pragma once
