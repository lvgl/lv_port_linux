#!/bin/sh

# Temporary solution called by CMake to determine what features
# are enabled in lv_conf.h - This will replaced by a solution that can also
# be used with GNU make or CMake
# It also currently doesn't handle an alternate location for lv_conf.h
# other than the one in repository

if test $# -ne 2
then
    echo "usage: backend_conf.sh FEATURE_NAME lv_conf_path"
    exit 1
fi

grep "^#define LV_USE_$1" "$2" | sed 's/#define //g' | awk '{ if ($2=="1") { printf "ON" } else { printf "OFF" }}'
