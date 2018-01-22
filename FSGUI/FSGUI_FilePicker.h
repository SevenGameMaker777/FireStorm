// include this file only once
#pragma once

// include the needed header files
#include "FSGUI_Toolbar.h"
#include "FSGUI_Systembar.h"
#include "FSGUI_Window.h"
#include "FSGUI_Desktop.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIButton.h"
#include "IGUIStaticText.h"
#include "IGUIFont.h"
#include "IGUIFontBitmap.h"
#include "IFileList.h"
#include "IGUIFileOpenDialog.h"
#include "IrrCompileConfig.h"
#include "IGUIFileOpenDialog.h"
#include "IGUIButton.h"
#include "IGUIListBox.h"
#include "IGUIEditBox.h"
#include "IFileSystem.h"
#include <locale.h>
#define FSID_FILEPICKER 101

namespace FS
{

	const s32 FOD_WIDTH = 350;
	const s32 FOD_HEIGHT = 250;

	class FSGUI_FileOpenDialog : public IGUIFileOpenDialog
	{
	protected:

		core::position2d<s32> DragStart;
		core::stringw FileName;
		io::path FileDirectory;
		io::path RestoreDirectory;
		io::path StartDirectory;

		IGUIButton* CloseButton;
		IGUIButton* OKButton;
		IGUIButton* CancelButton;
		IGUIListBox* FileBox;
		IGUIEditBox* FileNameText;
		IGUIElement* EventParent;
		io::IFileSystem* FileSystem;
		io::IFileList* FileList;
		bool Dragging;
public:	
	FilePickerType m_FilePickerType;

	bool isCorrectType(FilePickerType type, stringc s)
	{
		stringc extension;
		extension = getFileNameExtension(extension,s);
		
		switch (type)
		{
		case FPT_STATICMODEL:
		case FPT_ANIMATEDMODEL:
		{
			if (extension == ".x") return true;
			if (extension == ".obj") return true;
			if (extension == ".ms3d") return true;
			if (extension == ".b3d") return true;
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

		return true;
	}


	//! constructor
		FSGUI_FileOpenDialog::FSGUI_FileOpenDialog(const wchar_t* title,
				IGUIEnvironment* environment, IGUIElement* parent, s32 id,
				bool restoreCWD, io::path::char_type* startDir, FilePickerType type)
				: IGUIFileOpenDialog(environment, parent, id,
				core::rect<s32>((parent->getAbsolutePosition().getWidth() - FOD_WIDTH) / 2,
				(parent->getAbsolutePosition().getHeight() - FOD_HEIGHT) / 2,
				(parent->getAbsolutePosition().getWidth() - FOD_WIDTH) / 2 + FOD_WIDTH,
				(parent->getAbsolutePosition().getHeight() - FOD_HEIGHT) / 2 + FOD_HEIGHT)),
				FileNameText(0), FileList(0), Dragging(false), m_FilePickerType(type)
			{
#ifdef _DEBUG
				IGUIElement::setDebugName("CGUIFileOpenDialog");
#endif

				Text = title;

				FileSystem = Environment ? Environment->getFileSystem() : 0;

				if (FileSystem)
				{
					FileSystem->grab();

					if (restoreCWD)
						RestoreDirectory = FileSystem->getWorkingDirectory();
					if (startDir)
					{
						StartDirectory = startDir;
						FileSystem->changeWorkingDirectoryTo(startDir);
					}
				}
				else
					return;

				IGUISpriteBank* sprites = 0;
				video::SColor color(255, 255, 255, 255);
				IGUISkin* skin = Environment->getSkin();
				if (skin)
				{
					sprites = skin->getSpriteBank();
					color = skin->getColor(EGDC_WINDOW_SYMBOL);
				}

				const s32 buttonw = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);
				const s32 posx = RelativeRect.getWidth() - buttonw - 4;

/*
				CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1,
					L"", skin ? skin->getDefaultText(EGDT_WINDOW_CLOSE) : L"Close");
				CloseButton->setSubElement(true);
				CloseButton->setTabStop(false);
				if (sprites)
				{
					CloseButton->setSpriteBank(sprites);
					CloseButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_CLOSE), color);
					CloseButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_CLOSE), color);
				}
				CloseButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
				CloseButton->grab();

				OKButton = Environment->addButton(
					core::rect<s32>(RelativeRect.getWidth() - 80, 30, RelativeRect.getWidth() - 10, 50),
					this, -1, skin ? skin->getDefaultText(EGDT_MSG_BOX_OK) : L"OK");
				OKButton->setSubElement(true);
				OKButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
				OKButton->grab();

				CancelButton = Environment->addButton(
					core::rect<s32>(RelativeRect.getWidth() - 80, 55, RelativeRect.getWidth() - 10, 75),
					this, -1, skin ? skin->getDefaultText(EGDT_MSG_BOX_CANCEL) : L"Cancel");
				CancelButton->setSubElement(true);
				CancelButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
				CancelButton->grab();
*/
				FileBox = Environment->addListBox(core::rect<s32>(10, 55, RelativeRect.getWidth() -2, 230), this, -1, true);
				FileBox->setSubElement(true);
				FileBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
				FileBox->grab();

				FileNameText = Environment->addEditBox(0, core::rect<s32>(10, 30, RelativeRect.getWidth() -2, 50), true, this,10000);
				FileNameText->setSubElement(true);
				FileNameText->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
				FileNameText->grab();

				setTabGroup(true);

				fillListBox();
			}


			//! destructor
		virtual ~FSGUI_FileOpenDialog()
			{
				if (CloseButton)
					CloseButton->drop();

				if (OKButton)
					OKButton->drop();

				if (CancelButton)
					CancelButton->drop();

				if (FileBox)
					FileBox->drop();

				if (FileNameText)
					FileNameText->drop();

				if (FileSystem)
				{
					// revert to original CWD if path was set in constructor
					if (RestoreDirectory.size())
						FileSystem->changeWorkingDirectoryTo(RestoreDirectory);
					FileSystem->drop();
				}

				if (FileList)
					FileList->drop();
			}


			//! returns the filename of the selected file. Returns NULL, if no file was selected.
			virtual const wchar_t* getFileName() const
			{
				return FileName.c_str();
			}

			//! Returns the directory of the selected file. Returns NULL, if no directory was selected.
			virtual const io::path& getDirectoryName()
			{
				FileSystem->flattenFilename(FileDirectory);
				return FileDirectory;
			}


			//! called if an event happened.
			virtual bool OnEvent(const SEvent& event)
			{
				if (isEnabled())
				{
					switch (event.EventType)
					{
					case EET_GUI_EVENT:
						switch (event.GUIEvent.EventType)
						{
						case EGET_ELEMENT_FOCUS_LOST:
							Dragging = false;
							break;
						case EGET_BUTTON_CLICKED:
							if (event.GUIEvent.Caller == CloseButton ||
								event.GUIEvent.Caller == CancelButton)
							{
								sendCancelEvent();
//								remove();
								return true;
							}
							else
								if (event.GUIEvent.Caller == OKButton)
								{
									if (FileDirectory != L"")
									{
										sendSelectedEvent(EGET_DIRECTORY_SELECTED);
									}
									if (FileName != L"")
									{
										sendSelectedEvent(EGET_FILE_SELECTED);
//										remove();
										return true;
									}
								}
							break;

						case EGET_LISTBOX_CHANGED:
						{
							s32 selected = FileBox->getSelected();
							stringw selectedText = FileBox->getListItem(selected);

							if (FileList && FileSystem)
							{
								if (FileList->isDirectory(selected))
								{
									FileName = L"";
									FileDirectory = FileList->getFullFileName(selected);
									sendSelectedEvent(EGET_DIRECTORY_SELECTED);
								}
								else
								{
									FileDirectory = L"";
									FileName = FileList->getFullFileName(selected);
									FileName = FileList->getPath() + selectedText;
									if (isFileType(FPT_STATICMODEL, FileName)) m_FilePickerType = FPT_STATICMODEL;
									if (isFileType(FPT_ANIMATEDMODEL, FileName)) m_FilePickerType = FPT_ANIMATEDMODEL;
									if (isFileType(FPT_TEXTURE, FileName)) m_FilePickerType = FPT_TEXTURE;
									
									sendSelectedEvent(EGET_FILE_SELECTED);
								}
								return true;
							}
						}
						break;

						case EGET_LISTBOX_SELECTED_AGAIN:
						{
							const s32 selected = FileBox->getSelected();
							
							
							stringw selectedText = FileBox->getListItem(selected);

							if (FileList && FileSystem)
							{
								if (FileList->isDirectory(selected))
								{
									FileDirectory = FileList->getFullFileName(selected);
									FileSystem->changeWorkingDirectoryTo(FileList->getFileName(selected));
									fillListBox();
									FileName = "";
									sendSelectedEvent(EGET_DIRECTORY_SELECTED);
								}
								else
								{
									FileName = FileList->getFullFileName(selected);
									FileName = FileList->getPath() + selectedText;
									if (isFileType(FPT_STATICMODEL, FileName)) m_FilePickerType = FPT_STATICMODEL;
									if (isFileType(FPT_TEXTURE, FileName)) m_FilePickerType = FPT_TEXTURE;

								}
								return true;
							}
						}
						break;
						case EGET_EDITBOX_ENTER:
							if (event.GUIEvent.Caller == FileNameText)
							{
								io::path dir(FileNameText->getText());
								if (FileSystem->changeWorkingDirectoryTo(dir))
								{
									fillListBox();
									FileName = L"";
								}
								return true;
							}
							break;
						default:
							break;
						}
						break;
					case EET_MOUSE_INPUT_EVENT:
						switch (event.MouseInput.Event)
						{
						case EMIE_MOUSE_WHEEL:
							return FileBox->OnEvent(event);
						case EMIE_LMOUSE_PRESSED_DOWN:
							DragStart.X = event.MouseInput.X;
							DragStart.Y = event.MouseInput.Y;
							Dragging = true;
							Environment->setFocus(this);
							return true;
						case EMIE_LMOUSE_LEFT_UP:
							Dragging = false;
							return true;
						case EMIE_MOUSE_MOVED:

							/*
							if (!event.MouseInput.isLeftPressed())
								Dragging = false;

							if (Dragging)
							{
								// gui window should not be dragged outside its parent
								if (Parent)
									if (event.MouseInput.X < Parent->getAbsolutePosition().UpperLeftCorner.X + 1 ||
										event.MouseInput.Y < Parent->getAbsolutePosition().UpperLeftCorner.Y + 1 ||
										event.MouseInput.X > Parent->getAbsolutePosition().LowerRightCorner.X - 1 ||
										event.MouseInput.Y > Parent->getAbsolutePosition().LowerRightCorner.Y - 1)

										return true;

								move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
								DragStart.X = event.MouseInput.X;
								DragStart.Y = event.MouseInput.Y;
								return true;
							}
*/
							break;
						default:
							break;
						}
					default:
						break;
					}
				}

				return IGUIElement::OnEvent(event);
			}


			//! draws the element and its children
			virtual void draw()
			{
				//if (!IsVisible)
				//	return;

				IGUISkin* skin = Environment->getSkin();

				core::rect<s32> rect = AbsoluteRect;

				rect = skin->draw3DWindowBackground(this, true, skin->getColor(EGDC_ACTIVE_BORDER),
					rect, &AbsoluteClippingRect);

				if (Text.size())
				{
					rect.UpperLeftCorner.X += 2;
					rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

					IGUIFont* font = skin->getFont(EGDF_WINDOW);
					if (font)
						font->draw(Text.c_str(), rect,
						skin->getColor(EGDC_ACTIVE_CAPTION),
						false, true, &AbsoluteClippingRect);
				}

				IGUIElement::draw();
			}


			//! Writes attributes of the element.
			/* Not sure if this will really work out properly. Saving paths can be
			rather problematic. */
			virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				IGUIFileOpenDialog::serializeAttributes(out, options);

				out->addString("StartDirectory", StartDirectory.c_str());
				out->addBool("RestoreDirectory", (RestoreDirectory.size() != 0));
			}


			//! Reads attributes of the element
			/* Note that thiese paths changes will happen at arbitrary places upon
			load of the gui description. This may be undesired. */
			virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{
				StartDirectory = in->getAttributeAsString("StartDirectory");
				const bool restore = in->getAttributeAsBool("RestoreDirectory");
				if (restore)
					RestoreDirectory = FileSystem->getWorkingDirectory();
				else
					RestoreDirectory = "";
				if (StartDirectory.size())
					FileSystem->changeWorkingDirectoryTo(StartDirectory);

				IGUIFileOpenDialog::deserializeAttributes(in, options);
			}


			//! fills the listbox with files.
			void fillListBox()
			{
				IGUISkin *skin = Environment->getSkin();

				if (!FileSystem || !FileBox || !skin)
					return;

				if (FileList)
					FileList->drop();

				FileBox->clear();

				FileList = FileSystem->createFileList();
				core::stringw s;

#if !defined(_IRR_WINDOWS_CE_PLATFORM_)
				setlocale(LC_ALL, "");
#endif

				if (FileList)
				{
					for (u32 i = 0; i < FileList->getFileCount(); ++i)
					{
#ifndef _IRR_WCHAR_FILESYSTEM
						const c8 *FS = (const c8 *)FileList->getFileName(i).c_str();

						/*						wchar_t *ws = new wchar_t[strlen(FS) + 1];
												size_t len = 0;
												errno_t e = mbstowFS_s(&len,ws, strlen(FS) + 1, FS, strlen(FS));
												ws[len] = 0;
												s = ws;
												delete[] ws;
												*/
						s = stringw(FS).c_str();
#else
						s = FileList->getFileName(i).c_str();
#endif

						if (FileList->isDirectory(i)) 
						{ 
							FileBox->addItem(s.c_str(), skin->getIcon(FileList->isDirectory(i) ? EGDI_DIRECTORY : EGDI_FILE)); 
						}
						else
						if (isCorrectType(m_FilePickerType, s))
						{
							FileBox->addItem(s.c_str(), skin->getIcon(FileList->isDirectory(i) ? EGDI_DIRECTORY : EGDI_FILE));
						}
					}
				}

				if (FileNameText)
				{
#ifndef _IRR_WCHAR_FILESYSTEM
					const c8 *FS = (const c8 *)FileSystem->getWorkingDirectory().c_str();
					s = stringw(FS).c_str();
					/*
					wchar_t *ws = new wchar_t[strlen(FS) + 1];
					size_t len = 0;
					errno_t e = mbstowFS_s(&len,ws,strlen(FS)+1, FS, strlen(FS));
					//int len = mbstowFS(ws, FS, strlen(FS));
					ws[len] = 0;
					s = ws;
					delete[] ws;
					*/
#else
					s = FileSystem->getWorkingDirectory();
#endif

					FileDirectory = s;
					FileNameText->setText(s.c_str());
				}
			}


			//! sends the event that the file has been selected.
			void sendSelectedEvent(EGUI_EVENT_TYPE type)
			{
				SEvent event;
				event.EventType = EET_GUI_EVENT;
				event.GUIEvent.Caller = this;
				event.GUIEvent.Element = 0;
				event.GUIEvent.EventType = type;
				Parent->OnEvent(event);
				//Environment->postEventFromUser(event);
			}


			//! sends the event that the file choose process has been canceld
			void sendCancelEvent()
			{
				SEvent event;
				event.EventType = EET_GUI_EVENT;
				event.GUIEvent.Caller = this;
				event.GUIEvent.Element = 0;
				event.GUIEvent.EventType = EGET_FILE_CHOOSE_DIALOG_CANCELLED;
				Parent->OnEvent(event);
			}
		};

		class FSGUI_FilePicker : public FSGUI_Window
		{
		public:
			FSGUI_FileOpenDialog* m_FileOpenDlg;

			FilePickerType getFilePickerType()
			{
				if (m_FileOpenDlg) return m_FileOpenDlg->m_FilePickerType;
				return FPT_NONE;
			}

			// simple constructor that auto sets the FSGUI_LAYOUT::FSGUI_BORDER flag and layout
			FSGUI_FilePicker(FSGUI_Desktop* desktop, IGUIEnvironment* env, FSGUI_Window* FSparent, int id, rect<s32> r, long flags, FSGUI_LAYOUTSTYLE ls, wchar_t* text, int minWidth, int minHeight, FilePickerType type)
				: FSGUI_Window(desktop, env, FSparent, id, r, flags, ls, text,minWidth, minHeight)
			{
				m_FileOpenDlg = new FSGUI_FileOpenDialog(L"FilePicker", Environment, this, FSID_FILEPICKER, true,0,type);
			}

			virtual bool OnEvent(const SEvent& e)
			{
				return Parent->OnEvent(e);
			}

			stringc getFileName()
			{
				if (m_FileOpenDlg)	return stringc(m_FileOpenDlg->getFileName());
				else return "UNKNOWN";
			}

			virtual void calculateLayout()
			{
				// call the base class
				FSGUI_Window::calculateLayout();

				rect<s32> r = getClientRect();
				r.LowerRightCorner -= getAbsolutePosition().UpperLeftCorner;
				r.UpperLeftCorner -= getAbsolutePosition().UpperLeftCorner;
				r.UpperLeftCorner += 3;
				r.LowerRightCorner -= 3;

				// position all of the textboxes where we want them
				if (m_FileOpenDlg)
				{
					m_FileOpenDlg->setRelativePosition(r);
					m_FileOpenDlg->updateAbsolutePosition();
				}
			}
		};

} // end namespace