# LVGL for frame buffer device

Example project to use LVGL with a Linux frame buffer device,
by default `/dev/fb0`.


Check out this blog post for a step by step tutorial:
https://blog.lvgl.io/2018-01-03/linux_fb

## Clone the project

Clone the LVGL Framebuffer Demo project and its related sub modules.

```
git clone https://github.com/lvgl/lv_port_linux_frame_buffer.git
cd lv_port_linux_frame_buffer/
git submodule update --init --recursive
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
