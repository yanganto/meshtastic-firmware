  Why Bit-Bang SPI (not hardware SPI)?

  The core problem: GPIO 22 is the SX1262's CS pin, but the ESP32-C6 I2C peripheral also claims GPIO 22 (as SDA) during peripheral initialization. This creates a conflict —
  the hardware can't use GPIO 22 as a GPIO-controlled CS while I2C holds it.

  The fix: BBTxRXHal (defined in variants/esp32c6/xiao_esp32c6/BBHal.h) is a bit-bang (software) SPI HAL that:
  1. Calls gpio_reset_pin(22) in its constructor to forcibly reclaim GPIO 22 from the I2C peripheral
  2. Manually toggles SCK/MOSI/MISO and CS lines in software
  3. Runs at 1 MHz to stay reliable without hardware assist

  This is enabled via the -DUSE_BB_HAL build flag in platformio.ini.

  ---
  Initialization Sequence (why the specific order matters)

  1. RESET pulse: 10 ms LOW → 20 ms wait — required by SX1262 datasheet before any SPI communication
  2. BUSY monitoring: Wait for BUSY pin to go LOW before sending commands — SX1262 signals "not ready" via BUSY=HIGH
  3. WDT extended to 120 s: The radio init is slow enough to trigger the default 5 s watchdog, so it's increased

  ---
  Summary

  The fix-spi-addr branch (your current branch, commits 05d5537 and ea4187d) exists because GPIO 22's dual role as I2C SDA and SX1262 CS required a non-standard bit-bang SPI
   approach, plus explicit pin reclamation logic that normal hardware SPI cannot handle. The pin numbers themselves match the Seeed Wio-SX1262 for XIAO schematic.

     Why SX1262 Uses SPI (not I2C)

  1. Speed

  SPI runs at 1–16+ MHz. I2C tops out at 400 kHz (fast mode) or 1 MHz (fast-plus, rarely used). For a radio transceiver, you need to:
  - Load a full packet payload into the TX FIFO quickly
  - Read an incoming packet from the RX FIFO before the next one arrives

  At LoRa data rates these aren't bottlenecks, but SPI gives headroom and predictability.

  2. Full-duplex transfers

  SPI has separate MOSI and MISO lines — you can send a command and receive status back simultaneously in one clock cycle. I2C is half-duplex (one direction at a time), which adds round-trip overhead for every register read.

  3. No addressing overhead

  I2C multiplexes multiple devices on two wires using 7-bit addresses, start/stop conditions, and ACK bits on every byte. SPI uses a dedicated CS line per device and sends raw bytes — lower protocol overhead, more deterministic timing.

  4. Simpler for the chip to implement

  RF transceivers already need tight timing control for the radio PHY. Implementing I2C's clock-stretching, arbitration, and address logic inside the silicon adds complexity and die area for little benefit in a point-to-point peripheral.

  ---
  What I2C Is Used For (on this board)

  In this design, I2C pins (GPIO 22/23) ended up being repurposed for the SX1262 CS and RXEN signals — which is exactly the root cause of the conflict this branch is fixing. The board has no I2C peripherals at all.

  ---
  In Short

  ┌──────────┬────────────────────────────────────────────┬──────────────────────────────────┐
  │ Property │                    SPI                     │               I2C                │
  ├──────────┼────────────────────────────────────────────┼──────────────────────────────────┤
  │ Speed    │ 1–16+ MHz                                  │ 100–400 kHz typical              │
  ├──────────┼────────────────────────────────────────────┼──────────────────────────────────┤
  │ Duplex   │ Full-duplex                                │ Half-duplex                      │
  ├──────────┼────────────────────────────────────────────┼──────────────────────────────────┤
  │ Wires    │ 4 (SCK, MOSI, MISO, CS)                    │ 2 (SDA, SCL)                     │
  ├──────────┼────────────────────────────────────────────┼──────────────────────────────────┤
  │ Overhead │ Minimal                                    │ Address + ACK per byte           │
  ├──────────┼────────────────────────────────────────────┼──────────────────────────────────┤
  │ Use case │ Fast peripherals (radios, flash, displays) │ Slow sensors (IMU, temp, EEPROM) │
  └──────────┴────────────────────────────────────────────┴──────────────────────────────────┘

  SX1262 is a fast peripheral that needs low-latency burst transfers — SPI is the right interface for it. I2C would work electronically but Semtech simply didn't build that option into the chip.


---

<div align="center" markdown="1">

<img src=".github/meshtastic_logo.png" alt="Meshtastic Logo" width="80"/>
<h1>Meshtastic Firmware</h1>

![GitHub release downloads](https://img.shields.io/github/downloads/meshtastic/firmware/total)
[![CI](https://img.shields.io/github/actions/workflow/status/meshtastic/firmware/main_matrix.yml?branch=master&label=actions&logo=github&color=yellow)](https://github.com/meshtastic/firmware/actions/workflows/ci.yml)
[![CLA assistant](https://cla-assistant.io/readme/badge/meshtastic/firmware)](https://cla-assistant.io/meshtastic/firmware)
[![Fiscal Contributors](https://opencollective.com/meshtastic/tiers/badge.svg?label=Fiscal%20Contributors&color=deeppink)](https://opencollective.com/meshtastic/)
[![Vercel](https://img.shields.io/static/v1?label=Powered%20by&message=Vercel&style=flat&logo=vercel&color=000000)](https://vercel.com?utm_source=meshtastic&utm_campaign=oss)

<a href="https://trendshift.io/repositories/5524" target="_blank"><img src="https://trendshift.io/api/badge/repositories/5524" alt="meshtastic%2Ffirmware | Trendshift" style="width: 250px; height: 55px;" width="250" height="55"/></a>

</div>

</div>

<div align="center">
	<a href="https://meshtastic.org">Website</a>
	-
	<a href="https://meshtastic.org/docs/">Documentation</a>
</div>

## Overview

This repository contains the official device firmware for Meshtastic, an open-source LoRa mesh networking project designed for long-range, low-power communication without relying on internet or cellular infrastructure. The firmware supports various hardware platforms, including ESP32, nRF52, RP2040/RP2350, and Linux-based devices.

Meshtastic enables text messaging, location sharing, and telemetry over a decentralized mesh network, making it ideal for outdoor adventures, emergency preparedness, and remote operations.

### Get Started

- 🔧 **[Building Instructions](https://meshtastic.org/docs/development/firmware/build)** – Learn how to compile the firmware from source.
- ⚡ **[Flashing Instructions](https://meshtastic.org/docs/getting-started/flashing-firmware/)** – Install or update the firmware on your device.

Join our community and help improve Meshtastic! 🚀

## Stats

![Alt](https://repobeats.axiom.co/api/embed/8025e56c482ec63541593cc5bd322c19d5c0bdcf.svg "Repobeats analytics image")
