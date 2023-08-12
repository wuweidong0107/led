#ifndef _LED_H
#define _LED_H

#ifdef __cplusplus
extern "C" {
#endif

enum led_error_code {
    LED_ERROR_ARG = -1,
    LED_ERROR_OPEN = -2,
    LED_ERROR_QUERY = -3,
    LED_ERROR_IO = -4,
    LED_ERROR_CLOSE = -5,
};

typedef struct led_handle led_t;

led_t* led_new(void);
void led_free(led_t *led);
int led_open(led_t* led, const char* name);
int led_close(led_t* led);
int led_write(led_t* led, unsigned int value);
int led_read(led_t* led, unsigned int* value);
int led_get_max_brightness(led_t* led, unsigned int* max_brightness);
int led_get_brightness(led_t* led, unsigned int* brightness);
int led_set_brightness(led_t* led, unsigned int brightness);
void led_dump(void);

#ifdef __cplusplus
}
#endif

#endif