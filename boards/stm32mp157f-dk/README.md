# LVGL ported to STM32MP157F-DK2

## Overview

A versatile dual ARM Cortex-A7 MPU from ST equipped to run Linux with
impressive graphics capabilities and lots of IO.

## Buy

You can purchase a STM32MP157F-DK2 from https://www.st.com/en/evaluation-tools/stm32mp157f-dk2.html

## Benchmark

<!-- <a href="https://www.youtube.com/watch?v=XXXXXXXXXXXXXXXXXXXXXXX">
    <img src="https://github.com/user-attachments/assets/87c1f2e5-0260-4772-b711-13fdab467474" width="75%">
</a> -->

Wayland default config, fullscreen.

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              | 17%      | 26       | 15        | 1           | 14         |
| Moving wallpaper          | 20%      | 30       | 5         | 3           | 2          |
| Single rectangle          | 10%      | 29       | 0         | 0           | 0          |
| Multiple rectangles       | 14%      | 30       | 1         | 1           | 0          |
| Multiple RGB images       | 10%      | 30       | 1         | 1           | 0          |
| Multiple ARGB images      | 16%      | 30       | 4         | 4           | 0          |
| Rotated ARGB images       | 20%      | 30       | 8         | 8           | 0          |
| Multiple labels           | 31%      | 30       | 5         | 5           | 0          |
| Screen sized text         | 32%      | 24       | 38        | 17          | 21         |
| Multiple arcs             | 8%       | 30       | 1         | 1           | 0          |
| Containers                | 10%      | 30       | 2         | 2           | 0          |
| Containers with overlay   | 27%      | 25       | 38        | 12          | 26         |
| Containers with opa       | 13%      | 30       | 4         | 4           | 0          |
| Containers with opa_layer | 20%      | 30       | 10        | 9           | 1          |
| Containers with scrolling | 17%      | 25       | 37        | 6           | 31         |
| Widgets demo              | 33%      | 26       | 25        | 8           | 17         |
| All scenes avg.           | 18%      | 28       | 12        | 5           | 7          |

## Specification

### CPU and Memory
- **MPU:** Dual core ARM Cortex-A7 @ 800MHz
- **MCU:** ARM Cortex-M4
- **RAM:** 512MB DDR3L 16-bit @ 533MHz
- **Flash:** MicroSD Card slot
- **GPU:** 3D GPU: Vivante - OpenGL ES 2.0

### Display and Touch
- **Resolution:** 800x480
- **Display Size:** 4"
- **Interface:** MIPI DSI
- **Color Depth:** 24-bit
- **Technology:** TFT
- **DPI:** 233 px/inch
- **Touch Pad:** Capacitive

### Connectivity
- Ethernet
- HDMI
- Wi-Fi 802.11b/g/n
- Bluetooth Low Energy 4.1
- 4x USB Type-A
- 3.5mm headset jack
- GPIO pins (Raspberry Pi and Arduino compatible)
- USB serial port
- Onboard ST-LINK/V2-1 debugger/programmer

## Getting started

### Hardware setup

You will need the following:
- Development PC
- USB-C cable (included)
- USB-A cable
- 5V/3A USB-C power supply
- Ethernet cable
- MicroSD card (included)

Insert the SD card, connect the USB cables to the PC, and connect
the ethernet cable. Apply power with the USB-C power supply.
You may test the included OS image on the included SD card before continuing.

Be prepared to set the boot switches with the power removed, later.

### Software setup

- Ubuntu 20.04 or 22.04, optionally inside a Docker container or VM

Follow all the steps documented in
https://wiki.st.com/stm32mpu/wiki/Getting_started/STM32MP1_boards/STM32MP157x-DK2/Let%27s_start.
It's best to install STM32CubeProgrammer outside of a Docker container or VM, if using those
for development. When prompted to extract tar files, you may need to use the `gzip` command
before the `tar` command. For example, if prompted to extract a file like this...
```shell
tar xvf example.tar.gz
```
...you may need to do the following...
```shell
gzip --decompress --keep example.tar.gz
tar xvf example.tar
```

All you need to do in the "Develop on Arm Cortex-A7" section is "Install the SDK".
https://wiki.st.com/stm32mpu/wiki/Getting_started/STM32MP1_boards/STM32MP157x-DK2/Develop_on_Arm%C2%AE_Cortex%C2%AE-A7/Install_the_SDK

Now you can clone this repo.
```shell
git clone --recursive https://github.com/lvgl/lv_port_linux
cd lv_port_linux
```
Ensure the SDK environment has been activated. Here's an example of what
activating the SDK can look like, depending on where it's installed:
```shell
. /root/STM32MPU_workspace/Developer-Package/SDK/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi
```
Build the project:
```shell
cmake -B build -DCONFIG=wayland
cmake --build build -j$(nproc)
```

### Run the project

Transfer the binary to the board over SSH:
```
scp build/bin/lvglsim <board IP address>:/home/root/
```
On the board, run the binary:
```shell
./lvglsim -m
```

Use the `-m` flag when using the Wayland backend to fill the screen except
for the taskbar. Use the `-f` flag when using the wayland backend to fill
the whole screen.

#### Driver backends

This board supports a few different driver backends.

The `cmake -B build -DCONFIG=wayland` command above uses Wayland, which is
suitable while the Wayland desktop environment is active.

The `sdl` defaults also work unmodified.

The `drm-egl-2d` defaults can be used for the DRM driver backend. You must stop
the Wayland compositor first. Run `systemctl stop weston-graphical-session`.
You should also disable all OpenGL config options in the `drm-egl-2d` defaults
since they depend on OpenGL ES 3 while only OpenGL ES 2 is available on this board.
```shell
cp configs/drm-egl-2d.defaults lv_conf.defaults
# now disable LV_USE_OPENGLES and LV_LINUX_DRM_USE_EGL in lv_conf.defaults
cmake -B build
```

### Debugging
`gdbserver` is installed in the image. Use `gdbserver` on the board with `gdb` on
the development PC to interactively debug applications.

## Notes

Find documentation for the board here:
https://www.st.com/en/evaluation-tools/stm32mp157f-dk2.html#documentation

Here is detailed info about the board hardware:
https://wiki.st.com/stm32mpu/wiki/STM32MP157x-DKx_-_hardware_description

## Contribution and Support

For board-specific issues, please open an issue in this repository.
For LVGL-related issues, use the main LVGL repository.
