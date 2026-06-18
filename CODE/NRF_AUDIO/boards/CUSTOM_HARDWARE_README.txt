#
# Copyright (c) 2025 Custom Audio Hardware Configuration - Implementation Guide
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

# CUSTOM HARDWARE IMPLEMENTATION GUIDE
# ====================================
#
# This document describes how to use the custom hardware configuration files
# for your nRF5340 Audio DK with custom PCB.
#
# FILES CREATED:
# ==============
#
# 1. boards/custom_hardware.overlay
#    - Device tree overlay defining all GPIO, SPI, I2S, and ADC pin assignments
#    - Maps nRF5340 pins to your custom hardware peripherals
#    - Configures ST7789 LCD, MicroSD card, PCM5102A DAC, button ladder
#
# 2. boards/custom_hardware.conf
#    - Kconfig settings enabling all required hardware drivers
#    - SPI, I2S, ADC, GPIO, MicroSD, filesystem support
#    - Button handler, power management, debug/logging settings
#
# 3. boards/resistor_ladder_button_guide.txt
#    - Detailed electrical design for analog button ladder on AIN0
#    - Resistor values, voltage calculations, BOM, assembly steps
#    - Troubleshooting and calibration procedures
#
#
# PIN MAPPING SUMMARY
# ===================
#
# CATEGORY        | PIN     | FUNCTION      | CONNECTED DEVICE
# =================================================================
# SPI1 Bus        | P1.02   | SCK           | MicroSD (J4:5) + ST7789 (U4:11)
#                 | P0.09   | MOSI          | MicroSD (J4:3) + ST7789 (U4:12)
#                 | P1.03   | MISO          | MicroSD (J4:7)
#
# SPI1 Chip Sel   | P0.13   | MSD_CS        | MicroSD (J4:2)
#                 | P1.05   | SST_CS        | ST7789 (U4:10)
#
# ST7789 Control  | P1.08   | DC            | ST7789 (U4:8)
#                 | P0.24   | RES           | ST7789 (U4:9)
#
# I2S0 Audio      | P0.16   | BCK           | PCM5102A (U1:2)
#                 | P0.21   | LRCK          | PCM5102A (U1:3)
#                 | P0.19   | DIN (TX)      | PCM5102A (U1:4)
#
# Button Ladder   | P0.04   | AIN0          | Resistor Ladder Network
#
# Amp Control     | P0.25   | AMP_SHDN      | TPA6132A2 (U2:8 EN)
#                 | P0.26   | AMP_MUTE      | Audio Mute Transistor
#
# Power Monitor   | P0.05   | BAT_CHG       | MCP73831 (U3:1 STAT)
#                 | P0.06   | VBUS_DETECT   | USB Protection Array
#                 | P0.07   | BAT_SENSE     | Battery Voltage ADC
#
# Button Wake     | P0.28   | BTN_WAKE      | Dual-Diode Wake Circuit
#
#
# BUTTON CONFIGURATION
# ====================
#
# All buttons are implemented on a single ADC pin (AIN0 / P0.04) using a
# resistor ladder network. Pressing each button creates a unique voltage
# level read by the ADC, which is mapped to a button function.
#
# Button Name      | ADC Reading | Tolerance | Hardware Connection
# ==================================================================
# BUTTON_VOLUME_DOWN  | 820        | ±150      | 68kΩ resistor to GND
# BUTTON_VOLUME_UP    | 1640       | ±150      | 33kΩ resistor to GND
# BUTTON_PLAY_PAUSE   | 2457       | ±150      | 22kΩ resistor to GND
# BUTTON_4            | 3277       | ±150      | 15kΩ resistor to GND
# BUTTON_5            | 4000       | ±150      | 10kΩ resistor to GND
#
# Pull-up resistor: 10kΩ from AIN0 to VCC
# Decoupling: 100nF capacitor from AIN0 to GND
#
# Each button when pressed shorts its resistor to ground, creating a voltage
# divider with the pull-up. The ADC reads the resulting voltage level.
#
#
# HOW TO USE THIS CONFIGURATION
# ==============================
#
# Option 1: Using the Custom Overlay with Default Board
# -------------------------------------------------------
#
# If you want to keep using nrf5340_audio_dk but override pins:
#
#   west build -b nrf5340_audio_dk/nrf5340/cpuapp \
#     --sysbuild \
#     -- -DDTC_OVERLAY_FILE=custom_hardware.overlay \
#     -DCONF_FILE=prj.conf \
#     -DEXTRA_CONF_FILE=boards/custom_hardware.conf
#
#
# Option 2: Create Custom Board Definition
# ------------------------------------------
#
# If you want a permanent custom board definition:
#
# 1. Create a new board directory in your project:
#    mkdir -p boards/my_custom_hw
#
# 2. Copy the device tree and config files:
#    cp boards/custom_hardware.overlay boards/my_custom_hw/my_custom_hw.overlay
#    cp boards/custom_hardware.conf boards/my_custom_hw/my_custom_hw.conf
#
# 3. Create board description file (boards/my_custom_hw/board.yml):
#    identifier: my_custom_hw
#    name: Custom Audio Hardware
#    type: mcu
#    arch: arm
#    mcu: nrf5340_cpuapp
#
# 4. Build with:
#    west build -b my_custom_hw --sysbuild \
#      -- -DCONF_FILE=prj.conf \
#      -DEXTRA_CONF_FILE=broadcast_source/overlay-broadcast_source.conf
#
#
# PERIPHERAL FEATURES ENABLED
# ============================
#
# Serial Communication:
#   - UART console via RTT and serial
#   - 4KB logging buffer
#   - Shell access
#
# SPI Master (SPI1):
#   - 20 MHz clock for ST7789 display
#   - 25 MHz clock for MicroSD card
#   - Separate chip selects for each device
#
# I2S (I2S0):
#   - Master TX mode for streaming audio to PCM5102A DAC
#   - 1ms frame duration
#   - Configurable sample rate and bit depth
#
# ADC (SAADC):
#   - Channel 7 (AIN0) for button ladder input
#   - Channel 5 (AIN5) for battery voltage monitoring (optional)
#   - 1/4 gain for extended range
#   - 12-bit resolution
#
# GPIO:
#   - GPIO0 and GPIO1 ports enabled
#   - Used for SPI chip selects, display control, amp control
#
# MicroSD/FAT-FS:
#   - SDHC interface for MicroSD card
#   - FAT32 filesystem support
#   - Mounted at /sdcard
#   - 10 files, 10 directories max
#
# Power Management:
#   - PM and PM_DEVICE enabled for sleep modes
#   - Button press can wake device from low-power states
#
#
# VERIFYING THE CONFIGURATION
# ============================
#
# After building and flashing:
#
# 1. Check serial console for startup messages
# 2. Verify GPIO pins blink (if LED test code present)
# 3. Test SPI: Check if MicroSD card is detected
# 4. Test I2S: Verify audio output via PCM5102A DAC
# 5. Test buttons: Press each button, watch for ADC readings in debug output
#
# Sample debug output when button pressed:
#   [button_handler] DEBUG: Pushed analog button name: BUTTON_VOLUME_UP
#   [button_handler] DEBUG: ADC value: 1640
#
#
# MODIFYING THE CONFIGURATION
# ============================
#
# To add more features or change pinouts:
#
# 1. Edit boards/custom_hardware.overlay for device tree changes
#    (pin assignments, peripheral configuration)
#
# 2. Edit boards/custom_hardware.conf for driver options
#    (enable/disable features, buffer sizes, timeouts)
#
# 3. Update resistor_ladder_button_guide.txt if button thresholds change
#
# 4. Rebuild: west build -b nrf5340_audio_dk/nrf5340/cpuapp --pristine
#
#
# TROUBLESHOOTING BUILD ISSUES
# =============================
#
# Error: "Cannot find source file"
#   - Check all referenced source files exist
#   - Verify CMakeLists.txt paths are correct
#
# Error: "Unknown device tree node"
#   - Verify pin names match nRF5340 datasheet
#   - Check pinctrl node names are correct
#
# Error: "Undefined CONFIG option"
#   - Verify Kconfig symbol exists in relevant Kconfig files
#   - Check for typos in CONFIG_* option names
#
# Build succeeds but device doesn't work:
#   - Check serial console for runtime errors
#   - Verify physical connections match pin assignments
#   - Use multimeter to verify voltages at key nodes
#
#
# REFERENCES
# ==========
#
# - nRF5340 DK Product Specification: Nordic Semiconductor docs
# - ST7789V Display Datasheet: Sitronix
# - PCM5102A DAC Datasheet: Texas Instruments
# - TPA6132A2 Amplifier Datasheet: Texas Instruments
# - MCP73831 Charger Datasheet: Microchip
# - Zephyr Device Tree Documentation: https://docs.zephyrproject.org/latest/build/dts/
# - Zephyr Kconfig Guide: https://docs.zephyrproject.org/latest/build/kconfig/
#
