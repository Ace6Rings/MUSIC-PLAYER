/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "battery_monitor.h"

#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(battery_monitor, CONFIG_MODULE_BATTERY_MONITOR_LOG_LEVEL);

/* Zbus channel for battery events */
ZBUS_CHAN_DEFINE(battery_chan, struct battery_msg, NULL, NULL, ZBUS_OBSERVERS_EMPTY,
		ZBUS_MSG_INIT(0));

/* ADC device and channel config for AIN5 (P0.26) — battery voltage sense */
static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));

static struct adc_channel_cfg battery_channel_cfg = {
	.gain             = ADC_GAIN_1_6,          /* divide-by-6 to accommodate ~4.2V */
	.reference        = ADC_REF_INTERNAL,
	.acquisition_time = ADC_ACQ_TIME_DEFAULT,
	.channel_id       = 5,
	.input_positive   = 6,                     /* NRF_SAADC_INPUT_AIN5 */
};

static uint16_t battery_voltage_mv;
static uint8_t battery_charge_percent;

/* Periodically poll battery voltage and publish over zbus */
static void battery_monitor_thread(void)
{
	int16_t adc_reading;
	struct adc_sequence seq = {
		.channels    = BIT(5),
		.buffer      = &adc_reading,
		.buffer_size = sizeof(adc_reading),
		.resolution  = 12,
	};

	while (1) {
		int ret = adc_read(adc_dev, &seq);
		if (ret == 0) {
			/* ADC is 12-bit (0-4095), Vref = 0.6V, gain 1/6
			 * Raw reading × (0.6V / 4095) gives ADC input voltage.
			 * The SAADC gain of 1/6 means the sensed voltage is 6× the ADC input.
			 * If the board uses a 2:1 resistor divider on battery sense, multiply by 2 again.
			 * Total conversion: raw × (0.6 / 4095) × 6 × 2 = raw × 7200 / 4095.
			 */
			battery_voltage_mv = (adc_reading * 7200UL) / 4095;

			/* Simple Li-Po discharge curve: 4.2V=100%, 3.7V=50%, 3.3V=0% */
			if (battery_voltage_mv >= 4200) {
				battery_charge_percent = 100;
			} else if (battery_voltage_mv <= 3300) {
				battery_charge_percent = 0;
			} else {
				/* Linear interpolation between 3.3V (0%) and 4.2V (100%) */
				battery_charge_percent =
					((battery_voltage_mv - 3300) * 100) / (4200 - 3300);
			}

			/* Publish battery event */
			struct battery_msg msg = {
				.voltage_mv = battery_voltage_mv,
				.charge_percent = battery_charge_percent,
			};
			zbus_chan_pub(&battery_chan, &msg, K_NO_WAIT);

			LOG_DBG("Battery: %u mV (%u%%)", battery_voltage_mv, battery_charge_percent);
		} else {
			LOG_WRN("ADC read failed: %d", ret);
		}

		/* Poll every 10 seconds */
		k_sleep(K_SECONDS(10));
	}
}

K_THREAD_DEFINE(battery_monitor_tid, 1024, battery_monitor_thread, NULL, NULL, NULL,
		CONFIG_BATTERY_MONITOR_THREAD_PRIO, 0, 0);

int battery_monitor_init(void)
{
	int ret;

	if (!device_is_ready(adc_dev)) {
		LOG_WRN("ADC device not ready; battery monitoring disabled");
		return 0;
	}

	ret = adc_channel_setup(adc_dev, &battery_channel_cfg);
	if (ret < 0) {
		LOG_ERR("ADC channel setup failed: %d", ret);
		return ret;
	}

	LOG_INF("Battery monitor initialized (P0.26/AIN5)");
	return 0;
}

uint16_t battery_get_voltage_mv(void)
{
	return battery_voltage_mv;
}

uint8_t battery_get_charge_percent(void)
{
	return battery_charge_percent;
}
