/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <stdint.h>

/**
 * @brief Battery status event published over zbus
 */
struct battery_msg {
	/** Battery voltage in millivolts */
	uint16_t voltage_mv;
	/** Battery charge percentage (0-100) */
	uint8_t charge_percent;
};

/**
 * @brief Initialize battery monitor (ADC on AIN5/P0.26)
 *
 * @return 0 on success, negative errno on failure
 */
int battery_monitor_init(void);

/**
 * @brief Get current battery voltage
 *
 * @return Battery voltage in millivolts
 */
uint16_t battery_get_voltage_mv(void);

/**
 * @brief Get current battery charge percentage
 *
 * @return Charge percentage (0-100)
 */
uint8_t battery_get_charge_percent(void);

#endif /* BATTERY_MONITOR_H */
