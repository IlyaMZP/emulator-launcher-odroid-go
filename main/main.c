#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "display.h"
#include "keypad.h"
#include "sdcard.h"

#include "graphics.h"
#include "tf.h"
#include "OpenSans_Regular_11X12.h"
#include "periodic.h"
#include "statusbar.h"
#include "ui_dialog.h"

#include "emulator_dialog.h"
#include "settings_dialog.h"

static void launcher_task(void *arg);

void app_main(void)
{
	display_init();

	tf_t *tf = tf_new(&font_OpenSans_Regular_11X12, 0xFFFF, 240, TF_ALIGN_CENTER | TF_WORDWRAP);
	const char *s;
	tf_metrics_t m;
	point_t p;

	s = "Initializing...";
	m = tf_get_str_metrics(tf, s);
	p.x = DISPLAY_WIDTH/2 - tf->width/2;
	p.y = DISPLAY_HEIGHT/2 - m.height/2;
	tf_draw_str(fb, tf, s, p);
	display_update();

	keypad_init();
	ESP_ERROR_CHECK(nvs_flash_init());
	sdcard_init("/sd");
	statusbar_init();

	tf_free(tf);

	xTaskCreate(launcher_task, "launcher", 8192, NULL, 5, NULL);
}

static void launcher_task(void *arg)
{
	tf_t *tf = tf_new(&font_OpenSans_Regular_11X12, 0xFFFF, 240, TF_ALIGN_CENTER | TF_WORDWRAP);

	QueueHandle_t keypad = keypad_get_queue();

	char *s = "Press Menu button for the menu.";
	tf_metrics_t m = tf_get_str_metrics(tf, s);
	point_t p = {
		.x = fb->width/2 - tf->width/2,
		.y = fb->height/2 - m.height/2,
	};
	memset(fb->data + fb->width * 16 * fb->bytes_per_pixel, 0, fb->width * (fb->height - 32) * fb->bytes_per_pixel);
	tf_draw_str(fb, tf, s, p);
	display_update();

	while (true) {
		rect_t r = {
			.x = DISPLAY_WIDTH/2 - 310/2,
			.y = DISPLAY_HEIGHT/2 - 200/2,
			.width = 310,
			.height = 215,
		};

		ui_dialog_t *d = ui_dialog_new(NULL, r, NULL);
		d->keypad = keypad;
		rect_t lr = {
			.x = 0,
			.y = 0,
			.width = 310 - 2,
			.height = 215 - 2,
		};
		ui_list_t *list = ui_dialog_add_list(d, lr);
		ui_list_append_text(list, "Emulators", emulator_list_dialog, NULL);
		ui_list_append_text(list, "Emulator Configuration", emulator_settings_dialog, NULL);
		ui_dialog_showmodal(d);
		ui_dialog_destroy(d);

		keypad_info_t keys;
		while (true) {
			if (keypad_queue_receive(keypad, &keys, 50/portTICK_RATE_MS)) {
				if (keys.pressed & KEYPAD_MENU) {
					break;
				}
			}
			periodic_tick();
		}
	}
}
