# LVGL on top of Linux graphics stack

Example project to use LVGL on top of Linux graphics stack.
Currently supported backends are either legacy framebuffer
(fbdev), modern DRM/KMS, or SDL2.

By default, legacy framebuffer backend uses `/dev/fb0` device node,
DRM/KMS backend uses '/dev/dri/card0' card node, SDL2 uses window
resolution of 800x480.

Check out this blog post for a step by step tutorial:
https://blog.lvgl.io/2018-01-03/linux_fb

## Clone the project

Clone the LVGL Framebuffer Demo project and its related sub modules.

```
git clone https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux/
git submodule update --init --recursive
```

## Select graphics backend (optional)

To use legacy framebuffer (fbdev) support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	1
#define LV_USE_LINUX_DRM	0
#define LV_USE_SDL		0
```

To use modern DRM/KMS support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	0
#define LV_USE_LINUX_DRM	1
#define LV_USE_SDL		0
```

To use SDL2 support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	0
#define LV_USE_LINUX_DRM	0
#define LV_USE_SDL		1
```

## Build the project (cmake or Makefile)

### cmake

```
mkdir build
cd build 
cmake ..
make -j
```

### Makefile

```
make -j
```

## Environment variables

Environment variables can be set to modify behavior of the demo.
The following variables are supported.

### Legacy framebuffer (fbdev)

- `LV_LINUX_FBDEV_DEVICE` - override default (`/dev/fb0`) framebuffer device node.

### DRM/KMS

- `LV_LINUX_DRM_CARD` - override default (`/dev/dri/card0`) card.

### SDL2

- `LV_SDL_VIDEO_WIDTH` - width of SDL2 surface (default `800`).
- `LV_SDL_VIDEO_HEIGHT` - height of SDL2 surface (default `480`).

## Run the demo application

### As root

Normal users don't have access to `/dev/fb0` so use `sudo` (or see below) : 

cmake:
```
cd ../bin
sudo main
```

Makefile:
```
cd build/bin/
sudo main
```

### Userland

You can give a normal user access to the framebuffer by adding them to the `video` group : 


cmake:
```
sudo adduser $USER video
newgrp video
cd ../bin
./main
```

Makefile:
```
sudo adduser $USER video
newgrp video
cd build/bin/
./main
```
