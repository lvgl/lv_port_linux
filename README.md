# LVGL on top of Linux graphics stack

Example project to use LVGL on top of Linux graphics stack.
Currently supported backends are legacy framebuffer
(fbdev), modern DRM/KMS, Wayland or SDL2.

By default, legacy framebuffer backend uses `/dev/fb0` device node,
DRM/KMS backend uses '/dev/dri/card0' card node, SDL2 uses window
resolution of 800x480.

Check out this blog post for a step by step tutorial:
https://blog.lvgl.io/2018-01-03/linux_fb

## Build dependencies
Check the [Dockerfiles](docker/) for the build dependencies.  

## Clone the project

Clone the LVGL Framebuffer Demo project and its related sub modules.

```
git clone https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux/
git submodule update --init --recursive
```

## Select graphics backend

To use legacy framebuffer (fbdev) support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	1
#define LV_USE_LINUX_DRM	0
#define LV_USE_SDL		0
#define LV_USE_WAYLAND		0
```

To use modern DRM/KMS support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	0
#define LV_USE_LINUX_DRM	1
#define LV_USE_SDL		0
#define LV_USE_WAYLAND		0
```

To use SDL2 support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	0
#define LV_USE_LINUX_DRM	0
#define LV_USE_SDL		1
#define LV_USE_WAYLAND		0
```


To use wayland, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	0
#define LV_USE_LINUX_DRM	0
#define LV_USE_SDL		0
#define LV_USE_WAYLAND		1
```

### cmake

1. gcc compiler:

```
cmake -B build -S .
make -C build -j
```
2. Cross compiler:

```
vim ./user_cross_compile_setup.cmake
cmake -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake -B build -S .
make  -C build -j
```

### Makefile

```
make -j
```

## Command line options

Command line options are used to modify behavior of the demo, they take precedence over environment variables.

### Wayland

- `-f` - enters fullscreen on startup
- `-m` - maximizes window on startup
- `-w <window width>` - set the width of the window
- `-h <window height>` - set the height of the window

### SDL2

- `-w <window width>` - set the width of the window
- `-h <window height>` - set the height of the window

## Environment variables

Environment variables can be set to modify the behavior of the demo.

### Legacy framebuffer (fbdev)

- `LV_LINUX_FBDEV_DEVICE` - override default (`/dev/fb0`) framebuffer device node.


### EVDEV touchscreen/mouse pointer device

- `LV_LINUX_EVDEV_POINTER_DEVICE` - override default (`/dev/input/by-id/my-mouse-or-touchscreen`) input device

### DRM/KMS

- `LV_LINUX_DRM_CARD` - override default (`/dev/dri/card0`) card.

### SDL2

- `LV_SIM_WINDOW_WIDTH` - width of SDL2 surface (default `800`).
- `LV_SIM_WINDOW_HEIGHT` - height of SDL2 surface (default `480`).

### Wayland

- `LV_SIM_WINDOW_WIDTH` - width of the window (default `800`).
- `LV_SIM_WINDOW_HEIGHT` - height of the window (default `480`).


## Run the demo application

### FBDEV

Unpriviledged users don't have access to the framebuffer device `/dev/fb0`
`sudo` or `su` must be used.

cmake:
```
sudo ./bin/lvglsim
```

Makefile:
```
cd build/bin/
sudo main
```

Access to the framebuffer device can be granted by adding the unpriviledged user to the `video` group

cmake:
```
sudo adduser $USER video
newgrp video
./bin/lvglsim
```

