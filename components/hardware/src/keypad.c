#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#include "keypad.h"

#define REPEAT_HOLDOFF (250/portTICK_PERIOD_MS)
#define REPEAT_RATE (80/portTICK_PERIOD_MS)

#define KEYPAD_IO_X ADC1_CHANNEL_6
#define KEYPAD_IO_Y ADC1_CHANNEL_7
#define KEYPAD_IO_SELECT GPIO_NUM_27
#define KEYPAD_IO_START GPIO_NUM_39
#define KEYPAD_IO_A GPIO_NUM_32
#define KEYPAD_IO_B GPIO_NUM_33
#define KEYPAD_IO_MENU GPIO_NUM_13
#define KEYPAD_IO_VOLUME GPIO_NUM_0

static size_t num_queues;
static QueueHandle_t *queues;

static void keypad_task(void *arg)
{
    keypad_info_t keypad_info;
    uint16_t changes;

    TickType_t down_ticks[4];
    uint16_t repeat_count[4];

    while (true) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        keypad_info.state = keypad_debounce(keypad_sample(), &changes);
        keypad_info.pressed = keypad_info.state & changes;
        keypad_info.released = ~keypad_info.state & changes;

        TickType_t now = xTaskGetTickCount();
        for (int i = 0; i < 4; i++) {
            if ((keypad_info.pressed >> i) & 1) {
                down_ticks[i] = now;
                repeat_count[i] = UINT16_MAX;
            } else if ((keypad_info.state >> i) & 1) {
                if (now - down_ticks[i] >= REPEAT_HOLDOFF) {
                    uint16_t n = (now - down_ticks[i] - REPEAT_HOLDOFF) / REPEAT_RATE;
                    if (repeat_count[i] != n) {
                        repeat_count[i] = n;
                        keypad_info.pressed |= (1 << i);
                    }
                }
            }
        }

        if (keypad_info.pressed || keypad_info.released) {
            for (int i = 0; i < num_queues; i++) {
                xQueueSend(queues[i], &keypad_info, 0);
            }
        }
    }
}

void keypad_init(void)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(KEYPAD_IO_X, ADC_ATTEN_11db);
    adc1_config_channel_atten(KEYPAD_IO_Y, ADC_ATTEN_11db);

    gpio_set_direction(KEYPAD_IO_SELECT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEYPAD_IO_SELECT, GPIO_PULLUP_ONLY);
    
    gpio_set_direction(KEYPAD_IO_START, GPIO_MODE_INPUT);
    
    gpio_set_direction(KEYPAD_IO_A, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEYPAD_IO_A, GPIO_PULLUP_ONLY);

    gpio_set_direction(KEYPAD_IO_B, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEYPAD_IO_B, GPIO_PULLUP_ONLY);

    gpio_set_direction(KEYPAD_IO_MENU, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEYPAD_IO_MENU, GPIO_PULLUP_ONLY);

    gpio_set_direction(KEYPAD_IO_VOLUME, GPIO_MODE_INPUT);

    xTaskCreate(keypad_task, "keypad", 1024, NULL, 5, NULL);
}

uint16_t keypad_sample(void)
{
    uint16_t sample = 0;

    int joyX = adc1_get_raw(KEYPAD_IO_X);
    int joyY = adc1_get_raw(KEYPAD_IO_Y);

    if (joyX > 2048 + 1024) {
        sample |= KEYPAD_LEFT;
    } else if (joyX > 1024) {
        sample |= KEYPAD_RIGHT;
    }

    if (joyY > 2048 + 1024) {
        sample |= KEYPAD_UP;
    } else if (joyY > 1024) {
        sample |= KEYPAD_DOWN;
    }

    if (!gpio_get_level(KEYPAD_IO_SELECT)) {
        sample |= KEYPAD_SELECT;
    }

    if (!gpio_get_level(KEYPAD_IO_START)) {
        sample |= KEYPAD_START;
    }

    if (!gpio_get_level(KEYPAD_IO_A)) {
        sample |= KEYPAD_A;
    }

    if (!gpio_get_level(KEYPAD_IO_B)) {
        sample |= KEYPAD_B;
    }

    if (!gpio_get_level(KEYPAD_IO_MENU)) {
        sample |= KEYPAD_MENU;
    }

    if (!gpio_get_level(KEYPAD_IO_VOLUME)) {
        sample |= KEYPAD_VOLUME;
    }

    return sample;
}

uint16_t keypad_debounce(uint16_t sample, uint16_t *changes)
{
    static uint16_t state, cnt0, cnt1;
    uint16_t delta, toggle;

    delta = sample ^ state;
    cnt1 = (cnt1 ^ cnt0) & delta;
    cnt0 = ~cnt0 & delta;

    toggle = delta & ~(cnt0 | cnt1);
    state ^= toggle;
    if (changes) {
        *changes = toggle;
    }

    return state;
}

QueueHandle_t keypad_get_queue(void)
{
    QueueHandle_t queue = xQueueCreate(2, sizeof(keypad_info_t));
    assert(queue != NULL);
    queues = realloc(queues, sizeof(QueueHandle_t) * (num_queues + 1));
    queues[num_queues] = queue;
    num_queues += 1;
    return queue;
}