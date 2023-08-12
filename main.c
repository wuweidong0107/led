#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#include "led.h"

static struct option opts[] = {
	{ "path", 1, NULL, 'p' },
	{ "write", 1, NULL, 'w' },
	{ "help", 0, NULL, 'h' },
	{ "list", 0, NULL, 'l' },
	{ "version", 0, NULL, 'v' },
	{ 0, 0, NULL, 0 }
};

void usage(void)
{
	printf("Usage: led [OPTION...]\n");
	printf("  -l, list              list available led device\n");
	printf("  -p, path              led sysfs path\n");
	printf("  -w, --write           write to led\n");
	printf("  -h, --help            show this help message\n");
	exit(0);
}

int main(int argc, char** argv)
{
    int c, option_index = 0;
    char *led_path = NULL;
    int val, w_val = -1;

    if (geteuid() != 0) {
		printf("Need to run as root\n");
		exit(1);
	}

    while((c = getopt_long(argc, argv, "lp:w:hv", opts, &option_index)) != -1) {
    	switch(c) {
    	case 'l':
    		led_dump();
    		exit(0);
    		break;
    	case 'p':
    		led_path = optarg;
    		break;
    	case 'w':
    		w_val = strtoul(optarg, NULL, 10);
    		break;
    	case 'h':
    		usage();
    		break;
    	}
    }

    if (led_path == NULL) {
        usage();
    }

    led_t* led = led_new();
    if (led == NULL) {
        printf("led_new() fail\n");
        return -1;
    }

    if (led_open(led, led_path) != 0) {
        printf("led_open() fail\n");
        return -1;
    }

    if (w_val != -1) {
        if (led_write(led, !!w_val) != 0) {
            printf("led_write() fail\n");
            return -1;
        }
    }

    if (led_read(led, &val) != 0) {
        printf("led_read() fail\n");
        return -1;
    }
    printf("%d\n", val);
    led_free(led);
    return 0;
}