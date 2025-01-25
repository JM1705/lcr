#ifndef LIBINPUT_INTERFACE_H
#define LIBINPUT_INTERFACE_H

#define MAX_ITEM_LENGTHS 1024
#define MAX_DEVICE_STR_LENGTH 4096
#define BUFSIZE 1024

typedef struct LIBINPUT_DEVICE {
  char name[MAX_ITEM_LENGTHS];
  char path[MAX_ITEM_LENGTHS];
  char group[MAX_ITEM_LENGTHS];
  char seat[MAX_ITEM_LENGTHS];
  char capabilities[MAX_ITEM_LENGTHS];
} LIBINPUT_DEVICE;

typedef struct LIBINPUT_DEVICES_ARRAY {
  struct LIBINPUT_DEVICE **devices;
  int device_count;
} LIBINPUT_DEVICES_ARRAY;

struct LIBINPUT_DEVICES_ARRAY *get_devices();

#endif

