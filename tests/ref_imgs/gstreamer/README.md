This folder stores screenshot baselines for GStreamer integration tests.

Planned baseline for `test_gstreamer_webrtc_smpte75`:
- file: `webrtc_smpte75.lp64.png` (and optional `webrtc_smpte75.lp32.png`)
- resolution: 320x240
- color depth: 32-bit test option (`OPTIONS_TEST_SYSHEAP`)
- source pattern: `videotestsrc pattern=smpte75`
- capture timing: after PLAYING state and 500 ms stabilization delay

Capture/update process:
1. Run `scripts/run_gstreamer_webrtc_test.sh` in a deterministic container.
2. Save generated screenshot as the expected baseline image name.
3. Re-run the test to validate exact comparison.


TODO:
- Replace the videotestsrc pattern `smte75` with `ball`. Screenshot at a give frame can be generated
