# LVGL on Linux

This is an example project that shows how to run [LVGL](https://github.com/lvgl/lvgl)
on GNU/Linux and other Unix-like systems. It builds a small simulator
application, `lvglsim`, that opens a window (or drives a display directly) and
runs LVGL's demos. It can also be used as a starting point to cross-compile LVGL to 
an embedded linux board.

LVGL ships drivers for the whole Linux graphics stack, and this port can build
against any of them:

| Backend            | Description                            |
| ------------------ | -------------------------------------- |
| `LV_USE_SDL`       | SDL2 (default) — a desktop window      |
| `LV_USE_WAYLAND`   | Wayland                                |
| `LV_USE_X11`       | X11                                    |
| `LV_USE_GLFW`      | GLFW3 (OpenGL)                         |
| `LV_USE_LINUX_DRM` | DRM/KMS (`/dev/dri/*`)                 |
| `LV_USE_LINUX_FBDEV` | Legacy framebuffer (`/dev/fb*`)      |

Configuration is driven by **Kconfig**, the same system used by the Linux
kernel, so you can pick drivers, features and demos from a menu instead of
editing a header by hand.

## Get started

### Prerequisites

CMake, Python 3, and python3-venv are required regardless of configuration.
Ninja is optional but recommended to speed-up building

Install all system dependencies in one go:

```bash
# Debian / Ubuntu
sudo apt install \
  build-essential cmake python3 python3-venv ninja-build \
  libsdl2-dev \
  libwayland-dev libxkbcommon-dev wayland-protocols \
  libx11-dev \
  libdrm-dev libgbm-dev \
  libevdev-dev \
  libwebp-dev \
  libegl-dev libgles-dev libgl-dev
```

```bash
# Arch
sudo pacman -S \
    git cmake python ninja \
    sdl2 \
    wayland wayland-protocols libxkbcommon \
    libx11 \
    libdrm \
    libevdev \
    libwebp \
    mesa
```

```bash
# Fedora
sudo dnf install \
  @development-tools gcc-c++ cmake python3 python3-devel ninja-build \
  SDL2-devel \
  wayland-devel wayland-protocols-devel libxkbcommon-devel \
  libX11-devel \
  libdrm-devel \
  libevdev-devel \
  libwebp-devel \
  mesa-libEGL-devel mesa-libGLES-devel mesa-libGL-devel mesa-libgbm-devel
```

Libraries that can be fetched from source (FreeType, libpng, libjpeg-turbo, ...) are built automatically by CMake if not found on the system.

FFmpeg and GStreamer must be installed manually if you enable them:

```bash
# Debian / Ubuntu
sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
# Arch
sudo pacman -S ffmpeg gstreamer gst-plugins-base
# Fedora
sudo dnf install ffmpeg-devel gstreamer1-devel gstreamer1-plugins-base-devel
```

Install the system packages with your distribution's package manager, then
install the Python tools in a virtual environment (recommended so they don't
pollute your system Python):

```bash
python3 -m venv env
source env/bin/activate
pip install kconfiglib pcpp
```

### Clone

LVGL is included as a git submodule, so clone recursively:

```bash
git clone --recurse-submodules https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux
```

### Build and run

```bash
cmake -B build -GNinja
cmake --build build
./build/bin/lvglsim
```

## Configuring LVGL

Configuration lives in a `.config` file at the repository root. When it exists,
the build uses it; otherwise it falls back to `configs/get_started.defconfig`.

The Kconfig tools (`defconfig`, `menuconfig`) come from `kconfiglib` and must be
run from the repository root, where the top-level `Kconfig` lives.

### Start from a defconfig

Generate a `.config` from a defconfig:

```bash
defconfig configs/get_started.defconfig
```

Besides the port's own `get_started.defconfig`, LVGL ships several ready-made
defconfigs in [`lvgl/configs/defconfigs/`](https://github.com/lvgl/lvgl/tree/master/configs/defconfigs).

Apply one the same way, e.g.:

```bash
defconfig lvgl/configs/defconfigs/wayland.defconfig
```

### Tweak with menuconfig

Once you have a `.config`, open the interactive menu to enable/disable drivers,
features, fonts and demos:

```bash
menuconfig
```

Save and exit, then rebuild:

```bash
cmake -B build
cmake --build build -j$(nproc)
```

## Backends

This port supports these Linux display and input backend that LVGL provides.
A backend is compiled in when its `CONFIG_LV_USE_*` option is enabled (through
Kconfig, as above). The glue code for each one lives in:

- Displays: [`src/lib/display_backends`](src/lib/display_backends) —
  `sdl`, `wayland`, `x11`, `glfw3`, `drm`, `fbdev`
- Input devices: [`src/lib/indev_backends`](src/lib/indev_backends) — `evdev`

If several backends are enabled at once, pick one at runtime with `-b`:

```bash
./build/bin/lvglsim -b sdl
```

List the backends compiled into your binary with `-B`. Run `./build/bin/lvglsim -h`
for all options (fullscreen, window size, rotation, …).

## 3D Rendering & glTF Support

LVGL can load and render 3D scenes (such as `.gltf` and `.glb` files)
when using hardware-accelerated backends:

```bash
defconfig configs/get_started_3d.defconfig
cmake -B build -GNinja
cmake --build build
./build/bin/lvglsim
```

## Cross compilation

Cross compilation is supported with CMake. Edit
`cmake/user_cross_compile_setup.cmake` to point at your toolchain, then:

```bash
cmake -B build -GNinja -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake
cmake --build build
```

## Installing LVGL

You can install the built LVGL library and headers to your system:

```bash
cmake --install ./build
```

## Boards

The `boards/` directory contains hardware-specific documentation and
configuration files for running LVGL on officially supported embedded Linux development
boards. Currently supported boards:

- **F&S PicoCoreMX93**: NXP i.MX93-based system-on-module
- **GLYN UI2 Starter Kit**: GLYN starter kit with the Renesas RZ/G2L
- **NXP i.MX93**: NXP's i.MX93 evaluation platform
- **Renesas RZ/G3E EVKit**: Renesas RZ/G3E evaluation kit
- **Texas Instruments SK-AM62B-P1**: AM62B starter kit
- **Texas Instruments SK-AM62L**: AM62L starter kit
- **Texas Instruments SK-AM62P-LP**: AM62P low-power starter kit
- **Toradex Verdin AM62**: Toradex Verdin module with TI AM62 processor
- **Toradex Verdin iMX8M Mini**: Toradex Verdin module with NXP i.MX8M Mini processor
- **STM32MP157F-DK2**: STMicroelectronics MP1 processor devkit with touchscreen
- **STM32MP257F-DK**: STMicroelectronics MP2 processor devkit with 3D GPU and NPU

## Environment variables

Environment variables can be set to modify the behavior of the simulator.
Check the [driver documentation](https://docs.lvgl.io/master/integration/embedded_linux/drivers/)
for more details.

Simply export them on your current shell instance before launching the compiled application to see them take effect:

```bash
export LV_LINUX_FBDEV_DEVICE="/dev/fb1"
./build/bin/lvglsim
```

### Simulator

- `LV_SIM_WINDOW_WIDTH` — width of the window (default `800`).
- `LV_SIM_WINDOW_HEIGHT` — height of the window (default `480`).

### Legacy framebuffer (fbdev)

- `LV_LINUX_FBDEV_DEVICE` — override the default (`/dev/fb0`) framebuffer device node.

### EVDEV touchscreen / mouse pointer device

- `LV_LINUX_EVDEV_POINTER_DEVICE` — the path of the input device, e.g.
  `/dev/input/by-id/my-mouse-or-touchscreen`. If not set, devices are
  discovered and added automatically.

### DRM/KMS

- `LV_LINUX_DRM_CARD` — override the default (`/dev/dri/card0`) card.

## Permissions

Some devices require special permissions. We recommend adding your user to the appropriate group to avoid running the application with `sudo`.

Access requirements depend on which drivers you choose to use:

- `video` group: Grants access to the framebuffer (`/dev/fb0`) if using `fbdev`.
- `input` group: Grants access to event devices (`/dev/input/*`) if using `evdev`.

If you are using either driver, add your user to the corresponding group(s):

```bash
# Add your user to the relevant group(s)
sudo adduser $USER video
sudo adduser $USER input

# Apply group changes to the current terminal session
newgrp video
./build/bin/lvglsim
```

Note: If your display/input setup does not rely on `fbdev` or `evdev` (for example, if you are using SDL, X11, or Wayland), you can safely skip these group permission steps.
