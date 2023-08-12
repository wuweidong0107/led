#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "led.h"

struct led_handle {
    char name[64];
    unsigned int max_brightness;

    struct {
        int c_errno;
        char errmsg[96];
    } error;
};

static int _led_error(led_t* led, int code, int c_errno, const char* fmt, ...)
{
    va_list ap;
    led->error.c_errno = c_errno;
    va_start(ap, fmt);
    vsnprintf(led->error.errmsg, sizeof(led->error.errmsg), fmt, ap);
    va_end(ap);

    if (c_errno) {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(led->error.errmsg + strlen(led->error.errmsg), 
            sizeof(led->error.errmsg) - strlen(led->error.errmsg),
            ": %s [errno %d]", buf, c_errno);
    }
    return code;
}

int led_errno(led_t* led)
{
    return led->error.c_errno;
}

const char* led_errmsg(led_t* led)
{
    return led->error.errmsg;
}

int led_get_max_brightness(led_t* led, unsigned int* max_brightness)
{
    char led_path[PATH_MAX];
    int fd, ret;
    char buf[16];

    snprintf(led_path, sizeof(led_path), "/sys/class/leds/%s/brightness", led->name);

    if ((fd = open(led_path, O_RDWR)) < 0) {
        return _led_error(led, LED_ERROR_OPEN, errno, "Openning LED maxbrightness");
    }

    if ((ret = read(fd, buf, sizeof(buf))) < 0) {
        int errsv = errno;
        close(fd);
        return _led_error(led, LED_ERROR_IO, errno, "Reading LED maxbrightness");
    }

    if (close(fd) < 0) {
        return _led_error(led, LED_ERROR_CLOSE, errno, "Closing LED maxbrightness");
    }

    buf[ret] = '\0';
    *max_brightness = strtoul(buf, NULL, 10);
    return 0;
}

int led_get_brightness(led_t* led, unsigned int* brightness)
{
    char led_path[PATH_MAX];
    int fd, ret;
    char buf[16];

    snprintf(led_path, sizeof(led_path), "/sys/class/leds/%s/brightness", led->name);

    if ((fd = open(led_path, O_RDWR)) < 0) {
        return _led_error(led, LED_ERROR_OPEN, errno, "Openning LED brightness");
    }

    if ((ret = read(fd, buf, sizeof(buf))) < 0) {
        int errsv = errno;
        close(fd);
        return _led_error(led, LED_ERROR_IO, errno, "Reading LED brightness");
    }

    if (close(fd) < 0) {
        return _led_error(led, LED_ERROR_CLOSE, errno, "Closing LED brightness");
    }

    buf[ret] = '\0';
    *brightness = strtoul(buf, NULL, 10);
    return 0;
}

int led_set_brightness(led_t* led, unsigned int brightness)
{
    char led_path[PATH_MAX];
    char buf[16];
    int fd, len, ret;

    snprintf(led_path, sizeof(led_path), "/sys/class/leds/%s/brightness", led->name);

    if ((fd = open(led_path, O_RDWR)) < 0) {
        return _led_error(led, LED_ERROR_OPEN, errno, "Openning LED brightness");
    }

    snprintf(buf, sizeof(buf), "%u", brightness);


    if ((ret = write(fd, buf, sizeof(buf))) < 0) {
        int errsv = errno;
        close(fd);
        return _led_error(led, LED_ERROR_IO, errno, "Writing LED brightness");
    }

    if (close(fd) < 0) {
        return _led_error(led, LED_ERROR_CLOSE, errno, "Closing LED brightness");
    }

    return 0;
}

led_t* led_new(void)
{
    led_t* led = calloc(1, sizeof(led_t));
    if (led == NULL)
        return NULL;
    
    return led;
}

void led_free(led_t *led)
{
    free(led);
}

int led_open(led_t* led, const char* name)
{
    char led_path[PATH_MAX];
    int fd, ret;

    snprintf(led_path, sizeof(led_path), "/sys/class/leds/%s/brightness", name);

    if ((fd = open(led_path, O_RDWR)) < 0)
        return _led_error(led, LED_ERROR_OPEN, errno, "Opening LED brightness");

    close(fd);
    strncpy(led->name, name, sizeof(led->name)-1);
    led->name[sizeof(led->name) - 1] = '\0';

    if((ret = led_get_max_brightness(led, &led->max_brightness)) < 0) {
        return ret;
    }

    return 0;
}

int led_read(led_t* led, unsigned int* value)
{
    int ret;
    unsigned int brightness;

    if((ret = led_get_brightness(led, &brightness)) < 0)
        return ret;
    
    *value = brightness;
    return 0;
}

int led_write(led_t* led, unsigned int value)
{
    return led_set_brightness(led, value);
}

void led_dump(void)
{
    DIR *dir;
    struct dirent *d;
    size_t len;

    dir = opendir("/sys/class/leds/");
    if (!dir)
        return;

    while((d = readdir(dir))) {
        if (d->d_name[0] == '.')
            continue;
        printf("%s\n", d->d_name);
    }
    closedir(dir);
}

int led_close(led_t* led)
{
    (void)led;
    return 0;
}