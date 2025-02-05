#ifndef DIRECT_LCR_H
#define DIRECT_LCR_H

#include "libinput_interface.h"
#include "json_io.h"

int main_loop(struct maps_config* config, LIBINPUT_DEVICES_ARRAY* devarr, int* selected_device_indices, int fd, int (*loop_cb)(struct maps_config*, LIBINPUT_DEVICES_ARRAY*, int*, int, struct input_event*, int));

#endif

