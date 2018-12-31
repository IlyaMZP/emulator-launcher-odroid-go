#include <string.h>

//#include "esp_wifi.h"

#include "display.h"
#include "sdcard.h"

#include "OpenSans_Regular_11X12.h"
#include "icons_16X16.h"
#include "periodic.h"
#include "statusbar.h"
#include "tf.h"
#include "odroid_settings.h"


#define STATUSBAR_HEIGHT (16)

typedef struct {
	bool sdcard_present;
} stateinfo_t;

static tf_t *s_icons;
static rect_t s_rect;
static stateinfo_t last_state;


static void statusbar_update(periodic_handle_t handle, void *arg)
{

	stateinfo_t state = {
		.sdcard_present = sdcard_present(),
	};

	if (memcmp(&last_state, &state, sizeof(stateinfo_t)) == 0) {
		return;
	}

	memcpy(&last_state, &state, sizeof(stateinfo_t));
	memset(fb->data, 0, DISPLAY_WIDTH * STATUSBAR_HEIGHT * fb->bytes_per_pixel);

	point_t p = {
		.x = fb->width - 16,
		.y = 0,
	};
	tf_draw_glyph(fb, s_icons, FONT_ICON_BATTERY5, p);
	p.x -= 16;
	if (sdcard_present()) {
		tf_draw_glyph(fb, s_icons, FONT_ICON_SDCARD, p);
		p.x -= 16;
	}

	switch(odroid_settings_Volume_get()) {
		case ODROID_VOLUME_LEVEL4:
			tf_draw_glyph(fb, s_icons, FONT_ICON_SPEAKER3, p);
			break;
		case ODROID_VOLUME_LEVEL3:
			tf_draw_glyph(fb, s_icons, FONT_ICON_SPEAKER2, p);
			break;
		case ODROID_VOLUME_LEVEL2:
			tf_draw_glyph(fb, s_icons, FONT_ICON_SPEAKER1, p);
			break;
		case ODROID_VOLUME_LEVEL1:
			tf_draw_glyph(fb, s_icons, FONT_ICON_SPEAKER1, p);
			break;
		default:
			tf_draw_glyph(fb, s_icons, FONT_ICON_SPEAKER0, p);
			break;
	}
	p.x -= 16;

	display_update_rect(s_rect);
}

void statusbar_init(void)
{
	s_icons = tf_new(&font_icons_16X16, 0xFFFF, 0, 0);
	s_rect.x = 0;
	s_rect.y = 0;
	s_rect.width = DISPLAY_WIDTH;
	s_rect.height = STATUSBAR_HEIGHT;

	statusbar_update(NULL, NULL);
	periodic_register(250/portTICK_PERIOD_MS, statusbar_update, NULL);
}
