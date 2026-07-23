# Usage:
# cmake -DCMAKE_TOOLCHAIN_FILE=cmake/user_cross_compile_setup.cmake -B build -S .
# cmake --build build -j$(nproc)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(tools /home/ubuntu/Your_SDK/prebuilt/rootfsbuilt/arm/toolchain-glibc-gcc/toolchain)
set(CMAKE_C_COMPILER ${tools}/bin/arm-openwrt-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-openwrt-linux-gnueabi-g++)

# If necessary, set STAGING_DIR
# if not work, please try(in shell command): export STAGING_DIR=/home/ubuntu/Your_SDK/out/xxx/openwrt/staging_dir/target
#set(ENV{STAGING_DIR} "/home/ubuntu/Your_SDK/out/xxx/openwrt/staging_dir/target")

