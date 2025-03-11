#!/bin/sh
# Generate wayland xdg shell protocol

if ! test -d "$SDKTARGETSYSROOT/usr/share/wayland-protocols"
then
	exit 1
fi

if ! test -d wl_protocols
then
	mkdir wl_protocols
	wayland-scanner client-header "$SDKTARGETSYSROOT/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.h"
	wayland-scanner private-code "$SDKTARGETSYSROOT/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.c"
	wayland-scanner client-header "$SDKTARGETSYSROOT/usr/share/wayland-protocols/unstable/linux-dmabuf/linux-dmabuf-unstable-v1.xml" "wl_protocols/linux-dmabuf-unstable-v1-client-protocol.h"
	wayland-scanner private-code "$SDKTARGETSYSROOT/usr/share/wayland-protocols/unstable/linux-dmabuf/linux-dmabuf-unstable-v1.xml" "wl_protocols/linux-dmabuf-unstable-v1-protocol.c"

	wayland-scanner client-header "$SDKTARGETSYSROOT/usr/share/wayland-protocols/unstable/linux-explicit-synchronization/linux-explicit-synchronization-unstable-v1.xml" "wl_protocols/linux-explicit-synchronization-unstable-v1-client-protocol.h"
	wayland-scanner private-code "$SDKTARGETSYSROOT/usr/share/wayland-protocols/unstable/linux-explicit-synchronization/linux-explicit-synchronization-unstable-v1.xml" "wl_protocols/linux-explicit-synchronization-unstable-v1-protocol.c"
fi
