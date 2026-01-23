# LVGL ported to GLYN UI2 Starter Kit

## Overview

This guide provides steps to set up the **GLYN UI2 Starter Kit** board (based on the **Ka-Ro QSRZ-G2L0** module with **Renesas RZ/G2L**)
and to cross-compile an LVGL application to run on the target.

The setup is based on **Yocto kirkstone**, which slightly differs from newer official Ka-Ro documentation.

## Buy

You can buy this Kit from Glyn directly: https://www.glyn.com/products/computer-on-module/qs-evaluationkits/

---

## Benchmark

### DRM + EGL + Software rendering

```
Benchmark Summary (9.5.0 dev)
Name, Avg. CPU, Avg. FPS, Avg. time, render time, flush time
Empty screen, 18%, 40, 21, 3, 18
Moving wallpaper, 22%, 52, 13, 4, 9
Single rectangle, 12%, 46, 17, 0, 17
Multiple rectangles, 17%, 51, 12, 1, 11
Multiple RGB images, 17%, 51, 12, 1, 11
Multiple ARGB images, 22%, 48, 16, 3, 13
Rotated ARGB images, 26%, 53, 13, 6, 7
Multiple labels, 27%, 55, 10, 5, 5
Screen sized text, 33%, 48, 18, 12, 6
Multiple arcs, 17%, 54, 10, 1, 9
Containers, 14%, 52, 13, 3, 10
Containers with overlay, 29%, 51, 16, 9, 7
Containers with opa, 16%, 51, 14, 4, 10
Containers with opa_layer, 17%, 47, 17, 5, 12
Containers with scrolling, 25%, 54, 13, 6, 7
Widgets demo, 35%, 55, 9, 5, 4
All scenes avg.,21%, 50, 13, 4, 9
```

Other available configurations:

* DRM
* Wayland (SHM)
* Wayland (EGL)
* fbdev

Each configuration can be used with single or multi-threaded rendering.

---

## Specification

### CPU and memory

* **MPU**:
  * Dual Cortex-A55 up to 1.2 GHz
  * Cortex-M33 real-time core

* **RAM**:
  * 1 GB DDR3L

* **Flash**:
  * eMMC (typically 4 GB on module)
  * microSD support

* **GPU**:
  * Arm Mali-G31

### Display and camera interfaces

* MIPI-DSI
* Parallel RGB
* MIPI-CSI-2

### Connectivity

* GbE Ethernet
* USB 2.0
* CAN / CAN-FD
* UART, SPI, I2C, PWM, ADC
* GPIO via carrier board

### Debug

* UART
* JTAG
* Fastboot via USB

---

## Getting started

### Hardware setup

Follow the [Glyn Manual](GLYNKaRo_UI2_KIT_Manual.pdf), chapters 3, 4 and 5 in order to setup the display.

Ensure:

* Display is connected
* Ethernet is connected to the same LAN as the host
* Serial console is available via a Micro USB cable connected to port X2
* Micro USB cable is connected to port X16

---

## Yocto setup (kirkstone)

The Yocto setup is based on **kirkstone**, which slightly deviates from the official [Ka-Ro documentation](https://karo-electronics.github.io/docs/getting-started/quickguides/qsrz-g2l0.html).

The following steps were tested on an Ubuntu 22.04 host.

A docker container can also be used:

```bash
docker run --rm -it -v $(pwd):/workdir crops/poky:ubuntu-22.04 bash
```

### Install Required Packages

If you're using the previously mentioned docker container, this step can be skipped.

```bash
sudo apt update
sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio \
python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 \
libegl1-mesa libsdl1.2-dev python3-subunit mesa-common-dev zstd liblz4-tool file locales
sudo locale-gen en_US.UTF-8
sudo apt install python-is-python3
```

### Setup Git

```bash
git config --global user.name "Your Name"
git config --global user.email "Your Email"
```

### Setup Repo

```bash
mkdir ~/bin
curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
chmod a+x ~/bin/repo
export PATH=~/bin:$PATH
```

### Create BSP

> [!NOTE]
> From here onwards, the setup is different from the official Ka-Ro documentation.

```bash
mkdir ~/karo-bsp
cd karo-bsp
repo init -u https://github.com/karo-electronics/karo-bsp -b kirkstone
repo sync
```

### Patch the recipes

Some repositories changed their default branch from `master` to `main` which means we need to patch the recipes.

You can apply [this patch](./kirkstone.patch) inside `~/karo-bsp/layers/poky`

```bash
cd ~/karo-bsp/layers/poky
git apply path/to/kirkstone.patch
```

### Setup the Build Environment

```bash
export MACHINE=qsrz-g2l0
export DISTRO=karo-wayland
export KARO_BASEBOARD=qsglyn1
source setup-environment build-qsrz-g2l0
```

> [!TIP]
> If you see the following message:
> ```bash
>   $ source setup-environment build-qsrz-g2l0
>   Configuring for qsrz-g2l0
>   The project root content has been updated. Please run 'setup-environment' again.
> ```
> Simply run the `source setup-environment build-qsrz-g2l0` command again.
>

### Build the image

```bash
bitbake karo-image-weston
```


---

## SDK setup for LVGL

Build the SDK:

```bash
cd ~/karo-bsp/build-qsrz-g2l0
bitbake karo-image-weston -c populate_sdk
```

Install the generated SDK on the host:

```bash
./tmp/deploy/sdk/karo-wayland-glibc-x86_64-karo-image-weston-cortexa55-qsrz-g2l0-toolchain-5.10-kirkstone.sh
```

---

## Deployment

1. Connect to the serial terminal using the DEBUG port.
2. Reset the board
3. Interrupt U-Boot by pressing any key
4. Run:


```bash
u-boot> fastboot usb 0
```

5. Flash the module using `uuu`

```bash
cd ~/karo-bsp/build-qsrz-g2l0/tmp/deploy/images/qsrz-g2l0/karo-image-weston
wget https://github.com/nxp-imx/mfgtools/releases/download/uuu_1.5.141/uuu
chmod a+x uuu
sudo ./uuu -v
```

### Display startup

Set the correct video mode and overlays in U-Boot:

```bash
u-boot> setenv videomode ET0500
u-boot> setenv overlays_qsglyn1 'karo-led karo-sdcard qsrz-lcd-panel qsrz-ili2130 qsrz-ksz9131'
u-boot> saveenv
u-boot> reset
```

> [!NOTE]
> Available video modes can be listed with:
> 
> ```bash
> u-boot> fdt print display
> ```
> 
> Examples include: `TM101`, `ETQ570`, `ET0700`, `ET0500`, `ET0430`, `ET0350`, `ETV570`, `VGA`.

## Build and run your application

### Clone the application

```bash
git clone https://github.com/lvgl/lv_port_linux
cd lv_port_linux
```

### Build using the SDK

```bash
source /opt/karo-wayland/5.10-kirkstone/environment-setup-cortexa55-poky-linux
cmake -B build
cmake --build build -j$(nproc)
```

---

### Prepare the run environment

1. Copy the executable to the board:

```bash
scp -O build/bin/lvglsim root@<BOARD_IP>:/root
```

2. SSH into the board:

```bash
ssh root@<BOARD_IP>
```

#### FBDEV and DRM setup

If using **DRM** or **FBDEV**, stop Weston:

```bash
/etc/init.d/weston stop
```

#### Wayland setup

If using **Wayland**, do not stop Weston, or if you've already stopped it, you can restart it with:

```bash
/etc/init.d/weston start
```

Also make sure to setup these environment variables:

```bash
export XDG_RUNTIME_DIR=/run/user/1000
export WAYLAND_DISPLAY=wayland-1
```


### Run the application

```bash
./lvglsim
```

---

## Contribution and Support

For board-specific issues, please open an issue in this repository.
For LVGL-related issues, use the main LVGL repository.
