# LVGL on top of Linux graphics stack

Example project to use LVGL on top of Linux graphics stack.
Currently supported backends are either legacy framebuffer
(fbdev) or modern DRM/KMS.

By default, legacy framebuffer backend uses `/dev/fb0` device node,
DRM/KMS backend uses '/dev/dri/card0' card node.

Check out this blog post for a step by step tutorial:
https://blog.lvgl.io/2018-01-03/linux_fb

## Clone the project

Clone the LVGL Framebuffer Demo project and its related sub modules.

```
git clone https://github.com/lvgl/lv_port_linux_frame_buffer.git
cd lv_port_linux_frame_buffer/
git submodule update --init --recursive
```

## Select graphics backend (optional)

To use legacy framebuffer (fbdev) support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	1
#define LV_USE_LINUX_DRM	0
```

To use modern DRM/KMS support, adjust `lv_conf.h` as follows:
```
#define LV_USE_LINUX_FBDEV	0
#define LV_USE_LINUX_DRM	1
```

## Build the project

```
mkdir build
cd build 
cmake ..
make -j
```

## Environment variables

Environment variables can be set to modify behavior of the demo.
The following variables are supported.

### Legacy framebuffer (fbdev)

- `LV_LINUX_FBDEV_DEVICE` - override default (`/dev/fb0`) framebuffer device node.

### DRM/KMS

- `LV_LINUX_DRM_CARD` - override default (`/dev/dri/card0`) card.

## Run the demo application

### As root

Normal users don't have access to `/dev/fb0` so use `sudo` (or see below) : 

```
cd ../bin
sudo main
```

### Userland

You can give a normal user access to the framebuffer by adding them to the `video` group : 

```
sudo adduser $USER video
newgrp video
cd ../bin
./main
```

## Notes

- The Makefile is broken right now, so Cmake shall be used.
