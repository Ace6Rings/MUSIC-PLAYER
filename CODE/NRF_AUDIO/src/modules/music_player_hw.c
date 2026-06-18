/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "music_player_hw.h"
#include "headphone_amp.h"
#include "battery_monitor.h"
#include "music_ui.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(music_player_hw, CONFIG_MODULE_MUSIC_PLAYER_HW_LOG_LEVEL);

/* Display device from zephyr,display chosen node */
#ifdef CONFIG_BOARD_MUSIC_PLAYER_NRF5340_CPUAPP
static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
#else
static const struct device *display_dev = NULL;
#endif

/* Backlight GPIO from the device tree (gpio-leds binding) */
#ifdef CONFIG_BOARD_MUSIC_PLAYER_NRF5340_CPUAPP
static const struct gpio_dt_spec backlight_gpio = GPIO_DT_SPEC_GET(DT_NODELABEL(backlight), gpios);
#else
static struct gpio_dt_spec backlight_gpio = {0};
#endif

int music_player_hw_init(void)
{
	int ret;

	/* 1. Initialize display and enable backlight (only if available) */
	if (display_dev != NULL) {
		LOG_INF("Display device found, turning on backlight");

		if (backlight_gpio.port != NULL && gpio_is_ready_dt(&backlight_gpio)) {
			ret = gpio_pin_configure_dt(&backlight_gpio, GPIO_OUTPUT_ACTIVE);
			if (ret) {
				LOG_WRN("Failed to configure backlight: %d", ret);
			} else {
				LOG_DBG("Backlight enabled (P1.13)");
			}
		}

		/* Turn off display blanking */
		if (display_dev && device_is_ready(display_dev)) {
			ret = display_blanking_off(display_dev);
			if (ret) {
				LOG_WRN("display_blanking_off failed: %d", ret);
			} else {
				LOG_DBG("Display blanking disabled");
			}
		}
	} else {
		LOG_DBG("No display device configured; skipping backlight init");
	}

	/* 2. Initialize headphone amplifier */
	ret = headphone_amp_init();
	if (ret) {
		LOG_ERR("Failed to initialize headphone amplifier");
		return ret;
	}

	ret = headphone_amp_enable(true);
	if (ret) {
		LOG_WRN("Failed to enable headphone amplifier: %d", ret);
	}

	/* 3. Initialize battery monitor */
	ret = battery_monitor_init();
	if (ret) {
		LOG_ERR("Failed to initialize battery monitor");
		return ret;
	}

	/* 4. Initialize the music UI layer */
	ret = music_ui_init();
	if (ret) {
		LOG_ERR("Failed to initialize music UI");
		return ret;
	}

	LOG_INF("Music player hardware initialized successfully");
	return 0;
}
