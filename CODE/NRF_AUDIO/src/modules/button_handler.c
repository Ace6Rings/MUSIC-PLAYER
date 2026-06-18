/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "button_handler.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/util.h>
#include <zephyr/shell/shell.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/toolchain/common.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "button_assignments.h"
#include "macros_common.h"
#include "zbus_common.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(button_handler, CONFIG_MODULE_BUTTON_HANDLER_LOG_LEVEL);

/* Define ZBUS Channel safely now that headers are loaded sequentially */
ZBUS_CHAN_DEFINE(button_chan, struct button_msg, NULL, NULL, ZBUS_OBSERVERS_EMPTY,
        ZBUS_MSG_INIT(0));
#define BASE_10 10

/* Only allow one button msg at a time, as a mean of debounce */
K_MSGQ_DEFINE(button_q, sizeof(struct button_msg), 1, sizeof(void *));

static bool debounce_is_ongoing;

/* Direct device pointer lookup to your ADC hardware peripheral */
static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));

/* * FIX: Manual configuration block. This bypasses the broken macro engine entirely
 * while mapping precisely to internal hardware SAADC Channel 7 / AIN7 (P0.28).
 */
static const struct adc_channel_cfg m_1st_channel_cfg = {
    .gain             = ADC_GAIN_1_4,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = 7,
    .input_positive   = 8, /* SAADC_INPUT_AIN7 = P0.28 */
};

/* Map our ADC readings back into virtual ID values for the system shell */
struct analog_btn_map {
    const char *name;
    uint16_t target_voltage;
    uint32_t virtual_pin;
};

/* FIX: virtual_pin uses button assignment macro values so zbus subscribers
 * can identify buttons by the same constants defined in button_assignments.h */
static struct analog_btn_map btn_config_data[] = {
    {"BUTTON_VOLUME_DOWN", 820,  BUTTON_VOLUME_DOWN},
    {"BUTTON_VOLUME_UP",   1640, BUTTON_VOLUME_UP},
    {"BUTTON_PLAY_PAUSE",  2457, BUTTON_PLAY_PAUSE},
    {"BUTTON_4",           3277, BUTTON_4},
    {"BUTTON_5",           4000, BUTTON_5}
};

#define NUM_BUTTONS ARRAY_SIZE(btn_config_data)

static void on_button_debounce_timeout(struct k_timer *timer)
{
    debounce_is_ongoing = false;
}
K_TIMER_DEFINE(button_debounce_timer, on_button_debounce_timeout, NULL);

/* Process matches and push to the application's Zbus message queue */
static void process_analog_press(uint32_t virtual_pin, const char *name)
{
    if (debounce_is_ongoing) {
        return;
    }

    struct button_msg msg;
    int ret;

    LOG_DBG("Pushed analog button name: %s", name);

    msg.button_pin = virtual_pin;
    msg.button_action = BUTTON_PRESS;

    ret = k_msgq_put(&button_q, (void *)&msg, K_NO_WAIT);
    if (ret == -EAGAIN) {
        LOG_WRN("Btn msg queue full");
    }

    debounce_is_ongoing = true;
    k_timer_start(&button_debounce_timer, K_MSEC(CONFIG_BUTTON_DEBOUNCE_MS), K_NO_WAIT);
}

/* Worker thread that continuously reads our ADC hardware ladder */
static void analog_button_scan_thread(void)
{
    int16_t sample_buffer[1];
    struct adc_sequence sequence = {
        .channels    = BIT(7),
        .buffer      = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution  = 12,
    };

    while (1) {
        int err = adc_read(adc_dev, &sequence);
        if (err == 0) {
            int32_t reading = sample_buffer[0];

            /* Ignore resting voltage window so floating states don't loop active button events */
            if (reading < 200) { 
                k_msleep(50);
                continue;
            }

            for (size_t i = 0; i < NUM_BUTTONS; i++) {
                if (reading >= (btn_config_data[i].target_voltage - ADC_TOLERANCE) &&
                    reading <= (btn_config_data[i].target_voltage + ADC_TOLERANCE)) {
                    
                    process_analog_press(btn_config_data[i].virtual_pin, btn_config_data[i].name);
                    break;
                }
            }
        }
        k_msleep(50); // Scan window sleep
    }
}
K_THREAD_DEFINE(analog_scan_tid, 1024, analog_button_scan_thread, NULL, NULL, NULL, 5, 0, 0);

static void button_publish_thread(void)
{
    int ret;
    struct button_msg msg;

    while (1) {
        k_msgq_get(&button_q, &msg, K_FOREVER);

        ret = zbus_chan_pub(&button_chan, &msg, K_NO_WAIT);
        if (ret) {
            LOG_ERR("Failed to publish button msg, ret: %d", ret);
        }
    }
}
K_THREAD_DEFINE(Msg_pub_btn, CONFIG_BUTTON_PUBLISH_STACK_SIZE, button_publish_thread, NULL, NULL,
        NULL, K_PRIO_PREEMPT(CONFIG_BUTTON_PUBLISH_THREAD_PRIO), 0, 0);

/* Stubs kept to prevent breaking dependencies in other files */
int button_pressed(uint32_t button_pin, bool *is_pressed)
{
    ARG_UNUSED(button_pin);
    if (is_pressed) {
        *is_pressed = false;
    }
    return 0;
}

int button_handler_init(void)
{
    int ret;

    if (!device_is_ready(adc_dev)) {
        LOG_ERR("ADC device not ready for button handling");
        return -ENODEV;
    }

    ret = adc_channel_setup(adc_dev, &m_1st_channel_cfg);
    if (ret < 0) {
        LOG_ERR("ADC channel setup failed (%d)", ret);
        return ret;
    }

    LOG_INF("Analog Button Handler initialized successfully.");
    return 0;
}

/* Shell functions updated to read our static map data */
static int cmd_print_all_btns(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        shell_print(shell, "Id %d: Target ADC: %d -> %s", i, btn_config_data[i].target_voltage,
                btn_config_data[i].name);
    }

    return 0;
}

static int cmd_push_btn(const struct shell *shell, size_t argc, char **argv)
{
    int ret;
    uint8_t btn_idx;
    struct button_msg msg;

    if (argc != 2) {
        shell_error(shell, "Wrong number of arguments provided");
        return -EINVAL;
    }

    if (!isdigit((int)argv[1][0])) {
        shell_error(shell, "Supplied argument is not numeric");
        return -EINVAL;
    }

    btn_idx = strtoul(argv[1], NULL, BASE_10);

    if (btn_idx >= NUM_BUTTONS) {
        shell_error(shell, "Selected button ID out of range");
        return -EINVAL;
    }

    msg.button_pin = btn_config_data[btn_idx].virtual_pin;
    msg.button_action = BUTTON_PRESS;

    ret = zbus_chan_pub(&button_chan, &msg, K_NO_WAIT);
    if (ret) {
        LOG_ERR("Failed to publish button msg, ret: %d", ret);
    }

    shell_print(shell, "Simulated pushing button idx: %d : %s", btn_idx, btn_config_data[btn_idx].name);

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(buttons_cmd,
                   SHELL_COND_CMD(CONFIG_SHELL, print, NULL, "Print all buttons.",
                          cmd_print_all_btns),
                   SHELL_COND_CMD(CONFIG_SHELL, push, NULL, "Push button.",
                          cmd_push_btn),
                   SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(buttons, &buttons_cmd, "List and push buttons", NULL);