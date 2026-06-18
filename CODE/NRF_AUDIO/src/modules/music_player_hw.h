/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef MUSIC_PLAYER_HW_H
#define MUSIC_PLAYER_HW_H

/**
 * @brief Initialize all music player hardware peripherals
 *
 * Initializes:
 * - Display (backlight on, blanking off)
 * - Headphone amplifier
 * - Battery monitor
 *
 * @return 0 on success, negative errno on failure
 */
int music_player_hw_init(void);

#endif /* MUSIC_PLAYER_HW_H */
