#!/usr/bin/env bash
set -euo pipefail

SCRIPTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_DIR="$(cd "${SCRIPTS_DIR}/.." && pwd)"
ROOT_DIR="$(cd "${TESTS_DIR}/.." && pwd)"

# Preflight checks: fail early with actionable messages
_missing=()
for _cmd in gst-launch-1.0 gst-inspect-1.0 cmake ruby pkg-config wayland-scanner; do
    command -v "$_cmd" >/dev/null 2>&1 || _missing+=("$_cmd")
done
if [[ ${#_missing[@]} -gt 0 ]]; then
    echo "[ERROR] Missing required tools: ${_missing[*]}"
    exit 1
fi
if ! ruby -e 'require "rbconfig"' >/dev/null 2>&1; then
    echo "[ERROR] Ruby headers not found. Install ruby-dev (e.g. sudo apt install ruby-dev)"
    exit 1
fi

ARTIFACT_DIR="${ROOT_DIR}/artifacts/gstreamer-webrtc"
LOG_DIR="${ARTIFACT_DIR}/logs"
mkdir -p "${LOG_DIR}"

SIGNALLER_PORT="${SIGNALLER_PORT:-8443}"
SIGNALLER_URI="${SIGNALLER_URI:-ws://127.0.0.1:${SIGNALLER_PORT}}"
SENDER_PIPELINE="${SENDER_PIPELINE:-videotestsrc pattern=smpte75 is-live=true ! queue ! webrtcsink signaller::uri=${SIGNALLER_URI}}"
TEST_BINARY="${TEST_BINARY:-test_gstreamer_webrtc}"
TEST_NAME="${TEST_NAME:-test_gstreamer_webrtc_smpte75}"

# Helper: print the tail of a log file and exit with a clear error message.
fail_with_log() {
    local step="$1"
    local log="$2"
    echo ""
    echo "[ERROR] Step '${step}' failed. Last 30 lines of ${log}:"
    echo "------------------------------------------------------------------------"
    tail -30 "${log}" || true
    echo "------------------------------------------------------------------------"
    echo "[ERROR] Full log: ${log}"
    exit 1
}

cleanup() {
    set +e
    if [[ -n "${SENDER_PID:-}" ]]; then kill "${SENDER_PID}" >/dev/null 2>&1; fi
    if [[ -n "${SIGNALLER_PID:-}" ]]; then kill "${SIGNALLER_PID}" >/dev/null 2>&1; fi
}
trap cleanup EXIT

# gst-plugins-rs is pre-built into the Docker image by build_gst_plugins_rs.sh
PLUGIN_LIB_DIR="${GST_PLUGINS_RS_DIR:-/opt/gst-plugins-rs}/target/release"
SIGNALLER_BIN="${PLUGIN_LIB_DIR}/gst-webrtc-signalling-server"

export GST_PLUGIN_PATH="${PLUGIN_LIB_DIR}:${GST_PLUGIN_PATH:-}"
export LV_TEST_WEBRTC_SIGNAL_URI="${SIGNALLER_URI}"

echo "[INFO] Checking GStreamer plugin visibility"
gst-inspect-1.0 webrtcsrc > "${LOG_DIR}/gst-inspect-webrtcsrc.log" 2>&1 || true
gst-inspect-1.0 webrtcsink > "${LOG_DIR}/gst-inspect-webrtcsink.log" 2>&1 || true

echo "[INFO] Starting local WebRTC signaller"
"${SIGNALLER_BIN}" --port "${SIGNALLER_PORT}" > "${LOG_DIR}/signaller.log" 2>&1 &
SIGNALLER_PID=$!
sleep 2

echo "[INFO] Starting sender pipeline"
gst-launch-1.0 -e ${SENDER_PIPELINE} > "${LOG_DIR}/sender.log" 2>&1 &
SENDER_PID=$!
sleep 2

BUILD_DIR="${ROOT_DIR}/build-gstreamer-tests"
rm -rf "${BUILD_DIR}"

echo "[INFO] Generating Wayland protocol sources"
WAYLAND_PROTO_DIR="${BUILD_DIR}/wayland_protocols"
mkdir -p "${WAYLAND_PROTO_DIR}"
XDG_SHELL_XML="/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml"
wayland-scanner private-code  "${XDG_SHELL_XML}" "${WAYLAND_PROTO_DIR}/wayland_xdg_shell.c"
wayland-scanner client-header "${XDG_SHELL_XML}" "${WAYLAND_PROTO_DIR}/wayland_xdg_shell.h"

echo "[INFO] Configuring LVGL GStreamer WebRTC test build"
# LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE=1: auto-create the .png baseline when
# it does not exist yet, instead of failing. On the first run, commit the generated
# file at tests/ref_imgs/gstreamer/webrtc_smpte75.lp64.png as the baseline.
cmake -S "${TESTS_DIR}" -B "${BUILD_DIR}" \
    -DCONFIG_LV_USE_GSTREAMER=1 \
    -DCONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE="${LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE:-1}" \
    > "${LOG_DIR}/cmake-configure.log" 2>&1 \
    || fail_with_log "cmake configure" "${LOG_DIR}/cmake-configure.log"

echo "[INFO] Building targeted test binary"
cmake --build "${BUILD_DIR}" --target "${TEST_BINARY}" -j"$(nproc)" \
    > "${LOG_DIR}/cmake-build.log" 2>&1 \
    || fail_with_log "cmake build" "${LOG_DIR}/cmake-build.log"

echo "[INFO] Running ${TEST_BINARY}:${TEST_NAME}"
pushd "${TESTS_DIR}" >/dev/null
ctest --test-dir "${BUILD_DIR}" --output-on-failure -R "^${TEST_BINARY}$" \
    --timeout 10 \
    2>&1 | tee "${LOG_DIR}/ctest.log" || TEST_EXIT=$?
popd >/dev/null

TEST_EXIT="${TEST_EXIT:-0}"

if [[ "${TEST_EXIT}" != "0" ]]; then
    echo ""
    echo "[ERROR] Test failed. Last 30 lines of ctest output:"
    echo "------------------------------------------------------------------------"
    tail -30 "${LOG_DIR}/ctest.log" || true
    echo "------------------------------------------------------------------------"
    grep -rn "screenshot\|ref_imgs\|expected\|actual" "${LOG_DIR}" \
        > "${LOG_DIR}/artifact-hints.log" 2>/dev/null || true
    exit "${TEST_EXIT}"
fi

echo "[INFO] GStreamer WebRTC test passed"
