#!/bin/sh
gcc test.c
intercept /dev/input/event9 | ./a.out # for evremapped keyboard
# intercept /dev/input/by-path/platform-i8042-serio-0-event-kbd | ./a.out
