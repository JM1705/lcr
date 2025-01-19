#!/bin/sh
# gcc test.c
gcc json_io.c cJSON/cJSON.c lcr.c
sudo intercept /dev/input/event29 | ./a.out # for keyboard media
