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

Adjust `lv_conf.h` to select the drivers and libraries that will be compiled by
modifying the following definitions, setting them to `1` or `0`

### Graphics drivers

| Definition         | Description                             |
| ------------------ | ----------------------------------------|
| LV_USE_LINUX_FBDEV | Legacy frame buffer (/dev/fb*)          |
| LV_USE_LINUX_DRM   | DRM/KMS (/dev/dri/*)                    |
| LV_USE_SDL         | SDL                                     |
| LV_USE_WAYLAND     | WAYLAND                                 |
| LV_USE_X11         | X11                                     |
| LV_USE_OPENGLES    | GLFW3                                   |

### Device drivers

| Definition         | Description                             |
| ------------------ | ----------------------------------------|
| LV_USE_EVDEV       | libevdev input devices                  |

## Install dependencies

Be sure to install the required dependencies for the selected drivers by checking
the documentation for each driver here:
https://docs.lvgl.io/master/details/integration/driver/

You can also check the [Dockerfiles](docker/) to get the names
of the packages for various distributions

## Build instructions

LVGL supports GNU make and CMake

### CMake

```
cmake -B build -S .
make -C build -j
```

### GNU make

```
make -j
```

Cross compilation is supported with CMake, edit the `user_cross_compile_setup.cmake`
to set the location of the compiler toolchain and build using the commands below

```
cmake -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake -B build -S .
make  -C build -j
```

### Installing LVGL

It is possible to install LVGL to your system however, this is currently only
supported with cmake.

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

When using fbdev or DRM, run lvglsim with `sudo` or `su`,
Usually, unpriviledged users don't have access to the framebuffer device `/dev/fb0`
`sudo` or `su` must be used.

Access to the framebuffer device can be granted by adding the unpriviledged user to the `video` group

```
sudo adduser $USER video
newgrp video
./build/bin/lvglsim
```
