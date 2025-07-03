#!/bin/sh
# Generate wayland xdg shell protocol

OUTPUT_DIR="$1"

if [ -n "$SDKTARGETSYSROOT" ] && [ -n "$SYSROOT" ]; then
	echo "Error: Both SDKTARGETSYSROOT and SYSROOT are set. Please set only one."
	exit 1
fi

if [ -n "$SDKTARGETSYSROOT" ]; then
	PROTOCOL_ROOT=$SDKTARGETSYSROOT
elif [ -n "$SYSROOT" ]; then
	PROTOCOL_ROOT=$SYSROOT
fi

PROTOCOL_ROOT="${PROTOCOL_ROOT}/usr/share/wayland-protocols"

if ! test -d $PROTOCOL_ROOT
then
	echo "Error: Wayland protocols not found at $PROTOCOL_ROOT" >&2
	echo "For cross-compilation, set SYSROOT environment variable" >&2
	exit 1
fi

if ! test -d $OUTPUT_DIR
then
	mkdir $OUTPUT_DIR
	wayland-scanner client-header "$PROTOCOL_ROOT/stable/xdg-shell/xdg-shell.xml" "$OUTPUT_DIR/wayland_xdg_shell.h"
	wayland-scanner private-code  "$PROTOCOL_ROOT/stable/xdg-shell/xdg-shell.xml" "$OUTPUT_DIR/wayland_xdg_shell.c"
	wayland-scanner client-header "$PROTOCOL_ROOT/stable/linux-dmabuf/linux-dmabuf-v1.xml" "$OUTPUT_DIR/wayland_linux_dmabuf.h"
	wayland-scanner private-code "$PROTOCOL_ROOT/stable/linux-dmabuf/linux-dmabuf-v1.xml" "$OUTPUT_DIR/wayland_linux_dmabuf.c"
fi

echo "$OUTPUT_DIR/wayland_xdg_shell.c"
echo "$OUTPUT_DIR/wayland_linux_dmabuf.c"
