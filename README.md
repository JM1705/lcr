# [Migrated] Light Control Remapper

## This project has been migrated to [Codeberg](https://codeberg.org/cloudsinspace/lcr)

CLI (and hopefully GUI in the future) program to emulate a virtual controller with other devices (e.g. keyboard, mouse, etc)

## Compiling:
```$ ./compile.sh```

## Usage:
```# bin/lcr path/to/configuration/file.json```

## Configuration:
An example configuration file is provided at ```examples/example_map.json```  
You will need to change the device name at CHANGEME to the name of the device you want to map from.  
The device being mapped from will not be locked, so you can still use the device while it's being remapped.  

### Explanation of the JSON configuration:
The keycodes used in this program are from the [standard Linux keymap](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h). You can also find them with ```# evtest```

The "output" section specifies the specifications of the emulated controller, e.g. the buttons and axes it has. The default config has the buttons and axes of the xbox one controller specified.

The "inputs" section specifies the real devices to be mapped from. This is a list and can contain up to 256 devices. This list should contain device names from ```# libinput list-devices```.

The "mappings" section specifies the mapping operations to be done. Each map is independent from each other and shouldn't cause interference.

For each map, there are 4 attributes:  
  "type" - The type of mapping operation to be done, listed below  
  "io" - Keycodes for the operations  
  "desc" - A description  
  "attrs" - More attributes, where the function depends on the mapping operation.   

### Map operations
1. maps a key press directly to a button  
  io[0]: keycode to map from  
  io[1]: keycode to map to  
  attrs[0]: device index from "inputs" section above to map  

2. maps a key press directly to an axis field as a binary button  
  io[0]: keycode to map from  
  io[1]: keycode to map to  
  attrs[0]: device index from "inputs" section above to map  
  attrs[1]: minimum value for axis  
  attrs[2]: maximum value for axis  

3. maps 2 keys directly to an axis field, like pressing on the ends on one of those springed seesaw things at playgrounds  
  io[0]: keycode for high  
  io[1]: keycode for low  
  io[2]: keycode to map to  
  attrs[0]: device index from "inputs" section above to map  
  attrs[1]: minimum value for axis  
  attrs[2]: maximum value for axis  

4. maps 2 keys to an axis, so one increases and the other decreases the axis value. No drift should be observed.  
  io[0]: keycode to increase  
  io[1]: keycode to decrease 
  io[2]: keycode to map to 
  attrs[0]: device index from "inputs" section above to map  
  attrs[1]: minimum value for axis  
  attrs[2]: maximum value for axis  
  attrs[2]: change value for every press  


## Explanation of the source files:
lcr.c - the main file  
libinput_interface.c - used for parsing the output of libinput list-devices  
json_io.c - used for parsing the JSON configuration files  
direct.c - used for directly reading inputs from the /dev/input files  
utils.c - random utility functions  
mapping_funcs.c - functions for mapping inputs into gamepad inputs  

cJSON/cJSON.c is used for JSON configuration parsing  

## TODO:
Implement EV_ABS event options
