# LVGL ported to STM32MP257F-DK

**:rocket: This repository is available in the [LVGL Project Creator](https://lvgl.io/tools/project-creator), making it easy to create and customize a new project in just a few clicks.**

## Overview

The STM32MP257F-DK is ST's Discovery kit for the STM32MP25 series: a dual Arm
Cortex-A35 application processor with a Cortex-M33 real-time core, a VeriSilicon
GC8000UL 3D GPU and a 1.35 TOPS NPU. The GPU speaks OpenGL ES 3.2, so this board
runs both LVGL's 2D rendering and its 3D/glTF rendering.

Two ready-made configurations are provided in this folder:

| Defconfig                                              | Rendering                                       | Demo                     |
| ------------------------------------------------------ | ----------------------------------------------- | ------------------------ |
| [`wayland.defconfig`](wayland.defconfig)               | Software rendering, Wayland SHM backend         | Widgets                  |
| [`wayland_3d.defconfig`](wayland_3d.defconfig)         | GPU rendering (NanoVG/GLES) + glTF, Wayland EGL | 3D truck                 |

## Buy

You can purchase a STM32MP257F-DK from https://www.st.com/en/evaluation-tools/stm32mp257f-dk.html

## Benchmark

<!-- TODO: record the benchmark demo on the board and fill in the numbers below.
     Run the `wayland.defconfig` build fullscreen (`./lvglsim -f`) and use the
     values printed by the benchmark demo's summary screen. -->

Wayland SHM backend, software rendering, 2 draw units, 32 bit color depth, fullscreen.
Benchmark Summary (9.6.0 dev)
Name, Avg. CPU, Avg. FPS, Avg. time, render time, flush time

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              | 11%      | 26       | 31        | 1           | 30         |
| Moving wallpaper          | 16%      | 30       | 31        | 2           | 29         |
| Single rectangle          | 11%      | 30       | 32        | 0           | 32         |
| Multiple rectangles       | 11%      | 30       | 31        | 1           | 30         |
| Multiple RGB images       | 14%      | 30       | 31        | 1           | 30         |
| Multiple ARGB images      | 16%      | 30       | 31        | 2           | 29         |
| Rotated ARGB images       | 19%      | 30       | 31        | 5           | 26         |
| Multiple labels           | 19%      | 29       | 31        | 4           | 27         |
| Screen sized text         | 30%      | 30       | 32        | 8           | 24         |
| Multiple arcs             | 21%      | 30       | 31        | 4           | 27         |
| Containers                | 13%      | 35       | 27        | 1           | 26         |
| Containers with overlay   | 29%      | 30       | 31        | 9           | 22         |
| Containers with opa       | 15%      | 34       | 28        | 2           | 26         |
| Containers with opa_layer | 18%      | 31       | 30        | 4           | 26         |
| Containers with scrolling | 26%      | 30       | 31        | 7           | 24         |
| Widgets demo              | 35%      | 29       | 31        | 11          | 20         |
| All scenes avg.           | 19%      | 30       | 29        | 3           | 26         |

> [!NOTE]
> Flush time appears high, but this is expected: it reflects the driver waiting for vsync, not rendering overhead.
> Actual render time is only a few ms per frame in most scenes, and CPU usage stays low (11–35%) across the board, confirming LVGL runs comfortably on the STM32MP2.

## Specification

### CPU and Memory
- **MPU:** Dual core Arm Cortex-A35 @ 1.5 GHz
- **MCU:** Arm Cortex-M33 @ 400 MHz (TrustZone, FPU/MPU)
- **RAM:** 4 GB (32-Gbit) LPDDR4
- **Flash:** 8 GB (64-Gbit) eMMC 5.1, microSD card slot
- **GPU:** VeriSilicon (Vivante) GC8000UL @ up to 900 MHz — OpenGL ES 3.2,
  Vulkan 1.2, OpenCL 3.0, OpenVX 1.3; up to 150 Mtriangle/s and 900 Mpixel/s
- **NPU:** 1.35 TOPS
- **Video:** hardware H.264 encode/decode

### Display and Touch
- **Interface:** 4-lane LVDS display connector, HDMI output
- **Color Depth:** 32-bit (`LV_COLOR_DEPTH 32`)
- **Panel:** not included with the kit — attach an LVDS panel or an HDMI monitor
- **Camera:** 2-lane MIPI CSI-2 connector

### Connectivity
- Gigabit Ethernet
- 1x USB Type-C (USB 3.0 + Power Delivery, also the power input)
- 2x USB 2.0 High-Speed Type-A
- Wi-Fi 802.11b/g/n and Bluetooth Low Energy 4.1
- GPIO expansion connectors
- Onboard STLINK-V3EC debugger/programmer with virtual COM port

## Getting started

### Hardware setup

You will need the following:
- Development PC
- USB Type-C power supply with Power Delivery
- USB Type-C cable for the STLINK-V3EC (serial console + debug)
- Ethernet cable (recommended, for `scp` and `ssh`)
- A display: HDMI monitor, or an LVDS panel on the display connector
- microSD card, unless you boot from the onboard eMMC

Connect the display, the serial/debug USB-C cable to your PC and the ethernet
cable, then apply power through the USB-C PD input.

### Software setup

Follow ST's *Starter Package* to flash OpenSTLinux onto the board and boot it:
https://wiki.st.com/stm32mpu/wiki/STM32MP25_Discovery_kits_-_Starter_Package

The image boots into a Weston Wayland compositor, which is what the
configurations in this folder target.

Then install the SDK from the *Developer Package* to cross-compile for the
Cortex-A35:
https://wiki.st.com/stm32mpu/wiki/Getting_started/STM32MP2_boards/STM32MP257x-DK/Develop_on_Arm_Cortex-A35/Install_the_SDK

Activate the SDK environment in every terminal you build from. The path depends
on where you installed it, for example:

```shell
. $HOME/STM32MPU_workspace/STM32MP2-Ecosystem-v6.0.0/Developer-Package/SDK/environment-setup-cortexa35-ostl-linux
```

### Build the project

Clone this repository with its submodules:

```shell
git clone --recurse-submodules https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux
```

Configuration is driven by Kconfig, so the Kconfig tools are needed on the host.
Install them in a virtual environment (see the [main README](../../README.md)):

```shell
python3 -m venv env
source env/bin/activate
pip install kconfiglib pcpp
```

Make sure the SDK environment has been activated as described above, then select
one of the two board configurations. Without 3D:

```shell
defconfig boards/stm32mp257f-dk/wayland.defconfig
```

Or with 3D rendering and the glTF truck demo:

```shell
defconfig boards/stm32mp257f-dk/wayland_3d.defconfig
```

Both write a `.config` at the repository root, which you can further tweak with
`menuconfig`. Then build:

```shell
cmake -B build -GNinja
cmake --build build
```

`cmake` and `ninja` are included in the SDK, so you don't need to install them
separately.

### Run the project

Find the board's IP address by running `ip a` on the serial console, then copy
the binary over:

```shell
scp build/bin/lvglsim root@<board IP>:/home/root/
```

The 3D build also needs its assets. Copy the `3d` folder next to the binary:

```shell
scp -r 3d root@<board IP>:/home/root/
```

On the board, point the client at Weston's socket and run it fullscreen:

```shell
./lvglsim -f
```

Use `-f` to fill the whole screen and `-m` to maximize the window but keep the
taskbar visible. `-W` and `-H` set the window size, e.g. `./lvglsim -W 800 -H 480`.
Run `./lvglsim -h` for all options.

If the truck demo can't find its assets, set `LV_LINUX_3D_PATH` to the directory
holding them, e.g. `export LV_LINUX_3D_PATH=/home/root/3d`.

#### Driver backends

These configurations use the **Wayland** driver, which is the natural fit on
OpenSTLinux since Weston is running by default:

- `wayland.defconfig` uses the DMABUF backend: LVGL renders in software and hands
  buffers to the compositor through the dmabuf protocol.
- `wayland_3d.defconfig` uses the EGL backend, which is required for
  hardware-accelerated and 3D/glTF rendering.

**DRM/KMS works fine on this board too** if you'd rather drive the display
directly without a compositor. Stop Weston first
(`systemctl stop weston-graphical-session`), then configure
`CONFIG_LV_USE_LINUX_DRM`, with `CONFIG_LV_LINUX_DRM_BACKEND_EGL` for GPU/3D
rendering, or `CONFIG_LV_LINUX_DRM_BACKEND_FBDEV` for software rendering into
dumb buffers. LVGL also ships several DRM defconfigs in
[`lvgl/configs/defconfigs/`](https://github.com/lvgl/lvgl/tree/master/configs/defconfigs).

### Debugging

`gdbserver` is available in the OpenSTLinux image. Run it on the board and
connect from the development PC with the `gdb` from the SDK:

```shell
# on the board
gdbserver :2345 ./lvglsim -f
# on the PC, with the SDK environment activated
$GDB build/bin/lvglsim -ex "target remote <board IP>:2345"
```

## Notes

Documentation and resources for the board and the SoC:
- Product page: https://www.st.com/en/evaluation-tools/stm32mp257f-dk.html
- Board user manual (UM3385): https://www.st.com/resource/en/user_manual/um3385-discovery-kit-with-stm32mp257f-mpu-stmicroelectronics.pdf
- ST wiki, getting started: https://wiki.st.com/stm32mpu/wiki/Getting_started/STM32MP2_boards/STM32MP257x-DK
- STM32MP2 graphics on the ST wiki: https://wiki.st.com/stm32mpu/wiki/Category:Graphics

## Contribution and Support

If you find any issues with the development board feel free to open an Issue in this repository. For LVGL related issues (features, bugs, etc) please use the main [lvgl repository](https://github.com/lvgl/lvgl).

If you found a bug and found a solution too please send a Pull request. If you are new to Pull requests refer to [Our Guide](https://lvgl.io/docs/open/contributing/pull_requests) to learn the basics.
