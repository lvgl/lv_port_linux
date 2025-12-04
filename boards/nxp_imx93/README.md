# LVGL ported to i.MX 93 Evaluation Kit

**:rocket: This repository is available in the [LVGL Project Creator](https://lvgl.io/tools/project-creator), making it easy to create and customize a new project in just a few clicks.**


## Overview

This guide provides steps to setup the i.MX 93 Evaluation Kit and to cross-compile an LVGL application to run it on the target.

## Buy

You can purchase the i.MX 93 Evaluation Kit from [NXP website](https://www.nxp.com/design/design-center/development-boards-and-designs/i-mx-evaluation-and-development-boards/i-mx-93-evaluation-kit:i.MX93EVK).

## Benchmark


<a href="https://www.youtube.com/watch?v=BwlU0H-azSA">
    <img src="https://github.com/user-attachments/assets/bafd50d8-0daf-4635-928c-33fb21f41e9f" width="75%" alt="image">
</a>

The default buffering is fbdev.

**Frame buffer, 1 thread**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| ------------------------- | -------- | -------- | --------- | ----------- | ---------- |
| Empty screen              | 6.00%    | 25       | 2         | 1           | 1          |
| Moving wallpaper          | 13.00%   | 27       | 4         | 3           | 1          |
| Single rectangle          | 2.00%    | 27       | 0         | 0           | 0          |
| Multiple rectangles       | 9.00%    | 29       | 2         | 1           | 1          |
| Multiple RGB images       | 12.00%   | 28       | 3         | 2           | 1          |
| Multiple ARGB images      | 26.00%   | 27       | 9         | 8           | 1          |
| Rotated ARGB images       | 85.00%   | 16       | 54        | 53          | 1          |
| Multiple labels           | 20.00%   | 26       | 5         | 4           | 1          |
| Screen sized text         | 0.00%    | 27       | 0         | 0           | 0          |
| Multiple arcs             | 20.00%   | 28       | 6         | 5           | 1          |
| Containers                | 20.00%   | 27       | 5         | 5           | 0          |
| Containers with overlay   | 56.00%   | 26       | 20        | 19          | 1          |
| Containers with opa       | 34.00%   | 28       | 11        | 10          | 1          |
| Containers with opa_layer | 46.00%   | 28       | 15        | 14          | 1          |
| Containers with scrolling | 38.00%   | 29       | 12        | 11          | 1          |
| Widgets demo              | 21.00%   | 27       | 6         | 6           | 0          |
| All scenes avg.           | 25.00%   | 26       | 8         | 8           | 0          |

**Frame buffer, 2 threads**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| ------------------------- | -------- | -------- | --------- | ----------- | ---------- |
| Empty screen              | 7.00%    | 25       | 2         | 1           | 1          |
| Moving wallpaper          | 13.00%   | 27       | 4         | 3           | 1          |
| Single rectangle          | 3.00%    | 27       | 0         | 0           | 0          |
| Multiple rectangles       | 11.00%   | 28       | 2         | 1           | 1          |
| Multiple RGB images       | 13.00%   | 27       | 3         | 2           | 1          |
| Multiple ARGB images      | 20.00%   | 28       | 6         | 5           | 1          |
| Rotated ARGB images       | 80.00%   | 25       | 32        | 31          | 1          |
| Multiple labels           | 24.00%   | 27       | 5         | 4           | 1          |
| Screen sized text         | 1.00%    | 27       | 0         | 0           | 0          |
| Multiple arcs             | 17.00%   | 27       | 5         | 4           | 1          |
| Containers                | 28.00%   | 27       | 6         | 6           | 0          |
| Containers with overlay   | 64.00%   | 29       | 21        | 20          | 1          |
| Containers with opa       | 41.00%   | 27       | 12        | 11          | 1          |
| Containers with opa_layer | 60.00%   | 28       | 19        | 18          | 1          |
| Containers with scrolling | 43.00%   | 28       | 13        | 12          | 1          |
| Widgets demo              | 23.00%   | 28       | 6         | 6           | 0          |
| All scenes avg.           | 28.00%   | 27       | 7         | 7           | 0          |

The other configurations that can be used are:

-   DRM
-   Wayland
-   SDL

Any of these buffering strategies can be used with multiple threads to render the frames.

Check out i.MX 93 Evaluation Kit in action, running LVGL's benchmark demo:

## Specification

### CPU and memory

-   **MCU**:
    -   Dual Cortex-A55 @1.7GHz
    -   Cortex-M33 @250MHz
    -   Arm® Ethos™ U-65 MicroNPU
    -   EdgeLock® Secure Enclave
-   **RAM**: 2 GB LPDDR4X / LPDDR4

    -   16 bits data bus with inline ECC
    -   3.7 GT/s

-   **Flash**:
    -   16 GB eMMC5.1
    -   MicroSD Slot
-   **GPU**: PowerVR

### Display and camera interfaces

-   MIPI DSI (mini-SAS)
-   X4 Lane LVDS (HDR)
-   MIPI-CSI (Camera 22P RPi)
-   Parallel RGB LCD (2x20 RPi)
-   Parallel Camera (2x20 RPi)

### Connectivity

-   X2 USB C 2.0 Connectors
-   2x GbE RJ45
-   CAN (HDR)
-   RPi 2X20 GPIO HDR
-   MFi Module Support
-   X4 CH ADC Support

### Debug

-   JTAG Connector
-   UART Via USB C
-   Remote Debug Support
-   Power Measurement Support

## Getting started

### Hardware setup

This [document](https://www.nxp.com/docs/en/user-guide/IMX_YOCTO_PROJECT_USERS_GUIDE.pdf) from NXP provides detailed information for the hardware setup. The following guide is inspired from this.

The display used in this guide is the LVDS panel, with a resolution of 1280x800.

### Prepare the board

The EMMC on the board should come flashed with an image.

-   To specify the board booting from the SD card, follow the booting switch table: ![boot_device_table](./docs/img/boot_device_table.png)

    The i.MX93 used has a Cortex-A55 (**use SW 4-1[0010]**):

    ![boot_mode_sw1301](./docs/img/boot_mode_sw1301.jpg)

-   The following picture shows the jumper setup for J704, J703 and J702:
    ![imx93_jumpers_j70s](./docs/img/imx93_jumpers_j70s.png)

-   Board setup:

    <p align="center"><img src="./docs/img/imx93_board_setup.jpg"></p>

    -   Connect the screen to the LVDS connector
    -   Connect RJ45 on any ethernet port. The board must be connected on the same LAN than the host.
    -   Connect USB-C power (black USB - J301)
    -   (Optional) Connect USB-C debug (gray - J401)

The board should boot and the screen should display something

#### Flash SD card

There are two options:

- **Option 1**: build Yocto image:

  -   Required packages to install on host:

      ```bash
      sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
      ```

  -   (optional) If you already have the "repo utility", skip this step.

      ```bash
      sudo apt install repo
      ```

  -   Clone the yocto project

      ```bash
      mkdir imx-yocto-bsp
      cd imx-yocto-bsp
      repo init -u https://github.com/nxp-imx/imx-manifest -b imx-linux-scarthgap -m imx-6.12.3-1.0.0.xml
      repo sync
      ```

  -   Build the image and the sdk

      ```bash
      # Use the script to setup the build folder and modify the conf files
      DISTRO=fsl-imx-wayland MACHINE=imx93-11x11-lpddr4x-evk source imx-setup-release.sh -b build-media
      # Build the image
      bitbake imx-image-multimedia
      # Build with the SDK and install it (optional but recommended)
      bitbake imx-image-multimedia -c populate_sdk
      ./tmp/deploy/sdk/fsl-imx-wayland-glibc-x86_64-imx-image-multimedia-armv8a-imx93-11x11-lpddr4x-evk-toolchain-6.12-styhead.sh
      ```

  -   A tutorial to get lvgl recipe setup on Yocto is provided in [LVGL official documentation - Yocto](https://docs.lvgl.io/master/details/integration/os/yocto/lvgl_recipe.html)

-   **Option 2**: download a pre-built image:
    The board comes supplied with an image on the EMMC. First we replicate this setup on the SD card:

    -   Download the pre-built images and binaries [here](https://www.nxp.com/design/design-center/software/embedded-software/i-mx-software/embedded-linux-for-i-mx-applications-processors:IMXLINUX). Choose the linux version and download the image for i.MX93 EVK

After downloading or building the image, flash it on the SD card:

```bash
zstdcat imx-image-multimedia-imx93-11x11-lpddr4x-evk.rootfs-20240918104911.wic.zst | sudo dd of=/dev/sdX bs=1M conv=fsync status=progress
```

**Replace /dev/sdX with the correct device (e.g., /dev/sdb). Use 'lsblk' to identify your SD card.**

Use the correct dtb. Mount the image and on the boot partition, use the dtb called "imx93-11x11-evk-boe-wxga-lvds-panel.dtb"

```bash
# cd into the boot partition of the SD card mounted and do the following command
cp imx93-11x11-evk-boe-wxga-lvds-panel.dtb imx93-11x11-evk.dtb
```

This modification can also be applied using the file manager.

### Build and run your application

#### Install utilities

```bash
sudo apt install picocom
```

#### Prepare the application environment

Clone the repository:

```bash
git clone --recurse-submodules https://github.com/lvgl/lv_port_linux.git
```

**IMPORTANT**: 

- The default application from lv_port_linux runs the widget demo. To run the benchmark demo, modify `main.c` : 

  ```c
  /*Create a Demo*/
  // lv_demo_widgets();
  // lv_demo_widgets_start_slideshow();
  lv_demo_benchmark();
  ```

- The default lvgl config allows you to build the application without the SDK. If you have have access to the SDK, we recommend using Wayland with G2D.

  ```bash
  cmake -B build -DCONFIG=wayland-g2d
  ```

#### Build using the SDK 

-   Source the SDK
    ```bash
    # Modify the path according to where you've installed the SDK
    source /opt/fsl-imx-wayland/6.12-styhead/environment-setup-armv8a-poky-linux
    ```

-   Build your application

    ```bash
    cmake -B build
    cmake --build build -j$(nproc)
    ```

#### Build using docker

Build the docker image and the lvgl benchmark application:

```bash
cd lv_port_linux
# Build the docker image
docker build --platform linux/arm64/v8 -t lvgl-build-arm64-image -f docker/Dockerfile_cross .
# Start the docker image
docker run -it --rm --platform linux/arm64/v8 -v $(pwd):/app lvgl-build-arm64-image

cmake -B build -DCONFIG=default
cmake --build build -j$(nproc)
```

#### Modifying the default config

Adjust `lv_conf.defaults` to select the drivers and libraries that will be compiled by
modifying the following definitions, setting them to `1` or `0`

You can also start with a default config based on the drivers you want to use,
you can find a default config for each graphic driver inside the configs folder.

You can replace `lv_conf.defaults` manually or use CMake to build another config:

```bash
cmake -B build -DCONFIG=<config_name> 
```

With `<config_name>` the name of the config without the `.defaults` extension, eg: `configs/wayland.defaults` becomes `wayland`.

### Run the project 

-   Get the IP of the target board:

```bash
sudo picocom -b 115200 /dev/ttyUSB0
## Then inside the console, log as "root", no password required
## Then retrieve the ip of the board
ip a
```

-   Then transfer the executable on the board:

    ```bash
    scp lv_port_linux/build/bin/lvglsim root@<BOARD_IP>:/root
    ```

-   Start the application

    ```bash
    ssh root@<BOARD_IP>
    ######################################
    ## WARNING: do not stop these services if using wayland demo
    systemctl stop weston.socket
    systemctl stop weston.service
    ######################################
    ./lvglsim
    ```

### Start with your own application

The folder `lv_port_linux` is an example of an application using LVGL.

LVGL is integrated as a submodule in the folder. To change the version of the library:

```bash
cd lvgl
git checkout <branch_name_or_commit_hash>
```

The file `main.c` is the default application provided and is configured to run the benchmark demo provided by LVGL library.

The main steps to create your own application are:

-   Modify `main.c`
-   Add any folders and files to extend the functionalities
-   Update `Dockerfile` to add any package
-   Modify `CMakeLists.txt` provided file to ensure all the required files are compiled and linked
-   Use the docker scripts provided to build the application for ARM64 architecture.

## TroubleShooting

### Output folder permissions

If there is any problem with the output folder generated permissions, modify the permissions:

```bash
sudo chown -R $(whoami):$(whoami) lv_port_linux/bin
```

### Fbdev example runtime error

This error can be printed when running the application:

```bash
[Warn]	(1382.767, +37)	 lv_display_refr_timer: No draw buffer lv_refr.c:374
[Warn]	(1382.804, +37)	 lv_display_refr_timer: No draw buffer lv_refr.c:374
[Warn]	(1382.841, +37)	 lv_display_refr_timer: No draw buffer lv_refr.c:374
[Warn]	(1382.878, +37)	 lv_display_refr_timer: No draw buffer lv_refr.c:374
```

To fix the issue find the existing fbdev available:

```bash
ls /dev/fb*
```

Export the variable to match the fbdev name:

```bash
export LV_LINUX_FBDEV_DEVICE=/dev/fb0
```

### Wayland example runtime error

While running the application, if there is an error about `XDG_RUNTIME_DIR`, add the following environment variable on the board.

```bash
export XDG_RUNTIME_DIR=/run/user/1000
```

### Changing configuration causes errors building the application

CMake may have troubles with CMakeLists.txt changes with some variables setup. If there is any problem building, try to clean the build folder:

```bash
rm -rf lv_port_linux/build
```

## Contribution and Support

If you find any issues with the development board feel free to open an Issue in this repository. For LVGL related issues (features, bugs, etc) please use the main [lvgl repository](https://github.com/lvgl/lvgl).

If you found a bug and found a solution too please send a Pull request. If you are new to Pull requests refer to [Our Guide](https://docs.lvgl.io/master/CONTRIBUTING.html#pull-request) to learn the basics.
