# LVGL ported to AM62B-P1 (Texas Instruments)

**:rocket: This repository is available in the [LVGL Project Creator](https://lvgl.io/tools/project-creator), making it easy to create and customize a new project in just a few clicks.**


## Overview

This guide provides steps to setup the SK-AM62B-P1 and to cross-compile an LVGL application to run it on the target.

## Buy

You can purchase the SK-AM62B-P1 board from [Texas Instruments website](https://www.ti.com/tool/SK-AM62B-P1). Check out the product specification in the [product folder](https://www.ti.com/product/AM625)!


## Benchmark

The default buffering is fbdev.

Check out SK-AM62B-P1 in action, running LVGL's benchmark demo:

<a href="https://www.youtube.com/watch?v=fgpnzjSEyWM">
    <img src="https://github.com/user-attachments/assets/bcb85b05-bb05-42b7-aa9b-467c786e8e4d" width="75%" alt="image">
</a>


**Frame buffer, 1 thread**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------- | -------- | --------- | ----------- | ---------- |
| Empty screen              | 37.00%   | 24       | 14        | 5           | 9          |
| Moving wallpaper          | 86.00%   | 15       | 57        | 48          | 9          |
| Single rectangle          | 14.00%   | 28       | 1         | 1           | 0          |
| Multiple rectangles       | 35.00%   | 28       | 10        | 6           | 4          |
| Multiple RGB images       | 79.00%   | 29       | 27        | 18          | 9          |
| Multiple ARGB images      | 70.00%   | 29       | 22        | 13          | 9          |
| Rotated ARGB images       | 85.00%   | 23       | 36        | 27          | 9          |
| Multiple labels           | 73.00%   | 27       | 24        | 15          | 9          |
| Screen sized text         | 88.00%   | 18       | 48        | 39          | 9          |
| Multiple arcs             | 75.00%   | 28       | 24        | 15          | 9          |
| Containers                | 44.00%   | 27       | 12        | 10          | 2          |
| Containers with overlay   | 88.00%   | 15       | 57        | 48          | 9          |
| Containers with opa       | 72.00%   | 22       | 31        | 26          | 5          |
| Containers with opa_layer | 91.00%   | 15       | 58        | 49          | 9          |
| Containers with scrolling | 86.00%   | 26       | 31        | 22          | 9          |
| Widgets demo              | 30.00%   | 24       | 14        | 11          | 3          |
| All scenes avg.           | 65.00%   | 23       | 29        | 22          | 7          |

**Frame buffer, 4 threads**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| ------------------------- | -------- | -------- | --------- | ----------- | ---------- |
| Empty screen              | 37.00%   | 24       | 14        | 5           | 9          |
| Moving wallpaper          | 80.00%   | 28       | 28        | 19          | 9          |
| Single rectangle          | 11.00%   | 28       | 1         | 1           | 0          |
| Multiple rectangles       | 37.00%   | 29       | 12        | 8           | 4          |
| Multiple RGB images       | 54.00%   | 27       | 19        | 10          | 9          |
| Multiple ARGB images      | 56.00%   | 28       | 18        | 9           | 9          |
| Rotated ARGB images       | 66.00%   | 29       | 21        | 12          | 9          |
| Multiple labels           | 56.00%   | 28       | 17        | 8           | 9          |
| Screen sized text         | 75.00%   | 27       | 25        | 16          | 9          |
| Multiple arcs             | 56.00%   | 28       | 17        | 8           | 9          |
| Containers                | 36.00%   | 27       | 10        | 8           | 2          |
| Containers with overlay   | 83.00%   | 22       | 36        | 27          | 9          |
| Containers with opa       | 57.00%   | 28       | 16        | 14          | 2          |
| Containers with opa_layer | 77.00%   | 24       | 31        | 26          | 5          |
| Containers with scrolling | 71.00%   | 28       | 23        | 14          | 9          |
| Widgets demo              | 29.00%   | 26       | 11        | 8           | 3          |
| All scenes avg.           | 55.00%   | 26       | 18        | 12          | 6          |

The other configurations are:

-   DRM
-   Wayland

Any of these buffering strategies can be used with multiple threads to render the frames.

## Specification

### CPU and memory

-   **MCU**:

    -   AM625 with Quad 64-bit Arm Cortex-A53 up to 1.4GHz
    -   1 Arm Cortex-M4F

-   **RAM**: 2GB DDR4

    -   16-bits data bus with inline ECC
    -   Supports speeds up to 1600 MT/s

-   **Flash**: 32GB SD
-   **GPU**: PowerVR

### Display

-   **Screen**: HDMI 1920x1080 touchscreen

### Connectivity

-   1 Type-A USB 2.0
-   1 Type-C dual-role device (DRD) USB 2.0 supports USB booting
-   UART
-   USB
-   Onboard XDS110 Joint Test Action Group (JTAG) emulator
-   4 universal asynchronous receiver-transmitters (UARTs) via USB 2.0-B
-   Ethernet

## Getting started

### Hardware setup

This [document](https://dev.ti.com/tirex/content/tirex-product-tree/processors-devtools_1_02_01/am62x_skevm_quick_start_guide.html) from Texas Instruments provides detailed information for the hardware setup

-   Connect to the board the following:

    -   UART
    -   Power
    -   Screen (HDMI)
    -   Ethernet (Connect the board to the same LAN the host is, the board obtains an IP address from DHCP)

-   SD card is needed to flash the image.

    -   Follow the [guide](https://dev.ti.com/tirex/content/tirex-product-tree/processors-devtools_1_02_01/am62x_skevm_quick_start_guide.html) to download a pre-built `.wic` image

    -   Follow this [guide](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/linux/Overview_Building_the_SDK.html) to build the image with Yocto
        -   A tutorial to get lvgl recipe setup on Yocto is provided in [LVGL official documentation - Yocto](https://docs.lvgl.io/master/details/integration/os/yocto/lvgl_recipe.html)

-   If there are problems encountered flashing the SD card with BalenaEtcher as mentioned in the documentation, use this command instead:

    ```bash
    # Mount the SD on your system and find where it is mounted (e.g.: sda, sdb)
    sudo dd if=path/to/am62-image.wic of=/dev/sdX bs=4M status=progress conv=fsync
    ```

-   The demo application should be displayed on the screen.

### Software setup

This guide was tested on Ubuntu 22.04 host.

#### Install docker

-   Follow this [tutorial](https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-22-04) to install and setup docker on your system.

-   Support to run arm64 docker containers on the host:

    ```bash
    sudo apt-get install qemu-user-static
    docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
    ```

#### Install utilities

```bash
sudo apt install picocom nmap
```

### Build the default project

Clone the repository: 

```bash
git clone --recurse-submodules https://github.com/lvgl/lv_port_linux.git
```

**IMPORTANT**: 

- default application from lv_port_linux runs the widget demo. To run the benchmark demo, modify `lv_port_linux/main.c` : 

```c
/*Create a Demo*/
// lv_demo_widgets();
// lv_demo_widgets_start_slideshow();
lv_demo_benchmark();
```

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

    -   <u>Option 1</u>: from the UART, on the board:

        ```bash
        sudo picocom -b 115200 /dev/ttyUSB0
        ## Then inside the console, log as "root", no password required
        ## Then retrieve the ip of the board
        ip a
        ```

    -   <u>Option 2</u>: Get the IP from your host with nmap

        ```bash
        ## Find the IP of the board. You need to know your ip (ifconfig or ip a)
        ## HOST_IP should be built like this :
        ## If the ip is 192.168.1.86, in the following command HOST_IP = 192.168.1.0/24
        nmap -sn <HOST_IP>/24 | grep am62
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
