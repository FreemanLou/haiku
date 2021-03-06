////////////////////////////////////////////////////////////////////////////////
//
//	File: GIFView.cpp
//
//	Date: December 1999
//
//	Author: Daniel Switkin
//
//	Copyright 2003 (c) by Daniel Switkin. This file is made publically available
//	under the BSD license, with the stipulations that this complete header must
//	remain at the top of the file indefinitely, and credit must be given to the
//	original author in any about box using this software.
//
////////////////////////////////////////////////////////////////////////////////

// Additional authors:	Stephan Aßmus, <superstippi@gmx.de>
//                      Maxime Simon, <maxime.simon@gmail.com>
//			Philippe Saint-Pierre, <stpere@gmail.com>

#include "GIFView.h"
#include "GIFTranslator.h"

#include <stdio.h>
#include <stdlib.h>

#include <Catalog.h>
#include <InterfaceKit.h>
#include <LayoutBuilder.h>
#include <String.h>

#include "SavePalette.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "GIFView"


// constructor
GIFView::GIFView(TranslatorSettings* settings)
	: 
	BView("GIFView", B_WILL_DRAW),
	fSettings(settings)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BStringView *title = new BStringView("Title", B_TRANSLATE("GIF image translator"));
	title->SetFont(be_bold_font);

        char version_string[100];
        snprintf(version_string, sizeof(version_string), B_TRANSLATE("v%d.%d.%d %s"),
                int(B_TRANSLATION_MAJOR_VERSION(GIF_TRANSLATOR_VERSION)),
                int(B_TRANSLATION_MINOR_VERSION(GIF_TRANSLATOR_VERSION)),
                int(B_TRANSLATION_REVISION_VERSION(GIF_TRANSLATOR_VERSION)),
                __DATE__);

	BStringView *version = new BStringView("Version", version_string);

	const char *copyrightString = "©2003 Daniel Switkin, software@switkin.com";
	BStringView *copyright = new BStringView("Copyright", copyrightString);

	// menu fields (Palette & Colors)
	fWebSafeMI = new BMenuItem(B_TRANSLATE("Websafe"),
		new BMessage(GV_WEB_SAFE), 0, 0);
	fBeOSSystemMI = new BMenuItem(B_TRANSLATE("BeOS system"),
		new BMessage(GV_BEOS_SYSTEM), 0, 0);
	fGreyScaleMI = new BMenuItem(B_TRANSLATE("Greyscale"),
		new BMessage(GV_GREYSCALE), 0, 0);
	fOptimalMI = new BMenuItem(B_TRANSLATE("Optimal"),
		new BMessage(GV_OPTIMAL), 0, 0);
	fPaletteM = new BPopUpMenu("PalettePopUpMenu", true, true, B_ITEMS_IN_COLUMN);
	fPaletteM->AddItem(fWebSafeMI);
	fPaletteM->AddItem(fBeOSSystemMI);
	fPaletteM->AddItem(fGreyScaleMI);
	fPaletteM->AddItem(fOptimalMI);

	fColorCountM = new BPopUpMenu("ColorCountPopUpMenu", true, true,
		B_ITEMS_IN_COLUMN);
	int32 count = 2;
	for (int32 i = 0; i < 8; i++) {
		BMessage* message = new BMessage(GV_SET_COLOR_COUNT);
		message->AddInt32(GIF_SETTING_PALETTE_SIZE, i + 1);
		BString label;
		label << count;
		fColorCountMI[i] = new BMenuItem(label.String(), message, 0, 0);
		fColorCountM->AddItem(fColorCountMI[i]);
		count *= 2;
	}
	fColorCount256MI = fColorCountMI[7];

 	fPaletteMF = new BMenuField(B_TRANSLATE("Palette"), fPaletteM);
 	fColorCountMF = new BMenuField(B_TRANSLATE("Colors"), fColorCountM);

 	// check boxes
 	fUseDitheringCB = new BCheckBox(B_TRANSLATE("Use dithering"),
 		new BMessage(GV_USE_DITHERING));

 	fInterlacedCB = new BCheckBox(B_TRANSLATE("Write interlaced images"),
 		new BMessage(GV_INTERLACED));

 	fUseTransparentCB = new BCheckBox(B_TRANSLATE("Write transparent images"),
 		new BMessage(GV_USE_TRANSPARENT));

 	// radio buttons
 	fUseTransparentAutoRB = new BRadioButton(
 		B_TRANSLATE("Automatic (from alpha channel)"),
 		new BMessage(GV_USE_TRANSPARENT_AUTO));

 	fUseTransparentColorRB = new BRadioButton(B_TRANSLATE("Use RGB color"),
 		new BMessage(GV_USE_TRANSPARENT_COLOR));

 	fTransparentRedTC = new BTextControl("", "0", new BMessage(GV_TRANSPARENT_RED));
 	fTransparentGreenTC = new BTextControl("", "0", new BMessage(GV_TRANSPARENT_GREEN));
 	fTransparentBlueTC = new BTextControl("", "0", new BMessage(GV_TRANSPARENT_BLUE));

	BTextView *tr = fTransparentRedTC->TextView();
	BTextView *tg = fTransparentGreenTC->TextView();
	BTextView *tb = fTransparentBlueTC->TextView();

	for (uint32 x = 0; x < 256; x++) {
		if (x < '0' || x > '9') {
			tr->DisallowChar(x);
			tg->DisallowChar(x);
			tb->DisallowChar(x);
		}
	}

	BLayoutBuilder::Group<>(this, B_VERTICAL, 7)
		.SetInsets(5)
		.AddGrid(10, 10)
			.Add(title, 0, 0)
			.Add(version, 1, 0)
		.End()
		.Add(copyright)
		.AddGlue()

		.AddGrid(10, 10)
			.Add(fPaletteMF->CreateLabelLayoutItem(), 0, 0)
        	.Add(fPaletteMF->CreateMenuBarLayoutItem(), 1, 0)

			.Add(fColorCountMF->CreateLabelLayoutItem(), 0, 1)
        	.Add(fColorCountMF->CreateMenuBarLayoutItem(), 1, 1)
		.End()
		.AddGlue()

		.Add(fUseDitheringCB)
		.Add(fInterlacedCB)
		.Add(fUseTransparentCB)

		.Add(fUseTransparentAutoRB)
		.AddGrid(10, 10)
			.Add(fUseTransparentColorRB, 0, 0)
			.Add(fTransparentRedTC, 1, 0)
			.Add(fTransparentGreenTC, 2, 0)
			.Add(fTransparentBlueTC, 3, 0)
		.End()
		.AddGlue();

	BFont font;
	GetFont(&font);
	SetExplicitPreferredSize(BSize((font.Size() * 400)/12, (font.Size() * 300)/12));

	RestorePrefs();
}


GIFView::~GIFView()
{
	fSettings->Release();
}


void
GIFView::RestorePrefs()
{
	fColorCountMF->SetEnabled(false);
	fUseDitheringCB->SetEnabled(true);

	switch (fSettings->SetGetInt32(GIF_SETTING_PALETTE_MODE)) {
		case WEB_SAFE_PALETTE:
			fWebSafeMI->SetMarked(true);
			break;
		case BEOS_SYSTEM_PALETTE:
			fBeOSSystemMI->SetMarked(true);
			break;
		case GREYSCALE_PALETTE:
			fGreyScaleMI->SetMarked(true);
			fUseDitheringCB->SetEnabled(false);
			break;
		case OPTIMAL_PALETTE:
			fOptimalMI->SetMarked(true);
			fColorCountMF->SetEnabled(true);
			break;
		default:
			int32 value = WEB_SAFE_PALETTE;
			fSettings->SetGetInt32(GIF_SETTING_PALETTE_MODE, &value);
			fSettings->SaveSettings();
			fWebSafeMI->SetMarked(true);
			break;
	}

	if (fColorCountMF->IsEnabled()
		&& fSettings->SetGetInt32(GIF_SETTING_PALETTE_SIZE) > 0
		&& fSettings->SetGetInt32(GIF_SETTING_PALETTE_SIZE) <= 8) {
		// display the stored color count
		fColorCountMI[fSettings->SetGetInt32(GIF_SETTING_PALETTE_SIZE) - 1]
			->SetMarked(true);
	} else {
		// display 256 colors
		fColorCount256MI->SetMarked(true);
		int32 value = 8;
		fSettings->SetGetInt32(GIF_SETTING_PALETTE_SIZE, &value);
		fSettings->SaveSettings();
	}

	fInterlacedCB->SetValue(fSettings->SetGetBool(GIF_SETTING_INTERLACED));

	if (fGreyScaleMI->IsMarked()) 
		fUseDitheringCB->SetValue(false);
	else 
		fUseDitheringCB->SetValue(
			fSettings->SetGetBool(GIF_SETTING_USE_DITHERING));
	fUseTransparentCB->SetValue(
		fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT));
	fUseTransparentAutoRB->SetValue(
		fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT_AUTO));
	fUseTransparentColorRB->SetValue(
		!fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT_AUTO));
	if (fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT)) {
		fUseTransparentAutoRB->SetEnabled(true);
		fUseTransparentColorRB->SetEnabled(true);
		fTransparentRedTC->SetEnabled(
			!fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT_AUTO));
		fTransparentGreenTC->SetEnabled(
			!fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT_AUTO));
		fTransparentBlueTC->SetEnabled(
			!fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT_AUTO));
	} else {
		fUseTransparentAutoRB->SetEnabled(false);
		fUseTransparentColorRB->SetEnabled(false);
		fTransparentRedTC->SetEnabled(false);
		fTransparentGreenTC->SetEnabled(false);
		fTransparentBlueTC->SetEnabled(false);
	}

	char temp[4];
	sprintf(temp, "%d",
		(int)fSettings->SetGetInt32(GIF_SETTING_TRANSPARENT_RED));
	fTransparentRedTC->SetText(temp);
	sprintf(temp, "%d",
		(int)fSettings->SetGetInt32(GIF_SETTING_TRANSPARENT_GREEN));
	fTransparentGreenTC->SetText(temp);
	sprintf(temp, "%d",
		(int)fSettings->SetGetInt32(GIF_SETTING_TRANSPARENT_BLUE));
	fTransparentBlueTC->SetText(temp);
}


// AllAttached
void
GIFView::AllAttached()
{
	BView::AllAttached();
	BMessenger msgr(this);
	fInterlacedCB->SetTarget(msgr);
	fUseDitheringCB->SetTarget(msgr);
	fUseTransparentCB->SetTarget(msgr);
	fUseTransparentAutoRB->SetTarget(msgr);
	fUseTransparentColorRB->SetTarget(msgr);
	fTransparentRedTC->SetTarget(msgr);
	fTransparentGreenTC->SetTarget(msgr);
	fTransparentBlueTC->SetTarget(msgr);
	fPaletteM->SetTargetForItems(msgr);
	fColorCountM->SetTargetForItems(msgr);
}


// MessageReceived
void
GIFView::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case GV_WEB_SAFE:
		{
			int32 value = WEB_SAFE_PALETTE;
			fSettings->SetGetInt32(GIF_SETTING_PALETTE_MODE, &value);
			fUseDitheringCB->SetEnabled(true);
			fColorCountMF->SetEnabled(false);
			fColorCount256MI->SetMarked(true);
			break;
		}
		case GV_BEOS_SYSTEM:
		{
			int32 value = BEOS_SYSTEM_PALETTE;
			fSettings->SetGetInt32(GIF_SETTING_PALETTE_MODE, &value);
			fUseDitheringCB->SetEnabled(true);
			fColorCountMF->SetEnabled(false);
			fColorCount256MI->SetMarked(true);
			break;
		}
		case GV_GREYSCALE:
		{
			int32 value = GREYSCALE_PALETTE;
			bool usedithering = false;
			fSettings->SetGetInt32(GIF_SETTING_PALETTE_MODE, &value);
			fSettings->SetGetBool(GIF_SETTING_USE_DITHERING, &usedithering);
			fUseDitheringCB->SetEnabled(false);
			fUseDitheringCB->SetValue(false);
			fColorCountMF->SetEnabled(false);
			fColorCount256MI->SetMarked(true);
			break;
		}
		case GV_OPTIMAL:
		{
			int32 value = OPTIMAL_PALETTE;
			fSettings->SetGetInt32(GIF_SETTING_PALETTE_MODE, &value);
			fUseDitheringCB->SetEnabled(true);
			fColorCountMF->SetEnabled(true);
			fColorCountMI[fSettings->SetGetInt32(GIF_SETTING_PALETTE_SIZE) - 1]
				->SetMarked(true);
			break;
		}
		case GV_SET_COLOR_COUNT:
			if (fColorCountMF->IsEnabled()) {
				int32 sizeInBits;
				if (message->FindInt32(GIF_SETTING_PALETTE_SIZE, &sizeInBits)
					>= B_OK) {
					if (sizeInBits > 0 && sizeInBits <= 8)
						fSettings->SetGetInt32(GIF_SETTING_PALETTE_SIZE,
							&sizeInBits);
				}
			}
			break;
		case GV_INTERLACED:
		{
			bool value = fInterlacedCB->Value();
			fSettings->SetGetBool(GIF_SETTING_INTERLACED, &value);
			break;
		}
		case GV_USE_DITHERING:
		{
			bool value = fUseDitheringCB->Value();
			fSettings->SetGetBool(GIF_SETTING_USE_DITHERING, &value);
			break;
		}
		case GV_USE_TRANSPARENT:
		{
			bool value = fUseTransparentCB->Value();
			fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT, &value);
			if (value) {
				fUseTransparentAutoRB->SetEnabled(true);
				fUseTransparentColorRB->SetEnabled(true);
				fTransparentRedTC->SetEnabled(fUseTransparentColorRB->Value());
				fTransparentGreenTC->SetEnabled(fUseTransparentColorRB->Value());
				fTransparentBlueTC->SetEnabled(fUseTransparentColorRB->Value());
			} else {
				fUseTransparentAutoRB->SetEnabled(false);
				fUseTransparentColorRB->SetEnabled(false);
				fTransparentRedTC->SetEnabled(false);
				fTransparentGreenTC->SetEnabled(false);
				fTransparentBlueTC->SetEnabled(false);
			}
			break;
		}
		case GV_USE_TRANSPARENT_AUTO:
		{
			bool value = true;
			fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT_AUTO, &value);
			fTransparentRedTC->SetEnabled(false);
			fTransparentGreenTC->SetEnabled(false);
			fTransparentBlueTC->SetEnabled(false);
			break;
		}
		case GV_USE_TRANSPARENT_COLOR:
		{
			bool value = false;
			fSettings->SetGetBool(GIF_SETTING_USE_TRANSPARENT_AUTO, &value);
			fTransparentRedTC->SetEnabled(true);
			fTransparentGreenTC->SetEnabled(true);
			fTransparentBlueTC->SetEnabled(true);
			break;
		}
		case GV_TRANSPARENT_RED:
		{	
			int32 value = CheckInput(fTransparentRedTC);
			fSettings->SetGetInt32(GIF_SETTING_TRANSPARENT_RED, &value);
			break;
		}
		case GV_TRANSPARENT_GREEN:
		{
			int32 value = CheckInput(fTransparentGreenTC);
			fSettings->SetGetInt32(GIF_SETTING_TRANSPARENT_GREEN, &value);
			break;
		}
		case GV_TRANSPARENT_BLUE:
		{
			int32 value = CheckInput(fTransparentBlueTC);
			fSettings->SetGetInt32(GIF_SETTING_TRANSPARENT_BLUE, &value);
			break;
		}
		default:
			BView::MessageReceived(message);
			break;
	}
	fSettings->SaveSettings();
}


int GIFView::CheckInput(BTextControl *control) {
	int value = atoi(control->Text());
	if (value < 0 || value > 255) {
		value = (value < 0) ? 0 : 255;
		char temp[4];
		sprintf(temp, "%d", value);
		control->SetText(temp);
	}
	return value;
}

