/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "headphone_amp.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(headphone_amp, CONFIG_MODULE_HEADPHONE_AMP_LOG_LEVEL);

/* Get the amp_enable GPIO from the device tree (gpio-leds binding) if available */
#ifdef CONFIG_BOARD_MUSIC_PLAYER_NRF5340_CPUAPP
static const struct gpio_dt_spec amp_enable_gpio = GPIO_DT_SPEC_GET(DT_NODELABEL(amp_enable), gpios);
#else
static struct gpio_dt_spec amp_enable_gpio = {0};
#endif

int headphone_amp_init(void)
{
	int ret;

	if (amp_enable_gpio.port == NULL) {
		LOG_WRN("Headphone amp GPIO node not found in device tree; skipping init");
		return 0;
	}

	if (!gpio_is_ready_dt(&amp_enable_gpio)) {
		LOG_ERR("Headphone amp GPIO device not ready");
		return -ENODEV;
	}

	/* Configure as output, initially disabled (LOW) */
	ret = gpio_pin_configure_dt(&amp_enable_gpio, GPIO_OUTPUT_INACTIVE);
	if (ret) {
		LOG_ERR("Failed to configure amp enable pin: %d", ret);
		return ret;
	}

	LOG_INF("Headphone amplifier initialized (P1.15)");
	return 0;
}

int headphone_amp_enable(bool enable)
{
	int ret;

	/* If GPIO was not initialized, silently return success (not an error on unsupported boards) */
	if (amp_enable_gpio.port == NULL) {
		LOG_DBG("Headphone amp GPIO not available; skipping enable/disable");
		return 0;
	}

	if (!gpio_is_ready_dt(&amp_enable_gpio)) {
		LOG_WRN("Headphone amp GPIO device not ready");
		return -ENODEV;
	}

	ret = gpio_pin_set_dt(&amp_enable_gpio, enable ? 1 : 0);
	if (ret) {
		LOG_ERR("Failed to set amp enable pin: %d", ret);
		return ret;
	}

	LOG_DBG("Headphone amplifier %s", enable ? "enabled" : "disabled");
	return 0;
}
