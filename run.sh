#!/bin/sh
intercept /dev/input/by-path/platform-i8042-serio-0-event-kbd | ./a.out
