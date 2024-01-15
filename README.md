# LVGL for frame buffer device

Example project to use LVGL with a Linux frame buffer device, for example `/dev/fb0`.


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

## Run the demo application

```
cd ../bin
sudo main
```

## Notes

- The Makefile is broken right now, so Cmake shall be used.
