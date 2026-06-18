/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef MUSIC_UI_H
#define MUSIC_UI_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize music player UI (LVGL-based display and input handling)
 *
 * Initializes:
 * - LVGL display driver
 * - Display screens (now playing, file browser)
 * - Input handlers (encoder, button pad)
 *
 * @return 0 on success, negative errno on failure
 */
int music_ui_init(void);

/**
 * @brief Update UI with now-playing track info
 *
 * @param title Track title (may be truncated for display)
 * @param artist Artist name (may be truncated for display)
 * @param position_ms Current playback position in milliseconds
 * @param duration_ms Total track duration in milliseconds
 * @param is_playing true if currently playing, false if paused
 */
void music_ui_update_now_playing(const char *title, const char *artist,
				  uint32_t position_ms, uint32_t duration_ms,
				  bool is_playing);

/**
 * @brief Update UI battery indicator
 *
 * @param percent Battery charge percentage (0-100)
 */
void music_ui_update_battery(uint8_t percent);

/**
 * @brief Update UI volume indicator
 *
 * @param volume Volume level (0-100)
 */
void music_ui_update_volume(uint8_t volume);

#endif /* MUSIC_UI_H */
