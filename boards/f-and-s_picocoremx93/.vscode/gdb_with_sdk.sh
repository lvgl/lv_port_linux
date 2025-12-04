#!/usr/bin/env bash
set -e

# the last arg is the env setup script path
FUS_SDK_SETUP_SCRIPT="${@: -1}"
# all the args except the last shall be passed to the debugger
ALL_DEBUGGER_ARGS="${@:1:$#-1}"

# set toolchain environement variables before running the debuger
source $FUS_SDK_SETUP_SCRIPT
# $GDB is set by the env setup script. It's the debugger, i.e. aarch64-fslc-linux-gdb
$GDB $ALL_DEBUGGER_ARGS
