/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 *
 * MP3 decoder wrapper using minimp3 library (CC0 / public domain)
 * Minimp3 source: https://github.com/lieff/minimp3
 */

#include "mp3_decoder.h"
#include <string.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mp3_decoder, CONFIG_MODULE_MP3_DECODER_LOG_LEVEL);

/* Placeholder minimp3 implementation stub for build.
 * In production: download minimp3.h and minimp3_ex.h from lieff/minimp3 repo
 * and define MINIMP3_IMPLEMENTATION in this .c file to enable full decoder.
 */

int mp3_decoder_init(struct mp3_decoder *decoder, int16_t *pcm_buffer, uint16_t pcm_buffer_size)
{
	if (!decoder || !pcm_buffer || pcm_buffer_size == 0) {
		return -EINVAL;
	}

	decoder->pcm_buffer = pcm_buffer;
	decoder->pcm_buffer_size = pcm_buffer_size;
	decoder->sample_rate = 0;
	decoder->channels = 0;

	LOG_INF("MP3 decoder initialized (buffer: %u samples)", pcm_buffer_size);
	return 0;
}

int mp3_decoder_decode_frame(struct mp3_decoder *decoder, const uint8_t *mp3_data,
			      size_t mp3_size, uint32_t *samples_out)
{
	if (!decoder || !mp3_data || mp3_size == 0 || !samples_out) {
		return -EINVAL;
	}

	/* Placeholder: in production, call minimp3's mp3dec_decode_frame() here.
	 * For now, return 0 samples and consume frame data.
	 */
	*samples_out = 0;
	LOG_DBG("MP3 decode stub: consumed %u bytes, output 0 samples", (unsigned int)mp3_size);

	return 0;  /* bytes consumed */
}
