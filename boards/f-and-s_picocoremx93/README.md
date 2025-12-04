# LVGL ported to F&S PicoCore™MX93

## Overview

The F&S PicoCore™MX93 is a highly capable dual core compact SoM MPU
(System-on-Module) which exposes abundant IO through its
board-to-board mezzanine connectors. The processor is a dual-core 64-bit ARM Cortex-A55.
The evaluation kit comes with a 1024x600 capacitive touch display
and an evaluation board for the SoM which breaks-out
some common interfaces like USB, Ethernet, serial, GPIO, SD Card, and more.

## Buy

You can purchase the PicoCore™MX93 starter kit from https://fs-net.de/en/embedded-modules/computer-on-module-picocore/picocoremx93-with-nxp-imx93-cpu/starterkit-picocoremx93-linux/

## Benchmark

fbdev single-buffered partial, no OS, 32 bit color depth.

<a href="https://www.youtube.com/watch?v=F2lOlv2X46E">
    <img src="https://github.com/user-attachments/assets/3d3b4727-e6f3-4a37-83bb-9e9b892d984f" width="75%">
</a>

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              | 3%       | 26       | 0         | 0           | 0          |
| Moving wallpaper          | 7%       | 30       | 1         | 1           | 0          |
| Single rectangle          | 0%       | 29       | 0         | 0           | 0          |
| Multiple rectangles       | 1%       | 30       | 0         | 0           | 0          |
| Multiple RGB images       | 1%       | 29       | 0         | 0           | 0          |
| Multiple ARGB images      | 3%       | 30       | 1         | 1           | 0          |
| Rotated ARGB images       | 9%       | 29       | 2         | 2           | 0          |
| Multiple labels           | 6%       | 29       | 1         | 1           | 0          |
| Screen sized text         | 33%      | 30       | 10        | 10          | 0          |
| Multiple arcs             | 13%      | 29       | 2         | 2           | 0          |
| Containers                | 3%       | 29       | 0         | 0           | 0          |
| Containers with overlay   | 26%      | 29       | 8         | 8           | 0          |
| Containers with opa       | 7%       | 29       | 1         | 1           | 0          |
| Containers with opa_layer | 6%       | 29       | 2         | 2           | 0          |
| Containers with scrolling | 17%      | 29       | 4         | 4           | 0          |
| Widgets demo              | 26%      | 29       | 6         | 6           | 0          |
| All scenes avg.           | 10%      | 29       | 2         | 2           | 0          |

## Specification

### CPU and Memory
- **CPU:** NXP i.MX 93 (1.7 GHz dual-core ARM Cortex-A55)
- **RAM:** 2GB LPDDR4
- **Flash:** 64GB eMMC
- **GPU:** PXP

### Display and Touch
- **Resolution:** 1024x600
- **Display Size:** 7"
- **Interface:** LVDS
- **Color Depth:** 32-bit
- **Technology:** LCD
- **DPI:** 170 px/inch
- **Touch Pad:** Capacitive

### Connectivity
- 2x USB
- 2x ethernet
- Micro SD Card
- GPIO
- headphone jack

## Getting started

### Hardware setup

There are some hardware prerequisites required to use the
evaluation kit. You will need:

- A power supply capable of 5V output with two banana plug
  connector outputs. You may use adapters. The observed current draw
  of both the board and display
  while testing was less than 1 amp (5 watts).
- a null modem serial port on your PC. You may use a serial port to
  USB adapter. This is necessary to get started.

With the above supplies and the contents of the evaluation kit:

- Connect the two pairs of banana plugs to a power supply outputting 5V.
- Connect the other ends of the banana plug cables into both the display
  and the board. The connectors are each unique.
- Connect the null modem serial port connector to your PC via the port
  on the evaluation board. It's the black connector closest to the board's
  edge - one of four.
- Leave the DIP switches set to OFF.


### Software setup

There are releases with build artifacts maintained [in this repo on GitHub](https://github.com/lvgl/lv_port_f-and-s_picocoremx93).
You can skip building everything with Yocto and simply download the release
artifacts which were created ahead of time and can be reproduced.

The artifacts in the release are:

- emmc-fsimx93.sysimg: the OS image
- sdk-fsimx93-Y2025.02-pre.tar.gz: the SDK for cross-compiling applications
- lvglsim: the benchmark app which can be copied to the board and run

The following explains how to build everything with Yocto. See below when
these release artifacts apply and how to use them.

The main software support for this SoM is a Yocto Linux build and
application cross compiling with the SDK/toolchain exported by Yocto.
Building Linux images and development tools with Yocto takes a long time
and uses a lot of hard disk space. It is recommended to have 150 GB of free
disk space available on your development PC. These steps assume a linux PC.

At the time of writing, `fsimx93-Y2025.02-pre` was the suggested tagged release
to use.

These steps use Docker so that the system dependencies of Yocto do not need
to be installed globally on your system. If you don't want to use Docker,
Omit the `--docker` flag. Instead of being placed in a Docker shell, you will
remain in your main shell.

```shell
mkdir fands_picocoremx93
cd fands_picocoremx93
git clone -b fsimx93-Y2025.02-pre-fixes https://github.com/lvgl/lv_f-and-s_releases-fus.git releases-fus
cd releases-fus
./setup-yocto ../yocto-fus
# needed to work around an issue building one of the Yocto packages in Docker
echo 'DOCKER_PARAMS="--security-opt seccomp=unconfined"' > .docker_params
./setup-yocto ../yocto-fus --docker
# (you are inside a docker container now)
cd yocto-fus
MACHINE=fsimx93 DISTRO=fus-imx-wayland source fus-setup-release.sh
# include gdbserver in the image for debugging
echo 'EXTRA_IMAGE_FEATURES:append = " tools-debug"' >> conf/local.conf
# this will start the several hour long build of the Linux image and SDK.
bitbake fus-image-std && bitbake fus-image-std -c populate_sdk
```

https://github.com/lvgl/lv_f-and-s_releases-fus.git is used above. It's a fork of
https://github.com/FSEmbedded/releases-fus.git that includes fixes.
At the time of writing, the branch `fsimx93-Y2025.02-pre-fixes` which is checked out in
the steps above is based on the tag `fsimx93-Y2025.02-pre` from the upstream
and fixes something in `fs-release-manifest.xml`.

Once this process has completed, you will have an OS image for your kit.
There is already one pre-installed on the board when you receive it; however, it's best
to install this one so your SDK which you will build LVGL with will have
maximum compatibility with the OS image.

There are many custom ways to flash the image to the board which are documented
extensively in the [F&S doc PDFs](#notes). The most beginner-friendly method is to use the board's
bootloader to expose the MMC as a USB mass storage device to your PC which you
can then flash with Etcher (or similar) like it's an SD Card.

1. Connect the included USB-C cable to the board's USB-C port and your PC.
2. Power up the board with the serial terminal open. The baud rate is 115200.
3. Interrupt the boot process by pressing a key before the 3-second boot countdown
   ends to enter the U-Boot prompt.
4. At the `u-boot=>` prompt, enter the command `ums 0 mmc 0`.
5. Find the block device exported by the board on your PC (likely staring with `/dev/sd*`). It will likely show two
   existing partitions: "boot" and "root". Unmount both so that the block device has
   no mounted partitions.
6. Use Etcher or any other method like `dd` (which can take longer) to write the built
   Yocto image to the exposed block device. The image is located at
   `fands_picocoremx93/yocto-fus/yocto-fus/build-fsimx93-fus-imx-wayland/tmp/deploy/images/fsimx93/emmc-fsimx93.sysimg`.
   You may also use `emmc-fsimx93.sysimg` which is a release artifact of this repo on GitHub.

You must "install" the SDK built by Yocto to begin cross-compiling LVGL for the board.
There is an install script for the SDK generated at
`fands_picocoremx93/yocto-fus/yocto-fus/build-fsimx93-fus-imx-wayland/tmp/deploy/sdk/fus-imx-wayland-glibc-x86_64-fus-image-std-cortexa55-fsimx93-toolchain-5.0.sh`.
Docker was used for the Yocto build, but you may install this SDK in your main filesystem
outside of Docker since it will be confined to one directory. Simply execute the install script
`./fus-imx-wayland-glibc-x86_64-fus-image-std-cortexa55-fsimx93-toolchain-5.0.sh`
and optionally override the default install location (`/opt/fus-imx-wayland`).

You can also use the SDK release artifact of this repo on GitHub called sdk-fsimx93-Y2025.02-pre.tar.gz.
Extract it to `/opt` with the following command. In `/opt` you will have `fus-imx-wayland`.

```shell
cd /opt
tar -xf /path/to/sdk-fsimx93-Y2025.02-pre.tar.gz
```

To uninstall it later, simply delete the install directory (`fus-imx-wayland`).

### Run the project

Finally, it is time to clone this repo.

```bash
git clone --recursive https://github.com/lvgl/lv_port_linux.git
```

You will need the IP address of the board. Connect it to your local network with an ethernet cable.
Find the IP address with the command `ip a`.

At the time of writing, only the fbdev driver is supported. Wayland works, but touch is not
supported because the LVGL driver does not support this board's version of the protocol yet.
Stop the wayland compositor with the following command so the display is free to be used by LVGL fbdev.

```shell
systemctl stop weston
```

Copy the vscode settings and tasks to the root folder of the repository:

```bash
cp -r boards/f-and-s_picocoremx93/.vscode .
```

Open the folder in VS Code.

Edit `.vscode/settings.json`

- `"BOARD_IP"` - the local IP address of the board on your network
  so you can deploy, run, and debug remotely within VS Code.
- `"SDK_SETUP_SCRIPT"` - the path to the Yocto SDK you installed earlier.

Press ctrl+shift+p and choose "Tasks: Run Task". The tasks do the following

- "build" - Runs CMake and builds the app
- "fus_deploy-to-board" - builds the app and uploads it to `/tmp/lvglsim` on the board
- "fus_launch-on-board" - builds the app, uploads it, and runs it

If you cannot proceed during deploy/launch/debug due to
`WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!`, one solution is to delete `~/.ssh/known_hosts`.
I.e., `ssh-keygen -R <board_ip_address>`.

There is also a pre-built `lvglsim` binary in the release artifacts of this repo on GitHub. You
can upload that to the board and run it directly.

### Debugging

Interactively debug with VS Code with the "Debug on FuS Board" option
in the debug pane. Ensure `"BOARD_IP"` is correctly set in
`.vscode/settings.json`.

If you see "Errors exist after running preLaunchTask 'fus_debug-on-board'.",
simply click "Debug Anyways".

## Notes

Read documentation about the board here:
https://fs-net.de/en/embedded-modules/computer-on-module-picocore/picocoremx93-with-nxp-imx93-cpu/#panel-7

## Contribution and Support

If you find any issues with the development board feel free to open an Issue in this repository. For LVGL related issues (features, bugs, etc) please use the main [lvgl repository](https://github.com/lvgl/lvgl).

If you found a bug and found a solution too please send a Pull request. If you are new to Pull requests refer to [Our Guide](https://docs.lvgl.io/master/CONTRIBUTING.html#pull-request) to learn the basics.

