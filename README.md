# Light Control Remapper

CLI (and hopefully GUI in the future) program to emulate a virtual controller with other devices (e.g. keybaord, mouse, etc)

## Explanation of the source files:

lcr.c - the main file
libinput_interface.c - used for parsing the output of libinput list-devices
json_io.c - used for parsing the JSON configuration files
direct.c - used for directly reading inputs from the /dev/input files 
utils.c - random utility functions
mapping_funcs.c - functions for mapping inputs into gamepad inputs

cJSON is used for JSON configuration parsing
