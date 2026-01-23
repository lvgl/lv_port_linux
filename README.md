# LVGL on top of Linux graphics stack

This is an example project demonstrating how to use LVGL on
a GNU/Linux systems and other Unix-like operating systems

LVGL provides drivers for many graphics backends.
Legacy framebuffer (fbdev), modern DRM/KMS, Wayland, X11, GLFW3 and SDL2.

Check out this blog post for a step by step tutorial for fbdev
https://blog.lvgl.io/2018-01-03/linux_fb

## Clone the project

Clone the project

```
git clone https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux/
```

LVGL is a submodule of `lv_port_linux`, use the following command
to fetch it, it will be downloaded to the `lvgl/` directory

```
git submodule update --init --recursive
```

## Configure drivers and libraries

By default `lv_conf.defaults` is by used to generate the configuration header
file `lv_conf.h` that is used during the build.
Adjust `lv_conf.defaults` to select the drivers and libraries that will be compiled by
modifying the following definitions, setting them to `1` or `0`

You can also start with a default config based on the drivers you want to use,
you can find a default config for each graphic driver inside the `configs/` directory.

You can either replace `lv_conf.defaults` manually before configuring:

```bash
mv configs/<config_name>.defaults lv_conf.defaults
cmake -B build
```

or have CMake directly use that file in-place during configuration:

```bash
cmake -B build -DCONFIG=<config_name> 
```

With `<config_name>` the name of the config without the `.defaults` extension,
eg: `-DCONFIG=wayland` will use `configs/wayland.defaults`.

### Graphics drivers

| Definition         | Description                             |
| ------------------ | ----------------------------------------|
| LV_USE_LINUX_FBDEV | Legacy frame buffer (/dev/fb*)          |
| LV_USE_LINUX_DRM   | DRM/KMS (/dev/dri/*)                    |
| LV_USE_SDL         | SDL                                     |
| LV_USE_WAYLAND     | WAYLAND                                 |
| LV_USE_X11         | X11                                     |
| LV_USE_GLFW        | GLFW3                                   |

### Device drivers

| Definition         | Description                             |
| ------------------ | ----------------------------------------|
| LV_USE_EVDEV       | libevdev input devices                  |
| LV_USE_LIBINPUT    | libinput input devices                  |

## Install dependencies

Install the required dependencies for the selected drivers by checking 
the documentation for each driver [here](https://docs.lvgl.io/master/integration/embedded_linux/drivers/)

You can also check the [Dockerfiles](docker/) to get the names
of the packages for various distributions

## Build instructions

LVGL supports GNU make and CMake

### CMake

```
cmake -B build
cmake --build build -j$(nproc)
```

Cross compilation is supported with CMake, edit the `user_cross_compile_setup.cmake`
to set the location of the compiler toolchain and build using the commands below

```
cmake -B build -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake 
cmake --build build -j$(nproc)
```

### Installing LVGL

It is possible to install LVGL to your system using cmake:

```
cmake --install ./build
```

## Run the demo application

```
./build/bin/lvglsim
```

This will start the widgets demo

If multiple backends are enabled you can run with a specific backend via the `-b` option

```
./build/bin/lvglsim -b sdl
```

To get a list of supported backends use the `-B` option

## Supported Boards

The `boards/` directory contains hardware-specific documentation and configuration files for running LVGL on various embedded Linux development boards.
Each board folder includes setup instructions, build configurations, and optimization guides tailored to that platform's specific hardware capabilities. Currently supported boards include:

- **F&S PicoCoreMX93** - NXP i.MX93-based system-on-module
- **GLYN UI2 Starter Kit** - GLYN starter kit with the Renesas RZ/G2L
- **NXP i.MX93** - NXP's i.MX93 evaluation platform
- **Renesas RZ/G3E EVKit** - Renesas RZ/G3E evaluation kit
- **Texas Instruments SK-AM62B-P1** - AM62B starter kit
- **Texas Instruments SK-AM62L** - AM62L starter kit
- **Texas Instruments SK-AM62P-LP** - AM62P low-power starter kit
- **Toradex Verdin AM62** - Toradex Verdin module with TI AM62 processor
- **Toradex Verdin iMX8M Mini** - Toradex Verdin module with NXP i.MX8M Mini processor

## Environment variables

Environment variables can be set to modify the behavior of the driver(s)
Check the documentation of the drivers for more details


### Legacy framebuffer (fbdev)

- `LV_LINUX_FBDEV_DEVICE` - override default (`/dev/fb0`) framebuffer device node.


### EVDEV touchscreen/mouse pointer device

- `LV_LINUX_EVDEV_POINTER_DEVICE` - the path of the input device, i.e.
  `/dev/input/by-id/my-mouse-or-touchscreen`. If not set, devices will
  be discovered and added automatically.

### DRM/KMS

- `LV_LINUX_DRM_CARD` - override default (`/dev/dri/card0`) card.

### Simulator

- `LV_SIM_WINDOW_WIDTH` - width of the window (default `800`).
- `LV_SIM_WINDOW_HEIGHT` - height of the window (default `480`).


## Permissions

By default, unprivileged users don't have access to the framebuffer device `/dev/fb0`. In such cases, you can either run the application
with `sudo` privileges or you can grant access to the `video` group.

```bash
sudo adduser $USER video
newgrp video
./build/bin/lvglsim
```
