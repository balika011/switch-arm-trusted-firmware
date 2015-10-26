/*
 * Copyright (c) 2014-2015, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <common_def.h>
#include <debug.h>
#include <io_driver.h>
#include <io_storage.h>
#include <io_semihosting.h>
#include <plat_arm.h>
#include <semihosting.h>	/* For FOPEN_MODE_... */

/* Semihosting filenames */
#define BL2_IMAGE_NAME			"bl2.bin"
#define BL31_IMAGE_NAME			"bl31.bin"
#define BL32_IMAGE_NAME			"bl32.bin"
#define BL33_IMAGE_NAME			"bl33.bin"

#if TRUSTED_BOARD_BOOT
#define BL2_CERT_NAME			"bl2.crt"
#define TRUSTED_KEY_CERT_NAME		"trusted_key.crt"
#define BL31_KEY_CERT_NAME		"bl31_key.crt"
#define BL32_KEY_CERT_NAME		"bl32_key.crt"
#define BL33_KEY_CERT_NAME		"bl33_key.crt"
#define BL31_CERT_NAME			"bl31.crt"
#define BL32_CERT_NAME			"bl32.crt"
#define BL33_CERT_NAME			"bl33.crt"
#endif /* TRUSTED_BOARD_BOOT */

/* IO devices */
static const io_dev_connector_t *sh_dev_con;
static uintptr_t sh_dev_handle;

static const io_file_spec_t sh_file_spec[] = {
	[BL2_IMAGE_ID] = {
		.path = BL2_IMAGE_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL31_IMAGE_ID] = {
		.path = BL31_IMAGE_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL32_IMAGE_ID] = {
		.path = BL32_IMAGE_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL33_IMAGE_ID] = {
		.path = BL33_IMAGE_NAME,
		.mode = FOPEN_MODE_RB
	},
#if TRUSTED_BOARD_BOOT
	[BL2_CERT_ID] = {
		.path = BL2_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
	[TRUSTED_KEY_CERT_ID] = {
		.path = TRUSTED_KEY_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL31_KEY_CERT_ID] = {
		.path = BL31_KEY_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL32_KEY_CERT_ID] = {
		.path = BL32_KEY_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL33_KEY_CERT_ID] = {
		.path = BL33_KEY_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL31_CERT_ID] = {
		.path = BL31_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL32_CERT_ID] = {
		.path = BL32_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
	[BL33_CERT_ID] = {
		.path = BL33_CERT_NAME,
		.mode = FOPEN_MODE_RB
	},
#endif /* TRUSTED_BOARD_BOOT */
};


static int open_semihosting(const uintptr_t spec)
{
	int result = IO_FAIL;
	uintptr_t local_image_handle;

	/* See if the file exists on semi-hosting.*/
	result = io_dev_init(sh_dev_handle, (uintptr_t)NULL);
	if (result == IO_SUCCESS) {
		result = io_open(sh_dev_handle, spec, &local_image_handle);
		if (result == IO_SUCCESS) {
			VERBOSE("Using Semi-hosting IO\n");
			io_close(local_image_handle);
		}
	}
	return result;
}

void plat_arm_io_setup(void)
{
	int io_result;

	arm_io_setup();

	/* Register the additional IO devices on this platform */
	io_result = register_io_dev_sh(&sh_dev_con);
	assert(io_result == IO_SUCCESS);

	/* Open connections to devices and cache the handles */
	io_result = io_dev_open(sh_dev_con, (uintptr_t)NULL, &sh_dev_handle);
	assert(io_result == IO_SUCCESS);

	/* Ignore improbable errors in release builds */
	(void)io_result;
}

/*
 * FVP provides semihosting as an alternative to load images
 */
int plat_arm_get_alt_image_source(unsigned int image_id, uintptr_t *dev_handle,
				  uintptr_t *image_spec)
{
	int result = open_semihosting((const uintptr_t)&sh_file_spec[image_id]);
	if (result == IO_SUCCESS) {
		*dev_handle = sh_dev_handle;
		*image_spec = (uintptr_t)&sh_file_spec[image_id];
	}

	return result;
}