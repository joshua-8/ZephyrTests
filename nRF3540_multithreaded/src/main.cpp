/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define BUTTON0_NODE DT_ALIAS(sw0)
#define BUTTON1_NODE DT_ALIAS(sw1)
#define BUTTON2_NODE DT_ALIAS(sw2)
#define BUTTON3_NODE DT_ALIAS(sw3)

static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led4 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios);
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(BUTTON2_NODE, gpios);
static const struct gpio_dt_spec button4 = GPIO_DT_SPEC_GET(BUTTON3_NODE, gpios);

uint32_t millis() // ms
{
    return (uint64_t)k_cycle_get_32() * 1000 / CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
}

struct button_time_msg_type {
    uint32_t timeField;
};

K_MSGQ_DEFINE(led3_msgq, sizeof(struct button_time_msg_type), 10, 1);
K_MSGQ_DEFINE(led4_msgq, sizeof(struct button_time_msg_type), 10, 1);

volatile uint32_t button4DownTime = 0;
volatile bool button4LastState = false;
static struct gpio_callback button4_cb;
void button4Callback(const struct device* dev, struct gpio_callback* cb,
    uint32_t pins)
{
    if (gpio_pin_get_dt(&button4) == 1) {
        button4DownTime = millis();
    } else {
        struct button_time_msg_type data;
        data.timeField = millis() - button4DownTime;
        if (data.timeField > 0) {
            k_msgq_put(&led4_msgq, &data, K_NO_WAIT);
        }
    }
}

int main(void)
{
    if (!gpio_is_ready_dt(&led1) || !gpio_is_ready_dt(&led2) || !gpio_is_ready_dt(&led3) || !gpio_is_ready_dt(&led4)) {
        printf("Error: LED device is not ready.\n");
        return -1;
    }

    if (gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE) < 0 || gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE) < 0 || gpio_pin_configure_dt(&led3, GPIO_OUTPUT_ACTIVE) < 0 || gpio_pin_configure_dt(&led4, GPIO_OUTPUT_ACTIVE) < 0) {
        printf("Error: LED device didn't configure.\n");
        return -1;
    }

    // Set up the buttons
    if (!gpio_is_ready_dt(&button1) || !gpio_is_ready_dt(&button2) || !gpio_is_ready_dt(&button3) || !gpio_is_ready_dt(&button4)) {
        printf("Error: Button device is not ready.\n");
        return -1;
    }

    if (gpio_pin_configure_dt(&button1, GPIO_INPUT) < 0 || gpio_pin_configure_dt(&button2, GPIO_INPUT) < 0 || gpio_pin_configure_dt(&button3, GPIO_INPUT) < 0 || gpio_pin_configure_dt(&button4, GPIO_INPUT) < 0) {
        printf("Error: Button device didn't configure.\n");
        return -1;
    }

    gpio_pin_interrupt_configure_dt(&button4, GPIO_INT_EDGE_BOTH);

    // init callback and add callback
    gpio_init_callback(&button4_cb, button4Callback, BIT(button4.pin));
    gpio_add_callback(button4.port, &button4_cb);

    gpio_pin_set_dt(&led3, 0);
    gpio_pin_set_dt(&led4, 0);

    while (1) {
        gpio_pin_set_dt(&led1, gpio_pin_get_dt(&button1));
        gpio_pin_set_dt(&led2, gpio_pin_get_dt(&button2));

        k_sleep(K_MSEC(10)); // give other threads a chance to run
    }
    return 0;
}

void led3main(void)
{
    while (true) {
        struct button_time_msg_type data;
        if (k_msgq_get(&led3_msgq, &data, K_FOREVER) == 0) {
            gpio_pin_set_dt(&led3, 1);
            k_sleep(K_MSEC(data.timeField));
            gpio_pin_set_dt(&led3, 0);
            k_sleep(K_MSEC(250));
        }
    }
}

void led4main(void)
{
    while (true) {
        struct button_time_msg_type data;
        if (k_msgq_get(&led4_msgq, &data, K_FOREVER) == 0) {
            gpio_pin_set_dt(&led4, 1);
            k_sleep(K_MSEC(data.timeField));
            gpio_pin_set_dt(&led4, 0);
            k_sleep(K_MSEC(250));
        }
    }
}

void button3main(void)
{
    uint32_t buttonDownTime = 0;
    uint32_t buttonUpTime = 0;
    struct button_time_msg_type data;
    while (true) {
        // bad, blocking code for polling the button, but it won't block the whole program since it's in a thread
        if (gpio_pin_get_dt(&button3) == 1) {
            buttonDownTime = millis();
            while (gpio_pin_get_dt(&button3) == 1) {
                k_sleep(K_MSEC(1));
            }
            buttonUpTime = millis();
            data.timeField = buttonUpTime - buttonDownTime;
            k_msgq_put(&led3_msgq, &data, K_NO_WAIT);
        }
        k_sleep(K_MSEC(1));
    }
}

#define STACKSIZE 1024
#define PRIORITY 7
K_THREAD_DEFINE(button3_id, STACKSIZE, button3main, NULL, NULL, NULL,
    PRIORITY, 0, 0);

K_THREAD_DEFINE(led3_id, STACKSIZE, led3main, NULL, NULL, NULL,
    PRIORITY, 0, 0);

K_THREAD_DEFINE(led4_id, STACKSIZE, led4main, NULL, NULL, NULL,
    PRIORITY, 0, 0);
