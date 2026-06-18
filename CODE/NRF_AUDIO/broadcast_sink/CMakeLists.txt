#
# Copyright (c) 2023 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
zephyr_include_directories(${ZEPHYR_LVGL_MODULE_DIR})
makezephyr_include_directories(${ZEPHYR_BASE}/../modules/lib/gui/lvgl)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
