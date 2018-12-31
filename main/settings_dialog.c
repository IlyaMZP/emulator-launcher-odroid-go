#include <string.h>

//#include "emulator.h"
#include "display.h"
#include "ui_dialog.h"
#include <ctype.h>
#include "odroid_settings.h"

int setting;

enum {
VOLUME = 0,
VREF,
APPSLOT,
DATASLOT,
BACKLIGHT,
STARTACTION,
SCALEDISABLED_NES,
SCALEDISABLED_GB,
SCALEDISABLED_SMS,
AUDIOSINK,
};

void setter(ui_list_item_t *item, void *arg) {
	switch(setting) {
		case VOLUME:
			odroid_settings_Volume_set((int) arg);
			break;
		case SCALEDISABLED_NES:
			odroid_settings_ScaleDisabled_set(ODROID_SCALE_DISABLE_NES, (int) arg);
			break;
		case SCALEDISABLED_GB:
			odroid_settings_ScaleDisabled_set(SCALEDISABLED_GB, (int) arg);
			break;
		case SCALEDISABLED_SMS:
			odroid_settings_ScaleDisabled_set(SCALEDISABLED_SMS, (int) arg);
			break;
		case BACKLIGHT:
			odroid_settings_Backlight_set((int) arg);
			break;
		case AUDIOSINK:
			odroid_settings_AudioSink_set((ODROID_AUDIO_SINK) arg);
			break;
		case VREF: break;
		case APPSLOT: break;
		case DATASLOT: break;
		case STARTACTION: break;
		default: break;
	}
}

void placeholder(ui_list_item_t *item, void *arg) {
	setting  = (int) arg;


	rect_t r = {
		.x = fb->width/2 - 300/2,
		.y = fb->height/2 - 180/2,
		.width = 300,
		.height = 200,
	};
	ui_dialog_t *d = ui_dialog_new(item->list->d, r, NULL);

	rect_t lr = {
		.x = 0,
		.y = 0,
		.width = d->cr.width,
		.height = d->cr.height,
	};
	ui_list_t *list = ui_dialog_add_list(d, lr);

	switch(setting) {
		case VOLUME:
			ui_list_append_text(list, "Volume 0", setter, (void *)ODROID_VOLUME_LEVEL0);
			ui_list_append_text(list, "Volume 1", setter, (void *)ODROID_VOLUME_LEVEL1);
			ui_list_append_text(list, "Volume 2", setter, (void *)ODROID_VOLUME_LEVEL2);
			ui_list_append_text(list, "Volume 3", setter, (void *)ODROID_VOLUME_LEVEL3);
			ui_list_append_text(list, "Volume 4", setter, (void *)ODROID_VOLUME_LEVEL4);
			break;
		case BACKLIGHT:
			ui_list_append_text(list, "Brightness 1", setter, (void *)60);
			ui_list_append_text(list, "Brightness 2", setter, (void *)100);
			ui_list_append_text(list, "Brightness 3", setter, (void *)150);
			ui_list_append_text(list, "Brightness 4", setter, (void *)255);
			break;
		case SCALEDISABLED_NES:
			ui_list_append_text(list, "Enable", setter, (void *)1);
			ui_list_append_text(list, "Disable", setter, (void *)0);
			break;
		case SCALEDISABLED_GB:
			ui_list_append_text(list, "Enable", setter, (void *)1);
			ui_list_append_text(list, "Disable", setter, (void *)0);
			break;
		case SCALEDISABLED_SMS:
			ui_list_append_text(list, "Enable", setter, (void *)1);
			ui_list_append_text(list, "Disable", setter, (void *)0);
			break;
		case AUDIOSINK:
			ui_list_append_text(list, "Speacker", setter, (void *)ODROID_AUDIO_SINK_SPEAKER);
			ui_list_append_text(list, "DAC", setter, (void *)ODROID_AUDIO_SINK_DAC);
			break;
		case VREF: break;
		case APPSLOT: break;
		case DATASLOT: break;
		case STARTACTION: break;

		default: break;
	}

	ui_dialog_showmodal(d);
	ui_dialog_destroy(d);
}

void emulator_settings_dialog(ui_list_item_t *item, void *arg) {
	rect_t r = {
		.x = fb->width/2 - 310/2,
		.y = fb->height/2 - 200/2,
		.width = 310,
		.height = 215,
	};
	ui_dialog_t *d = ui_dialog_new(item->list->d, r, "Settings");

	rect_t lr = {
		.x = 0,
		.y = 0,
		.width = d->cr.width,
		.height = d->cr.height,
	};

	ui_list_t *list = ui_dialog_add_list(d, lr);
	ui_list_append_text(list, "Volume", placeholder, (void *)VOLUME);
//	ui_list_append_text(list, "VRef", placeholder, (void *)VREF);
//	ui_list_append_text(list, "AppSlot", placeholder, (void *)APPSLOT);
//	ui_list_append_text(list, "DataSlot", placeholder, (void *)DATASLOT);
	ui_list_append_text(list, "Backlight", placeholder, (void *)BACKLIGHT);
	ui_list_append_text(list, "Scaling for NES", placeholder, (void *)SCALEDISABLED_NES);
	ui_list_append_text(list, "Scaling for GB", placeholder, (void *)SCALEDISABLED_GB);
	ui_list_append_text(list, "Scaling for SMS", placeholder, (void *)SCALEDISABLED_SMS);
//	ui_list_append_text(list, "StartAction", placeholder, (void *)STARTACTION);
	ui_list_append_text(list, "Audio Sink", placeholder, (void *)AUDIOSINK);
	ui_dialog_showmodal(d);
	ui_dialog_destroy(d);
}
