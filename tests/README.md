# Linux GStreamer Integration Tests

## Directory structure

```
tests/
  src/
    test_gstreamer_webrtc.c       LVGL Unity test case
  scripts/
    run_gstreamer_webrtc_test.sh  Orchestrates signaller, sender and ctest
    build_gst_plugins_rs.sh       Clones + cargo-builds gst-plugins-rs (run at image build time)
  ref_imgs/gstreamer/             Baseline screenshots for comparison
  wayland_protocols/              Auto-generated Wayland protocol sources (git-ignored)
  CMakeLists.txt                  Standalone CMake build for the test binary
  README.md                       This file
```

## How it works

The test validates the full GStreamer WebRTC receive path inside a single process:

```
videotestsrc pattern=smpte75
        |
   webrtcsink  ──SDP/ICE──►  local signalling server
                                      │
                               webrtcsrc (LVGL widget)
                                      │
                              screenshot comparison
                           against ref_imgs/gstreamer/
```

1. `gst-webrtc-signalling-server` starts locally on port 8443.
2. `gst-launch-1.0` runs a sender pipeline (`videotestsrc pattern=smpte75 ! webrtcsink`) that connects to the signaller.
3. The LVGL test binary (`tests/src/test_gstreamer_webrtc.c`) creates a `lv_gstreamer` widget backed by `webrtcsrc`, waits up to 15 s for the stream to reach PLAYING state, then captures a screenshot and compares it against the baseline in `ref_imgs/gstreamer/`. Due to streaming protocol we compare with REF_IMG_TOLERANCE=5

The WebRTC plugin (`webrtcsrc`/`webrtcsink`) is built from [gst-plugins-rs branch 0.13](https://gitlab.freedesktop.org/gstreamer/gst-plugins-rs) using `tests/scripts/build_gst_plugins_rs.sh`. When using Docker this script is executed **once during the image build** so the plugins are baked into the image layer — no Rust/Cargo toolchain is required at test runtime.

## Running locally — Docker (recommended)

Requires Docker and the `lvgl` submodule to be initialised (`git submodule update --init --recursive`).

```bash
# from the repo root — builds the image (includes cargo build of gst-plugins-rs)
docker build \
  --build-arg BASE_OS=ubuntu:24.04 \
  -f docker/Dockerfile_gstreamer_webrtc_test \
  -t lvgl_gstreamer_webrtc_test .

# run the test
docker run --rm \
  -v "$(pwd)":/workdir \
  -w /workdir \
  lvgl_gstreamer_webrtc_test \
  /bin/bash -ec "chmod +x tests/scripts/run_gstreamer_webrtc_test.sh && tests/scripts/run_gstreamer_webrtc_test.sh"
```

## Running locally — host

Install the required packages first:

```bash
sudo apt install -y \
  build-essential cmake ruby ruby-dev pkg-config \
  gstreamer1.0-tools gstreamer1.0-plugins-base \
  gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
  libgstreamer-plugins-bad1.0-dev \
  libpng-dev libjpeg-dev libfreetype-dev \
  wayland-protocols libwayland-dev
# Rust toolchain (needed to build gst-plugins-rs on the host)
curl https://sh.rustup.rs -sSf | sh -s -- -y --profile minimal
```

Build the GStreamer Rust plugins once:

```bash
./tests/scripts/build_gst_plugins_rs.sh
```

Then run the test from the repo root:

```bash
./tests/scripts/run_gstreamer_webrtc_test.sh
```

### Environment variable overrides

| Variable | Default | Purpose |
|----------|---------|---------|
| `SIGNALLER_PORT` | `8443` | Port the signalling server listens on |
| `SIGNALLER_URI` | `ws://127.0.0.1:8443` | URI passed to `webrtcsrc` and `webrtcsink` |
| `SENDER_PIPELINE` | `videotestsrc pattern=smpte75 ...` | Full `gst-launch-1.0` pipeline string |
| `TEST_BINARY` | `test_gstreamer_webrtc` | CTest target name |
| `GST_PLUGINS_RS_DIR` | `/opt/gst-plugins-rs` | Override clone/build location for gst-plugins-rs |

## Logs and artifacts

All output is written to `artifacts/gstreamer-webrtc/logs/` (git-ignored):

| File | Contents |
|------|----------|
| `signaller.log` | Signalling server output |
| `sender.log` | `gst-launch-1.0` sender pipeline output |
| `gst-inspect-webrtcsrc.log` | Plugin visibility check |
| `cmake-configure.log` | CMake configure step |
| `cmake-build.log` | CMake build step |
| `ctest.log` | Test run output and pass/fail result |

## Baseline screenshot

The reference image `tests/ref_imgs/gstreamer/webrtc_smpte75.lp64.png` does not exist until you create it.

**Creating it for the first time**

The run script passes `-DCONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE=1` to cmake by default. When the reference image is missing, LVGL automatically saves the rendered frame as the new baseline and marks the test as passed. After the run, commit the generated file:

```bash
git add tests/ref_imgs/gstreamer/webrtc_smpte75.lp64.png
git commit -m "Add WebRTC smpte75 baseline screenshot"
```

**Updating the baseline**

If the visual output changes intentionally, delete the old file and re-run once with `LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE=1` (the default) to regenerate it, then commit.

To run in strict comparison mode (no auto-creation), pass `LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE=0`:

```bash
LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE=0 ./tests/scripts/run_gstreamer_webrtc_test.sh
```




## TODO:
- Replace the videotestsrc pattern `smte75` with `ball`. Screenshot at a give frame can be generated

