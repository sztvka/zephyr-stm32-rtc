/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/counter.h>
#include <string.h>
#include <zephyr/posix/time.h>
#define TIMER DT_INST(0, st_stm32_rtc)

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct device *const counter_dev = DEVICE_DT_GET(TIMER);
time_t compile_timestamp = 1678984720;
struct timespec time_now_ts;

void main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}



    if (!device_is_ready(counter_dev)) {
        printk("device not ready.\n");
        return;
    };


    //if(counter_start(counter_dev)!=0) {
   //     printk("counter start failed.\n");
   //     return;
  //  };

    uint32_t *ticks = k_malloc(sizeof(uint32_t));
    memset(ticks, 0, sizeof(uint32_t));

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}

        int req = counter_get_value(counter_dev, ticks);

        if(req) {
            printk("counter get value failed.\n\r");
            return;
        }
        else{
            struct tm   ttm;

            time_t time_now = compile_timestamp + *ticks + 3600;
            ttm = * gmtime(&time_now);

            printk("counter value: %d\n\r", *ticks);
            printk("Current time: %02d:%02d:%02d\n\r", ttm.tm_hour, ttm.tm_min, ttm.tm_sec);
        }
		k_msleep(SLEEP_TIME_MS);
	}

}
