# LVGL ported to TMDS62LEVM (Texas Instruments)

**:rocket: This repository is available in the [LVGL Project Creator](https://lvgl.io/tools/project-creator), making it easy to create and customize a new project in just a few clicks.**

## Overview

This guide provides steps to setup the TMDS62LEVM and to cross-compile an LVGL application to run it on the target.

## Buy

You can purchase an TMDS62LEVM from https://www.ti.com/tool/TMDS62LEVM

## Benchmark

The default buffering is fbdev.

Check out TMDS62LEVM in action, running LVGL's benchmark demo:

<a href="https://www.youtube.com/watch?v=F9qKCQL7aJg">
    <img src="https://github.com/user-attachments/assets/47b767bf-2372-44ed-9a30-fa46dbd8b088" width="75%" alt="image">
</a>

**Frame buffer, 32 bit color depth, 1 thread**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              | 40%      | 26       | 14        | 4           | 10         |
| Moving wallpaper          | 70%      | 29       | 25        | 15          | 10         |
| Single rectangle          | 17%      | 29       | 0         | 0           | 0          |
| Multiple rectangles       | 23%      | 29       | 7         | 2           | 5          |
| Multiple RGB images       | 63%      | 30       | 19        | 9           | 10         |
| Multiple ARGB images      | 81%      | 29       | 23        | 13          | 10         |
| Rotated ARGB images       | 98%      | 24       | 39        | 29          | 10         |
| Multiple labels           | 99%      | 29       | 28        | 18          | 10         |
| Screen sized text         | 96%      | 20       | 47        | 37          | 10         |
| Multiple arcs             | 93%      | 29       | 26        | 16          | 10         |
| Containers                | 90%      | 23       | 33        | 27          | 6          |
| Containers with overlay   | 99%      | 12       | 71        | 61          | 10         |
| Containers with opa       | 99%      | 13       | 58        | 49          | 9          |
| Containers with opa_layer | 99%      | 9        | 87        | 77          | 10         |
| Containers with scrolling | 99%      | 14       | 58        | 49          | 9          |
| Widgets demo              | 31%      | 25       | 15        | 13          | 2          |
| All scenes avg.           | 74%      | 23       | 34        | 26          | 8          |

**Frame buffer, 32 bit color depth, 2 threads**

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              | 39%      | 26       | 14        | 4           | 10         |
| Moving wallpaper          | 70%      | 30       | 24        | 14          | 10         |
| Single rectangle          | 15%      | 29       | 0         | 0           | 0          |
| Multiple rectangles       | 25%      | 29       | 8         | 3           | 5          |
| Multiple RGB images       | 63%      | 29       | 19        | 9           | 10         |
| Multiple ARGB images      | 80%      | 29       | 23        | 13          | 10         |
| Rotated ARGB images       | 85%      | 29       | 28        | 18          | 10         |
| Multiple labels           | 90%      | 29       | 24        | 14          | 10         |
| Screen sized text         | 94%      | 28       | 32        | 22          | 10         |
| Multiple arcs             | 89%      | 29       | 25        | 15          | 10         |
| Containers                | 81%      | 26       | 24        | 21          | 3          |
| Containers with overlay   | 99%      | 13       | 63        | 53          | 10         |
| Containers with opa       | 99%      | 15       | 52        | 42          | 10         |
| Containers with opa_layer | 99%      | 9        | 90        | 80          | 10         |
| Containers with scrolling | 99%      | 16       | 53        | 43          | 10         |
| Widgets demo              | 32%      | 25       | 15        | 13          | 2          |
| All scenes avg.           | 72%      | 24       | 30        | 22          | 8          |

The other configurations are:

-   DRM
-   Wayland

Any of these buffering strategies can be used with multiple threads to render the frames.

## Specification

### CPU and memory

-   **MCU**:

    -   AM62Lx with Dual 64-bit Arm Cortex-A53 up to 1.25GHz

-   **RAM**: 2GB DDR4

    -   16-bits data bus
    -   Supports speeds up to 1600 MT/s

-   **Flash**: Micro SD Card
-   **GPU**: **No GPU**

### Display

-   **Screen**: HDMI 1920x1080 @ 60fps touchscreen

### Connectivity

-   1 Type-A USB 2.0
-   1 Type-C dual-role device (DRD) USB 2.0 supports USB booting
-   UART
-   USB
-   Onboard XDS110 Joint Test Action Group (JTAG) emulator
-   4 universal asynchronous receiver-transmitters (UARTs) via USB 2.0-B
-   Ethernet
-   Headphone jack

## Getting started

### Hardware setup

Coming soon.

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
