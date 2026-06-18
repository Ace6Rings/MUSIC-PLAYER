/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief MP3 decoder context for maintaining state across frames
 */
struct mp3_decoder {
	/** PCM output buffer (stereo, 16-bit samples) */
	int16_t *pcm_buffer;
	/** PCM buffer size in samples */
	uint16_t pcm_buffer_size;
	/** Sample rate of current frame (Hz) */
	uint32_t sample_rate;
	/** Number of channels (1=mono, 2=stereo) */
	uint8_t channels;
};

/**
 * @brief Initialize MP3 decoder context
 *
 * @param decoder Decoder context to initialize
 * @param pcm_buffer Output PCM buffer
 * @param pcm_buffer_size Size of PCM buffer in samples
 * @return 0 on success
 */
int mp3_decoder_init(struct mp3_decoder *decoder, int16_t *pcm_buffer, uint16_t pcm_buffer_size);

/**
 * @brief Decode one MP3 frame from compressed data
 *
 * @param decoder Decoder context
 * @param mp3_data Compressed MP3 frame data
 * @param mp3_size Size of compressed frame
 * @param samples_out Output: number of PCM samples written to pcm_buffer
 * @return Number of bytes consumed from mp3_data, <0 on error
 */
int mp3_decoder_decode_frame(struct mp3_decoder *decoder, const uint8_t *mp3_data,
			      size_t mp3_size, uint32_t *samples_out);

#endif /* MP3_DECODER_H */
