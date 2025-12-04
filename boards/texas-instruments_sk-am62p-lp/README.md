# LVGL ported to AM62P-LP (Texas Instruments)

**:rocket: This repository is available in the [LVGL Project Creator](https://lvgl.io/tools/project-creator), making it easy to create and customize a new project in just a few clicks.**

## Overview

This guide provides steps to setup the SK-AM62P-LP starter kit and to cross-compile an LVGL application to run it on the target.

## Buy

You can purchase the AM62P-LP board from [Texas Instruments website](https://www.ti.com/tool/SK-AM62P-LP). Check out the product specification in the [product folder](https://www.ti.com/product/AM62P)!


## Benchmark

The default buffering is fbdev.


Check out SK-AM62P-LP in action, running LVGL's benchmark demo:

<a href="https://www.youtube.com/watch?v=IAalHCDVPQw">
    <img src="https://github.com/user-attachments/assets/1fb9877c-1993-4b85-a92f-ad6822c8a247" width="75%" alt="image">
    
</a>


**Frame buffer, 1 thread**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| ------------------------- | -------- | -------- | --------- | ----------- | ---------- |
| Empty screen              | 17.00%   | 24       | 6         | 1           | 5          |
| Moving wallpaper          | 83.00%   | 16       | 55        | 50          | 5          |
| Single rectangle          | 12.00%   | 27       | 0         | 0           | 0          |
| Multiple rectangles       | 13.00%   | 27       | 4         | 2           | 2          |
| Multiple RGB images       | 53.00%   | 26       | 20        | 15          | 5          |
| Multiple ARGB images      | 43.00%   | 27       | 14        | 9           | 5          |
| Rotated ARGB images       | 84.00%   | 26       | 30        | 26          | 4          |
| Multiple labels           | 47.00%   | 27       | 16        | 11          | 5          |
| Screen sized text         | 87.00%   | 22       | 36        | 32          | 4          |
| Multiple arcs             | 52.00%   | 28       | 15        | 11          | 4          |
| Containers                | 33.00%   | 27       | 9         | 9           | 0          |
| Containers with overlay   | 86.00%   | 17       | 51        | 46          | 5          |
| Containers with opa       | 67.00%   | 26       | 23        | 22          | 1          |
| Containers with opa_layer | 88.00%   | 18       | 46        | 42          | 4          |
| Containers with scrolling | 71.00%   | 29       | 23        | 18          | 5          |
| Widgets demo              | 28.00%   | 26       | 9         | 8           | 1          |
| All scenes avg.           | 54.00%   | 24       | 21        | 18          | 3          |

**Frame buffer, 4 threads**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| ------------------------- | -------- | -------- | --------- | ----------- | ---------- |
| Empty screen              | 17.00%   | 24       | 6         | 1           | 5          |
| Moving wallpaper          | 52.00%   | 28       | 19        | 14          | 5          |
| Single rectangle          | 7.00%    | 27       | 0         | 0           | 0          |
| Multiple rectangles       | 15.00%   | 26       | 5         | 3           | 2          |
| Multiple RGB images       | 33.00%   | 28       | 11        | 6           | 5          |
| Multiple ARGB images      | 29.00%   | 27       | 9         | 4           | 5          |
| Rotated ARGB images       | 41.00%   | 27       | 14        | 9           | 5          |
| Multiple labels           | 30.00%   | 27       | 9         | 4           | 5          |
| Screen sized text         | 47.00%   | 27       | 16        | 11          | 5          |
| Multiple arcs             | 33.00%   | 27       | 9         | 4           | 5          |
| Containers                | 23.00%   | 28       | 6         | 5           | 1          |
| Containers with overlay   | 68.00%   | 27       | 24        | 19          | 5          |
| Containers with opa       | 44.00%   | 28       | 12        | 11          | 1          |
| Containers with opa_layer | 59.00%   | 28       | 19        | 18          | 1          |
| Containers with scrolling | 39.00%   | 29       | 12        | 7           | 5          |
| Widgets demo              | 20.00%   | 27       | 6         | 5           | 1          |
| All scenes avg.           | 34.00%   | 27       | 10        | 7           | 3          |

The other configurations are:

-   DRM
-   Wayland
-   SDL

Any of these buffering strategies can be used with multiple threads to render the frames.

## Specification

### CPU and memory

-   **MCU**: AM625P with Quad 64-bit Arm Cortex-A53 up to 1.4GHz, two ARM Cortex R5F single core up to 800MHz
-   **RAM**: 8GB LPDDR4

    -   32-bits data bus with inline ECC
    -   Supports speeds up to 3200 MT/s

-   **Flash**: 32GB SD
-   **GPU**: PowerVR

### Display

-   Screen: HDMI 1920x1080 touchscreen

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

This [document](https://dev.ti.com/tirex/content/tirex-product-tree/am62px-devtools/docs/am62px_skevm_quick_start_guide.html) from Texas Instruments provides detailed information for the hardware setup

-   Connect to the board the following:

    -   UART
    -   Power
    -   Screen (HDMI)
    -   Ethernet (Connect the board to the same LAN the host is, the board obtains an IP address from the network manager)

- SD card is needed to flash the image.

  -   Follow the [guide](https://dev.ti.com/tirex/content/tirex-product-tree/am62px-devtools/docs/am62px_skevm_quick_start_guide.html) to download a pre-built `.wic` image

  -   Follow this [guide](https://software-dl.ti.com/processor-sdk-linux-rt/esd/AM62PX/09_01_00_08/exports/docs/linux/Overview_Building_the_SDK.html) to build the image with Yocto. 
      -   A tutorial to get lvgl recipe setup on Yocto is provided in [LVGL official documentation - Yocto](https://docs.lvgl.io/master/details/integration/os/yocto/lvgl_recipe.html)

-   If there are problems encountered flashing the SD card with BalenaEtcher as mentioned in the documentation, use this command instead:

    ```bash
    # Mount the SD on your system and find where it was mounter (e.g.: sda, sdb)
    sudo dd if=path/to/am62p-image.wic of=/dev/sdX bs=4M status=progress conv=fsync
    ```

-   Insert the SD card in the slot and power up the board. The ti-app launcher should display an UI on the screen.

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
