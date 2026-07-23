# LVGL on Linux

This is an example project (a "port") that shows how to run [LVGL](https://github.com/lvgl/lvgl)
on GNU/Linux and other Unix-like systems. It builds a small simulator
application, `lvglsim`, that opens a window (or drives a display directly) and
runs LVGL's demos.

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

You will need:

- `git`
- a C and a C++ compiler (e.g. GCC or Clang)
- `cmake` (3.11 or newer)
- `python3` and `python3-venv`
- [`kconfiglib`](https://pypi.org/project/kconfiglib/) — provides the
  `menuconfig`/`defconfig` tools
- [`pcpp`](https://pypi.org/project/pcpp/) — recommended; a C pre-processor
  used to generate the LVGL configuration

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

If you already cloned without `--recurse-submodules`, fetch it with:

```bash
git submodule update --init --recursive
```

### Build and run

```bash
cmake -B build
cmake --build build -j$(nproc)
./build/bin/lvglsim
```

A window should open running an LVGL demo

### What just happened?

- Because no `.config` file existed yet, the build used the default
  configuration in [`configs/get_started.defconfig`](configs/get_started.defconfig).
  It enables the **SDL2** backend along with a set of demos.
- LVGL first tries to find SDL2 on your system. If it isn't installed,
  LVGL **fetches and builds SDL2 from source** automatically, no action needed,
  but the first build takes longer.
  To use the system copy instead (faster builds), install SDL2 with your
  package manager, e.g. `sudo dnf install SDL2-devel` (Fedora) or
  `sudo apt install libsdl2-dev` (Debian/Ubuntu).

That's all you need to see something on screen. The sections below explain how
to change the configuration and how to use the other backends.

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
defconfigs in [`lvgl/configs/defconfigs/`](lvgl/configs/defconfigs).

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

This port supports every Linux display and input backend that LVGL provides.
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

## Cross compilation

Cross compilation is supported with CMake. Edit
`user_cross_compile_setup.cmake` to point at your toolchain, then:

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake
cmake --build build -j$(nproc)
```

## Installing LVGL

You can install the built LVGL library and headers to your system:

```bash
cmake --install ./build
```

## Supported boards

The `boards/` directory contains hardware-specific documentation and
configuration files for running LVGL on various embedded Linux development
boards. Each board folder includes setup instructions, build configurations and
optimization guides tailored to that platform. Currently supported boards:

- **F&S PicoCoreMX93** — NXP i.MX93-based system-on-module
- **GLYN UI2 Starter Kit** — GLYN starter kit with the Renesas RZ/G2L
- **NXP i.MX93** — NXP's i.MX93 evaluation platform
- **Renesas RZ/G3E EVKit** — Renesas RZ/G3E evaluation kit
- **Texas Instruments SK-AM62B-P1** — AM62B starter kit
- **Texas Instruments SK-AM62L** — AM62L starter kit
- **Texas Instruments SK-AM62P-LP** — AM62P low-power starter kit
- **Toradex Verdin AM62** — Toradex Verdin module with TI AM62 processor
- **Toradex Verdin iMX8M Mini** — Toradex Verdin module with NXP i.MX8M Mini processor
- **STM32MP157F-DK2** — STMicroelectronics MP1 processor devkit with touchscreen

## Environment variables

Environment variables can be set to modify the behavior of the driver(s).
Check the [driver documentation](https://docs.lvgl.io/master/integration/embedded_linux/drivers/)
for more details.

### Legacy framebuffer (fbdev)

- `LV_LINUX_FBDEV_DEVICE` — override the default (`/dev/fb0`) framebuffer device node.

### EVDEV touchscreen / mouse pointer device

- `LV_LINUX_EVDEV_POINTER_DEVICE` — the path of the input device, e.g.
  `/dev/input/by-id/my-mouse-or-touchscreen`. If not set, devices are
  discovered and added automatically.

### DRM/KMS

- `LV_LINUX_DRM_CARD` — override the default (`/dev/dri/card0`) card.

### Simulator

- `LV_SIM_WINDOW_WIDTH` — width of the window (default `800`).
- `LV_SIM_WINDOW_HEIGHT` — height of the window (default `480`).

## Permissions

By default, unprivileged users don't have access to the framebuffer device
`/dev/fb0`. In such cases you can either run the application with `sudo`, or
grant your user access via the `video` group:

```bash
sudo adduser $USER video
newgrp video
./build/bin/lvglsim
```
