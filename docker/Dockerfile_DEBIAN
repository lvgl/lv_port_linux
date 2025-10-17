ARG BASE_OS
FROM "$BASE_OS"

RUN DEBIAN_FRONTEND="noninteractive" apt-get update

# Build tools
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y make cmake build-essential python3-venv git

# Required for LV_USE_SDL
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libsdl2-dev libsdl2-image-dev

# Required for LV_USE_LINUX_DRM
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libevdev-dev libdrm-dev linux-headers-generic libgbm-dev 

# Required for LV_USE_WAYLAND
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libwayland-dev libxkbcommon-dev wayland-protocols

# Required for LV_USE_GLFW
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libglfw3-dev libglew-dev


RUN mkdir /workdir
WORKDIR /workdir
