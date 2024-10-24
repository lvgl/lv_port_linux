#!/bin/sh
grep "^#define LV_USE_$1" lv_conf.h | sed 's/#define //g' | awk '{ if ($2=="1") { printf "ON" } else { printf "OFF" }}'
