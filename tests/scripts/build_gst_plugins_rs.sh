#!/usr/bin/env bash
# Clone (if needed) and build the GStreamer Rust WebRTC plugins.
# This script is intended to be run once during the Docker image build so that
# the compiled plugins are baked into the image layer.
#
# Environment variables:
#   GST_PLUGINS_RS_DIR  Override the clone/build destination (default: /opt/gst-plugins-rs)
set -euo pipefail

CLONE_DIR="${GST_PLUGINS_RS_DIR:-/opt/gst-plugins-rs}"

if [[ ! -d "${CLONE_DIR}/.git" ]]; then
    git clone --branch 0.13 --depth 1 \
        https://gitlab.freedesktop.org/gstreamer/gst-plugins-rs.git \
        "${CLONE_DIR}"
fi

cd "${CLONE_DIR}"
cargo build \
    -p gst-plugin-webrtc \
    -p gst-plugin-webrtc-signalling \
    -p gst-plugin-rtp \
    --release
