# LVGL ported to RZ/G3L-EVKIT

**:rocket: This repository is available in the [LVGL Project Creator](https://lvgl.io/tools/project-creator), making it easy to create and customize a new project in just a few clicks.**

## Overview

The RZ/G3L-EVKIT (P/N `RTK9846L46S01000BE`) is Renesas' evaluation kit for the
RZ/G3L (`R9A08G046`), a quad-core Arm Cortex-A55 MPU with a Cortex-M33 real-time
core, an Arm Mali GPU and a video codec engine, aimed at HMI applications.

The kit is a SMARC 2.1.1 module board paired with the Renesas *Common Carrier
Board II* and a display adapter sub-board. Renesas ships a ready-made
[RZ/G HMI SDK](https://renesas-rz.github.io/rzg_hmi_sdk/latest/) for it: a
prebuilt bootable Weston image and a prebuilt cross-compilation toolchain. There
is no Yocto build and no bootloader flashing involved — write the image to a
microSD card, boot, install the toolchain, and you can cross-compile LVGL.

Both LVGL rendering paths run well here — software rendering across the A55
cores, and GPU rendering through NanoVG on OpenGL ES. Two ready-made
configurations are provided in this folder:

| Defconfig                              | Rendering                                                      | Demo      |
| -------------------------------------- | -------------------------------------------------------------- | --------- |
| [`sw.defconfig`](sw.defconfig)         | Software rendering, NEON + 2 draw units; Wayland DMABUF/EGL, DRM | Benchmark |
| [`nanovg.defconfig`](nanovg.defconfig) | GPU rendering (NanoVG/GLES) + glTF; Wayland EGL, DRM EGL         | Benchmark |

## Buy

You can purchase the RZ/G3L-EVKIT from https://www.renesas.com/en/design-resources/boards-kits/rz-g3l-evkit

The kit does **not** include the 65 W USB Type-C power supply or the microSD
card — you need to provide both.

## Benchmark

Results of LVGL's benchmark demo at 1280x720, 32-bit color (`XRGB8888`), with a
16 ms refresh period (60 FPS target), measured with the two defconfigs in this
folder over the **Wayland** backend.

### Software rendering

NEON-accelerated, 2 draw units, DMABUF backend ([`sw.defconfig`](sw.defconfig)):

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              |      18% |       53 |        15 |           2 |         13 |
| Moving wallpaper          |      32% |       59 |        15 |           6 |          9 |
| Single rectangle          |      12% |       59 |        15 |           0 |         15 |
| Multiple rectangles       |      19% |       60 |        15 |           4 |         11 |
| Multiple RGB images       |      20% |       59 |        14 |           2 |         12 |
| Multiple ARGB images      |      23% |       60 |        15 |           4 |         11 |
| Rotated ARGB images       |      20% |       59 |        14 |           6 |          8 |
| Multiple labels           |      31% |       60 |        14 |           5 |          9 |
| Screen sized text         |      26% |       33 |        27 |          10 |         17 |
| Multiple arcs             |      32% |       59 |        14 |           6 |          8 |
| Containers                |      14% |       59 |        14 |           1 |         13 |
| Containers with overlay   |      25% |       29 |        32 |          13 |         19 |
| Containers with opa       |      17% |       59 |        14 |           2 |         12 |
| Containers with opa_layer |      18% |       59 |        14 |           4 |         10 |
| Containers with scrolling |      42% |       57 |        15 |           9 |          6 |
| Widgets demo              |      26% |       37 |        24 |          12 |         12 |
| All scenes avg.           |      23% |       53 |        16 |           5 |         11 |

### NanoVG (GPU) rendering

EGL backend ([`nanovg.defconfig`](nanovg.defconfig)):

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              |       8% |       54 |        13 |           2 |         11 |
| Moving wallpaper          |       9% |       39 |        24 |           5 |         19 |
| Single rectangle          |       9% |       59 |        15 |           2 |         13 |
| Multiple rectangles       |       9% |       59 |        15 |           2 |         13 |
| Multiple RGB images       |      17% |       59 |        15 |           7 |          8 |
| Multiple ARGB images      |      18% |       59 |        15 |           7 |          8 |
| Rotated ARGB images       |      13% |       59 |        15 |           4 |         11 |
| Multiple labels           |      18% |       39 |        23 |          13 |         10 |
| Screen sized text         |      24% |       19 |        48 |          41 |          7 |
| Multiple arcs             |      13% |       59 |        14 |           4 |         10 |
| Containers                |      17% |       40 |        22 |          12 |         10 |
| Containers with overlay   |      13% |       29 |        32 |          13 |         19 |
| Containers with opa       |      17% |       40 |        23 |          13 |         10 |
| Containers with opa_layer |      23% |       29 |        32 |          21 |         11 |
| Containers with scrolling |      19% |       29 |        31 |          20 |         11 |
| Widgets demo              |      10% |       34 |        30 |          12 |         18 |
| All scenes avg.           |      14% |       44 |        22 |          11 |         11 |

### Which one to pick

Both are good options on this board; they trade frame rate against CPU time.

**NanoVG uses far less CPU** — 14% average against 23%, and the gap is widest
exactly where software rendering works hardest: `Containers with scrolling`
drops from 42% to 19%, `Moving wallpaper` from 32% to 9%, `Multiple arcs` from
32% to 13%. If your application needs those cycles for logic, networking or
video, that headroom is worth more than the frame rate difference. It is also
the only way to draw 3D/glTF content.

**Software rendering with NEON reaches higher frame rates in 2D** — 53 FPS
average against 44. The two are at parity on flat geometry and images (empty
screen, rectangles, RGB/ARGB images, rotated images, arcs and overlays all land
within 1 FPS); the GPU path falls behind here:

In short: pick NanoVG for low CPU usage or anything 3D, and software rendering
for text- and layer-heavy 2D interfaces that need every frame.

## Specification

### CPU and Memory
- **MPU:** Quad-core Arm Cortex-A55 (RZ/G3L, `R9A08G046`)
- **MCU:** Single-core Arm Cortex-M33
- **RAM:** 2 GB LPDDR4
- **Flash:** 64 GB eMMC, 16 MB QSPI flash, microSD card slots on both the module
  and the carrier board
- **GPU:** Arm Mali — OpenGL ES 1.1/2.0/3.0/3.1/3.2, OpenCL 2.0 Full Profile
- **Video:** hardware video codec engine

### Display and Touch
- **Interfaces:** MIPI-DSI, LVDS and digital parallel, each through the matching
  display connector sub-board
- **Video output:** micro HDMI on the carrier board (from MIPI-DSI/LVDS) and
  micro HDMI on the module board (from the parallel interface)
- **Color format:** 32-bit; the configurations here set
  `CONFIG_LV_COLOR_FORMAT_XRGB8888`
- **Panel:** not included with the kit — attach an HDMI monitor, or a panel on
  the display connector
- **Camera:** MIPI CSI-2 camera interface

### Connectivity
- 2x Gigabit Ethernet (TSN capable)
- USB 2.0, 2 channels: 1x OTG, 2x host through an onboard hub
- PCIe Gen2 4-lane slot (the RZ/G3L drives 1 lane)
- M.2 Key E and Key B
- 2x CAN-FD
- Mono speaker, stereo headphone, mic and aux audio interfaces
- 3x Digilent Pmod (Type-2A, Type-3A, Type-6A)
- USB Type-C power input

## Getting started

### Hardware setup

You will need the following:
- Development PC (Linux is assumed below)
- 65 W USB Type-C Power Delivery supply — **not included in the kit**
- microSD card, SDHC UHS Class 10, 4 GB minimum and 8 GB recommended — **not included in the kit**
- USB Type-A to micro Type-B cable for the serial console
- Micro HDMI cable and an HDMI monitor
- Ethernet cable (recommended, for `scp` and `ssh`)
- USB keyboard and mouse (optional)

Connect the serial cable to your PC, the micro HDMI cable to the monitor and the
ethernet cable, then apply power through the USB Type-C input.

Set the boot mode DIP switches for eSD boot:

| Boot mode           | `SW_MODE[1]` | `SW_MODE[2]` | `SW_MODE[3]` | `SW_MODE[4]` |
| ------------------- | ------------ | ------------ | ------------ | ------------ |
| eSD                 | ON           | ON           | OFF          | ON           |

> [!IMPORTANT]
> There are two SD card slots, one on 
> SMARC module and another one on the SMARC carrier
> Use the **SMARC module**'s slot for in eSD mode

> [!IMPORTANT]
> Use the micro HDMI connector on the **carrier board**, which is fed by the
> DSI-to-HDMI adapter on the underside of the board. The micro HDMI connector on
> top of the module board is driven by the parallel interface and is not
> configured in the stock image.

### Software setup

Nothing has to be built from source to bring the board up. Renesas' **RZ/G HMI
SDK** ships a prebuilt bootable image and a matching cross-compilation
toolchain, both in a single download, and the board boots straight from the
microSD card — there is no bootloader to flash. Follow its
[Getting Started guide](https://renesas-rz.github.io/rzg_hmi_sdk/latest/getting_started/)
and grab the **HMI SDK Package (eSD Boot Image & Toolchain Installer)**,
v3.5.0.0 at the time of writing:

https://www.renesas.com/document/sws/rzg-hmi-software-development-kit-version-3500-rzg3l

That release is built on Yocto 5.0.9 (scarthgap) with kernel 6.12 from RZ/G3L
BSP v1.0.0, Wayland 1.22.0 and Weston 13.0.1.

#### Flash the microSD card

The package contains two images; pick the one matching your display sub-board:

- `image-file_rzg3l_hmi-sdk_v3.5.0.0.zip` — MIPI-DSI
- `image-file_rzg3l_hmi-sdk_lvds_v3.5.0.0.zip` — LVDS

Both extract to `core-image-weston-smarc-rzg3l.rootfs.wic.gz`. Install
`bmap-tools` and write the card, keeping the `.bmap` file **in the same
directory** as the image:

```bash
sudo apt-get install unzip bmap-tools
lsblk   # identify the card, e.g. /dev/sdX — double check before writing
sudo bmaptool copy core-image-weston-smarc-rzg3l.rootfs.wic.gz /dev/sdX
```

Insert the card into the **module board** slot, as the eSD boot mode above
requires, and power the board on by holding the red button for about a second.
The board boots from the card directly, and after roughly 30 seconds the
*HMI SDK Demo Launcher* appears on the display. 

You can shut the board down cleanly with `shutdown -h now`.

#### Install the SDK

The same package contains the toolchain installer. Run it, then activate the
environment in every terminal you build from, the version in the path is the
VLP version bundled with your SDK release:

```shell
. /opt/rz-vlp/5.0.14/environment-setup-cortexa55-poky-linux
```

> [!WARNING]
> You may hit the following error:
>
> ```
> Setting it up...xargs: unmatched single quote; by default quotes are special to xargs unless you use the -0 option
> Failed to replace perl. Relocate script failed. Abort!
> ```
>
> The error is caused by some files part of the SDK having `'` on them.
>
> The extracted SDK is fine, only the relocation is unfinished. Complete it
> with correct quoting, substituting your install path for `T`:
>
> ```bash
> T=/opt/rz-vlp/5.0.14
> N=$T/sysroots/x86_64-pokysdk-linux
> B=/usr/local/oe-sdk-hardcoded-buildpath
>
> { find "$T" -maxdepth 1 -type f; find "$N" -type f; } \
>   | xargs -d '\n' -n100 file \
>   | grep ":.*\(ASCII\|script\|source\).*text" \
>   | awk -F': ' '{print $1}' \
>   | grep -Fv -e "$T/environment-setup-" -e "$T/relocate_sdk" -e "$T/post-relocate-setup" \
>   | xargs -d '\n' -n100 sed -i -e "s:$B:$T:g" \
>         -e "s:^#! */usr/bin/perl.*:#! /usr/bin/env perl:g" \
>         -e "s: /usr/bin/perl: /usr/bin/env perl:g"
>
> find "$N" -type l -exec sh -c 'ln -sfn "$(readlink "$1" | sed -e "s:'"$B"':'"$T"':")" "$1"' _ {} \;
> sed -e "s:$B:$T:g" -i "$T/post-relocate-setup.sh" \
>   && sh "$T/post-relocate-setup.sh" "$T" "$B" && rm -f "$T/post-relocate-setup.sh"
> rm -f "$T/relocate_sdk.py" "$T/relocate_sdk.sh"
>
> grep -rlI "$B" "$T" | xargs -d '\n' -r sed -i -e "s:$B:$T:g"
> ```
>
> Verify with:
>
> ```shell
> grep -rlI oe-sdk-hardcoded-buildpath "$T"
> ```
>
> It should print nothing. Mind the exit status if you script this:
> `grep` returns **1** when it finds no match, so a clean SDK means empty
> output *and* exit 1.

> [!IMPORTANT]
> The SDK sets `PKG_CONFIG_PATH` but not `PKG_CONFIG_LIBDIR`, so `pkg-config`
> still searches the host's `/usr` directories and might pick a host package.
> The compiler warns about it so if you see this warning:
>
> ```
> cc1: warning: include location "/usr/include/webp" is unsafe for cross-compilation [-Wpoison-system-directories]
> ```
> Override the search path entirely after sourcing the environment:
>
>
> ```shell
> export PKG_CONFIG_LIBDIR=$PKG_CONFIG_PATH
> ```
>
> `PKG_CONFIG_PATH` *appends* paths, while `PKG_CONFIG_LIBDIR` *replaces* them,
> which is what you want when cross-compiling.
>
>

#### Building the image yourself (optional)

You only need this if you want to change the image itself — to boot from QSPI
instead of the SD card, or to add packages. Renesas publishes a separate
[HMI SDK Yocto Build Package](https://www.renesas.com/document/sws/rzg-hmi-sdk-version-3500-yocto-recipe-and-binaries-rzg3l)
with the recipes and the prebuilt QSPI boot binaries, documented under
[Building the HMI SDK with Yocto](https://renesas-rz.github.io/rzg_hmi_sdk/latest/building_the_hmi_sdk_with_yocto/).
Add the `meta-rz-graphics` layer, then:

```shell
MACHINE=smarc-rzg3l bitbake core-image-weston                 # image
MACHINE=smarc-rzg3l bitbake core-image-weston -c populate_sdk # toolchain
```

They land in `build/tmp/deploy/images/smarc-rzg3l/` and `build/tmp/deploy/sdk/`.

> [!TIP]
> Setting up QSPI boot means sending the bootloader SREC files to the board over
> serial. Renesas' instructions use Tera Term on Windows; on Linux, open the port
> once with a terminal emulator at the right baud rate, then send the file with
> plain `cat`:
>
> ```shell
> cat fip-smarc-rzg3l.srec > /dev/ttyUSB0
> ```

### Build the project

Clone this repository with its submodules:

```shell
git clone --recurse-submodules https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux
```

Configuration is driven by Kconfig, so the Kconfig tools are needed on the host.
Install them in a virtual environment (see the [main README](../../README.md)):

```shell
/usr/bin/python3 -m venv env
source env/bin/activate
pip install kconfiglib pcpp
```

Make sure the SDK environment has been activated, then select one of the two board configurations.
Software rendering:

```shell
defconfig boards/renesas_rz-g3l-evkit/sw.defconfig
```

Or GPU rendering with NanoVG:

```shell
defconfig boards/renesas_rz-g3l-evkit/nanovg.defconfig
```

Both write a `.config` at the repository root, which you can further tweak with
`menuconfig`. Then build:

```shell
cmake -B build -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

`cmake` and `ninja` come with the SDK, so you don't need to install them
separately.

### Run the project

Find the board's IP address by running `ip a` on the serial console, then copy
the binary over:

```shell
scp build/bin/lvglsim root@<board IP>:/home/root/
```

The `nanovg.defconfig` build also enables glTF. If you switch the demo to the 3D
truck, copy the `3d` folder next to the binary:

```shell
scp -r 3d root@<board IP>:/home/root/
```

Both configurations compile in the DRM and Wayland backends, so pick one at
runtime with `-b`. On the stock image Weston is already running, which makes
Wayland the natural choice:

```shell
./lvglsim -b wayland -f
```

Use `-f` to fill the whole screen and `-m` to maximize the window but keep the
taskbar visible. `-W` and `-H` set the window size, e.g.
`./lvglsim -b wayland -W 1280 -H 720`. Run `./lvglsim -h` for all options and
`-B` to list the backends compiled into your binary.

To drive the display directly through DRM/KMS instead, stop Weston first so it
releases the display, then run with the DRM backend:

```shell
systemctl stop weston weston.socket
./lvglsim -b drm
```

If the truck demo can't find its assets, set `LV_LINUX_3D_PATH` to the directory
holding them, e.g. `export LV_LINUX_3D_PATH=/home/root/3d`.

#### Driver backends

The benchmarks above were taken over Wayland, which is the natural fit on the
stock image since Weston is already running. The two configurations differ in
how they hand buffers to the compositor:

- `sw.defconfig` builds both `CONFIG_LV_WAYLAND_USE_DMABUF` and
  `CONFIG_LV_WAYLAND_USE_EGL`. Software rendering pairs with DMABUF, which
  passes the rendered buffer to Weston without an extra copy.
- `nanovg.defconfig` uses `CONFIG_LV_WAYLAND_BACKEND_EGL`, required for the GPU
  and glTF paths.

Both also enable the DRM backend with `CONFIG_LV_LINUX_DRM_BACKEND_EGL`, so you
can drive the display directly without a compositor, as described above. LVGL
ships further ready-made DRM defconfigs in
[`lvgl/configs/defconfigs/`](https://github.com/lvgl/lvgl/tree/master/configs/defconfigs).

### Debugging

`gdbserver` is available in the HMI SDK image. Run it on the board and connect
from the development PC with the `gdb` from the SDK:

```shell
# on the board
gdbserver :2345 ./lvglsim -b wayland -f
# on the PC, with the SDK environment activated
$GDB build/bin/lvglsim -ex "target remote <board IP>:2345"
```

## Notes

Documentation and resources for the board and the SoC:
- Product page: https://www.renesas.com/en/products/rz-g3l
- Evaluation kit: https://www.renesas.com/en/design-resources/boards-kits/rz-g3l-evkit
- Quick Start Guide: https://www.renesas.com/en/document/qsg/rzg3l-evaluation-kit-quick-start-guide
- RZ/G HMI SDK documentation: https://renesas-rz.github.io/rzg_hmi_sdk/latest/
- HMI SDK getting started: https://renesas-rz.github.io/rzg_hmi_sdk/latest/getting_started/
- HMI SDK downloads (image + toolchain): https://renesas-rz.github.io/rzg_hmi_sdk/latest/download/
- Building the image and SDK with Yocto: https://renesas-rz.github.io/rzg_hmi_sdk/latest/building_the_hmi_sdk_with_yocto/

## Contribution and Support

If you find any issues with the development board feel free to open an Issue in this repository. For LVGL related issues (features, bugs, etc) please use the main [lvgl repository](https://github.com/lvgl/lvgl).

If you found a bug and found a solution too please send a Pull request. If you are new to Pull requests refer to [Our Guide](https://lvgl.io/docs/open/contributing/pull_requests) to learn the basics.
