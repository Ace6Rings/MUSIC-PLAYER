/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef HEADPHONE_AMP_H
#define HEADPHONE_AMP_H

#include <stdbool.h>

/**
 * @brief Initialize headphone amplifier (TPA6132A2) on P1.15
 *
 * @return 0 on success, negative errno on failure
 */
int headphone_amp_init(void);

/**
 * @brief Enable or disable headphone amplifier
 *
 * @param enable true to enable (P1.15 HIGH), false to disable (P1.15 LOW)
 * @return 0 on success, negative errno on failure
 */
int headphone_amp_enable(bool enable);

#endif /* HEADPHONE_AMP_H */
