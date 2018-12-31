#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

enum {
    KEYPAD_UP     = 1,
    KEYPAD_RIGHT  = 2,
    KEYPAD_DOWN   = 4,
    KEYPAD_LEFT   = 8,
    KEYPAD_SELECT = 16,
    KEYPAD_START  = 32,
    KEYPAD_A      = 64,
    KEYPAD_B      = 128,
    KEYPAD_MENU   = 256,
    KEYPAD_VOLUME = 512,
};

typedef struct keypad_info_t {
    uint16_t state;
    uint16_t pressed;
    uint16_t released;
} keypad_info_t;

void keypad_init(void);
uint16_t keypad_sample(void);
uint16_t keypad_debounce(uint16_t sample, uint16_t *changes);
QueueHandle_t keypad_get_queue(void);
#define keypad_queue_receive(xQueue,pvBuffer,xTicksToWait) \
    xQueueGenericReceive( ( xQueue ), ( pvBuffer ), ( xTicksToWait ), pdFALSE )