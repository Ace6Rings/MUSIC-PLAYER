/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "music_ui.h"

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(music_ui, CONFIG_MODULE_MUSIC_UI_LOG_LEVEL);

static void music_ui_render(void);

/* UI state */
struct ui_state_t {
	char title[128];
	char artist[128];
	uint32_t position_ms;
	uint32_t duration_ms;
	bool is_playing;
	uint8_t volume;
	uint8_t battery_percent;
	bool initialized;
} ui_state = {
	.title = "Ready",
	.artist = "nRF5340 Music Player",
	.position_ms = 0,
	.duration_ms = 0,
	.is_playing = false,
	.volume = 50,
	.battery_percent = 100,
	.initialized = false,
};

int music_ui_init(void)
{
	if (ui_state.initialized) {
		return 0;
	}

	LOG_INF("========================================");
	LOG_INF("  nRF5340 Music Player UI Initialized  ");
	LOG_INF("========================================");
	LOG_INF("Display: 240x320 portrait");
	LOG_INF("Ready for playback");

	ui_state.initialized = true;

	/* Initial UI output */
	music_ui_render();

	return 0;
}

void music_ui_update_now_playing(const char *title, const char *artist,
				  uint32_t position_ms, uint32_t duration_ms,
				  bool is_playing)
{
	if (!ui_state.initialized) {
		return;
	}

	if (title) {
		strncpy(ui_state.title, title, sizeof(ui_state.title) - 1);
		ui_state.title[sizeof(ui_state.title) - 1] = '\0';
	}

	if (artist) {
		strncpy(ui_state.artist, artist, sizeof(ui_state.artist) - 1);
		ui_state.artist[sizeof(ui_state.artist) - 1] = '\0';
	}

	ui_state.position_ms = position_ms;
	ui_state.duration_ms = duration_ms;
	ui_state.is_playing = is_playing;

	music_ui_render();
}

void music_ui_update_battery(uint8_t percent)
{
	if (!ui_state.initialized) {
		return;
	}

	ui_state.battery_percent = (percent > 100) ? 100 : percent;
	music_ui_render();
}

void music_ui_update_volume(uint8_t level)
{
	if (!ui_state.initialized) {
		return;
	}

	ui_state.volume = level;
	music_ui_render();
}

/**
 * Render current UI state
 * Outputs display state to logs showing player interface
 */
void music_ui_render(void)
{
	/* Format minutes:seconds from milliseconds */
	uint32_t pos_sec = ui_state.position_ms / 1000;
	uint32_t dur_sec = ui_state.duration_ms / 1000;
	uint32_t pos_min = pos_sec / 60;
	uint32_t dur_min = dur_sec / 60;

	/* Render UI state to logs (display simulation) */
	LOG_INF("╔════════════════════════════════════════╗");
	LOG_INF("║  %s  ║", ui_state.is_playing ? "▶ PLAYING " : "⏸ PAUSED  ");
	LOG_INF("╟────────────────────────────────────────╢");
	LOG_INF("║ %s", ui_state.title);
	LOG_INF("║ %s", ui_state.artist);
	LOG_INF("╟────────────────────────────────────────╢");
	LOG_INF("║ [%02u:%02u / %02u:%02u]",
		pos_min, pos_sec % 60, dur_min, dur_sec % 60);

	/* Draw progress bar */
	if (ui_state.duration_ms > 0) {
		uint8_t progress = (ui_state.position_ms * 16) / ui_state.duration_ms;
		char bar[20] = {0};
		for (uint8_t i = 0; i < 16; i++) {
			bar[i] = i < progress ? '=' : '-';
		}
		LOG_INF("║ [%s]", bar);
	} else {
		LOG_INF("║ [────────────────]");
	}

	LOG_INF("╟────────────────────────────────────────╢");
	LOG_INF("║ Volume: %3u%%   Battery: %3u%%        ║",
		ui_state.volume, ui_state.battery_percent);
	LOG_INF("╚════════════════════════════════════════╝");
}

