#ifndef MAPPING_FUNCS_H
#define MAPPING_FUNCS_H

#include <linux/input.h>

// These are all the functions used for mapping input events to other input events, add more here to extend mapping functionality
// called from lcr.c

// mapping type 1
// maps a key press directly to a button
void key2btn(struct input_event* event, int fd, struct map* mapping, int updated_device);

// mapping type 2
// maps a key press directly to an axis field as a binary button
void key2abs(struct input_event* event, int fd, struct map* mapping, int updated_device);

// mapping type 3
// maps 2 keys directly to an axis field, like pressing on the ends on one of those springed seesaw things at playgrounds
void keys2abs(struct input_event* event, int fd, struct map* mapping, int updated_device);

// mapping type 4
// maps 2 keys to an axis, so one increases and the other decreases the axis value. No drift
void incrementabs(struct input_event* event, int fd, struct map* mapping, int updated_device);

#endif
